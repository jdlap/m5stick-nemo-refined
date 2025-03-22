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

extern int rotation;

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


/// BATTERY INFO ///

#if defined(PWRMGMT)
  int old_battery = 0;

  void battery_drawmenu(int battery) {
    DISP.setTextSize(SMALL_TEXT);
    DISP.fillScreen(BGCOLOR);
    DISP.setCursor(0, 8, 1);
    DISP.print(TXT_BATT);
    DISP.print(battery);
    DISP.println("%");
    DISP.println(TXT_EXIT);
  }

  int get_battery_voltage() {
    return M5.Power.getBatteryLevel();
  }

  void battery_setup() {
    int battery = get_battery_voltage();
    battery_drawmenu(battery);
    delay(500); // Prevent switching after menu loads up
  }

  void battery_loop() {
    delay(300);
    int battery = get_battery_voltage();

    if (battery != old_battery){
      battery_drawmenu(battery);
    }
    if (check_select_press()) {
      isSwitching = true;
      current_proc = 1;
    }
    old_battery = battery;
  }
#endif

#ifdef AXP
  int old_battery=0;
  void battery_drawmenu(int battery, int b, int c) {
    DISP.setTextSize(SMALL_TEXT);
    DISP.fillScreen(BGCOLOR);
    DISP.setCursor(0, 8, 1);
    DISP.print(TXT_BATT);
    DISP.print(battery);
    DISP.println("%");
    DISP.print("DeltaB: ");
    DISP.println(b);
    DISP.print("DeltaC: ");
    DISP.println(c);
    DISP.println("");
    DISP.println(TXT_EXIT);
  }

  void battery_setup() {
    rstOverride = false;
    float c = M5.Axp.GetVapsData() * 1.4 / 1000;
    float b = M5.Axp.GetVbatData() * 1.1 / 1000;
    int battery = ((b - 3.0) / 1.2) * 100;
    battery_drawmenu(battery, b, c);
    delay(500); // Prevent switching after menu loads up
  }

  void battery_loop() {
    delay(300);
    float c = M5.Axp.GetVapsData() * 1.4 / 1000;
    float b = M5.Axp.GetVbatData() * 1.1 / 1000;
    int battery = ((b - 3.0) / 1.2) * 100;
    if (battery != old_battery){
      battery_drawmenu(battery, b, c);
    }
    if (check_select_press()) {
      rstOverride = false;
      isSwitching = true;
      current_proc = 1;
    }
    old_battery = battery;
  }
#endif // AXP

#if defined(CARDPUTER)
  /// BATTERY INFO ///
  uint8_t oldBattery = 0;
  void battery_drawmenu(uint8_t battery) {
    // Battery bar color definition
    uint16_t batteryBarColor = BLUE;
    if(battery < 20) {
      batteryBarColor = RED;
    } else if(battery < 60) {
      batteryBarColor = ORANGE;
    } else {
      batteryBarColor = GREEN;
    }
    // Battery bar
    DISP.fillScreen(BGCOLOR);
    DISP.drawRect(10, 10, 220, 100, batteryBarColor);
    DISP.fillRect(10, 10, (220 * battery / 100), 100, batteryBarColor);
    // Battery percentage
    DISP.setTextColor(WHITE);
    DISP.setTextSize(BIG_TEXT);
    DISP.setCursor(80, 45, 1);
    DISP.print(battery);
    DISP.println("%");
    // Exit text
    DISP.setCursor(10, 120, 1);
    DISP.setTextSize(TINY_TEXT);
    DISP.println(TXT_EXIT);
    DISP.setTextColor(FGCOLOR, BGCOLOR);
  }

  void battery_setup() {
    rstOverride = false;
    pinMode(VBAT_PIN, INPUT);
    uint8_t battery = ((((analogRead(VBAT_PIN)) - 1842) * 100) / 738);
    battery_drawmenu(battery);
    delay(500); // Prevent switching after menu loads up
    /*
      Used minimum 3,0V and maximum 4,2V for battery. So it may show wrong values. Needs testing.
      It only shows decent values when disconnected from charger, due to HW limitations.
      Equation: Bat% = ((Vadc - 1842) / (2580 - 1842)) * 100. Where: 4,2V = 2580, 3,0V = 1842.
    */
  }

  void battery_loop() {
    // Read 30 battery values to calculate the average (avoiding unprecise and close values)
    uint16_t batteryValues = 0;
    for(uint8_t i = 0; i < 30; i++) { // 30 iterations X 100ms = 3 seconds for each refresh
      delay(100);
      batteryValues += ((((analogRead(VBAT_PIN)) - 1842) * 100) / 738);
      M5Cardputer.update();
      if(M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) { // If any key is pressed
        rstOverride = false;
        isSwitching = true;
        current_proc = 1;
        break;
      }
    }

    if(!isSwitching) { // If is not switching, calculate battery average
      uint8_t battery = batteryValues / 30; // Iteration times
      Serial.printf("Battery Level: %d\n", battery);
      if(battery != oldBattery) { // Only draw a new screen if value is different
        oldBattery = battery;
        battery_drawmenu(battery);
      }
    }
  }
#endif // Cardputer


/// CLOCK ///
/// TIMESET ///

#if defined(RTC)
  void clock_setup() {
    DISP.fillScreen(BGCOLOR);
    DISP.setTextSize(1);
  }

  void clock_loop() {
    DISP.setCursor(10, 40, 7);
    #if defined(STICK_C_PLUS2)
      auto dt = StickCP2.Rtc.getDateTime();
      DISP.printf("%02d:%02d:%02d\n", dt.time.hours, dt.time.minutes, dt.time.seconds);
    #else
      M5.Rtc.GetBm8563Time();
      DISP.printf("%02d:%02d:%02d\n", M5.Rtc.Hour, M5.Rtc.Minute, M5.Rtc.Second);
    #endif
    check_select_press();
  }

  /// TIME SETTING ///
  void timeset_setup() {
    rstOverride = true;
    DISP.fillScreen(BGCOLOR);
    DISP.setCursor(0, 0);
    DISP.println(TXT_SET_HOUR);
    delay(2000);
  }

  void timeset_loop() {
  #if defined(STICK_C_PLUS2)
    auto dt = StickCP2.Rtc.getDateTime();
    cursor = dt.time.hours;
  #else
    M5.Rtc.GetBm8563Time();
    cursor = M5.Rtc.Hour;
  #endif
    number_drawmenu(24);
    while(digitalRead(M5_BUTTON_HOME) == HIGH) {
      if (check_next_press()) {
        cursor++;
        cursor = cursor % 24 ;
        number_drawmenu(24);
        delay(100);
      }
    }
    int hour = cursor;
    DISP.fillScreen(BGCOLOR);
    DISP.setCursor(0, 0);
    DISP.println(TXT_SET_MIN);
    delay(2000);
    #if defined(STICK_C_PLUS2)
      cursor = dt.time.minutes;
    #else
      cursor = M5.Rtc.Minute;
    #endif
    number_drawmenu(60);
    while(digitalRead(M5_BUTTON_HOME) == HIGH) {
      if (check_next_press()) {
        cursor++;
        cursor = cursor % 60 ;
        number_drawmenu(60);
        delay(100);
      }
    }
    int minute = cursor;
    DISP.fillScreen(BGCOLOR);
    DISP.setCursor(0, 0);
    #if defined(STICK_C_PLUS2)
       StickCP2.Rtc.setDateTime( { { dt.date.year, dt.date.month, dt.date.date }, { hour, minute, 0 } } );
    #else
      RTC_TimeTypeDef TimeStruct;
      TimeStruct.Hours   = hour;
      TimeStruct.Minutes = minute;
      TimeStruct.Seconds = 0;
      M5.Rtc.SetTime(&TimeStruct);
    #endif
    DISP.printf("Setting Time:\n%02d:%02d:00",hour,minute);
    delay(2000);
    rstOverride = false;
    isSwitching = true;
    current_proc = 0;
  }
#endif // RTC


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

/// NEMO PORTAL

void portal_setup(){
  setupWiFi();
  setupWebServer();
  portal_active = true;
  cursor = 0;
  rstOverride = true;
  printHomeToScreen();
  #if defined(DEAUTHER)                                                                      // DEAUTH
  memcpy(deauth_frame, deauth_frame_default, sizeof(deauth_frame_default));                  // DEAUTH
  wsl_bypasser_send_deauth_frame(&ap_record, channel);                                       // DEAUTH  CREATE FRAME
  #endif                                                                                     // DEAUTH
  delay(500); // Prevent switching after menu loads up
}

void portal_loop(){
  if ((millis() - lastTick) > PortalTickTimer) {
    lastTick = millis();
    if (totalCapturedCredentials != previousTotalCapturedCredentials) {
      previousTotalCapturedCredentials = totalCapturedCredentials;
      printHomeToScreen();
    }
  }
  if (clone_flg==true) {
    #if defined(DEAUTHER)
      if (target_deauth_flg) {
        if (target_deauth == true) {                                                                 // DEAUTH
          if (deauth_tick==35) {                                                                     // 35 is +-100ms   (Add delay to attack, without reflection on portal)
            wsl_bypasser_send_raw_frame(deauth_frame, sizeof(deauth_frame_default));                 // DEAUTH   SEND FRAME
            deauth_tick=0;
          } else { 
            deauth_tick=deauth_tick+1; 
          }
          DISP.setTextSize(SMALL_TEXT);                                                              // DEAUTH
          DISP.setTextColor(TFT_RED, BGCOLOR);                                                       // DEAUTH
          DISP.setCursor(1, 115);                                                                    // DEAUTH
          DISP.println(TXT_DEAUTH_STOP);                                                             // DEAUTH
          DISP.setTextColor(FGCOLOR, BGCOLOR);                                                       // DEAUTH
        } else{                                                                                      // DEAUTH
          DISP.setTextSize(SMALL_TEXT);                                                              // DEAUTH
          DISP.setTextColor(TFT_RED, BGCOLOR);                                                       // DEAUTH
          DISP.setCursor(1, 115);                                                                    // DEAUTH
          DISP.println(TXT_DEAUTH_START);                                                            // DEAUTH
          DISP.setTextColor(FGCOLOR, BGCOLOR);                                                       // DEAUTH
        }                                                                                            // DEAUTH
        if (check_select_press()){                                                                    // DEAUTH
          target_deauth = !target_deauth;                                                             // DEAUTH
          delay(500);                                                                                 // DEAUTH
        }
      }
    #endif
  }
  dnsServer.processNextRequest();
  webServer.handleClient();
  
  if (check_next_press()){
    shutdownWebServer();
    portal_active = false;
    target_deauth_flg = false;                                                                     // DEAUTH
    target_deauth = false;                                                                         // DEAUTH
    clone_flg = false;                                                                             // DEAUTH
    current_proc = 12;
    delay(500);
  }
  check_select_press();
}

struct ProcessPair {
  void (*setup_func)();
  void (*loop_func)();
};

void defaultProc(){
    return;
}

#define MAX_PROC_DEFS 255
ProcessPair processArray[MAX_PROC_DEFS] = {&defaultProc, &defaultProc};


void setupProcArray(){
#if defined(RTC)
  processArray[0] = {&clock_setup, &clock_loop};
#endif
  processArray[1] = {&mmenu_setup, &mmenu_loop};
  processArray[2] = {&smenu_setup, &smenu_loop};
#if defined(RTC)
  processArray[3] = {&timeset_setup, &timeset_loop};
#endif
  processArray[4] = {&dmenu_setup, &dmenu_loop};
  processArray[5] = {&tvbgone_setup, &tvbgone_loop};
#if defined(AXP) || defined(PWRMGMT)
  processArray[6] = {&battery_setup, &battery_loop};
#endif
#if defined(CARDPUTER)
  processArray[6] = {&battery_setup, &battery_loop};
#endif
#if defined(ROTATION)
  processArray[7] = {&rmenu_setup, &rmenu_loop};
#endif
  processArray[8] = {&aj_setup, &aj_loop};
  processArray[9] = {&aj_adv_setup, &aj_adv};
  processArray[10] = {&credits_setup, &credits_loop};
  processArray[11] = {&wifispam_setup, &wifispam_loop};
  processArray[12] = {&wsmenu_setup, &wsmenu_loop};
  processArray[13] = {&tvbgmenu_setup, &tvbgmenu_loop};
  processArray[14] = {&wscan_setup, &wscan_loop};
  processArray[15] = {&wscan_result_setup, &wscan_result_loop};
  processArray[16] = {&btmenu_setup, &btmenu_loop};
  processArray[17] = {&btmaelstrom_setup, &btmaelstrom_loop};
  processArray[18] = {&qrmenu_setup, &qrmenu_loop};
  processArray[19] = {&portal_setup, &portal_loop};
  processArray[20] = {&wsAmenu_setup, &wsAmenu_loop};
#if defined(DEAUTHER)
  processArray[21] = {&deauth_setup, &deauth_loop};
#endif
  processArray[22] = {&color_setup, &color_loop};
  processArray[23] = {&theme_setup, &theme_loop};
#if defined(SDCARD)                                                // SDCARD M5Stick
  #ifndef CARDPUTER                                                // SDCARD M5Stick
    processArray[91] = {&defaultProc, &ToggleSDCard};                                         // SDCARD M5Stick
  #endif                                                           // SDCARD M5Stick
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
