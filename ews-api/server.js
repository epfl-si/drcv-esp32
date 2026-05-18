import express from 'express';
import httpntlm from 'httpntlm';
import xml2js from 'xml2js';
import pkg from 'pg';
import dotenv from 'dotenv';
import path from 'path';
import { fileURLToPath } from 'url';

const __dirname = path.dirname(fileURLToPath(import.meta.url));
dotenv.config({ path: path.resolve(__dirname, '.env') });
dotenv.config({ path: path.resolve(__dirname, '../.env') });

const { Client } = pkg;
const app = express();
const PORT = process.env.PORT;

const EWS_URL = process.env.EWS_URL;
const DOMAIN = process.env.DOMAIN;
const USERNAME = process.env.USERNAME;
const PASSWORD = process.env.PASSWORD;
const EMAIL_SUFFIX = process.env.EMAIL_SUFFIX;

function getBatteryPercentage(voltage) {
  if (voltage === 0) return 0;
  if (voltage <= 0.45) return 10;
  if (voltage <= 0.90) return 20;
  if (voltage <= 1.35) return 30;
  if (voltage <= 1.80) return 40;
  if (voltage <= 2.25) return 50;
  if (voltage <= 2.70) return 60;
  if (voltage <= 3.15) return 70;
  if (voltage <= 3.60) return 80;
  if (voltage <= 4.05) return 90;
  return 100;
}

async function getBatteryVoltage(deviceName) {
  const client = new Client({
    host: process.env.DATABASE_HOST,
    user: process.env.DATABASE_USER,
    password: process.env.DATABASE_PASSWORD,
    database: process.env.DATABASE_NAME,
    port: process.env.DATABASE_PORT,
  });

  await client.connect();

  // get the voltage of the device with this specific name
  const query = 'SELECT battery_voltage FROM device WHERE label = $1 LIMIT 1';
  const res = await client.query(query, [deviceName]);
  await client.end();

  const batteryPercentage = await getBatteryPercentage(res.rows[0].battery_voltage);

  console.log(`battery percentage : ${batteryPercentage}`);

  return batteryPercentage;
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

app.get('/api/screen', async (req, res) => {
  try {
    const roomsRaw = req.query.roomList || "";
    const roomList = roomsRaw.split(',').map(r => r.trim()).filter(r => r !== "");
    const deviceName = req.query?.deviceName;
    console.log(`📡 Starting fetch for ${Array.isArray(roomList) ? roomList.length : 0} rooms...`);
    console.log(`📡 Device Name is ${deviceName} `);
    console.log(`📡 Rooms:`, roomList);

    if (!Array.isArray(roomList) || roomList.length === 0) {
        console.warn("⚠️ Attention: roomList est vide ou n'est pas un tableau !");
    }

    const batteryPercentage = await getBatteryVoltage(deviceName);

    const startOfDay = new Date();
    startOfDay.setHours(0, 0, 0, 0); // Force the start to midnight this morning

    const endOfDay = new Date();
    endOfDay.setHours(23, 59, 59, 999); // Force the end to 11:59 PM tonight

    const parser = new xml2js.Parser({ explicitArray: false, ignoreAttrs: true });

    // The large array that will contain all rooms
    let allRoomsData = [];

    // Loop through each room defined in the list
    for (const roomName of roomList) {
      const ROOM_EMAIL = `${roomName}${EMAIL_SUFFIX}`;
      console.log(`⏳ Processing: ${ROOM_EMAIL}`);

      let fullRoomName = roomName.toUpperCase(); // Fallback if ResolveName fails
      let eventList = [];

      // 1. XML for ResolveNames
      const xmlResolve = `<?xml version="1.0" encoding="utf-8"?>
      <soap:Envelope xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/" xmlns:t="http://schemas.microsoft.com/exchange/services/2006/types">
        <soap:Header><t:RequestServerVersion Version="Exchange2013" /></soap:Header>
        <soap:Body>
          <ResolveNames xmlns="http://schemas.microsoft.com/exchange/services/2006/messages" ReturnFullContactData="true" SearchScope="ActiveDirectory">
            <UnresolvedEntry>${ROOM_EMAIL}</UnresolvedEntry>
          </ResolveNames>
        </soap:Body>
      </soap:Envelope>`;

      // 2. XML for FindItem (Calendar)
      const xmlSoap = `<?xml version="1.0" encoding="utf-8"?>
      <soap:Envelope xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                      xmlns:m="http://schemas.microsoft.com/exchange/services/2006/messages"
                      xmlns:t="http://schemas.microsoft.com/exchange/services/2006/types"
                      xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/">
        <soap:Header><t:RequestServerVersion Version="Exchange2013" /></soap:Header>
        <soap:Body>
          <m:FindItem Traversal="Shallow">
            <m:ItemShape>
              <t:BaseShape>IdOnly</t:BaseShape>
              <t:AdditionalProperties>
                <t:FieldURI FieldURI="item:Subject" />
                <t:FieldURI FieldURI="calendar:Start" />
                <t:FieldURI FieldURI="calendar:End" />
              </t:AdditionalProperties>
            </m:ItemShape>
            <m:CalendarView StartDate="${startOfDay.toISOString()}" EndDate="${endOfDay.toISOString()}" />
            <m:ParentFolderIds>
              <t:DistinguishedFolderId Id="calendar">
                <t:Mailbox><t:EmailAddress>${ROOM_EMAIL}</t:EmailAddress></t:Mailbox>
              </t:DistinguishedFolderId>
            </m:ParentFolderIds>
          </m:FindItem>
        </soap:Body>
      </soap:Envelope>`;

      try {
        // A. Execute ResolveNames
        const resolveData = await ewsNtlmRequest(xmlResolve);
        const resolveResult = await parser.parseStringPromise(resolveData);
        try {
          fullRoomName = resolveResult['s:Envelope']['s:Body']['m:ResolveNamesResponse']['m:ResponseMessages']['m:ResolveNamesResponseMessage']['m:ResolutionSet']['t:Resolution']['t:Contact']['t:DisplayName'];
        } catch (e) { /* Keep the fallback */ }

        // B. Execute FindItem (Calendar)
        const ewsData = await ewsNtlmRequest(xmlSoap);
        const result = await parser.parseStringPromise(ewsData);

        try {
          const items = result['s:Envelope']['s:Body']['m:FindItemResponse']['m:ResponseMessages']['m:FindItemResponseMessage']['m:RootFolder']['t:Items']['t:CalendarItem'];

          if (items) {
            // VITAL TRICK: If EWS returns a single event, xml2js creates a simple object.
            // If there are multiple events, it creates an array. We force everything into an array:
            const events = Array.isArray(items) ? items : [items];

            events.forEach(event => {
              const subject = event['t:Subject'] || "Untitled";
              const startRaw = event['t:Start'];
              const endRaw = event['t:End'];

              // Format the time WITH the Swiss timezone (to fix the 07:00 instead of 09:00 issue)
              const startHour = startRaw ? new Date(startRaw).toLocaleTimeString('fr-CH', {hour: '2-digit', minute:'2-digit', timeZone: 'Europe/Zurich'}) : "??";
              const endHour = endRaw ? new Date(endRaw).toLocaleTimeString('fr-CH', {hour: '2-digit', minute:'2-digit', timeZone: 'Europe/Zurich'}) : "??";

              // Populate the structured array
              eventList.push({
                start: startHour,
                end: endHour,
                title: subject
              });
            });
          }
        } catch (e) {
          console.log(`No events found for ${roomName}`);
        }
      } catch (error) {
        console.error(`Network error for ${roomName}:`, error.message);
      }

      // Add the processed room to the global result
      allRoomsData.push({
        room_name: fullRoomName,
        events: eventList
      });
    }

    console.log("✅ All rooms have been processed!");

    // PERFECT format for Terminus
    res.status(200).json({
      batteryPercentage: batteryPercentage,
      rooms: allRoomsData
    });

  } catch (error) {
    console.error("❌ General API Error:", error.message);
    res.status(500).json({ error: "Critical server error" });
  }
});

app.listen(PORT, '0.0.0.0', () => {
    console.log(`✅ Middleware Server started on port ${PORT}`);
    console.log(`📁 Multi-room mode activated!`);
});
