#ifndef CLOCK_H
#define CLOCK_H
/// CLOCK ///
/// TIMESET ///
#if defined(RTC)
  void clock_setup();
  void clock_loop();
  /// TIME SETTING ///
  void timeset_setup();
  void timeset_loop();
#endif // RTC

#endif