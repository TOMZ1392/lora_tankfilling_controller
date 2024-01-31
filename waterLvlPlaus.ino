#define RX_BUF_SIZE  11
#define RX_BUF_POS_IDX  10

typedef enum
{
  WATER_LVL_RISING=0,
  WATER_LVL_FALLING=1,
  WATER_LVL_STAGNANT=3,
  WATER_LVL_SPORADIC=0xF

}lvlStateType_en;

extern bool is_rxDataAvl();
extern uint8_t* getRxDataBuf();
void waterLvlPlau_init()
{

}

volatile bool motorOnRequestState;

void setMotorOnRequestState(bool state)
{
  motorOnRequestState=state;
}

bool getMotorOnRequestState()
{
  return motorOnRequestState;
}


volatile uint8_t currentWaterLvl=0;

void setCurrentWaterLvl(uint8_t lvl)
{
  currentWaterLvl=lvl;
}

uint8_t getCurrentWaterLvl(void)
{
   return currentWaterLvl;
}

lvlStateType_en  anlyzLvlData(uint8_t* lvlBuf)
{
      uint8_t* rxBuf=getRxDataBuf();
      uint8_t currDataPos=rxBuf[RX_BUF_POS_IDX];
      uint8_t i=0;
      short diff_sum=0;
      uint8_t n_obs=0;
      short errorBuf[8];

      for(i=0;i<currDataPos-1;i++)
      {
          short diff=(short)databuf[i] - (short)databuf[i+1];
          diff_sum+=diff;
          errorBuf[n_obs]=diff;
          n_obs++;
          
      }

      for(i=currDataPos;i<RX_BUF_POS_IDX-1;i++)
      {
          short diff=(short)databuf[i] - (short)databuf[i+1];
          diff_sum+=diff;
          errorBuf[n_obs]=diff;
          n_obs++;
      }

      float mean=(float)(diff_sum)/n_obs;

      float sqSum=0;
      for(i=0;i<n_obs;i++)
      {
          float err=(float)errorBuf[i]-mean;
          sqSum+=err*err;
      }
      float variance=(sqSum)/n_obs;
      if(abs(mean) < 4  && variance < 5)
      {
          setCurrentWaterLvl(rxBuf[currDataPos]);
          if(mean > 0)
          {
            return WATER_LVL_RISING;
          }
          else if (mean < 0)
          {
            return WATER_LVL_FALLING;
          }
          else if (abs(mean) < 0.5)
          {
            return WATER_LVL_STAGNANT;
          }
          else
          {
            return WATER_LVL_STAGNANT;
          }
      }
      else
      {
        return WATER_LVL_SPORADIC;
      }

      
}


void task_waterLvl_plaus()
{
  if(is_rxDataAvl())
  {
      lvlStateType_en lvlState=anlyzLvlData();
      switch(lvlState)
      {
          case WATER_LVL_RISING:
            if (getCurrentWaterLvl() < TANK_FULL_LVL && getMotorOnRequestState())
            {
              setMotorOnRequestState(false);
            }
            break;
          case WATER_LVL_FALLING:
            if (getCurrentWaterLvl() < TANK_EMPTY_LVL)
            {
              setMotorOnRequestState(True);
            }
            break;
          case WATER_LVL_STAGNANT:
            if (getCurrentWaterLvl() < TANK_EMPTY_LVL)
            {
              setMotorOnRequestState(True);
            }
            break;
          case WATER_LVL_SPORADIC:
            if(getMotorOnRequestState())
            {
              setMotorOnRequestState(false);
            }
            break;
          default:
            break;
        
      }
  }
}