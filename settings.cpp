
#include "Arduino.h"
#include "settings.h"







/// SETTINGS MENU ///

MENU smenu[] = {
  { TXT_BACK, 1},
#if defined(AXP) || defined(PWRMGMT)
  { TXT_BATT_INFO, 6},
#endif
#if defined(CARDPUTER)
  { TXT_BATT_INFO, 6},
#endif
  { TXT_BRIGHT, 4},
#if defined(RTC)
  { TXT_SET_CLOCK, 3},
#endif
#if defined(ROTATION)
  { TXT_ROTATION, 7},
#endif
#if defined(SDCARD)
  #ifndef CARDPUTER
    { TXT_SDCARD, 97},
  #endif
#endif
  { TXT_THEME, 23},
  { TXT_ABOUT, 10},
  { TXT_REBOOT, 98},
#if defined(USE_EEPROM)
  { TXT_CLR_SETTINGS, 99},
#endif
};


int smenu_size = sizeof(smenu) / sizeof (MENU);

void smenu_setup() {
  cursor = 0;
  rstOverride = true;
  drawmenu(smenu, smenu_size);
  delay(500); // Prevent switching after menu loads up
}

void clearSettings(){
  #if defined(USE_EEPROM)
  for(int i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(i, 255);
  }
  EEPROM.commit();
  #endif
  screenBrightness(100);
  DISP.fillScreen(BLUE);
  DISP.setTextSize(BIG_TEXT);
  DISP.setRotation(1);
  DISP.setTextColor(BLUE, WHITE);
  DISP.setCursor(40, 0);
  DISP.println("M5-NEMO");
  DISP.setTextColor(WHITE, BLUE);
  DISP.setTextSize(SMALL_TEXT);
  DISP.println(TXT_CLRING_SETTINGS);
  delay(5000);
  ESP.restart();
}

void smenu_loop() {
  if (check_next_press()) {
    cursor++;
    cursor = cursor % smenu_size;
    drawmenu(smenu, smenu_size);
    delay(250);
  }
  if (check_select_press()) {
    rstOverride = false;
    isSwitching = true;
    if(smenu[cursor].command == 98){
      ESP.restart();
    }
    if(smenu[cursor].command == 99){
      clearSettings();
    }
    current_proc = smenu[cursor].command;
  }
}


//// Color Menu ////
MENU cmenu[] = {
  { TXT_BACK, 0},
  { TXT_BLACK, 1},
  { TXT_NAVY, 2},
  { TXT_DARKGREEN, 3},
  { TXT_DARKCYAN, 4},
  { TXT_MAROON, 5},
  { TXT_PURPLE, 6},
  { TXT_OLIVE, 7},
  { TXT_LIGHTGREY, 8},
  { TXT_DARKGREY, 9},
  { TXT_BLUE, 10},
  { TXT_GREEN, 11},
  { TXT_CYAN, 12},
  { TXT_RED, 13},
  { TXT_MAGENTA, 14},
  { TXT_YELLOW, 15},
  { TXT_WHITE, 16},
  { TXT_ORANGE, 17},
  { TXT_GREENYELLOW, 18},
  { TXT_PINK, 19},
};
int cmenu_size = sizeof(cmenu) / sizeof (MENU);

void setcolor(bool fg, int col){
  uint16_t color = 0x0000;
  switch (col){
    case 1:
      color=0x0000;
      break; 
    case 2:
      color=0x000F;
      break;
    case 3:
      color=0x03E0;
      break;
    case 4:
      color=0x03EF;
      break;
    case 5:
      color=0x7800;
      break;
    case 6:
      color=0x780F;
      break;
    case 7:
      color=0x7BE0;
      break;
    case 8:
      color=0xC618;
      break;
    case 9:
      color=0x7BEF;
      break;
    case 10:
      color=0x001F;
      break;
    case 11:
      color=0x07E0;
      break;
    case 12:
      color=0x07FF;
      break;
    case 13:
      color=0xF800;
      break;
    case 14:
      color=0xF81F;
      break;
    case 15:
      color=0xFFE0;
      break;
    case 16:
      color=0xFFFF;
      break;
    case 17:
      color=0xFDA0;
      break;
    case 18:
      color=0xB7E0;
      break;
    case 19:
      color=0xFC9F;
      break;
  }
  if(fg){
    FGCOLOR=color;
  }else{
    BGCOLOR=color;
  }
  if(FGCOLOR == BGCOLOR){
    BGCOLOR = FGCOLOR ^ 0xFFFF;
  }
  DISP.setTextColor(FGCOLOR, BGCOLOR);
}

void color_setup() {
  DISP.fillScreen(BGCOLOR);
  DISP.setCursor(0, 0);
  DISP.println(String(TXT_SET_FGCOLOR));
  cursor = 0;
  #if defined(USE_EEPROM)
    cursor=EEPROM.read(4); // get current fg color
  #endif
  rstOverride = true;
  delay(1000);  
  drawmenu(cmenu, cmenu_size);
}

void color_loop() {
  if (check_next_press()) {
    setcolor(EEPROM.read(5), false);
    cursor++;
    cursor = cursor % cmenu_size;
    setcolor(true, cursor);
    drawmenu(cmenu, cmenu_size);
    delay(250);
  }
  if (check_select_press()) {
    #if defined(USE_EEPROM)
      Serial.printf("EEPROM WRITE (4) FGCOLOR: %d\n", cursor);
      EEPROM.write(4, cursor);
      EEPROM.commit();
      cursor=EEPROM.read(5); // get current bg color
    #endif
    DISP.fillScreen(BGCOLOR);
    DISP.setCursor(0, 0);
    DISP.println(String(TXT_SET_BGCOLOR));
    delay(1000);
    setcolor(false, cursor);
    drawmenu(cmenu, cmenu_size);
    while( !check_select_press()) {
      if (check_next_press()) {
        cursor++;
        cursor = cursor % cmenu_size ;
        setcolor(false, cursor);
        drawmenu(cmenu, cmenu_size);
        delay(250);
       }
    }
    #if defined(USE_EEPROM)
      Serial.printf("EEPROM WRITE (5) BGCOLOR: %d\n", cursor);
      EEPROM.write(5, cursor);
      EEPROM.commit();
    #endif
    rstOverride = false;
    isSwitching = true;
    current_proc = 2;
  }
}

//// Theme Menu ////

MENU thmenu[] = {
  { TXT_BACK, 0},
  { "Nemo", 1},
  { "Tux", 2},
  { "Bill", 3},
  { "Steve", 4},
  { "Lilac", 5},
  { "Contrast", 6},
  { "NightShift", 7},
  { "Camo", 8},
  { "BubbleGum", 9},
  { TXT_COLOR, 99},
};
int thmenu_size = sizeof(thmenu) / sizeof (MENU);

void theme_setup() {
  DISP.fillScreen(BGCOLOR);
  DISP.setCursor(0, 0);
  DISP.println(String(TXT_THEME));
  cursor = 0;
  rstOverride = true;
  delay(1000);  
  drawmenu(thmenu, thmenu_size);
}

int BG=0;
int FG=0;

void theme_loop() {
  if (check_next_press()) {
    cursor++;
    cursor = cursor % thmenu_size;
    switch (thmenu[cursor].command){
      case 0:
        FG=11;
        BG=1;
        break;       
      case 1: // Nemo
        FG=11;
        BG=1;
        break;
      case 2: // Tux
        FG=8;
        BG=1;
        break;  
      case 3: // Bill
        FG=16;
        BG=10;
        break;
      case 4: // Steve
        FG=1;
        BG=8;
        break;        
      case 5: // Lilac
        FG=19;
        BG=6;
        break;
      case 6: // Contrast
        FG=16;
        BG=1;
        break;
      case 7: // NightShift
        FG=5;
        BG=1;
         break;
      case 8: // Camo
        FG=1;
        BG=7;
        break;
      case 9: // BubbleGum
        FG=1;
        BG=19;
        break;
      case 99:
        FG=11;
        BG=1;
        break;
     }
    setcolor(true, FG);
    setcolor(false, BG);
    drawmenu(thmenu, thmenu_size);
    delay(250);
  }
  if (check_select_press()) {
    switch (thmenu[cursor].command){
      case 99:
        rstOverride = false;
        isSwitching = true;
        current_proc = 22;
        break;
      case 0:
        #if defined(USE_EEPROM)
          setcolor(true, EEPROM.read(4));
          setcolor(false, EEPROM.read(5));
        #endif
        rstOverride = false;
        isSwitching = true;
        current_proc = 2;
        break;
      default:
        #if defined(USE_EEPROM)
          Serial.printf("EEPROM WRITE (4) FGCOLOR: %d\n", FG);
          EEPROM.write(4, FG);
          Serial.printf("EEPROM WRITE (5) BGCOLOR: %d\n", BG);
          EEPROM.write(5, BG);
          EEPROM.commit();
        #endif
        rstOverride = false;
        isSwitching = true;
        current_proc = 2;
    }
  }
}

int rotation = 1;
#if defined(ROTATION)
  /// Rotation MENU ///
  MENU rmenu[] = {
    { TXT_BACK, rotation},
    { TXT_RIGHT, 1},
    { TXT_LEFT, 3},
  };
  int rmenu_size = sizeof(rmenu) / sizeof (MENU);

  void rmenu_setup() {
    cursor = 0;
    rstOverride = true;
    drawmenu(rmenu, rmenu_size);
    delay(500); // Prevent switching after menu loads up
  }

  void rmenu_loop() {
    if (check_next_press()) {
      cursor++;
      cursor = cursor % rmenu_size;
      drawmenu(rmenu, rmenu_size);
      delay(250);
    }
    if (check_select_press()) {
      rstOverride = false;
      isSwitching = true;
      rotation = rmenu[cursor].command;
      DISP.setRotation(rotation);
      #if defined(USE_EEPROM)
        EEPROM.write(0, rotation);
        EEPROM.commit();
      #endif
      current_proc = 2;
    }
  }
#endif //ROTATION