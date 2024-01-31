void loraRxInit() {
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
volatile bool rxDataAvailable=false;

bool is_rxDataAvl()
{
    return rxDataAvailable;
}

uint8_t databuf_rx[11];

uint8_t* getRxDataBuf()
{

  return &databuf_rx[0];
}

void task_loRa_Rx() {
    // try to parse packet
  if(LoRa.available()
  {
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
      // received a packet
      //Serial.print("Received..");
      //lcd.clear();
      lcd_print("Received packet",0);
      // read packet
      
      memset(databuf,0,11);
      uint16_t posidx=0;
      while (LoRa.available()) 
      {
        databuf_rx[posidx]=(uint8_t)LoRa.read();;
        posidx++;
      }
      
      //Serial.print(databuf);
      uint16_t i=0;
      for(;i<10;i++)
      {
        Serial.print(databuf_rx[i]);
        Serial.print(","); 
      }
      Serial.println(databuf_rx[10]);
      // print RSSI of packet
      //Serial.print("' with RSSI ");
      //Serial.println(LoRa.packetRssi());
 
      startTime=millis();
      notRecvFlag=false;
      rxDataAvailable=true;
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
}