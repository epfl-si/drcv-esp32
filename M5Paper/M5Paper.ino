
#include <M5GFX.h>

#include "pic_scenario.h" 

M5GFX display;

int x = 0;
int y = 0;
int withBeforeRotation = 0;
int heightBeforeRotation = 0;
bool isClicked = false;
int refreshIconX = 0;
int refreshIconY = 0;

void setup(void)
{
  Serial.begin(115200);
  display.begin();

  display.init();
  display.setFont(&fonts::Font4);

  if (!display.touch())
  {
    display.setTextDatum(textdatum_t::middle_center);
    display.drawString("Touch not found.", display.width() / 2, display.height() / 2);
  }

  if (display.width() < display.height())
  {
    display.setRotation(display.getRotation() ^ 1);
  }

  Serial.print("height");
  Serial.print(display.height());
  Serial.print("width");
  Serial.print(display.width());

  display.setEpdMode(epd_mode_t::epd_fastest);
  display.startWrite();

  display.clear();
  display.clearDisplay();

  refreshIconX = display.width() - epd_bitmap_refresh_width;
  refreshIconY = display.height() - epd_bitmap_refresh_height;

  //Write header
  display.drawBitmap(0, 0, EPFL_INN011_header, EPFL_INN011_header_width, EPFL_INN011_header_height, WHITE, BLACK); //x, y, bitmap, width, height, bg, fg

  //Write footer
  display.fillRect(0, display.height() - epd_bitmap_refresh_height, display.width(), display.height(), BLACK);
  display.drawBitmap(display.width() - epd_bitmap_refresh_width, display.height() - epd_bitmap_refresh_height, epd_bitmap_refresh, epd_bitmap_refresh_width, epd_bitmap_refresh_height, BLACK, WHITE); //x, y, bitmap, width, height, bg, fg
  
  display.display(); // Update display
}

void loop(void)
{
  static bool drawed = false;
  lgfx::touch_point_t tp[3];

  int nums = display.getTouchRaw(tp, 3);
  if (nums)
  {
    for (int i = 0; i < nums; ++i)
    {
      x = tp[i].y; //x = y because change rotation 90deg invert clock sense
      y = display.height() - tp[i].x;
      display.setCursor(16, 16 + i * 24);
      if ((x >= refreshIconX && y >= refreshIconY && x <= refreshIconX + epd_bitmap_refresh_width && y <= refreshIconY + epd_bitmap_refresh_height) && !isClicked){
        isClicked = true;
        display.printf("clicked X:%03d  Y:%03d", x, y);
        display.display();
      }
    }
  }
  vTaskDelay(1);  
}
