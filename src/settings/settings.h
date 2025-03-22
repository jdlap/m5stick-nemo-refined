#ifndef SETTINGS_H
#define SETTINGS_H

#include "../common/common.h"

//// Main Menu ////
void smenu_setup();
void clearSettings();
void smenu_loop();



//// Color Menu
void setcolor(bool fg, int col);
void color_setup();
void color_loop();


//// Theme Menu ////
void theme_setup();
void theme_loop();

//// Rotation Menu ////
#if defined(ROTATION)
  void rmenu_setup();
  void rmenu_loop();
#endif //ROTATION



#endif