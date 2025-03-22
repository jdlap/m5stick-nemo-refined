
#include "Arduino.h"
#include "dimmer.h"

void screen_dim_proc() {
  if(screen_dim_time > 0){
    if (screen_dim_dimmed == false) {
      if (uptime() == screen_dim_current || (uptime() + 1) == screen_dim_current || (uptime() + 2) == screen_dim_current) {
        screenBrightness(0);
        screen_dim_dimmed = true;
      }
    }
  }
}

/// Dimmer MENU ///
MENU dmenu[] = {
  { TXT_BACK, screen_dim_time},
  { TXT_NEVER, 0},
  { ("5 " TXT_SEC), 5},
  { ("10 " TXT_SEC), 10},
  { ("15 " TXT_SEC), 15},
  { ("30 " TXT_SEC), 30},
  { ("60 " TXT_SEC), 60},
  { ("120 " TXT_MIN), 120},
  { ("240 " TXT_MIN), 240},
};
int dmenu_size = sizeof(dmenu) / sizeof(MENU);

void dmenu_setup() {
  DISP.fillScreen(BGCOLOR);
  DISP.setCursor(0, 0);
  DISP.println(String(TXT_AUTO_DIM));
  delay(1000);
  cursor = 0;
  rstOverride = true;
  drawmenu(dmenu, dmenu_size);
  delay(500); // Prevent switching after menu loads up
}

void dmenu_loop() {
  if (check_next_press()) {
    cursor++;
    cursor = cursor % dmenu_size;
    drawmenu(dmenu, dmenu_size);
    delay(250);
  }
  if (check_select_press()) {
    screen_dim_time = dmenu[cursor].command;
    #if defined(USE_EEPROM)
      EEPROM.write(1, screen_dim_time);
      EEPROM.commit();
    #endif
    DISP.fillScreen(BGCOLOR);
    DISP.setCursor(0, 0);
    DISP.println(String(TXT_SET_BRIGHT));
    delay(1000);
    cursor = brightness / 10;
    number_drawmenu(11);
    while( !check_select_press()) {
      if (check_next_press()) {
        cursor++;
        cursor = cursor % 11 ;
        number_drawmenu(11);
        screenBrightness(10 * cursor);
        delay(250);
       }
    }
    screenBrightness(10 * cursor);
    #if defined(USE_EEPROM)
      EEPROM.write(2, 10 * cursor);
      EEPROM.commit();
    #endif
    rstOverride = false;
    isSwitching = true;
    current_proc = 2;
  }
}