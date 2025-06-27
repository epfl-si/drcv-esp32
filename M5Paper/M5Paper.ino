
#include <M5Unified.h>
#include <M5GFX.h>

#include "pic_scenario.h" 

int x = 0;
int y = 0;
int withBeforeRotation = 0;
int heightBeforeRotation = 0;
bool isClicked = false;
int refreshIconX = 0;
int refreshIconY = 0;
int beforeBattery = 0;
int isCharging = false;

void setup(void)
{
  Serial.begin(115200);
  M5.begin();

  M5.Display.init();
  M5.Display.setFont(&fonts::Font4);

  if (!M5.Display.touch())
  {
    M5.Display.setTextDatum(textdatum_t::middle_center);
    M5.Display.drawString("Touch not found.", M5.Display.width() / 2, M5.Display.height() / 2);
  }

  if (M5.Display.width() < M5.Display.height())
  {
    M5.Display.setRotation(M5.Display.getRotation() ^ 1);
  }

  Serial.print("height");
  Serial.print(M5.Display.height());
  Serial.print("width");
  Serial.print(M5.Display.width());

  M5.Display.setEpdMode(epd_mode_t::epd_fastest);
  M5.Display.startWrite();

  M5.Display.clear();
  M5.Display.clearDisplay();

  refreshIconX = M5.Display.width() - epd_bitmap_refresh_width;
  refreshIconY = M5.Display.height() - epd_bitmap_refresh_height;

  //Write header
  M5.Display.drawBitmap(0, 0, EPFL_INN011_header, EPFL_INN011_header_width, EPFL_INN011_header_height, WHITE, BLACK); //x, y, bitmap, width, height, bg, fg

  //Write footer
  M5.Display.drawBitmap(M5.Display.width() - epd_bitmap_refresh_width, M5.Display.height() - epd_bitmap_refresh_height, epd_bitmap_refresh, epd_bitmap_refresh_width, epd_bitmap_refresh_height, WHITE, BLACK); //x, y, bitmap, width, height, bg, fg
  
  M5.Display.display(); // Update display

  Serial.println();
}

void loop(void)
{
  //Serial.print("is charging : ");
  //Serial.print(M5.Power.isCharging());
  //Serial.print(", ");
  //Serial.println(M5.Power.getBatteryLevel());

  M5.update();
  int32_t batteryLevel = M5.Power.getBatteryLevel();
  if (batteryLevel != beforeBattery){
    beforeBattery = batteryLevel;
    M5.Display.setCursor(16, 32);
    M5.Display.setTextColor(WHITE);
    M5.Display.printf("Batterie : %3d %%            ", batteryLevel);
    M5.Display.setTextColor(BLACK);
    M5.Display.display(); 
  }
//  if (isCharging != M5.Power.isCharging() && M5.Power.isCharging()){
//    display.setCursor(16, 32);
//    display.printf("Batterie : en charge");
//    display.display();
//    isCharging = M5.Power.isCharging();    
//  }
//  else if (beforeBattery != M5.Power.getBatteryLevel()){
//    display.setCursor(16, 32);
//    display.printf("Batterie : %03d\%", M5.Power.getBatteryLevel());
//    display.display();
//    beforeBattery = M5.Power.getBatteryLevel();
//  }

  
  static bool drawed = false;
  lgfx::touch_point_t tp[3];

  int nums = M5.Display.getTouchRaw(tp, 3);
  if (nums)
  {
    for (int i = 0; i < nums; ++i)
    {
      x = tp[i].y; //x = y because change rotation 90deg invert clock sense
      y = M5.Display.height() - tp[i].x;
      M5.Display.setCursor(128, 128 + i * 24);
      if ((x >= refreshIconX && y >= refreshIconY && x <= refreshIconX + epd_bitmap_refresh_width && y <= refreshIconY + epd_bitmap_refresh_height) && !isClicked){
        isClicked = true;
        M5.Display.printf("clicked X:%03d  Y:%03d", x, y);
        M5.Display.display();
      }
    }
  }
  vTaskDelay(1);  
  delay(1000);
}
