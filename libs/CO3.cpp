#include <CO3.h>
#include <Wire.h>
#include <i2c.h>
#include <sensirion_common.h>
#include <sensirion_configuration.h>
#include <sgp30.h>
#include <sgp_featureset.h>
#include <rgb_lcd.h>
#include <Arduino.h>

rgb_lcd lcd;

#define DS1621_ADDRESS  0x48

void CO3::start(){
    s16 err;
    Serial.begin(115200);
    Serial.println("serial start!!");

    Wire.begin();
    Wire.beginTransmission(DS1621_ADDRESS);             // connect to DS1621 (#0)
    Wire.write(0xAC);                            // Access Config
    Wire.write(0x02);                            // set for continuous conversion
    Wire.beginTransmission(DS1621_ADDRESS);             // restart
    Wire.write(0xEE);                            // start conversions
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
  	err = sgp_measure_signals_blocking_read(&scaled_ethanol_signal,&scaled_h2_signal);
  	if (err == STATUS_OK) {
    	Serial.println("get ram signal!");
  	} else {
    	Serial.println("error reading signals");
  	}
  	err = sgp_iaq_init();

  	lcd.begin(16, 2);
  	lcd.setRGB(255, 255, 255);
}

void CO3::update(){

	err = sgp_measure_iaq_blocking_read(&cov, &co2);

	if (err != STATUS_OK){
		Serial.println("error reading IAQ values\n");
	}else{
		
	}
}

char CO3::getTemperature(){

	c_temp = CO3::getTemperature();
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
	return c_buffer;
}

int get_temperature(){
	int firstByte;
	int secondByte;
  	float temp = 0;

  	Wire.beginTransmission(DS1621_ADDRESS);
  	Wire.write(0xAA);                            // read temperature command
  	Wire.endTransmission();
  	Wire.requestFrom(DS1621_ADDRESS, 2);    // request two bytes from DS1621 (0.5 deg. resolution)

  	firstByte = Wire.read();           // get first byte
  	secondByte = Wire.read();    // get second byte

  	temp = firstByte;

  	if (secondByte)             // if there is a 0.5 deg difference
    	temp += 0.5;
  	return temp;
}

void CO3::blink(bool state){
	if (state){
		digitalWrite(3, HIGH);
	}else{
		digitalWrite(3, LOW);
	}
}

void CO3::bip(bool state){
	if (state){
		digitalWrite(4, HIGH);
	}else{
		digitalWrite(4, LOW);
	}
}

void CO3::writeLCD(char text, int line, int column){
	lcd.setCursor(line, column);
	lcd.print(text);
}

void CO3::setLCDColor(int R, int G, int B){
	lcd.setRGB(R, G, B);
}