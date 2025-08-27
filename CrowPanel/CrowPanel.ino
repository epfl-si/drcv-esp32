#include <WiFi.h>
#include "time.h"

#include <HTTPClient.h>
#include "sdkconfig.h"
#if CONFIG_ESP_WIFI_REMOTE_ENABLED
#error "WPA-Enterprise is only supported in SoCs with native Wi-Fi support"
#endif
#include <WiFiClientSecure.h>
#include "../src/secrets.h"  // Include the secrets.h file for WiFi credentials

#include "../src/xml_parser/xml_parser.h" // Include my xml parser method
#include "../src/xml_parser/xml_parser.cpp" // Include my xml parser method

#include "../src/split/split.h" // Include my Split method
#include "../src/split/split.cpp" // Include my Split method

#include "../src/event/event.h" // Include Event custom class
#include "../src/event/event.cpp" // Include Event custom class

#include "../src/datetime/datetime.h" // Include DateTime custom class
#include "../src/datetime/datetime.cpp" // Include DateTime custom class

#include <Base64.h>
#include <ArduinoHttpClient.h>

#include <SPI.h>

#include <Arduino.h>        // Include the core library for Arduino platform development
#include "EPD.h"            // Include the EPD library to control the E-Paper Display
#include "pic_scenario.h"   // Include the header file containing image data
#include "../src/variables.h"   // Include the header file containing text variables

// Define button pins
#define BUTTON_DOWN 1
#define BUTTON_UP 2
#define BUTTON_SCROLL_UP 6
#define BUTTON_SCROLL_DOWN 4
#define BUTTON_SCROLL_CLICK 5

bool boutton_clicked = false;
bool manualRefresh = false;

bool dateForTestingDevelopment = false;
String dateForTestingEnd = "2025-06-11"; //2025-06-12
String dateForTestingStart = dateForTestingEnd + "T12:30:24";
int dateIndents = 20;

int counter = 0;

String resetString = " " + separator + " " + separator + " " + separator + " " + separator + " " + separator + " ";
String APIText = "Default";
String response = "Default";

bool firstLaunch = true;
int autoRefreshMinutes = 15; //Every 15min
const int MIN_HOUR_REFRESH = 5;
const int MAX_HOUR_REFRESH = 22;

// NTP Server settings
const char* ntpServer = "pool.ntp.org";
unsigned long lastUpdateTime = 0;  // The timestamp of the last update time

// Adjust for your timezone
const long gmtOffset_sec = 7200;
const int daylightOffset_sec = 0;

struct tm timeinfo;

DateTime* current_date = new DateTime("2025-05-26T03:30:00");
DateTime* before_refresh_date = new DateTime("2025-05-26T03:30:00");

uint8_t Image_BW[15000];    // Declare an array of 15000 bytes to store black and white image data

int beforeStartX = 0;
int startX = 0; // Starting horizontal axis
int startY = 0;  // Starting vertical axis
int fontSize = 24; // Font size
int endX = 400;    // End horizontal axis
int endY = 300;    // End vertical axis



String xmlRequest = R"rawliteral(<?xml version="1.0" encoding="utf-8"?>
  <soap:Envelope xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
      xmlns:m="http://schemas.microsoft.com/exchange/services/2006/messages"
      xmlns:t="http://schemas.microsoft.com/exchange/services/2006/types"
      xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/">
    <soap:Header>
      <t:RequestServerVersion Version="Exchange2016" />
    </soap:Header>
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
        <m:CalendarView StartDate="{start}.000Z" EndDate="{end}T23:59:59Z" />
        <m:ParentFolderIds>
          <t:DistinguishedFolderId Id="calendar">
            <t:Mailbox>
              <t:EmailAddress>{email}</t:EmailAddress>
            </t:Mailbox>
          </t:DistinguishedFolderId>
        </m:ParentFolderIds>
      </m:FindItem>
    </soap:Body>
  </soap:Envelope>)rawliteral";

String xmlRequestModified = xmlRequest;


String xmlRequestGetName = R"rawliteral(<?xml version="1.0" encoding="utf-8"?>
<soap:Envelope xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/"
  xmlns:t="http://schemas.microsoft.com/exchange/services/2006/types">
  <soap:Header>
    <t:RequestServerVersion Version="Exchange2013" />
  </soap:Header>
  <soap:Body>
    <ResolveNames xmlns="http://schemas.microsoft.com/exchange/services/2006/messages"
      ReturnFullContactData="true"
      SearchScope="ActiveDirectory">
      <UnresolvedEntry>{email}</UnresolvedEntry>
    </ResolveNames>
  </soap:Body>
</soap:Envelope>)rawliteral";


/*
  ---------Function description: Display text content locally------------
  ----Parameter introduction:
      content: Text content
      startX: Starting horizontal axis
      startY: Starting vertical axis
      fontSize: Font size
      color: Font color
      endX: End horizontal axis
      endY: End vertical axis
*/
void Part_Text_Display(const char* content, int startX, int &startY, int fontSize, int color, int endX, int endY) {
  int length = strlen(content);
  int i = 0;
  char line[(endX - startX) / (fontSize / 2) + 1]; // Calculate the maximum number of characters per line based on the width of the area
  int currentX = startX;
  int currentY = startY;
  int lineHeight = fontSize;

  String mystring(content);

  int initX = startX;

  int len = 0;
  int lineLength = 0;
  int indentLocale = 0;
  String* SplitString = Split(content, " ", len);
  String strTempLine = "";
  for (int j = 0; j < len; j++) {
    String ct = SplitString[j];
    i = 0;
    int ctLen = ct.length();
    if (ctLen * (fontSize / 2) + initX + indentLocale > (endX - currentX)) {
      if (ctLen * (fontSize / 2) > (endX - initX + indentLocale)) { //Check if word if larger than one line
        int ctLengthPart = 0;
        while (ctLengthPart < ct.length()) {
          //int pxAvailableForCurrentLine = (endX - currentX) - (ctLen * (fontSize/2) + initX);
          int pxAvailableForCurrentLine = (endX - currentX) - initX;
          //int pxUsedForCurrentLine = ;
          int indexIfSeparator = (ct.substring(ctLengthPart, ct.length() - 1)).length() * (fontSize / 2) > pxAvailableForCurrentLine ? 1 : 0;
          int indexMaxAcceptedForCurrentLine = (pxAvailableForCurrentLine) / (fontSize / 2) - indexIfSeparator ;
          int indexForCutString = indexMaxAcceptedForCurrentLine < ct.length() - indexIfSeparator - ctLengthPart ? indexMaxAcceptedForCurrentLine : ct.length() - indexIfSeparator - ctLengthPart;
          String CurrentLine = ct.substring(ctLengthPart, ctLengthPart + indexForCutString);
          ctLengthPart += CurrentLine.length();
          CurrentLine += (indexIfSeparator == 1 ? "-" : "");
          indentLocale = prefixString.length() * fontSize / 2 * ((String(content).indexOf(prefixString) != -1) ? 2 : 1);
          EPD_ShowString(initX + indentLocale, currentY, CurrentLine.c_str(), fontSize, color);
          currentY += lineHeight;
        }
        strTempLine = "";
      }
      else {
        while (strTempLine.length() < (endX - startX) / (fontSize / 2)) {
          strTempLine += ' ';
        }
        EPD_ShowString(initX + indentLocale, currentY, strTempLine.c_str(), fontSize, color);
        indentLocale = prefixString.length() * fontSize / 2 * ((String(content).indexOf(prefixString) != -1) ? 2 : 1);
        currentX = 0;
        lineLength = currentX;
        currentY += lineHeight;
        strTempLine = "";
        j--;
      }
    }
    else {
      strTempLine += ct;
      strTempLine += ' ';

      currentX = strTempLine.length() * (fontSize / 2);

      // If the current Y coordinate plus font size exceeds the area height, stop displaying
      if (currentY + lineHeight > endY) {
        break;
      }

      if (j == len - 1) {
        while (strTempLine.length() < (endX - startX) / (fontSize / 2)) {
          strTempLine += ' ';
        }
        // Display this line
        EPD_ShowString(initX + indentLocale, currentY, strTempLine.c_str(), fontSize, color);
        indentLocale = prefixString.length() * fontSize / 2 * ((String(content).indexOf(prefixString) != -1) ? 2 : 1);
        currentX = 0;
        lineLength = currentX;
        currentY += lineHeight;
        strTempLine = "";
      }
    }
  }
  startY = currentY;
}




void refreshDateTime(DateTime* &datetime) {
  // Configure NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);



  int retry = 0;
  while (!getLocalTime(&timeinfo) && retry < 10) {
    Serial.println("Waiting for time...");
    delay(1000);
    retry++;
  }

  if (retry == 10) {
    Serial.println("Failed to get time from NTP.");
  } else {
    Serial.println("Time synced successfully!");
    char timeString[20];
    strftime(timeString, sizeof(timeString), "%Y-%m-%dT%H:%M:%S", &timeinfo);
    datetime = new DateTime(timeString);
    Serial.println("Time defined successfully!");
  }
}





void Update_Display(String APIText) {

  startY = 80 + 20;
  bool firstLine = true && APIText.indexOf(prefixString) != -1;

  Serial.print("Inside update_display : ");
  Serial.println(APIText);
  Serial.println(startX);
  Serial.println(startY);

  if (APIText != "No more events today") {
    if (APIText.indexOf(separator) != -1) {
      int len = 0;
      int maxElem = 2;
      String* APITextArray = Split(APIText, separator.c_str(), len);
      maxElem = len < maxElem ? len : maxElem; //Len for real data, max 2
      maxElem = APIText.indexOf(resetString) != -1 ? 6 : maxElem; //Len for clear, max 6
      if (len > 1) {
        for (int i = 0; i < maxElem; i++) {
          Serial.println("a");
          //startX = APIText.indexOf(prefixString) != -1 ? dateIndents : startX;
          Part_Text_Display(APITextArray[i].c_str(), startX, startY, fontSize, BLACK, endX, endY);

          startX = (firstLine ? prefixString.length() * fontSize / 2 : 0); //FAIRE LIGNE QUE prefixString.length() * fontSize / 2 SI PAS 1ère ligne ET QUE CELEL CI CONTENANT prefixString
          //firstLine = (firstLine ? !firstLine : firstLine);

          startY = APIText.indexOf(prefixString) != -1 ? (startY + fontSize / 2) : startY;
        }
      }
      else if (len == 1) {
        Serial.println("b");
        startX = dateIndents;
        Part_Text_Display(APIText.c_str(), startX, startY, fontSize, BLACK, endX, endY);
      }
      else {
        Serial.println("c");
        Part_Text_Display("error", startX, startY, fontSize, BLACK, endX, endY);
      }
    }
    else {
      Serial.println("d");
      //startX = APIText.indexOf(prefixString) != -1 ? dateIndents : startX;
      Part_Text_Display(APIText.c_str(), startX, startY, fontSize, BLACK, endX, endY);
    }
  }
  else {
    Serial.println("e");
    Part_Text_Display(APIText.c_str(), startX, startY, fontSize, BLACK, endX, endY);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Update_Display before refresh date");
    refreshDateTime(before_refresh_date);
    String current_date_refresh = current_date->year + "-" + current_date->month + "-" + current_date->day;
    String current_hour_refresh = current_date->hour + ":" + current_date->minute + ":" + current_date->second;
    EPD_ShowString(0, 11 * fontSize - 4, current_date_refresh.c_str(), fontSize, BLACK);
    EPD_ShowString(0, 12 * fontSize - 4, ("last refresh: " + current_hour_refresh).c_str(), 12, BLACK);
    EPD_DrawLine(0, 11 * fontSize - 5, 140, 11 * fontSize - 5, BLACK); // Horizontal line
    EPD_DrawLine(140, 11 * fontSize - 4, 140, 12 * fontSize - 4 + 12, BLACK); // Vertical line
    Serial.println("Update_Display after refresh date");
  }

  Serial.println("Update_Display before refresh");
  EPD_Display(Image_BW);
  Serial.println("Update_Display middle refresh");
  EPD_Update_Part();
  Serial.println("Update_Display after refresh");
}


void clear_all() {
  // Function to clear the screen content
  EPD_Clear();                 // Clear the screen content, restoring it to its default state
  Paint_NewImage(Image_BW, EPD_W, EPD_H, 0, WHITE); // Create a new image buffer, size EPD_W x EPD_H, background color white
  Paint_Clear(WHITE);           // Fill the entire canvas with white
  EPD_Display_Part(0, 0, EPD_W, EPD_H, Image_BW); // Display the image stored in the Image_BW array
}


bool isCurrentEvent(Event* event, DateTime* current_date) {
  return (current_date->hour >= (event->startDateTime).hour) ?
         (current_date->hour == (event->startDateTime).hour) ?
         (current_date->minute >= (event->startDateTime).minute) ?
         (current_date->hour == (event->endDateTime).hour) ?
         (current_date->minute <= (event->startDateTime).minute)
         :
         true
         :
         false
         :
         (current_date->hour == (event->endDateTime).hour) ?
         (current_date->minute <= (event->startDateTime).minute)
         :
         ((current_date->hour >= (event->startDateTime).hour) && (current_date->hour <= (event->endDateTime).hour))
         :
         false;
}



void bouton_click_handler(String button) {
  boutton_clicked = true;
  Serial.print(button);
  Serial.println(" Pressed");
  delay(600);
  boutton_clicked = false;
  if (button == "BUTTON_UP" || button == "BUTTON_DOWN" || button == "BUTTON_SCROLL_CLICK") {
    manualRefresh = true;
  }
}




String replaceAccentChar(String text) {
  struct KeyValue {
    const char* key;
    const char* value;
  };

  //This dict is made by AI (ask it before it easy but take too much time to do handmade)
  KeyValue dict[] = {
    {"à", "a"}, {"á", "a"}, {"â", "a"}, {"ã", "a"}, {"ä", "a"}, {"å", "a"}, {"ā", "a"},
    {"À", "A"}, {"Á", "A"}, {"Â", "A"}, {"Ã", "A"}, {"Ä", "A"}, {"Å", "A"}, {"Ā", "A"},

    {"è", "e"}, {"é", "e"}, {"ê", "e"}, {"ë", "e"}, {"ē", "e"}, {"ė", "e"}, {"ę", "e"},
    {"È", "E"}, {"É", "E"}, {"Ê", "E"}, {"Ë", "E"}, {"Ē", "E"}, {"Ė", "E"}, {"Ę", "E"},

    {"ì", "i"}, {"í", "i"}, {"î", "i"}, {"ï", "i"}, {"ī", "i"}, {"į", "i"}, {"ı", "i"},
    {"Ì", "I"}, {"Í", "I"}, {"Î", "I"}, {"Ï", "I"}, {"Ī", "I"}, {"Į", "I"},

    {"ò", "o"}, {"ó", "o"}, {"ô", "o"}, {"õ", "o"}, {"ö", "o"}, {"ø", "o"}, {"ō", "o"},
    {"Ò", "O"}, {"Ó", "O"}, {"Ô", "O"}, {"Õ", "O"}, {"Ö", "O"}, {"Ø", "O"}, {"Ō", "O"},

    {"ù", "u"}, {"ú", "u"}, {"û", "u"}, {"ü", "u"}, {"ū", "u"}, {"ů", "u"}, {"ų", "u"},
    {"Ù", "U"}, {"Ú", "U"}, {"Û", "U"}, {"Ü", "U"}, {"Ū", "U"}, {"Ů", "U"}, {"Ų", "U"},

    {"ç", "c"}, {"ć", "c"}, {"č", "c"},
    {"Ç", "C"}, {"Ć", "C"}, {"Č", "C"},

    {"ñ", "n"}, {"ń", "n"}, {"ň", "n"},
    {"Ñ", "N"}, {"Ń", "N"}, {"Ň", "N"},

    {"ß", "B"}, {"œ", "oe"}, {"Œ", "OE"}, {"æ", "ae"}, {"Æ", "AE"}, {"&amp;", "&"}
  };

  int len = sizeof(dict) / sizeof(dict[0]);


  for (int i = 0; i < len; i++) {
    text.replace(dict[i].key, dict[i].value);
  }

  return text;
}











void setup() {
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_SCROLL_UP, INPUT_PULLUP);
  pinMode(BUTTON_SCROLL_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_SCROLL_CLICK, INPUT_PULLUP);

  Serial.begin(115200);
  delay(10);
  xmlRequest.replace("{email}", API_EMAIL_TARGET); // Replace the {email} with the real email target in secrets.h
  xmlRequestModified = xmlRequest;


  // Initialization settings, executed only once when the program starts
  // Configure pin 7 as output mode and set it to high level to activate the screen power
  pinMode(7, OUTPUT);            // Set pin 7 as output mode
  digitalWrite(7, HIGH);         // Set pin 7 to high level, activating the screen power

  EPD_GPIOInit();
  EPD_Init();
  EPD_Clear();
  Paint_Clear(WHITE);
  EPD_Update();

  Paint_NewImage(Image_BW, EPD_W, EPD_H, 0, WHITE);
  Paint_Clear(WHITE);
  EPD_Display(Image_BW);
  EPD_Clear_R26H(Image_BW);
  EPD_Update();

  EPD_ShowPicture(0, 0, 400, 80, EPFL_INN011_header, BLACK);

  EPD_ShowPicture(400 - 32, 300 - 32 - 16, 32, 32, epd_bitmap_refresh, BLACK);


  Serial.println("Before Wifi text");
  Update_Display(replaceAccentChar(Loading_Message));
  Serial.println("After Wifi text");






  Serial.println();
  Serial.print("Connecting to network: ");
  Serial.println(ssid);
  WiFi.disconnect(true);  //disconnect form wifi to set new wifi connection
  WiFi.mode(WIFI_STA);    //init wifi mode

  // Example1 (most common): a cert-file-free eduroam with PEAP (or TTLS)
  WiFi.begin(ssid, WPA2_AUTH_PEAP, EAP_IDENTITY, EAP_USERNAME, EAP_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    counter++;
    if (counter >= 60) {  //after 30 seconds timeout - reset board
      ESP.restart();
    }
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address set: ");
  Serial.println(WiFi.localIP());  //print LAN IP

  HTTPClient https;
  https.begin(API_SERVICE_ENDPOINT);
  https.addHeader("Content-Type", "text/xml");
  https.setAuthorization(API_USERNAME, API_PASSWORD);
  xmlRequestGetName.replace("{email}", API_EMAIL_TARGET);
  int httpResponseCode  = 0;
  while (httpResponseCode != 200) {
    Serial.println("Get room name");
    httpResponseCode = https.POST(xmlRequestGetName);
    Serial.print("code: ");
    Serial.println(httpResponseCode);

    if (httpResponseCode == 200) {
      String body = https.getString();
      int itemsLength = 0;
      String* items = XMLParser(body, "<t:Contact>", "</t:Contact>", itemsLength);
      EPD_ShowString(20, 40, replaceAccentChar(XMLGetter(items[0], "<t:DisplayName>", "</t:DisplayName>")).c_str(), 24, WHITE);
    }
    else {
      delay(1000);
    }
  }
  refreshDateTime(current_date);
  refreshDateTime(before_refresh_date);
}





















void loop() {
  Serial.print("~");
  unsigned long currentTime = millis();
  delay(100);

  if (digitalRead(BUTTON_DOWN) == LOW && !boutton_clicked) {
    bouton_click_handler("BUTTON_DOWN");
  }

  if (digitalRead(BUTTON_UP) == LOW && !boutton_clicked) {
    bouton_click_handler("BUTTON_UP");
  }

  if (digitalRead(BUTTON_SCROLL_UP) == LOW && !boutton_clicked) {
    bouton_click_handler("BUTTON_SCROLL_UP");
  }

  if (digitalRead(BUTTON_SCROLL_DOWN) == LOW && !boutton_clicked) {
    bouton_click_handler("BUTTON_SCROLL_DOWN");
  }

  if (digitalRead(BUTTON_SCROLL_CLICK) == LOW && !boutton_clicked) {
    bouton_click_handler("BUTTON_SCROLL_CLICK");
  }

  //  Serial.print("WWW: ");
  //  Serial.println(current_date->hour.toInt());

  if ((current_date->hour.toInt() >= MIN_HOUR_REFRESH && current_date->hour.toInt() <= MAX_HOUR_REFRESH && currentTime - lastUpdateTime >= 60000 * autoRefreshMinutes) || manualRefresh || firstLaunch) {
    lastUpdateTime = currentTime;
    Update_Display(resetString); //Refresh partial replace (replacing all writing area with space)

    firstLaunch = false;

    Update_Display(replaceAccentChar(APIRequestText));

    xmlRequestModified = xmlRequest;









    Serial.println("Before hour request");
    if (WiFi.status() == WL_CONNECTED) {  //if we are connected to Eduroam network
      counter = 0;                        //reset counter
      Serial.println("Wifi is still connected with IP: ");
      Serial.println(WiFi.localIP());            //inform user about his IP address
    } else if (WiFi.status() != WL_CONNECTED) {  //if we lost connection, retry
      WiFi.begin(ssid);
    }
    while (WiFi.status() != WL_CONNECTED) {  //during lost connection, print dots
      delay(500);
      Serial.print(".");
      counter++;
      if (counter >= 60) {  //30 seconds timeout - reset board
        ESP.restart();
      }
    }
    Serial.println("Connecting to NTP server: ");


    refreshDateTime(current_date);


    String dateString = "" + current_date->year + "-" + current_date->month + "-" + current_date->day;
    String hourString = String(current_date->hour.toInt() - 2 >= 0 ? current_date->hour.toInt() - 2 : current_date->hour.toInt());
    String dateTimeString = dateString + "T" + (hourString.length() == 1 ? "0" + hourString : hourString) + ":" + current_date->minute + ":" + current_date->second;

    Serial.println("After hour request");
    Serial.println("After hour request");
    Serial.println("After hour request");
    Serial.println(dateTimeString);
    Serial.println("After hour request");
    Serial.println("After hour request");
    Serial.println("After hour request");

    xmlRequestModified.replace("{start}", dateForTestingDevelopment ? dateForTestingStart : dateTimeString); // Replace the {start} with the current datetime
    xmlRequestModified.replace("{end}", dateForTestingDevelopment ? dateForTestingEnd : dateString); // Replace the {end} with the current date

    Serial.println("xmlRequestModified");
    Serial.println("xmlRequestModified");
    Serial.println("xmlRequestModified");
    Serial.println(xmlRequestModified);
    Serial.println("xmlRequestModified");
    Serial.println("xmlRequestModified");
    Serial.println("xmlRequestModified");

    Serial.print("Connecting to website: ");
    Serial.println(API_SERVICE_ENDPOINT);

    Serial.println("Before API request");
    HTTPClient https;
    https.begin(API_SERVICE_ENDPOINT);
    https.addHeader("Content-Type", "text/xml");
    https.setAuthorization(API_USERNAME, API_PASSWORD);
    int httpResponseCode = https.POST(xmlRequestModified);
    Serial.println("After API request");
    Serial.println("Another API request !!");

    if (httpResponseCode == 200) {
      Serial.println("API request Success");
      String body = https.getString();
      Serial.println("Another API response !!");
      Serial.println(body);
      APIText = body;
      response = body;
      int itemsLength = 0;
      String* items = XMLParser(body, "<t:Items>", "</t:Items>", itemsLength);
      int calendarItemLength = 0;
      String* calendarItem = XMLParser(items[0], "<t:CalendarItem>", "</t:CalendarItem>", calendarItemLength);
      Event *eventList[calendarItemLength];
      for (int i = 0; i < calendarItemLength; i++) {
        eventList[i] = new Event(
          replaceAccentChar(XMLGetter(calendarItem[i], "<t:Subject>", "</t:Subject>")),
          DateTime(XMLGetter(calendarItem[i], "<t:Start>", "</t:Start>")),
          DateTime(XMLGetter(calendarItem[i], "<t:End>", "</t:End>"))
        );
      }

      if (calendarItemLength != 0) {
        bool isCurrent = isCurrentEvent(eventList[0], current_date);
        String prefix = isCurrent ? "current : " : "next : ";
        if (calendarItemLength == 1) {
          APIText = prefixString + String((eventList[0]->startDateTime).hour.toInt() + 2 < 10 ? "0" + String((eventList[0]->startDateTime).hour.toInt() + 2) : (eventList[0]->startDateTime).hour.toInt() + 2) + ":" + (eventList[0]->startDateTime).minute + "-" + ((eventList[0]->endDateTime).hour.toInt() + 2 < 10 ? "0" + String((eventList[0]->endDateTime).hour.toInt() + 2) : (eventList[0]->endDateTime).hour.toInt() + 2) + ":" + (eventList[0]->endDateTime).minute + " " + eventList[0]->subject;
        }
        else {
          APIText = "";
          for (int i = 0; i < calendarItemLength; i++) {
            String separator_string = i == 0 ? prefixString : separator;
            String res = separator_string + ((eventList[i]->startDateTime).hour.toInt() + 2 < 10 ? "0" + String((eventList[i]->startDateTime).hour.toInt() + 2) : (eventList[i]->startDateTime).hour.toInt() + 2) + ":" + (eventList[i]->startDateTime).minute + "-" + ((eventList[i]->endDateTime).hour.toInt() + 2 < 10 ? "0" + String((eventList[i]->endDateTime).hour.toInt() + 2) : (eventList[i]->endDateTime).hour.toInt() + 2) + ":" + (eventList[i]->endDateTime).minute + " " + eventList[i]->subject;
            APIText += res;
          }
        }
      }
      else {
        APIText = replaceAccentChar(noEventText);
      }
    }
    else {
      Serial.print("Err: ");
      Serial.println(httpResponseCode);
      APIText = "Error check Serial";
      response = "Error check Serial";
    }


    Serial.println("Another APIText !!");
    Serial.println(APIText);

    Update_Display(resetString); //Refresh partial replace (replacing all writing area with space)
    Update_Display(APIText);

    refreshDateTime(before_refresh_date);
    manualRefresh = false;
  }
}
