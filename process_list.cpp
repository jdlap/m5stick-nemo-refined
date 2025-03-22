
#include "Arduino.h"
#include "process_list.h"
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
#include "src/main_menu/main_menu.h"
#include "src/battery/battery.h"
#include "src/qrcode/qrcode.h"
#include "src/credits/credits.h"
#include "src/clock/clock.h"


void defaultProc(){
    return;
}

void setupProcArray(){
#if defined(RTC)
  processArray[CLOCK_PROC] = {&clock_setup, &clock_loop};
#endif
  processArray[MAIN_MENU_PROC] = {&mmenu_setup, &mmenu_loop};
  processArray[SET_MENU_PROC] = {&smenu_setup, &smenu_loop};
#if defined(RTC)
  processArray[TIME_SET_PROC] = {&timeset_setup, &timeset_loop};
#endif
  processArray[DIM_MENU_PROC] = {&dmenu_setup, &dmenu_loop};
  processArray[TVBG_PROC] = {&tvbgone_setup, &tvbgone_loop};
#if defined(AXP) || defined(PWRMGMT)
  processArray[BAT_PROC] = {&battery_setup, &battery_loop};
#endif
#if defined(CARDPUTER)
  processArray[BAT_PROC] = {&battery_setup, &battery_loop};
#endif
#if defined(ROTATION)
  processArray[ROT_MENU_PROC] = {&rmenu_setup, &rmenu_loop};
#endif
  processArray[APP_JACK_PROC] = {&aj_setup, &aj_loop};
  processArray[APP_JACK_ADV_PROC] = {&aj_adv_setup, &aj_adv};
  processArray[CREDITS_PROC] = {&credits_setup, &credits_loop};
  processArray[WIFISPAM_PROC] = {&wifispam_setup, &wifispam_loop};
  processArray[WIFI_SCAN_MENU_PROC] = {&wsmenu_setup, &wsmenu_loop};
  processArray[TCBG_MENU_PROC] = {&tvbgmenu_setup, &tvbgmenu_loop};
  processArray[WIFI_SCAN_PROC] = {&wscan_setup, &wscan_loop};
  processArray[WIFI_SCAN_RES_PROC] = {&wscan_result_setup, &wscan_result_loop};
  processArray[BLT_MENU_PROC] = {&btmenu_setup, &btmenu_loop};
  processArray[BLT_MAELS_PROC] = {&btmaelstrom_setup, &btmaelstrom_loop};
  processArray[QR_PROC] = {&qrmenu_setup, &qrmenu_loop};
  processArray[PROTAL_PROC] = {&portal_setup, &portal_loop};
  processArray[WIFI_ATCK_PROC] = {&wsAmenu_setup, &wsAmenu_loop};
#if defined(DEAUTHER)
  processArray[DEAUTH_PROC] = {&deauth_setup, &deauth_loop};
#endif
  processArray[COLOR_PROC] = {&color_setup, &color_loop};
  processArray[THEME_PROC] = {&theme_setup, &theme_loop};
#if defined(SDCARD)                                                // SDCARD M5Stick
  #ifndef CARDPUTER                                                // SDCARD M5Stick
    processArray[SDCARD_PROC] = {&defaultProc, &ToggleSDCard};                                         // SDCARD M5Stick
  #endif                                                           // SDCARD M5Stick
#endif  
  
}