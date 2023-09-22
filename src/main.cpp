#include <Arduino.h>
#include "./ExBusSerial/ExBusSerial.h"

JetiExBusSerial exbus(2);

void setup()
{
  // put your setup code here, to run once:
  //Serial.begin(115200);
  exbus.begin();
}

void loop()
{
  delay(1000);

  exbus.sendTelRequest(2);
  // exbus.receiveMessage();
}