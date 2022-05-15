#include "CO3.h"

CO3 co3;

void setup() {
  co3.start();
}

void loop() {
  co3.update();
  
  co3.writeLCD("co2: " + String(co3.getCO2()), 0, 0);
  co3.writeLCD(String(co3.getTemperature()) + "°C, 0, 10);
  co3.writeLCD("cov: " + String(co3.getCO2()), 1, 0);
  
  if((500 < co3.getCO2() < 800) || (250 < co3.getCOV() < 2000)){
  
    co3.setLCDColor(128, 0, 0);
    delay(500);
  
  }else if (co3.getCO2() >= 800 || co3.getCOV() >= 2000) {
    
    co3.setLCDColor(255, 0, 0);
    co3.bip(true);
    
    delay(500);
    co3.bip(false);
    
  }
  
  co3.setLCDColor(128, 128, 128);
  delay(2000);
}
