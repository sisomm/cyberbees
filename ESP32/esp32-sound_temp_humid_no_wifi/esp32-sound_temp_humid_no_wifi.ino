#include <Wire.h>
#include <driver/adc.h>
#include "DHT.h"
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  47        /* Time ESP32 will go to sleep (in seconds) */
#define DHTPIN A5
#define DHTTYPE DHT11

DHT dht(DHTPIN,DHTTYPE);

const int PIN_AP = 2;       // AP pin for Wifi manager
String EspName="";


void setup() {
  Serial.begin(115200);
    
  dht.begin();
  EspName=String((int)ESP.getEfuseMac(),HEX);

  pinMode(PIN_AP, INPUT);
  analogReadResolution(11);
  adc1_config_width(ADC_WIDTH_BIT_10);
  adc1_config_channel_atten(ADC1_CHANNEL_4,ADC_ATTEN_DB_11 );  //ADC_ATTEN_DB_11 = 0-3,6V
  
  pinMode(ADC1_CHANNEL_4, INPUT);

}

long lastMsg = 0;
int counter=0;

void loop() {

  String path = "/esp/"+EspName;
  long now = millis();
  if (now - lastMsg > 1000) {

    lastMsg = now;

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    int s = adc1_get_voltage( ADC1_CHANNEL_4 );
    
    Serial.println("/status/battery: "+String(analogRead(35)*2));
    Serial.println("/measurement/humidity: "+ String(h));
    Serial.println("/measurement/temperature: "+String(t));
    Serial.println("/measurement/soundlevel: " +String(s));

     
  }
}
