#ifndef MY_WIFI_H
#define MY_WIFI_H

#include "common.h"


/// WiFiSPAM ///
void wifispam_setup();
void wifispam_loop();

/// WIFI MENU ///
void wsmenu_setup();
void wsmenu_loop();
void wscan_drawmenu();

void wscan_result_setup();
void wscan_result_loop();

void wscan_setup();
void wscan_loop();

/// WIFI-Attack MENU and functions START///
void wsAmenu_setup();
void wsAmenu_loop();

// WIFI-Attack MENU and functions END
// DEAUTH ATTACK START
#if defined(DEAUTHER)
  void deauth_setup();
  void deauth_loop();
  // DEAUTH attack END
#endif


#endif