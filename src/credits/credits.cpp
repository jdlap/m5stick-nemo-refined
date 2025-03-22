
#include "Arduino.h"
#include "../common/common.h"
#include "credits.h"


/// CREDITS ///
void credits_setup(){
  DISP.fillScreen(WHITE);
  DISP.qrcode("https://github.com/n0xa/m5stick-nemo", 145, 22, 100, 5);
  DISP.setTextColor(BLACK, WHITE);
  DISP.setTextSize(MEDIUM_TEXT);
  DISP.setCursor(0, 10);
  DISP.print(" M5-NEMO\n");
  DISP.setTextSize(SMALL_TEXT);
  DISP.printf("  %s\n",NEMO_VERSION);
  DISP.println(" For M5Stack");
  DISP.printf(" %s\n\n", platformName);
  DISP.println("Contributors:");
  DISP.setCursor(155, 5);
  DISP.println("GitHub");
  DISP.setCursor(155, 17);
  DISP.println("Source:");
  delay(250);
  cursor = 0;
  advtime = 0;
}

void credits_loop(){
  if(millis() > advtime){
    DISP.setTextColor(BLACK, WHITE);  
    DISP.setCursor(0, 115);
    DISP.println("                   ");
    DISP.setCursor(0, 115);
    DISP.println(contributors[cursor]);
    cursor++;  
    cursor = cursor % (sizeof(contributors)/sizeof(contributors[0]));
    DISP.setTextColor(FGCOLOR, BGCOLOR);
    advtime=millis() + 2000;
  }
}
