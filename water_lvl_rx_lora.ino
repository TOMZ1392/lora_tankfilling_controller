
/* pump controller code 
lora RA02 reciever
*/

#define SERIAL_EN

#ifdef SERIAL_EN
  #define PRINT_SERIAL_MSG(x)     Serial.print(x)  
  #define PRINTLN_SERIAL_MSG(x)     Serial.println(x)  
#else
  #define PRINTLN_SERIAL_MSG(x)  
  #define PRINT_SERIAL_MSG(x)  
#endif



#define MOTOR_RELAY_PIN  7
#define BUZZER_PIN   9
#define MOTOR_LED_PIN 8

/*init*/
void setup() {
  Serial.begin(9600);
  pinMode(interruptPin, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MOTOR_RELAY_PIN, OUTPUT);
  pinMode(MOTOR_LED_PIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin),  signalIntrptHdlr, FALLING);
  PRINTLN_SERIAL_MSG("Init Success");
}

volatile long motorRunTimer = 0;
volatile bool motorTimerIsRunning = 0;
volatile uint16_t runDurnSec = 0;

/*timeout for motor pin if signal lost for more than n seconds*/
#define MOTOR_TIMEOUT_S  300

/*motor timer start/reset */
void startMotorRunTmr()
{
  motorRunTimer = millis();
  runDurnSec = 0;
  motorTimerIsRunning = 1;
}

/*motor timer monitor*/
bool monitorMotorTimer()
{
  bool retVal = true;
  if (motorTimerIsRunning)
  {
    if (millis() - motorRunTimer >= 1000)
    {
      motorRunTimer = millis();
      runDurnSec++;
      
    }
    if (runDurnSec >= MOTOR_TIMEOUT_S)
    {
      retVal = false;
      motorTimerIsRunning = 0;
      runDurnSec = 0;
    }
  }
  return retVal;
}


/*buzzer defines*/
#define BUZZER_TONE_INTERVAL   700
#define BUZZER_HIGH_TIME_1 300
#define BUZZER_LOW_TIME_1 50
#define BUZZER_HIGH_TIME_2 200
#define BUZZER_LOW_TIME_2 50

/* Let everyone know when the water pump turn on is requested*/
void generateAlarm()
{
  static bool hightone = false;
  static long startTime = millis();
  /* switch between tones every BUZZER_TONE_INTERVAL ms*/
  if (millis() - startTime <= BUZZER_TONE_INTERVAL) {
    if (hightone) {
      digitalWrite(BUZZER_PIN, 1);
      delayMicroseconds(BUZZER_HIGH_TIME_1);
      digitalWrite(BUZZER_PIN, 0);
      delayMicroseconds(BUZZER_LOW_TIME_1);

    }
    else {
      digitalWrite(BUZZER_PIN, 1);
      delayMicroseconds(BUZZER_HIGH_TIME_2);
      digitalWrite(BUZZER_PIN, 0);
      delayMicroseconds(BUZZER_LOW_TIME_2);

    }

  }
  else {
    if (hightone) {
      hightone = false;
    } else {
      hightone = true;
    }
    startTime = millis();
  }

}
/*stop alarm*/
void stopAlarm()
{
  analogWrite(BUZZER_PIN, 0);
}


/*start motor*/
void startMotor()
{
  digitalWrite(MOTOR_RELAY_PIN, 1);
}
/*stop motor*/
void stopMotor()
{
  digitalWrite(MOTOR_RELAY_PIN, 0);
}

static bool motorState = 0;
bool getMotorState(void)
{
  return motorState;
}

bool setMotorState(bool st)
{
 motorState=st;
}
void task_MotorRelay_buzzer_led()
{
 

/* generate alarm and monitor timer and turn off motor when timout occurrs */
  if (getMotorState())
  {
    digitalWrite(MOTOR_LED_PIN,HIGH);
    generateAlarm();
    if (!monitorMotorTimer())
    {
      digitalWrite(MOTOR_LED_PIN,LOW);
      PRINTLN_SERIAL_MSG("Motor Timeout occurred!!");
      setMotorState(false);
      stopAlarm();
      stopMotor();
    }
  }


}

void loop()
{
  task_loRa_Rx();
  task_waterLvl_plaus();
  task_MotorRelay_buzzer_led();
}
