#include <WiFi.h>
#include <Wire.h>
#include <driver/adc.h>
#include "DHT.h"
#include <WebServer.h>      // Local WebServer used to serve the configuration portal (  https://github.com/zhouhan0126/DNSServer---esp32  )      
#include <DNSServer.h>      // Local DNS Server used for redirecting all requests to the configuration portal (  https://github.com/zhouhan0126/DNSServer---esp32  )
#include <WiFiManager.h>    // WiFi Configuration Magic (  https://github.com/zhouhan0126/DNSServer---esp32  ) >>  https://github.com/zhouhan0126/DNSServer---esp32  (ORIGINAL)
                            // See also http://www.instructables.com/id/ESP8266-and-ESP32-With-WiFiManager/
#include <PubSubClient.h>

#define mqtt_server "192.168.2.139"
#define mqtt_user "your_username"
#define mqtt_password "your_password"
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  47        /* Time ESP32 will go to sleep (in seconds) */
#define DHTPIN A5
#define DHTTYPE DHT11

DHT dht(DHTPIN,DHTTYPE);

WiFiClient espClient;     // just to make MQTT library happy
PubSubClient client(espClient);


const int PIN_AP = 2;       // AP pin for Wifi manager
String EspName="";


void setup() {
  Serial.begin(115200);
    
  dht.begin();
  EspName=String((int)ESP.getEfuseMac(),HEX);

  pinMode(PIN_AP, INPUT);
  WiFiManager wifiManager;
  wifiManager.autoConnect("ESP_AP", "12345678"); 

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  analogReadResolution(11);
  adc1_config_width(ADC_WIDTH_BIT_10);
  adc1_config_channel_atten(ADC1_CHANNEL_4,ADC_ATTEN_DB_11 );  //ADC_ATTEN_DB_11 = 0-3,6V
  
  pinMode(ADC1_CHANNEL_4, INPUT);
  
  client.setServer(mqtt_server, 1883);

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

}

void reconnectMQTT() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
    if (client.connect(("ESP"+EspName).c_str())) {
    // if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void checkAP(){
 
  //See if AP button is pressed
   if ( digitalRead(PIN_AP) == HIGH ) {
    
      WiFiManager wifiManager;  
      Serial.println("Being AP now"); 
      if(!wifiManager.startConfigPortal("ESP_AP", "12345678") ){
        Serial.println("Failed to connect");
        delay(2000);
        ESP.restart();
        delay(1000);
      }
      Serial.println("Connnected to AP again!!!");
}}

long lastMsg = 0;
int counter=0;

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  checkAP();

  String path = "/esp/"+EspName;
  long now = millis();
  if (now - lastMsg > 1000) {

    lastMsg = now;

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    int s = adc1_get_voltage( ADC1_CHANNEL_4 );
    
    client.publish(String(path+"/status/battery").c_str(),String(analogRead(35)*2).c_str(),true);
    client.publish(String(path+"/measurement/humidity").c_str(), String(h).c_str(),true);
    client.publish(String(path+"/measurement/temperature").c_str(), String(t).c_str(),true);
    client.publish(String(path+"/measurement/soundlevel").c_str(), String(s).c_str(), true);

    if (counter++ > 10) {
        //client.publish(String(path+"/status").c_str(), "SLEEP", true);
        delay(100); //must allow mqtt client to finish
        esp_deep_sleep_start();
     }
     
  }
}
