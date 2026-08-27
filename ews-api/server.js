import express from 'express';
import rateLimit from 'express-rate-limit';
import pkg from 'pg';
import httpntlm from 'httpntlm';
import dotenv from 'dotenv';
import path from 'path';
import { fileURLToPath } from 'url';
import crypto from 'crypto';

const __dirname = path.dirname(fileURLToPath(import.meta.url));
dotenv.config({ path: path.resolve(__dirname, '.env') });
dotenv.config({ path: path.resolve(__dirname, '../.env') });

const { Client } = pkg;
const app = express();
app.use(express.json());
app.use(express.urlencoded({ extended: true }));

app.set('trust proxy', 1);

const apiLimiter = rateLimit({
  windowMs: 60 * 60 * 1000,
  max: 3,
  message: { error: "Trop de réservations. Veuillez réessayer dans une heure." }
});

const PORT = process.env.PORT || 5000;

const EWS_URL = process.env.EWS_URL;
const DOMAIN = process.env.DOMAIN;
const USERNAME = process.env.USERNAME;
const PASSWORD = process.env.PASSWORD;
const EMAIL_SUFFIX = process.env.EMAIL_SUFFIX;

const TENANT_ID = process.env.TENANT_ID;
const CLIENT_ID = process.env.CLIENT_ID;
const CLIENT_SECRET = process.env.CLIENT_SECRET;


async function getGraphToken() {
  const tokenResponse = await fetch(`https://login.microsoftonline.com/${TENANT_ID}/oauth2/v2.0/token`, {
    method: "POST",
    headers: { "Content-Type": "application/x-www-form-urlencoded" },
    body: new URLSearchParams({
      client_id: CLIENT_ID,
      client_secret: CLIENT_SECRET,
      scope: "https://graph.microsoft.com/.default",
      grant_type: "client_credentials",
    }),
  });
  const tokenData = await tokenResponse.json();
  if (!tokenData.access_token) throw new Error(JSON.stringify(tokenData));
  return tokenData.access_token;
}

const ewsNtlmRequest = (xmlSoap) => {
  return new Promise((resolve, reject) => {
    httpntlm.post({
      url: EWS_URL,
      username: USERNAME,
      password: PASSWORD,
      domain: DOMAIN,
      headers: { 'Content-Type': 'text/xml' },
      body: xmlSoap
    }, function (err, res) {
      if (err) reject(err);
      else if (res.statusCode !== 200) reject(new Error(`EWS Error: Code ${res.statusCode}`));
      else resolve(res.body);
    });
  });
};

function generateToken(roomName) {
  const swissTime = new Date(new Date().toLocaleString("en-US", { timeZone: "Europe/Zurich" }));
  const dateString = swissTime.toISOString().split('T')[0];
  const secret = process.env.QR_SECRET || "fallback_secret";
  return crypto.createHmac('sha256', secret).update(roomName + dateString).digest('hex');
}

function verifyToken(roomName, key) {
  if (!roomName || !key) return false;
  return generateToken(roomName) === key;
}

async function resolveRoomEmail(baseEmail, token) {
  try {
    const graphRes = await fetch(
      `https://graph.microsoft.com/v1.0/users/${encodeURIComponent(baseEmail)}?$select=mail,userPrincipalName`,
      { headers: { Authorization: `Bearer ${token}` } }
    );
    if (graphRes.ok) {
      const graphData = await graphRes.json();
      return graphData.mail || graphData.userPrincipalName || baseEmail;
    }
  } catch (err) {
    console.warn(`⚠️ Impossible de vérifier la casse pour ${baseEmail}.`);
  }
  return baseEmail;
}

async function fetchCalendarEvents(roomEmail, token, startDateTime, endDateTime) {
  const calendarUrl = `https://graph.microsoft.com/v1.0/users/${encodeURIComponent(roomEmail)}/calendarView?startDateTime=${startDateTime.toISOString()}&endDateTime=${endDateTime.toISOString()}&$select=subject,start,end`;

  const calendarResponse = await fetch(calendarUrl, {
    headers: { Authorization: `Bearer ${token}`, Prefer: 'outlook.timezone="UTC"' }
  });

  if (!calendarResponse.ok) throw new Error(`Graph Error ${calendarResponse.status}`);

  const calendarData = await calendarResponse.json();
  const events = calendarData.value || [];

  return events.map(e => ({
    subject: e.subject || "Untitled",
    start: new Date(e.start.dateTime + 'Z'),
    end: new Date(e.end.dateTime + 'Z')
  })).sort((a, b) => a.start - b.start);
}

function calculateBookingSlot(events, baseTime) {
  let actualStart = new Date(baseTime);
  actualStart.setSeconds(59, 0);

  let ongoingEvent = null;
  let changed = true;

  while (changed) {
    changed = false;
    for (const e of events) {
      const diffMins = (e.start.getTime() - actualStart.getTime()) / 60000;

      if (diffMins < 15 && e.end > actualStart) {
        actualStart = new Date(e.end);
        actualStart.setSeconds(59, 0);
        changed = true;
        if (!ongoingEvent) ongoingEvent = e;
      }
    }
  }

  let maxAvailableMinutes = 120;
  const nextEvent = events.find(e => e.start >= actualStart);
  if (nextEvent) {
    const cleanStart = new Date(actualStart);
    cleanStart.setSeconds(0, 0);
    const diffMins = Math.floor((nextEvent.start.getTime() - cleanStart.getTime()) / 60000);

    if (diffMins >= 0 && diffMins < 120) {
      maxAvailableMinutes = diffMins;
    } else if (diffMins < 0) {
      maxAvailableMinutes = 0;
    }
  }

  return { actualStart, ongoingEvent, maxAvailableMinutes };
}

function getBatteryPercentage(voltage) {
  if (!voltage || voltage <= 3.0) return 0;
  if (voltage >= 4.2) return 100;
  return Math.round(((voltage - 3.0) / (4.2 - 3.0)) * 100);
}

async function getBatteryVoltage(deviceName) {
  const client = new Client({ host: process.env.DATABASE_HOST, user: process.env.DATABASE_USER, password: process.env.DATABASE_PASSWORD, database: process.env.DATABASE_NAME, port: process.env.DATABASE_PORT });
  await client.connect();
  const res = await client.query('SELECT battery_voltage FROM device WHERE label = $1 LIMIT 1', [deviceName]);
  await client.end();
  return await getBatteryPercentage(res.rows[0].battery_voltage);
}

app.get('/api/screen', async (req, res) => {
  try {
    const roomsRaw = req.query.roomList || "";
    const roomList = roomsRaw.split(',').map(r => r.trim()).filter(r => r !== "");
    const deviceName = req.query?.deviceName;

    const batteryPercentage = await getBatteryVoltage(deviceName);

    const startOfDay = new Date(); startOfDay.setHours(0, 0, 0, 0);
    const endOfDay = new Date(); endOfDay.setHours(23, 59, 59, 999);

    const token = await getGraphToken();
    let allRoomsData = [];

    for (const roomName of roomList) {
      let ROOM_EMAIL = `${roomName}${EMAIL_SUFFIX}`;
      let fullRoomName = roomName.toUpperCase();
      let eventList = [];

      try {
        const userResponse = await fetch(`https://graph.microsoft.com/v1.0/users/${encodeURIComponent(ROOM_EMAIL)}?$select=displayName`, { headers: { Authorization: `Bearer ${token}` } });
        if (userResponse.ok) {
          const userData = await userResponse.json();
          if (userData.displayName) fullRoomName = userData.displayName;
        }

        ROOM_EMAIL = await resolveRoomEmail(ROOM_EMAIL, token);
        const rawEvents = await fetchCalendarEvents(ROOM_EMAIL, token, startOfDay, endOfDay);

        rawEvents.forEach(e => {
          eventList.push({
            start: e.start.toLocaleTimeString('fr-CH', { hour: '2-digit', minute: '2-digit', timeZone: 'Europe/Zurich' }),
            end: e.end.toLocaleTimeString('fr-CH', { hour: '2-digit', minute: '2-digit', timeZone: 'Europe/Zurich' }),
            title: e.subject
          });
        });
      } catch (error) { console.error(`Erreur pour ${roomName}:`, error.message); }

      allRoomsData.push({ room_name: fullRoomName, prefix_name: roomName, qr_token: generateToken(roomName), events: eventList });
    }
    res.status(200).json({ batteryPercentage, rooms: allRoomsData });
  } catch (error) { res.status(500).json({ error: "Critical server error" }); }
});


app.get('/api/get-room-event', async (req, res) => {
  try {
    const roomName = req.query.room;
    const key = req.query.key;

    if (!verifyToken(roomName, key)) return res.status(403).json({ error: "QR Code expiré ou falsifié." });

    const token = await getGraphToken();
    const ROOM_EMAIL = await resolveRoomEmail(`${roomName}${EMAIL_SUFFIX}`, token);

    const now = new Date();
    const checkUntil = new Date(now.getTime() + 4 * 60 * 60 * 1000);
    const events = await fetchCalendarEvents(ROOM_EMAIL, token, now, checkUntil);

    const slot = calculateBookingSlot(events, now);

    res.status(200).json({
      actualStart: slot.actualStart.toISOString(),
      maxAvailableMinutes: slot.maxAvailableMinutes,
      ongoingEvent: slot.ongoingEvent ? { end: slot.ongoingEvent.end.toISOString(), title: slot.ongoingEvent.subject } : null
    });

  } catch (error) { res.status(500).json({ error: "Erreur serveur" }); }
});


app.post('/api/add-event', apiLimiter, async (req, res) => {
  try {
    const { roomName, duration, subject, key, userEmail } = req.body;

    if (!verifyToken(roomName, key)) return res.status(403).json({ error: "QR Code expiré ou falsifié." });

    const token = await getGraphToken();
    const ROOM_EMAIL = await resolveRoomEmail(`${roomName}${EMAIL_SUFFIX}`, token);

    const now = new Date();
    const checkUntil = new Date(now.getTime() + 4 * 60 * 60 * 1000);
    const events = await fetchCalendarEvents(ROOM_EMAIL, token, now, checkUntil);

    const slot = calculateBookingSlot(events, now);

    let parsedDuration;
    if (duration === "next") {
      parsedDuration = slot.maxAvailableMinutes;
      if (parsedDuration <= 0 || parsedDuration > 120) {
         return res.status(409).json({ error: "Aucun créneau disponible pour 'jusqu'à la prochaine séance'." });
      }
    } else {
      parsedDuration = parseInt(duration);
      const allowedDurations = [15, 30, 45, 60, 90, 120];
      if (!allowedDurations.includes(parsedDuration)) return res.status(400).json({ error: "Durée non autorisée." });

      if (parsedDuration > slot.maxAvailableMinutes) {
        return res.status(409).json({ error: `Conflit ! La salle n'est libre que pour ${slot.maxAvailableMinutes} minutes.` });
      }
    }

    const actualEnd = new Date(slot.actualStart.getTime() + parsedDuration * 60000);
    actualEnd.setSeconds(0, 0);

    const safeSubject = subject ? subject.substring(0, 50) : "Séance instantanée";

    const hasConflict = events.some(e => e.start < actualEnd && e.end > slot.actualStart);
    if (hasConflict) {
      return res.status(409).json({ error: "Conflit temporel ! La salle est occupée à ce moment." });
    }

    let attendeesXml = `<t:Attendee><t:Mailbox><t:EmailAddress>${ROOM_EMAIL}</t:EmailAddress></t:Mailbox></t:Attendee>`;

    if (userEmail && /^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(userEmail)) {
      attendeesXml += `<t:Attendee><t:Mailbox><t:EmailAddress>${userEmail}</t:EmailAddress></t:Mailbox></t:Attendee>`;
    }

    const xmlCreateEvent = `<?xml version="1.0" encoding="utf-8"?>
    <soap:Envelope xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:m="http://schemas.microsoft.com/exchange/services/2006/messages" xmlns:t="http://schemas.microsoft.com/exchange/services/2006/types" xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/">
      <soap:Header><t:RequestServerVersion Version="Exchange2013" /></soap:Header>
      <soap:Body>
        <m:CreateItem SendMeetingInvitations="SendToAllAndSaveCopy">
          <m:Items>
            <t:CalendarItem>
              <t:Subject>${safeSubject}</t:Subject>
              <t:Body BodyType="Text">Réservation instantanée via écran Terminus.</t:Body>
              <t:Start>${slot.actualStart.toISOString()}</t:Start>
              <t:End>${actualEnd.toISOString()}</t:End>
              <t:IsAllDayEvent>false</t:IsAllDayEvent>
              <t:LegacyFreeBusyStatus>Busy</t:LegacyFreeBusyStatus>
              <t:RequiredAttendees>
                ${attendeesXml}
              </t:RequiredAttendees>
            </t:CalendarItem>
          </m:Items>
        </m:CreateItem>
      </soap:Body>
    </soap:Envelope>`;

    const ewsResponse = await ewsNtlmRequest(xmlCreateEvent);

    if (ewsResponse.includes('NoError')) {
      res.status(201).json({ message: "Success" });
    } else {
      const match = ewsResponse.match(/<m:ResponseCode>(.*?)<\/m:ResponseCode>/);
      throw new Error(`Refus EWS : ${match ? match[1] : "Erreur Inconnue"}`);
    }

  } catch (error) { res.status(500).json({ error: error.message }); }
});

app.listen(PORT, '0.0.0.0', () => {
    console.log(`✅ Middleware Server started on port ${PORT}`);
});
