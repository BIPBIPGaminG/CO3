#include <Wire.h>
#include <i2c.h>
#include <sensirion_common.h>
#include <sensirion_configuration.h>
#include <sgp30.h>
#include <sgp_featureset.h>
#include <rgb_lcd.h>

rgb_lcd lcd;

#define DS1621_ADDRESS  0x48

void setup() {
  s16 err;
  u16 scaled_ethanol_signal, scaled_h2_signal;
  Serial.begin(115200);
  Serial.println("serial start!!");

  Wire.begin();
  Wire.beginTransmission(DS1621_ADDRESS);             // connect to DS1621 (#0)
  Wire.send(0xAC);                            // Access Config
  Wire.send(0x02);                            // set for continuous conversion
  Wire.beginTransmission(DS1621_ADDRESS);             // restart
  Wire.send(0xEE);                            // start conversions
  Wire.endTransmission();

  pinMode(4, OUTPUT);
  pinMode(3, OUTPUT);

  /*For wio link!*/
#if defined(ESP8266)
  pinMode(15, OUTPUT);
  digitalWrite(15, 1);
  Serial.println("Set wio link power!");
  delay(500);
#endif
  /*  Init module,Reset all baseline,The initialization takes up to around 15 seconds, during which
      all APIs measuring IAQ(Indoor air quality ) output will not change.Default value is 400(ppm) for co2,0(ppb) for tvoc*/
  while (sgp_probe() != STATUS_OK) {
    Serial.println("SGP failed");
    while (1);
  }
  /*Read H2 and Ethanol signal in the way of blocking*/
  err = sgp_measure_signals_blocking_read(&scaled_ethanol_signal,
                                          &scaled_h2_signal);
  if (err == STATUS_OK) {
    Serial.println("get ram signal!");
  } else {
    Serial.println("error reading signals");
  }
  err = sgp_iaq_init();

  lcd.begin(16, 2);
  lcd.setRGB(255, 255, 255);
}

char c_buffer[8], f_buffer[8];

void loop() {
  lcd.setCursor(0, 0);
  s16 err = 0;
  u16 tvoc_ppb, co2_eq_ppm;
  err = sgp_measure_iaq_blocking_read(&tvoc_ppb, &co2_eq_ppm);

  int16_t c_temp = get_temperature();

  if (err == STATUS_OK) {
    lcd.print("COV:" + String(tvoc_ppb) + "ppb");
    lcd.setCursor(11, 0);

    if (c_temp < 0) {  // if temperature < 0 °C
      c_temp = abs(c_temp);  // absolute value
      sprintf(c_buffer, "-%02u.%1u%cC", c_temp / 10, c_temp % 10, 223);
    }
    else {
      if (c_temp >= 1000)    // if temperature >= 100.0 °C
        sprintf(c_buffer, "%03u.%1u%cC", c_temp / 10, c_temp % 10, 223);
      else
        sprintf(c_buffer, " %02u.%1u%cC", c_temp / 10, c_temp % 10, 223);
    }
    lcd.print(String(c_buffer) + "°C");
    lcd.setCursor(0, 1);
    lcd.print("CO2:" + String(co2_eq_ppm) + "ppm");

    if (co2_eq_ppm >= 800 || tvoc_ppb >= 2000) {
      Serial.println("alert");
      digitalWrite(3, HIGH);
      digitalWrite(4, HIGH);
      lcd.setRGB(255, 0, 0);
      delay(250);
      digitalWrite(3, LOW);
      digitalWrite(4, LOW);
      lcd.setRGB(255, 255, 255);
      delay(500);
      digitalWrite(3, HIGH);
      digitalWrite(4, HIGH);
      lcd.setRGB(255, 0, 0);
      delay(250);
      digitalWrite(3, LOW);
      digitalWrite(4, LOW);
      lcd.setRGB(255, 255, 255);
    } else if ((500 < co2_eq_ppm || 250 < tvoc_ppb) && (co2_eq_ppm < 800 || tvoc_ppb < 2000)) {
      Serial.println("attention");
      digitalWrite(3, HIGH);
      lcd.setRGB(255, 120, 91);
      delay(250);
      digitalWrite(3, LOW);
      lcd.setRGB(255, 255, 255);
    }

  } else {
    Serial.println("error reading IAQ values\n");
  }
  delay(1000);
}

int16_t get_temperature() {
  int8_t firstByte;
  int8_t secondByte;
  float temp = 0;

  delay(1000);                                // give time for measurement

  Wire.beginTransmission(DS1621_ADDRESS);
  Wire.send(0xAA);                            // read temperature command
  Wire.endTransmission();
  Wire.requestFrom(DS1621_ADDRESS, 2);    // request two bytes from DS1621 (0.5 deg. resolution)

  firstByte = Wire.receive();           // get first byte
  secondByte = Wire.receive();    // get second byte

  temp = firstByte;

  if (secondByte)             // if there is a 0.5 deg difference
    temp += 0.5;
  return temp;
}
