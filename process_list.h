#ifndef PROC_LIST_H
#define PROC_LIST_H


#define CLOCK_PROC           0
#define MAIN_MENU_PROC       1
#define SET_MENU_PROC        2 
#define TIME_SET_PROC        3
#define DIM_MENU_PROC        4
#define TVBG_PROC            5
#define BAT_PROC             6
#define ROT_MENU_PROC        7
#define APP_JACK_PROC        8
#define APP_JACK_ADV_PROC    9
#define CREDITS_PROC         10
#define WIFISPAM_PROC        11
#define WIFI_SCAN_MENU_PROC  12
#define TCBG_MENU_PROC       13
#define WIFI_SCAN_PROC       14
#define WIFI_SCAN_RES_PROC   15
#define BLT_MENU_PROC        16
#define BLT_MAELS_PROC       17
#define QR_PROC              18
#define PROTAL_PROC          19
#define WIFI_ATCK_PROC       20
#define DEAUTH_PROC          21
#define COLOR_PROC           22
#define THEME_PROC           23
#define SDCARD_PROC          91

struct ProcessPair {
  void (*setup_func)();
  void (*loop_func)();
};

void defaultProc();

#define MAX_PROC_DEFS 255
ProcessPair processArray[MAX_PROC_DEFS] = {&defaultProc, &defaultProc};

void setupProcArray();

#endif
