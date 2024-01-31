
#define __HCSR04_HOME__
#include "hcsr04_pubs.h"

#define ECHO_PIN 7
#define TRIG_PIN 6
#define ULT_SENSOR_EN_PIN 5
#define LEVEL_SAMPLING_INTERVAL_MS 200
#define MAX_LEVEL_SAMPLES 10
#define LVL_MEAS_INTERVAL 10000


#define SERIAL_EN

volatile static uint8_t average_lvl_buf[LVL_HIST_BUF_SIZE+1];
volatile static uint8_t bufWrPosIdx=0;

void init_ultrasonic_sensor() 
{
  
 pinMode(TRIG_PIN, OUTPUT); // Sets the trigPin as an OUTPUT
 pinMode(ECHO_PIN, INPUT);
 pinMode(ULT_SENSOR_EN_PIN,OUTPUT);
 memset(average_lvl_buf,0xFF,LVL_HIST_BUF_SIZE+1);
}


static void pushLvlToBuf(uint8_t lvl)
{
  average_lvl_buf[bufWrPosIdx]=lvl;
  average_lvl_buf[LVL_HIST_BUF_SIZE]=bufWrPosIdx;
  bufWrPosIdx++;
  
  if(bufWrPosIdx == LVL_HIST_BUF_SIZE)
  {
     bufWrPosIdx=0;
    }
}


static void measureLvl()
{
  static long starTime=millis();
  uint8_t avgLvl=0;
  while(1)
  {
      if (millis() - starTime >= LEVEL_SAMPLING_INTERVAL_MS)
    {
      long duration; // variable for the duration of sound wave travel
      static uint16_t sumLvls = 0;
      static uint16_t numSample = 0;
      uint8_t lvlMeas = 0;

      // Clears the trigPin condition
      digitalWrite(TRIG_PIN, LOW);
      delayMicroseconds(2);
      // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
      digitalWrite(TRIG_PIN, HIGH);
      delayMicroseconds(10);
      digitalWrite(TRIG_PIN, LOW);
      // Reads the echoPin, returns the sound wave travel time in microseconds
      duration = pulseIn(ECHO_PIN, HIGH);

      // Calculating the distance
      // Speed of sound wave divided by 2 (go and back)
      lvlMeas = (uint8_t)(uint32_t)(double)(duration * 0.017);  //.017=0.034/2
      /*average calc*/
      sumLvls += lvlMeas;
      numSample++;
      if (numSample >= MAX_LEVEL_SAMPLES)
      {
        avgLvl = (uint8_t)(uint32_t)((double)(sumLvls) / MAX_LEVEL_SAMPLES);
        pushLvlToBuf(avgLvl);
        sumLvls = 0;
        numSample = 0;
  #ifdef SERIAL_EN
        Serial.print(lvlMeas);
        Serial.print(",");
        Serial.println(avgLvl);
  #endif
        break;
        
      }
      starTime = millis();
    }
  }
}

uint8_t* getUltSnsrWaterLevelHist()
{
  return &average_lvl_buf[0];
}

void task_UltSenseWaterlvl() 
{
   static long lvlMeasStart=millis();
   if(millis() - lvlMeasStart >= LVL_MEAS_INTERVAL)
   {
      digitalWrite(ULT_SENSOR_EN_PIN,HIGH);
      measureLvl();
      digitalWrite(ULT_SENSOR_EN_PIN,LOW);
      lvlMeasStart=millis();
   }

}
