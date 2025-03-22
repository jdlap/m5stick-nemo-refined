
#include "Arduino.h"
#include "../common/common.h"
#include "clock.h"

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