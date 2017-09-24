#include <OxeoDio.h>

OxeoDio dio = OxeoDio();

void setup() {
  Serial.begin(9600);
  dio.setReceiverPin(2);
}

void loop() {
  unsigned long sender = 0;

  if ((sender = dio.read()) != 0) {
    Serial.println(sender);
  }
}