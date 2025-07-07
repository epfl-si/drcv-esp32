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

#include <M5Unified.h>
#include <M5GFX.h>

#include "pic_scenario.h"   // Include the header file containing image data
#include "../src/variables.h"   // Include the header file containing text variables

//M5Canvas canvas(&M5.Display);

bool boutton_clicked = false;
bool manualRefresh = false;

bool dateForTestingDevelopment = true;
String dateForTestingEnd = "2025-06-11"; //2025-06-12
String dateForTestingStart = dateForTestingEnd + "T06:30:24";
int dateIndents = 20;

int counter = 0;

String resetString = " " + separator + " " + separator + " " + separator + " " + separator + " " + separator + " ";
String APIText = "Default";
String response = "Default";

bool firstLaunch = true;
int autoRefreshMinutes = 60; //Every hour
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
int fontSize = M5.Display.fontHeight(); // Font size
int displayFontSize = M5.Display.fontHeight();
int batteryFontSize = 1;
int roomFontSize = 2;
int textFontSize = 2;
int endX = 400;    // End horizontal axis
int endY = 300;    // End vertical axis

String roomDisplayName;

int x = 0;
int y = 0;
int refreshIconX = 0;
int refreshIconY = 0;
int beforeBattery = 0;


int refreshX = 0;
int refreshX2 = 260;
int refreshY = 0;
int refreshY2 = 0;

int nbRefresh = 0;
int nbRefreshBeforeFullRefresh = 3;

int lastBatteryUpdateTime = 0;

int selectedRoomId = 0;
bool manualChangeRoom = false;


String xmlRequestOrigin = R"rawliteral(<?xml version="1.0" encoding="utf-8"?>
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


String xmlRequest = xmlRequestOrigin;


String xmlRequestGetNameOrigin = R"rawliteral(<?xml version="1.0" encoding="utf-8"?>
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

String xmlRequestGetName = xmlRequestGetNameOrigin;

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
    Serial.print(ct);
    Serial.print(" = ");
    Serial.print(ctLen);
    Serial.print(" (");
    Serial.print(M5.Display.textWidth(ct) + initX + indentLocale);
    Serial.print(" > ");
    Serial.print(endX - currentX);
    Serial.println(")");
    Serial.println(M5.Display.textWidth("W"));
    Serial.println(endX);
    Serial.println(endY);
    // ctLen * (fontSize / 2) -> M5.Display.textWidth(ct);
    if (M5.Display.textWidth(ct) + initX + indentLocale > (endX - currentX)) {
      if (M5.Display.textWidth(ct) > (endX - initX + indentLocale)) { //Check if word if larger than one line
        int ctLengthPart = 0;
        while (ctLengthPart < M5.Display.textWidth(ct)) {
          //int pxAvailableForCurrentLine = (endX - currentX) - (ctLen * (fontSize/2) + initX);
          int pxAvailableForCurrentLine = (endX - currentX) - initX;
          //int pxUsedForCurrentLine = ;
          // (ct.substring(ctLengthPart, ct.length() - 1)).length() * (fontSize / 2) -> M5.Display.textWidth(ct)
          int indexIfSeparator = M5.Display.textWidth(ct.substring(ctLengthPart, ct.length() - 1)) > pxAvailableForCurrentLine ? 1 : 0;
          // (fontSize / 2)  -> M5.Display.textWidth("W")
          int indexMaxAcceptedForCurrentLine = (pxAvailableForCurrentLine) / M5.Display.textWidth("W") - indexIfSeparator ;
          int indexForCutString = indexMaxAcceptedForCurrentLine < ct.length() - indexIfSeparator - ctLengthPart ? indexMaxAcceptedForCurrentLine : ct.length() - indexIfSeparator - ctLengthPart;
          String CurrentLine = ct.substring(ctLengthPart, ctLengthPart + indexForCutString);
          ctLengthPart += M5.Display.textWidth(CurrentLine);
          CurrentLine += (indexIfSeparator == 1 ? "-" : "");
          // prefixString.length() * fontSize / 2 -> M5.Display.textWidth(prefixString)
          indentLocale = M5.Display.textWidth(prefixString) * ((String(content).indexOf(prefixString) != -1) ? 2 : 1);
          M5.Display.setCursor(initX + indentLocale, currentY);
          M5.Display.printf(CurrentLine.c_str());
          //ycanvas.drawString(CurrentLine.c_str(), initX + indentLocale, currentY);
          //DrawString(CurrentLine.c_str(), initX + indentLocale, currentY);
          currentY += lineHeight;
        }
        strTempLine = "";
      }
      else {
        while (strTempLine.length() < (endX - startX) / M5.Display.textWidth("i")) {
          strTempLine += ' ';
        }
        M5.Display.setCursor(initX + indentLocale, currentY);
        M5.Display.printf(strTempLine.c_str());
        //canvas.drawString(strTempLine.c_str(), initX + indentLocale, currentY);
        // prefixString.length() * fontSize / 2 -> M5.Display.textWidth(prefixString)
        indentLocale = M5.Display.textWidth(prefixString) * ((String(content).indexOf(prefixString) != -1) ? 2 : 1);
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
        while (strTempLine.length() < (endX - startX) / M5.Display.textWidth("i")) {
          strTempLine += ' ';
        }
        // Display this line
        
        M5.Display.setCursor(initX + indentLocale, currentY);
        M5.Display.printf(strTempLine.c_str());
        //canvas.drawString(strTempLine.c_str(), initX + indentLocale, currentY);
        // prefixString.length() * fontSize / 2 -> M5.Display.textWidth(prefixString)
        indentLocale = M5.Display.textWidth(prefixString) * ((String(content).indexOf(prefixString) != -1) ? 2 : 1);
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
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println(fontSize);
  Serial.println(M5.Display.width());
  Serial.println(M5.Display.height());
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println();
  //Clear part
  //M5.Display.fillRect(0, EPFL_INN011_header_height, M5.Display.width(), M5.Display.height() - epd_bitmap_refresh_height - EPFL_INN011_header_height, WHITE);
  //canvas.fillScreen(WHITE);
  //M5.Display.display();

  //startY = 20;
  startY = EPFL_INN011_header_height + 20;
  bool firstLine = true && APIText.indexOf(prefixString) != -1;

  Serial.print("Inside update_display : ");
  Serial.println(APIText);
  Serial.println(startX);
  Serial.println(startY);

  if (APIText.indexOf(separator) != -1) {
    int len = 0;
    int maxElem = 2;
    String* APITextArray = Split(APIText, separator.c_str(), len);
    maxElem = len < maxElem ? len : maxElem; //Len for real data, max 2
    //maxElem = APIText.indexOf(resetString) != -1 ? 6 : maxElem; //Len for clear, max 6
    if (APIText == resetString){
      int currentResetY = startY;
      for (int i = 0; i < 6; i++) {
        String tempEmptyStr = "";
        while (M5.Display.textWidth(tempEmptyStr) < endX) {
          tempEmptyStr += ' ';
        }
        // Display this line
        
        M5.Display.setCursor(0, currentResetY);
        M5.Display.printf(tempEmptyStr.c_str());
        Serial.print("currentResetY Before: ");
        Serial.println(currentResetY);
        currentResetY += M5.Display.fontHeight();  
        Serial.print("currentResetY After: ");
        Serial.println(currentResetY);
      }
    }
    else if (len > 1) {
      for (int i = 0; i < maxElem; i++) {
        Serial.println("a");
        Serial.println(APITextArray[i].c_str());
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
    Serial.println(APIText.c_str());
    Part_Text_Display(APIText.c_str(), startX, startY, fontSize, BLACK, endX, endY);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Update_Display before refresh date");

    M5.Display.fillRect(refreshX, refreshY, refreshX2 + 5, M5.Display.height(), WHITE);
    //M5.Display.display();
    
    refreshDateTime(before_refresh_date);
    String current_date_refresh = current_date->year + "-" + current_date->month + "-" + current_date->day;
    String current_hour_refresh = current_date->hour + ":" + current_date->minute + ":" + current_date->second;
    M5.Display.setTextSize(roomFontSize);

    
    M5.Display.setCursor(refreshX, refreshY);
    M5.Display.printf(current_date_refresh.c_str());
    //M5.Display.drawString(current_date_refresh.c_str(), refreshX, refreshY);
    M5.Display.setTextSize(batteryFontSize);
    M5.Display.setCursor(refreshX, refreshY2);
    M5.Display.printf("last refresh: %s", current_hour_refresh);
    //M5.Display.drawString(("last refresh: " + current_hour_refresh).c_str(), x, y2);
    M5.Display.setTextSize(textFontSize);
    M5.Display.drawLine(refreshX, refreshY, refreshX2, refreshY, BLACK); // Horizontal line (For last refresh section, bottom left)
    M5.Display.drawLine(refreshX2, refreshY, refreshX2, M5.Display.height(), BLACK); // Vertical line (For last refresh section, bottom left)
    Serial.println("Update_Display after refresh date");
  }

  //M5.Display.setEpdMode(epd_mode_t::epd_fastest);
  //canvas.fillRect(0, EPFL_INN011_header_height, M5.Display.width(), M5.Display.height() - EPFL_INN011_header_height - epd_bitmap_refresh_height, WHITE);
  
  //canvas.pushSprite(0, EPFL_INN011_header_height);

  
  //M5.update();

  
  //M5.Display.display();                             // Trigger full refresh
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



String GetRoomName(String room){
    HTTPClient https;
    https.begin(API_SERVICE_ENDPOINT);
    https.addHeader("Content-Type", "text/xml");
    https.setAuthorization(API_USERNAME, API_PASSWORD);
    xmlRequestGetName = xmlRequestGetNameOrigin;
    xmlRequestGetName.replace("{email}", room);
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
        return replaceAccentChar(XMLGetter(items[0], "<t:DisplayName>", "</t:DisplayName>"));
      }
      else {
        delay(1000);
      }
    }
}







void setup() {
  Serial.begin(115200);
  delay(10);
  xmlRequest = xmlRequestOrigin;
  xmlRequest.replace("{email}", API_EMAIL_TARGET); // Replace the {email} with the real email target in secrets.h

  // Initialization settings, executed only once when the program starts
  M5.begin();
  M5.Display.setRotation(M5.Display.getRotation() ^ 1);
  M5.Display.setFont(&fonts::Font4);
  M5.Display.setEpdMode(epd_text);  // epd_quality, epd_text, epd_fast, epd_fastest
  M5.Display.clear();

  refreshY = M5.Display.height() - 70;
  refreshY2 = M5.Display.height() - 20;

  //canvas.setColorDepth(1); // mono color
  //M5.Display.setFont(&fonts::FreeMonoBold18pt7b);
  //canvas.setTextColor(0);
  //canvas.setBitmapColor(BLACK, WHITE);
  //canvas.fillSprite(15);
  //canvas.setTextColor(15);
  
  //canvas.createSprite(M5.Display.width(), M5.Display.height() - EPFL_INN011_header_height - epd_bitmap_refresh_height);

  endX = M5.Display.width();
  endY = M5.Display.height();

  M5.Display.init();
  //M5.Display.setFont(&fonts::Font4);
  M5.Display.setTextSize(textFontSize);
  //canvas.setTextSize(4);

  //M5.Display.setEpdMode(epd_text); //epd_mode_t::epd_fastest
  //M5.Display.startWrite();

  //M5.Display.clear();
  //M5.Display.clearDisplay();

  refreshIconX = M5.Display.width() - epd_bitmap_refresh_width;
  refreshIconY = M5.Display.height() - epd_bitmap_refresh_height;

  //Write header
  M5.Display.drawBitmap(0, 0, EPFL_INN011_header, EPFL_INN011_header_width, EPFL_INN011_header_height, WHITE, BLACK); //x, y, bitmap, width, height, bg, fg

  //Write footer refresh icon
  M5.Display.drawBitmap(M5.Display.width() - epd_bitmap_refresh_width, M5.Display.height() - epd_bitmap_refresh_height, epd_bitmap_refresh, epd_bitmap_refresh_width, epd_bitmap_refresh_height, WHITE, BLACK); //x, y, bitmap, width, height, bg, fg

  Serial.println(refreshY);
  Serial.println(refreshY);
  Serial.println(refreshY);
  Serial.println(refreshY);
  Serial.println(refreshY);
  Serial.println(refreshY);
  Serial.println(refreshY);
  //M5.Display.drawLine(refreshX, refreshY, refreshX2, refreshY, BLACK); // Horizontal line (For last refresh section, bottom left)
  //M5.Display.drawLine(refreshX2, refreshY, refreshX2, M5.Display.height(), BLACK); // Vertical line (For last refresh section, bottom left)
  //M5.Display.display(); // Update display

  M5.update();

  M5.Display.setEpdMode(epd_text);

  //M5.Display.setTextColor(BLACK);

  //canvas.fillScreen(WHITE);  // Or any other TFT_ color

  // Draw something
  //canvas.setTextColor(BLACK);

  //Serial.println(canvas.fontHeight());

  //fontSize = canvas.fontHeight();
  fontSize = M5.Display.fontHeight();

  Serial.println();
  Serial.printf("%3d (%3d)", M5.Display.fontHeight(), fontSize);

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

//  Serial.println("----------------------> RESET STRING <-----------------------");
//  Serial.println(resetString);
//  Update_Display(resetString);

  //delay(10000);

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
      M5.Display.setTextColor(WHITE, BLACK);
      M5.Display.setTextSize(roomFontSize);
      roomDisplayName = replaceAccentChar(XMLGetter(items[0], "<t:DisplayName>", "</t:DisplayName>"));
      //M5.Display.setCursor(200, (EPFL_INN011_header_height / 2) - displayFontSize * roomFontSize);
      M5.Display.setCursor((EPFL_INN011_header_width / 2) - (M5.Display.textWidth(roomDisplayName)  / 2), 5);
      M5.Display.printf(roomDisplayName.c_str());
      //M5.Display.drawString(replaceAccentChar(XMLGetter(items[0], "<t:DisplayName>", "</t:DisplayName>")).c_str(), 200, (EPFL_INN011_header_height / 2) - displayFontSize * roomFontSize);
      M5.Display.setTextSize(textFontSize);
      M5.Display.setTextColor(BLACK, WHITE);
    }
    else {
      delay(1000);
    }
  }
  refreshDateTime(current_date);
  refreshDateTime(before_refresh_date);
}





















void loop() {
  //Serial.print("~");
  unsigned long currentTime = millis();
  //delay(100);

  M5.update();
  int32_t batteryLevel = M5.Power.getBatteryLevel();
  int refreshEveryPercent = 5; //all 5%
//  if (batteryLevel / refreshEveryPercent != beforeBattery / refreshEveryPercent && batteryLevel / refreshEveryPercent > beforeBattery / refreshEveryPercent){ //actualise one time at every {refreshEveryPercent}%
  int batteryPercentageRefreshMinute = 10;
  //if (currentTime - lastBatteryUpdateTime >= 60000 * batteryPercentageRefreshMinute || firstLaunch){
  if ((beforeBattery != batteryLevel && currentTime - lastBatteryUpdateTime >= 5000) || firstLaunch){
    Serial.println();
    Serial.println(batteryLevel);
    Serial.println(beforeBattery != batteryLevel);
    Serial.println(currentTime - lastBatteryUpdateTime >= 5000);
    Serial.println(currentTime);
    Serial.println(lastBatteryUpdateTime);
    Serial.println(currentTime - lastBatteryUpdateTime);
    Serial.println(beforeBattery != batteryLevel && currentTime - lastBatteryUpdateTime >= 5000);
    Serial.println();
    //M5.update();
    lastBatteryUpdateTime = currentTime;
    String empty = "";
    if (batteryLevel < 100){
      for (int i = 0; i < 3 - String(batteryLevel).length(); i++){
        empty += "  ";
      }
    }
    String text = String("Batterie : ") + String(batteryLevel) + String("%") + empty;
    beforeBattery = batteryLevel;
    int xBattery = 16;
    int yBattery = (EPFL_INN011_header_height / 2) - displayFontSize * batteryFontSize;
    M5.Display.setCursor(xBattery, yBattery);
    //M5.Display.fillRect(xBattery, yBattery, xBattery + 150, yBattery + 18, BLACK);
    //M5.Display.display();
    
    M5.Display.setTextColor(WHITE, BLACK);
    M5.Display.setTextSize(batteryFontSize);
    M5.Display.print(text);
    M5.Display.setTextSize(textFontSize);
    
    //M5.Display.display();
    
    M5.Display.setTextColor(BLACK, WHITE);
  }



  //Check if refresh button is clicked
  lgfx::touch_point_t tp[3];

  int nums = M5.Display.getTouchRaw(tp, 3);
  if (nums)
  {
    for (int i = 0; i < nums; ++i)
    {
      x = tp[i].y; //x = y because change rotation 90deg invert clock sense
      y = M5.Display.height() - tp[i].x;
      M5.Display.setCursor(128, 128 + i * 24);
      if ((x >= refreshIconX && y >= refreshIconY && x <= refreshIconX + epd_bitmap_refresh_width && y <= refreshIconY + epd_bitmap_refresh_height) && !manualRefresh){
        manualRefresh = true;
      }
    }
  }


//    for (int i = 0; i < API_EMAIL_TARGET_ARRAY_LENGTH; i++){
//      Serial.println(API_EMAIL_TARGET_ARRAY[i]);
//    }
  if (M5.BtnA.wasPressed()) {
    Serial.println("BtnA was pressed");
    selectedRoomId = selectedRoomId - 1 >= 0 ? selectedRoomId - 1 : API_EMAIL_TARGET_ARRAY_LENGTH - 1;
        
    String selectedEmailTarget = String(API_EMAIL_TARGET_ARRAY[selectedRoomId]);
    selectedEmailTarget.replace(String(API_EMAIL_TARGET_SUFFIX), "");
    selectedEmailTarget.toUpperCase();
    Serial.println(selectedEmailTarget);
    
    M5.Display.setTextSize(batteryFontSize);
    M5.Display.setTextColor(WHITE, BLACK);
    String stringForWidthEstimation = "<- " + selectedEmailTarget + " ->";
    M5.Display.setCursor((EPFL_INN011_header_width / 2) - (M5.Display.textWidth(stringForWidthEstimation)  / 2), 70);
    M5.Display.printf("<- %s ->", selectedEmailTarget);
    M5.Display.setTextColor(BLACK, WHITE);
    M5.Display.setTextSize(textFontSize);
  }
  if (M5.BtnB.wasPressed()) {
    manualChangeRoom = true;
    Serial.println("BtnB was pressed");
    String selectedEmailTarget = String(API_EMAIL_TARGET_ARRAY[selectedRoomId]);
    selectedEmailTarget.replace(String(API_EMAIL_TARGET_SUFFIX), "");
    selectedEmailTarget.toUpperCase();
    Serial.println(selectedEmailTarget);
    
    M5.Display.setTextSize(batteryFontSize);
    M5.Display.setTextColor(WHITE, BLACK);
    String stringForWidthEstimation = "-> " + selectedEmailTarget + " <-";
    M5.Display.setCursor((EPFL_INN011_header_width / 2) - (M5.Display.textWidth(stringForWidthEstimation)  / 2), 70);
    M5.Display.printf("-> %s <-", selectedEmailTarget);
    M5.Display.setTextColor(BLACK, WHITE);
    M5.Display.setTextSize(textFontSize);

    manualChangeRoom = true;
    nbRefresh = 0;
  }
  if (M5.BtnC.wasPressed()) {
    Serial.println("BtnC was pressed");
    selectedRoomId = selectedRoomId + 1 < API_EMAIL_TARGET_ARRAY_LENGTH ? selectedRoomId + 1 : 0;
    
    String selectedEmailTarget = String(API_EMAIL_TARGET_ARRAY[selectedRoomId]);
    selectedEmailTarget.replace(String(API_EMAIL_TARGET_SUFFIX), "");
    selectedEmailTarget.toUpperCase();
    Serial.println(selectedEmailTarget);
    
    M5.Display.setTextSize(batteryFontSize);
    M5.Display.setTextColor(WHITE, BLACK);
    String stringForWidthEstimation = "<- " + selectedEmailTarget + " ->";
    M5.Display.setCursor((EPFL_INN011_header_width / 2) - (M5.Display.textWidth(stringForWidthEstimation)  / 2), 70);
    M5.Display.printf("<- %s ->", selectedEmailTarget);
    M5.Display.setTextColor(BLACK, WHITE);
    M5.Display.setTextSize(textFontSize);
  }


  //  Serial.print("WWW: ");
  //  Serial.println(current_date->hour.toInt());

  if ((current_date->hour.toInt() >= MIN_HOUR_REFRESH && current_date->hour.toInt() <= MAX_HOUR_REFRESH && currentTime - lastUpdateTime >= 60000 * autoRefreshMinutes) || manualRefresh || firstLaunch || manualChangeRoom) {
    xmlRequest = xmlRequestOrigin;
    if ((nbRefresh % nbRefreshBeforeFullRefresh == 0 && nbRefresh != 0) || manualChangeRoom){
      Serial.println("Mega Clear");
      //Clear
      M5.Display.clear();
    
      //Write header
      M5.Display.drawBitmap(0, 0, EPFL_INN011_header, EPFL_INN011_header_width, EPFL_INN011_header_height, WHITE, BLACK); //x, y, bitmap, width, height, bg, fg
      
      //Write room name
      if (!manualChangeRoom){
        M5.Display.setTextColor(WHITE, BLACK);
        M5.Display.setTextSize(roomFontSize);
        M5.Display.setCursor((EPFL_INN011_header_width / 2) - (M5.Display.textWidth(roomDisplayName)  / 2), 5);
        M5.Display.printf(roomDisplayName.c_str());
        M5.Display.setTextSize(textFontSize);
        M5.Display.setTextColor(BLACK, WHITE); 
      }
    
      //Write footer refresh icon
      M5.Display.drawBitmap(M5.Display.width() - epd_bitmap_refresh_width, M5.Display.height() - epd_bitmap_refresh_height, epd_bitmap_refresh, epd_bitmap_refresh_width, epd_bitmap_refresh_height, WHITE, BLACK); //x, y, bitmap, width, height, bg, fg
    }

    
    lastUpdateTime = currentTime;
    //Update_Display(resetString); //Refresh partial replace (replacing all writing area with space)

    firstLaunch = false;

    //Update_Display(resetString);

    
//    delay(1000);
//    Serial.println("resetString");
//    Serial.println("resetString");
//    Serial.println(resetString);
//    Serial.println("resetString");
//    Update_Display(resetString);
//    Serial.println(" ------------->                    RESEEEEEEEEEEEEEEEEEEEEEEET                    <-------------");
//    Serial.println(" ------------->                    RESEEEEEEEEEEEEEEEEEEEEEEET                    <-------------");
//    Serial.println(" ------------->                    RESEEEEEEEEEEEEEEEEEEEEEEET                    <-------------");
//    Serial.println(" ------------->                    RESEEEEEEEEEEEEEEEEEEEEEEET                    <-------------");
//    M5.update();
//    delay(1000);

    Update_Display(resetString);
    Update_Display(replaceAccentChar(APIRequestText));









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

    if (manualChangeRoom){
      manualChangeRoom = false;
      
      M5.Display.setTextColor(WHITE, BLACK);
      M5.Display.setTextSize(roomFontSize);
      
      roomDisplayName = GetRoomName(API_EMAIL_TARGET_ARRAY[selectedRoomId]);
      
      M5.Display.setCursor((EPFL_INN011_header_width / 2) - (M5.Display.textWidth(roomDisplayName)  / 2), 5);
      M5.Display.printf(roomDisplayName.c_str());
      
      M5.Display.setTextSize(textFontSize);
      M5.Display.setTextColor(BLACK, WHITE);
    }


    
    Serial.println("Connecting to NTP server: ");


    refreshDateTime(current_date);
    Serial.println("After hour request");

    Update_Display(resetString);
    //M5.update();


    String dateString = "" + current_date->year + "-" + current_date->month + "-" + current_date->day;
    String hourString = String(current_date->hour.toInt() - 2 >= 0 ? current_date->hour.toInt() - 2 : current_date->hour.toInt());
    String dateTimeString = dateString + "T" + (hourString.length() == 1 ? "0" + hourString : hourString) + ":" + current_date->minute + ":" + current_date->second;

    xmlRequest.replace("{start}", dateForTestingDevelopment ? dateForTestingStart : dateTimeString); // Replace the {start} with the current datetime
    xmlRequest.replace("{end}", dateForTestingDevelopment ? dateForTestingEnd : dateString); // Replace the {end} with the current date
    xmlRequest.replace("{email}", API_EMAIL_TARGET_ARRAY[selectedRoomId]); // Replace the {end} with the current date

    Serial.print("Connecting to website: ");
    Serial.println(API_SERVICE_ENDPOINT);

    Serial.println("Before API request");
    HTTPClient https;
    https.begin(API_SERVICE_ENDPOINT);
    https.addHeader("Content-Type", "text/xml");
    https.setAuthorization(API_USERNAME, API_PASSWORD);
    int httpResponseCode = https.POST(xmlRequest);
    Serial.println("After API request");
    Serial.println(xmlRequest);
    if (httpResponseCode == 200) {
      Serial.println("API request Success");
      String body = https.getString();
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


    Update_Display(APIText);

    refreshDateTime(before_refresh_date);
    manualRefresh = false;
    Serial.printf("nbRefresh (%s) %% nbRefreshBeforeFullRefresh (%s) == 0 (%s)", String(nbRefresh), String(nbRefreshBeforeFullRefresh), String(nbRefresh % nbRefreshBeforeFullRefresh == 0));
    Serial.println();
    Serial.printf("nbRefresh (%s) %% nbRefreshBeforeFullRefresh (%s) = (%s)", String(nbRefresh), String(nbRefreshBeforeFullRefresh), String(nbRefresh % nbRefreshBeforeFullRefresh));
    nbRefresh++;
    lastBatteryUpdateTime = currentTime;
  }
}
