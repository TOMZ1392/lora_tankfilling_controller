#include "ra02_comm_pubs.h"
#include "hcsr04_pubs.h"

#define TRANSMIT_INT_CTR_MS 2000
#define TRANSMIT_CTR_STEP  2
#define TRANSMIT_INTERVAL  4
uint32_t timerTicks_tx=0;

void  do_tasks();

void setup() 
{
  Serial.begin(9600);
 init_ra02_comm();
 init_ultrasonic_sensor();

}

void loop()
{
  do_tasks();
}


void do_tasks() 
{
  static bool transmitOnceFlag=false;
  static unsigned long startimeForTx=millis();
  task_UltSenseWaterlvl();
  uint8_t* hist=getUltSnsrWaterLevelHist();
  
  if(millis()-startimeForTx > TRANSMIT_INT_CTR_MS)
  {
    timerTicks_tx+=TRANSMIT_CTR_STEP;
    startimeForTx=millis();
    transmitOnceFlag=false;
  }

  if(hist[0]!=0xFF)
  {
    if(timerTicks_tx%TRANSMIT_INTERVAL==0)
    {
        

        
        if (!transmitOnceFlag)
        {
          char txBuf[LVL_HIST_BUF_SIZE];
          memcpy(txBuf,hist,LVL_HIST_BUF_SIZE+1);
          Serial.print(hist[0]);
          Serial.print(" [");
          Serial.print(txBuf);
          Serial.println(" ]");
          lora_transmitData(txBuf);
          transmitOnceFlag=true;
        }
    }

  }
}
