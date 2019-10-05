#include <WiFi.h>
#include <Wire.h>
#include <driver/adc.h>
#include "DHT.h"



void setup() {
  Serial.begin(115200);
    
   
  analogReadResolution(11);
  adc1_config_width(ADC_WIDTH_BIT_10);
  adc1_config_channel_atten(ADC1_CHANNEL_4,ADC_ATTEN_DB_11 );  //ADC_ATTEN_DB_11 = 0-3,6V
  
  pinMode(ADC1_CHANNEL_4, INPUT);
  
  
}


void loop() {

  
  
    int s = adc1_get_voltage( ADC1_CHANNEL_4 );
    

    Serial.println("soundlevel: "+String(s));

    delay(200);
     
  
}
