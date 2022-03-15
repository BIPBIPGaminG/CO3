#include <Arduino.h>

#include "sensirion_common.h"
#include "sgp30.h"


void setup() {
    s16 err;
    u16 scaled_ethanol_signal, scaled_h2_signal;
    Serial.begin(115200);
    Serial.println("serial start!!");

    /*For wio link!*/
    #if defined(ESP8266)
    pinMode(15, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
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
    //
}

void loop() {
    s16 err = 0;
    u16 tvoc_ppb, co2_eq_ppm;
    err = sgp_measure_iaq_blocking_read(&tvoc_ppb, &co2_eq_ppm);
    if (err == STATUS_OK) {
        Serial.print("tVOC  Concentration:");
        Serial.print(tvoc_ppb);
        Serial.println("ppb");

        Serial.print("CO2eq Concentration:");
        Serial.print(co2_eq_ppm);
        Serial.println("ppm");

        if ((500 < co2_eq_ppm || 250 < tvoc_ppb) && (co2_eq_ppm < 1000 || tvoc_ppb < 200)) {
          Serial.println("attention");
          digitalWrite(3, HIGH);
          delay(1000);
          digitalWrite(3, LOW);
        }else if (co2_eq_ppm > 1000 || tvoc_ppb > 2000) {
          Serial.println("alert");
          digitalWrite(3, HIGH);
          digitalWrite(2, HIGH);
          delay(250);
          digitalWrite(3, LOW);
          digitalWrite(2, LOW);
        }

    } else {
        Serial.println("error reading IAQ values\n");
    }
    delay(1000);
}