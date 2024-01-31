#include <SPI.h>
#include <LoRa.h>
#include <LiquidCrystal_I2C.h>

#define LED_PIN 5
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows

volatile unsigned long startTime=0;
bool notRecvFlag;
void lcd_init()
{
  lcd.init(); // initialize the lcd
  lcd.backlight();
  lcd.setCursor(0, 0);         // move cursor to   (0, 0)
  lcd.print("Lcd Initialized");        // print message at (0, 0)
  lcd.setCursor(0, 1);         // move cursor to   (2, 1)
  lcd.print("Waiting for packet..");
  //delay(1000);
  //lcd.noBacklight();
}

  void setup() {
    Serial.begin(9600);
    lcd_init();
    while (!Serial);

    Serial.println("LoRa Receiver");

    if (!LoRa.begin(433E6)) { //was 915E6
      Serial.println("Starting LoRa failed!");
      while (1);
    }
    startTime=millis();
  }


void ledBlink()
{
digitalWrite(LED_PIN,HIGH);
delay(300);
digitalWrite(LED_PIN,LOW);
delay(300);
}





void lcd_print(char msg[], uint8_t row)
{
  lcd.setCursor(0, row);
  lcd.print(msg);
}


  char databuf[11];
  void loop() {
    // try to parse packet
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
      // received a packet
      //Serial.print("Received..");
      //lcd.clear();
      lcd_print("Received packet",0);
      // read packet
      
      memset(databuf,0,11);
      uint16_t posidx=0;
      while (LoRa.available()) {
        char b=(char)LoRa.read();
        databuf[posidx]=b;
        posidx++;
        
        //ledBlink();
      }
      uint8_t valData[11];
      memcpy(valData,(uint8_t*)databuf,11);
      //Serial.print(databuf);
      uint16_t i=0;
      for(;i<10;i++)
      {
        Serial.print(valData[i]);
      Serial.print(","); 
      }
      Serial.println(valData[10]);
      // print RSSI of packet
      //Serial.print("' with RSSI ");
      //Serial.println(LoRa.packetRssi());
      lcd_print(valData[0],1);
      startTime=millis();
      notRecvFlag=false;
    }
    else
    {
      //reception lost
      if(millis() - startTime >=4000)
      {
        if(!notRecvFlag)
        {
          lcd.clear();
          lcd_print("Waiting for data..",0);
          
          startTime=millis();
        }
        notRecvFlag=true;
      }
    }
  }
