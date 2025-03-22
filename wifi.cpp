
#include "Arduino.h"
#include "wifispam.h"
#include "wifi.h"
#include "portal.h"

/// WiFiSPAM ///
void wifispam_setup() {
  // create empty SSID
  for (int i = 0; i < 32; i++)
    emptySSID[i] = ' ';
  // for random generator
  randomSeed(1);

  // set packetSize
  packetSize = sizeof(beaconPacket);
  if (wpa2) {
    beaconPacket[34] = 0x31;
  } else {
    beaconPacket[34] = 0x21;
    packetSize -= 26;
  }

  //change WiFi mode
  WiFi.mode(WIFI_MODE_STA);

  // set channel
  esp_wifi_set_channel(channels[0], WIFI_SECOND_CHAN_NONE);

  DISP.fillScreen(BGCOLOR);
  DISP.setTextSize(BIG_TEXT);
  DISP.setCursor(0, 0);
  DISP.println(TXT_WF_SP);
  delay(1000);
  DISP.setTextSize(TINY_TEXT);
  DISP.fillScreen(BGCOLOR);
  DISP.setCursor(0, 0);
  DISP.print(TXT_WF_SP);
    int ct = 0;
    const char *str;
    switch(spamtype) {
    case 1:
      for(str = funnyssids; *str; ++str) ct += *str == '\n';
      DISP.printf(" - %d SSIDs:\n", ct);
      DISP.print(funnyssids);
      break;
    case 2:
      for(str = rickrollssids; *str; ++str) ct += *str == '\n';
      DISP.printf(" - %d SSIDs:\n", ct);
      DISP.print(rickrollssids);
      break;
    case 3:
      DISP.printf(TXT_RND_SSID, ct);
      break;
  }
  DISP.setTextSize(SMALL_TEXT);
  current_proc = 11;
}

void wifispam_loop() {
  int i = 0;
  int len = 0;
#if defined(M5LED)
  digitalWrite(M5LED, M5LED_ON); //LED ON on Stick C Plus
  delay(1);
  digitalWrite(M5LED, M5LED_OFF); //LED OFF on Stick C Plus
#endif
  switch(spamtype) {
    case 1:
      len = sizeof(funnyssids);
      while(i < len){
        i++;
      }
      beaconSpamList(funnyssids);
      break;
    case 2:
      len = sizeof(rickrollssids);
      while(i < len){
        i++;
      }
      beaconSpamList(rickrollssids);
      break;
    case 3:
      char* randoms = randomSSID();
      len = sizeof(randoms);
      while(i < len){
        i++;
      }
      beaconSpamList(randoms);
      break;
  }
}

/// WIFI MENU ///
MENU wsmenu[] = {
  { TXT_BACK, 5},
  { TXT_WF_SCAN, 0},
  { TXT_WF_SPAM_FUN, 1},
  { TXT_WF_SPAM_RR, 2},
  { TXT_WF_SPAM_RND, 3},
  { "NEMO Portal", 4},
};
int wsmenu_size = sizeof(wsmenu) / sizeof (MENU);

void wsmenu_setup() {
  cursor = 0;
  rstOverride = true;
  drawmenu(wsmenu, wsmenu_size);
  delay(500); // Prevent switching after menu loads up
}

void wsmenu_loop() {
  if (check_next_press()) {
    cursor++;
    cursor = cursor % wsmenu_size;
    drawmenu(wsmenu, wsmenu_size);
    delay(250);
  }
  if (check_select_press()) {
    int option = wsmenu[cursor].command;
    rstOverride = false;
    current_proc = 11;
    isSwitching = true;
    switch(option) {
      case 0:
        rstOverride = false;
        isSwitching = true;
        current_proc = 14;
        break;
      case 1:
        spamtype = 1;
        break;
      case 2:
        spamtype = 2;
        break;
      case 3:
        spamtype = 3;
        break;
      case 4:
        current_proc = 19;
        break;
      case 5:
        current_proc = 1;
        break;
    }
  }
}


int wscan_result_cursor = 0;
void wscan_drawmenu() {
  char ssid[19];
  DISP.setTextSize(SMALL_TEXT);
  DISP.fillScreen(BGCOLOR);
  DISP.setCursor(0, 0);
  // scrolling menu
  if (cursor > 4) {
    for ( int i = 0 + (cursor - 4) ; i < wifict ; i++ ) {
      if(cursor == i){
        DISP.setTextColor(BGCOLOR, FGCOLOR);
      }
      DISP.print(" ");
      DISP.println(WiFi.SSID(i).substring(0,19));
      DISP.setTextColor(FGCOLOR, BGCOLOR);
      wscan_result_cursor = i;
    }
  } else {
    for ( int i = 0 ; i < wifict ; i++ ) {
      if(cursor == i){
        DISP.setTextColor(BGCOLOR, FGCOLOR);
      }
      DISP.print(" ");
      DISP.println(WiFi.SSID(i).substring(0,19));
      DISP.setTextColor(FGCOLOR, BGCOLOR);
      wscan_result_cursor = i;
    }
  }
  if(cursor == wifict){
    DISP.setTextColor(BGCOLOR, FGCOLOR);
  }
  DISP.println(TXT_WF_RESCAN);
  DISP.setTextColor(FGCOLOR, BGCOLOR);
  if(cursor == wifict + 1){
    DISP.setTextColor(BGCOLOR, FGCOLOR);
  }
  DISP.println(String(TXT_BACK));
  DISP.setTextColor(FGCOLOR, BGCOLOR);
}

void wscan_result_setup() {
  cursor = 0;
  rstOverride = true;
  wscan_drawmenu();
  delay(500); // Prevent switching after menu loads up
}

//TODO maybe change the i to the last cursor value from wscan_drawmenu()
//extern int i;
void wscan_result_loop(){
  if (check_next_press()) {
    cursor++;
    cursor = cursor % ( wifict + 2);
    wscan_drawmenu();
    delay(250);
  }
  if (check_select_press()) {
    delay(250);
    if(cursor == wifict){
      rstOverride = false;
      current_proc = 14;
    }
    if(cursor == wifict + 1){
      rstOverride = false;
      isSwitching = true;
      current_proc = 12;
    }
    String encryptType = "";
    switch (WiFi.encryptionType(cursor)) {
    case 1:
      encryptType = "WEP";
      break;
    case 2:
      encryptType = "WPA/PSK/TKIP";
      break;
    case 3:
      encryptType = "WPA/PSK/CCMP";
      break;
    case 4:
      encryptType = "WPA2/PSK/Mixed/CCMP";
      break;
    case 8:
      encryptType = "WPA/WPA2/PSK";
      break ;
    case 0:
      encryptType = TXT_WF_OPEN;
      break ;
    }
    DISP.fillScreen(BGCOLOR);
    DISP.setCursor(0, 0);
    DISP.setTextColor(BGCOLOR, FGCOLOR);
    if(WiFi.SSID(cursor).length() > 12){
      DISP.setTextSize(SMALL_TEXT);
    }else if(WiFi.SSID(cursor).length() > 20){
      DISP.setTextSize(TINY_TEXT);
    }else{
      DISP.setTextSize(MEDIUM_TEXT);
    }
    DISP.println(WiFi.SSID(cursor));
    DISP.setTextColor(FGCOLOR, BGCOLOR);
    DISP.setTextSize(SMALL_TEXT);
    DISP.printf(TXT_WF_CHANN, WiFi.channel(cursor));
    DISP.printf(TXT_WF_CRYPT, encryptType);
    //DISP.print("BSSID:\n" + WiFi.BSSIDstr(i));
    DISP.print("BSSID:\n" + WiFi.BSSIDstr(wscan_result_cursor));
    DISP.printf(TXT_SEL_BACK);
    DISP.setTextColor(BGCOLOR, FGCOLOR);
    DISP.printf(" %-19s\n", TXT_HOLD_ATTACK);
    DISP.setTextColor(FGCOLOR, BGCOLOR);
   if(check_select_press()){
      apMac=WiFi.BSSIDstr(cursor);
      apSsidName=WiFi.SSID(cursor);
      channel = static_cast<uint8_t>(WiFi.channel(cursor));                            // DEAUTH - save channel
      uint8_t* bssid = WiFi.BSSID(cursor);                                             // DEAUTH - save BSSID (AP MAC)
      memcpy(ap_record.bssid, bssid, 6);                                               // DEAUTH - cpy bssid to memory
      rstOverride = false;
      current_proc = 20;
      isSwitching = true;
      delay(100);
    }
  }
}

void wscan_setup(){
  rstOverride = false;  
  cursor = 0;
  DISP.fillScreen(BGCOLOR);
  DISP.setTextSize(BIG_TEXT);
  DISP.setCursor(0, 0);
  DISP.println(TXT_WF_SCN);
  delay(2000);
}

void wscan_loop(){
  DISP.fillScreen(BGCOLOR);
  DISP.setTextSize(MEDIUM_TEXT);
  DISP.setCursor(0, 0);
  DISP.println(TXT_WF_SCNING);
  wifict = WiFi.scanNetworks();
  DISP.fillScreen(BGCOLOR);
  DISP.setTextSize(SMALL_TEXT);
  DISP.setCursor(0, 0);
  if(wifict > 0){
    isSwitching = true;
    current_proc=15;
  }
}
/// WIFI-Attack MENU and functions START///
MENU wsAmenu[] = {
  { TXT_BACK, 5},
  { TXT_WFA_PORTAL, 0},
  #if defined(DEAUTHER)
    { TXT_WFA_DEAUTH, 1},
    { TXT_WFA_COMBINED, 2},
  #endif
};
int wsAmenu_size = sizeof(wsAmenu) / sizeof (MENU);

void wsAmenu_setup() {
  rstOverride = true;
  drawmenu(wsAmenu, wsAmenu_size);
  delay(500); // Prevent switching after menu loads up
}

void wsAmenu_loop() {
  if (check_next_press()) {
    cursor++;
    cursor = cursor % wsAmenu_size;
    drawmenu(wsAmenu, wsAmenu_size);
    delay(250);
  }
  if (check_select_press()) {
    int option = wsAmenu[cursor].command;
    rstOverride = false;
    current_proc = 20;
    isSwitching = true;
    switch(option) {
      case 0:                     //Go to Clone Nemo Portal
        rstOverride = false;
        isSwitching = true;
        clone_flg=true;
        target_deauth_flg=false;
        current_proc = 19;
        break;
      #if defined (DEAUTHER)
        case 1:                     //Go to Deauth
          rstOverride = false;
          isSwitching = true;
          target_deauth_flg=false;
          target_deauth=true;
          current_proc = 21;                                                                 // iserir codigo do deauth aqui
          break;
        case 2:                     //Go to Nemo with Deauth
          rstOverride = false;
          isSwitching = true;
          clone_flg=true;
          target_deauth_flg=true;
          target_deauth=true;
          current_proc = 19;
          break;
      #endif
      case 5:                     //Exit
        current_proc = 14;
        break;
    }
  }
}

// WIFI-Attack MENU and functions END
// DEAUTH ATTACK START
#if defined(DEAUTHER)
  void deauth_setup(){
    // Start the Access point service as Hidden
    WiFi.mode(WIFI_AP);
    WiFi.softAP(apSsidName, emptyString, channel, 1, 4, false);
    IPAddress apIP = WiFi.softAPIP();


    DISP.fillScreen(BGCOLOR);
    DISP.setCursor(0, 0);
    DISP.setTextSize(BIG_TEXT);
    DISP.setTextColor(TFT_RED, BGCOLOR);
    DISP.println("Deauth Atk");
    DISP.setTextSize(SMALL_TEXT);
    DISP.setTextColor(FGCOLOR, BGCOLOR);
    DISP.print("\nSSID: " + apSsidName);
    DISP.print("\n");
    DISP.printf(TXT_WF_CHANN, channel);
    DISP.print("> " + apMac);
    memcpy(deauth_frame, deauth_frame_default, sizeof(deauth_frame_default));
    wsl_bypasser_send_deauth_frame(&ap_record, channel);                                        // DEAUTH CREATE FRAME

    cursor = 0;
    rstOverride = false;
    delay(500); // Prevent switching after menu loads up
  }
  void deauth_loop(){

    if (target_deauth == true) {                                                                 // DEAUTH
      wsl_bypasser_send_raw_frame(deauth_frame, sizeof(deauth_frame_default));                   // DEAUTH SEND FRAME
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

    delay(100); //from 200

    if (check_select_press()){                                                                    // DEAUTH
      target_deauth = !target_deauth;                                                             // DEAUTH
      DISP.setCursor(1, 115);                                                                     // DEAUTH
      DISP.println("......................");                                                     // DEAUTH
      delay(500);                                                                                 // DEAUTH
    }                                                                                             // DEAUTH

    if (check_next_press()){
      WiFi.mode(WIFI_MODE_STA);
      rstOverride = false;
      isSwitching = true;
      target_deauth = false;                                                                      // DEAUTH
      current_proc = 12;
      delay(500);
    }
  }
  // DEAUTH attack END
#endif