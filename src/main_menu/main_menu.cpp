
#include "Arduino.h"
#include "../common/common.h"
#include "main_menu.h"



/// MAIN MENU ///
MENU mmenu[] = {
#if defined(RTC)
  { TXT_CLOCK, 0},
#endif
  { "TV-B-Gone", 13}, // We jump to the region menu first
  { "Bluetooth", 16},
  { "WiFi", 12},
  { "QR Codes", 18},
  { TXT_SETTINGS, 2},
};
int mmenu_size = sizeof(mmenu) / sizeof(MENU);

void mmenu_setup() {
  cursor = 0;
  rstOverride = true;
  drawmenu(mmenu, mmenu_size);
  delay(500); // Prevent switching after menu loads up
}

void mmenu_loop() {
  if (check_next_press()) {
    cursor++;
    cursor = cursor % mmenu_size;
    drawmenu(mmenu, mmenu_size);
    delay(250);
  }
  if (check_select_press()) {
    rstOverride = false;
    isSwitching = true;
    current_proc = mmenu[cursor].command;
  }
}

