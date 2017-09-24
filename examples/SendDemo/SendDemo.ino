#include <OxeoDio.h>

OxeoDio dio = OxeoDio();

void setup() {
  Serial.begin(9600);
  dio.setSenderPin(3);
}

void loop() {
  if (Serial.available() > 0) {
    int val = Serial.read();
    
    if (val == '1') {
      Serial.println("on");
      dio.send(1064840800);
    } else if (val == '2') {
      Serial.println("off");
      dio.send(1064840784);
    }
  }
}