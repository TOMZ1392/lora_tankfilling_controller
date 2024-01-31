#ifndef __RA02_HOME__
  extern void init_ra02_comm();
  extern void lora_transmitData(char data[]);
#else
  void init_ra02_comm();
  void lora_transmitData(char data[]);
#endif
