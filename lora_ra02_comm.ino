
#include <SPI.h>
#include <LoRa.h>

#define __RA02_HOME__
#include "ra02_comm_pubs.h"

#define TRANSMISSION_FREQUENCY  433E6

void init_ra02_comm() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Transmitter init..");

  if (!LoRa.begin(TRANSMISSION_FREQUENCY)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
   Serial.println("LoRa Transmitter init success..");
}

void lora_transmitData(char data[]) {
  Serial.print("Sending packet: ");
  // send packet
  LoRa.beginPacket();
  LoRa.print(data);
  LoRa.endPacket();
  Serial.println(data);
  delay(100);
}
