
#include "Arduino.h"
#include "../common/common.h"
#include "battery.h"


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