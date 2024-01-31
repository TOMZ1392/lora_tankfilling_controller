#ifndef __HCSR04_HOME__
  extern void init_ultrasonic_sensor();
  extern uint8_t* getUltSnsrWaterLevelHist();
  extern void task_UltSenseWaterlvl();
  #define LVL_HIST_BUF_SIZE  10
#else
  void init_ultrasonic_sensor();
  uint8_t* getUltSnsrWaterLevelHist();
  void task_UltSenseWaterlvl();
  static void measureLvl();
  static void pushLvlToBuf(uint8_t lvl);
  #define LVL_HIST_BUF_SIZE  10
#endif