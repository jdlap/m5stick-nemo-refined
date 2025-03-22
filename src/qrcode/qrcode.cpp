
#include "Arduino.h"
#include "../common/common.h"
#include "qrcode.h"

void qrmenu_drawmenu() {
  DISP.setTextSize(SMALL_TEXT);
  DISP.fillScreen(BGCOLOR);
  DISP.setCursor(0, 8, 1);
  for ( int i = 0 ; i < ( sizeof(qrcodes) / sizeof(QRCODE) ) ; i++ ) {
    DISP.print((cursor == i) ? ">" : " ");
    DISP.println(qrcodes[i].name);
  }
}

void qrmenu_setup() {
  cursor = 0;
  rstOverride = true;
  qrmenu_drawmenu();
  delay(500); // Prevent switching after menu loads up
}

void qrmenu_loop() {
  if (check_next_press()) {
    cursor++;
    cursor = cursor % ( sizeof(qrcodes) / sizeof(QRCODE) );
    qrmenu_drawmenu();
    activeQR = false;
    delay(250);
  }
  if (check_select_press()) {
    if (qrcodes[cursor].url.length() < 1){
      current_proc = 1;
      isSwitching = true;
    }else if ( activeQR == false ) {
      activeQR = true;
      DISP.fillScreen(WHITE);
      DISP.qrcode(qrcodes[cursor].url, (DISP.width() - DISP.height()) / 2, 0, DISP.height(), 5);
      delay(500);
    } else {
      isSwitching = true;
      activeQR = false;
      delay(250);
    }
  }
}

