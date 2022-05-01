#include "CO3.h"

CO3 co3;

void setup() {
  co3.start();
}

void loop() {
  co3.update();

  if((500 < co3.getCO2() < 800) || (250 < co3.getCOV() < 2000)){
    Serial.println("attention");
  }else if (co3.getCO2() >= 800 || co3.getCOV() >= 2000) {
    Serial.println("alerte");
  }
}
