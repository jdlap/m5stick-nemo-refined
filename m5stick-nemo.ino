// Nemo Firmware for the M5 Stack Stick C Plus
// github.com/n0xa | IG: @4x0nn


#include "src/common/common.h"

#include "src/applejuice/applejuice.h"
#include "src/world_ir_codes/world_ir_codes.h"

#include "src/wifispam/wifispam.h"
#include "src/sd/sd.h"
#include "src/portal/portal.h"
#include "NEMOMatrix.h"
#include "src/songs/songs.h"


#include "src/settings/settings.h"
#include "src/wifi/wifi.h"
#include "src/dimmer/dimmer.h"
#include "src/bluetooth/bluetooth.h"
#include "src/tvbg/tvbg.h"

#include "process_list.h"

extern int rotation;

void bootScreen(){
  // Boot Screen
  #ifdef SONG
  setupSongs();
  #endif
  #ifndef STICK_C
  BITMAP;
  delay(3000);
  #endif
  DISP.fillScreen(BGCOLOR);
  DISP.setTextSize(BIG_TEXT);
  DISP.setCursor(40, 0);
  DISP.println("M5-NEMO");
  DISP.setCursor(10, 30);
  DISP.setTextSize(SMALL_TEXT);
  DISP.printf("%s-%s\n",NEMO_VERSION,platformName);
#if defined(CARDPUTER)
  DISP.println(TXT_INST_NXT);
  DISP.println(TXT_INST_PRV);
  DISP.println(TXT_INST_SEL);
  DISP.println(TXT_INST_HOME);
  delay(1500);
  DISP.println(TXT_INST_PRSS_KEY);
  while(true){
    M5Cardputer.update();
    if (M5Cardputer.Keyboard.isChange()) {
      drawmenu(mmenu, mmenu_size);
      delay(250);
      break;
    }
  }
#else
  DISP.println(TXT_STK_NXT);
  DISP.println(TXT_STK_SEL);
  DISP.println(TXT_STK_HOME);
  delay(3000);
#endif
}


/// ENTRY ///
void setup() {
#if defined(CARDPUTER)
  auto cfg = M5.config();
  M5Cardputer.begin(cfg, true);
#else
  M5.begin();
#endif
#if defined(BACKLIGHT)
  pinMode(BACKLIGHT, OUTPUT); // Backlight analogWrite range ~150 - 255
#endif
  if(check_next_press()){
    clearSettings();
  }
  #if defined(USE_EEPROM)
    EEPROM.begin(EEPROM_SIZE);
    Serial.printf("EEPROM 0 - Rotation:   %d\n", EEPROM.read(0));
    Serial.printf("EEPROM 1 - Dim Time:   %d\n", EEPROM.read(1));
    Serial.printf("EEPROM 2 - Brightness: %d\n", EEPROM.read(2));
    Serial.printf("EEPROM 3 - TVBG Reg:   %d\n", EEPROM.read(3));
    Serial.printf("EEPROM 4 - FGColor:    %d\n", EEPROM.read(4));
    Serial.printf("EEPROM 5 - BGColor:    %d\n", EEPROM.read(5));
    if(EEPROM.read(0) > 3 || EEPROM.read(1) > 240 || EEPROM.read(2) > 100 || EEPROM.read(3) > 1 || EEPROM.read(4) > 19 || EEPROM.read(5) > 19) {
      // Assume out-of-bounds settings are a fresh/corrupt EEPROM and write defaults for everything
      Serial.println("EEPROM likely not properly configured. Writing defaults.");
      #if defined(CARDPUTER)
      EEPROM.write(0, 1);    // Right rotation for cardputer
      #else
      EEPROM.write(0, 3);    // Left rotation
      #endif
      EEPROM.write(1, 15);   // 15 second auto dim time
      EEPROM.write(2, 100);  // 100% brightness
      EEPROM.write(3, 0);    // TVBG NA Region
      EEPROM.write(4, 11);   // FGColor Green
      EEPROM.write(5, 1);    // BGcolor Black
      EEPROM.commit();
    }
    rotation = EEPROM.read(0);
    screen_dim_time = EEPROM.read(1);
    brightness = EEPROM.read(2);
    region = EEPROM.read(3);
    setcolor(true, EEPROM.read(4));
    setcolor(false, EEPROM.read(5));
  #endif
  getSSID();
  
  // Pin setup
#if defined(M5LED)
  pinMode(M5LED, OUTPUT);
  digitalWrite(M5LED, M5LED_OFF); //LEDOFF
#endif
#if defined(IRLED)
  pinMode(IRLED, OUTPUT);
  digitalWrite(IRLED, M5LED_OFF); //LEDOFF
#endif
#if !defined(KB)
  pinMode(M5_BUTTON_HOME, INPUT);
  pinMode(M5_BUTTON_RST, INPUT);
#endif
#if defined(M5_BUTTON_MENU)
  pinMode(M5_BUTTON_MENU, INPUT);
#endif
  // Random seed
  randomSeed(analogRead(0));

  // Create the BLE Server
  BLEDevice::init("");
  BLEServer *pServer = BLEDevice::createServer();
  pAdvertising = pServer->getAdvertising();
  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();

  // Nemo Portal Init
  setupSdCard();
  bootTime = lastActivity = millis();

  screenBrightness(brightness);
  dimtimer();
  DISP.setRotation(rotation);
  DISP.setTextColor(FGCOLOR, BGCOLOR);
  bootScreen();
  
  setupProcArray();
  
}


void loop() {
  // This is the code to handle running the main loops
  // Background processes
  switcher_button_proc();
  screen_dim_proc();
  check_menu_press();
  
  // Switcher
  if (isSwitching) {
    isSwitching = false;
    Serial.printf("Switching To Task: %d\n", current_proc);
    processArray[current_proc].setup_func();
  }

  processArray[current_proc].loop_func();
}
