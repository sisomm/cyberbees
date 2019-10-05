#include <WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <driver/adc.h>

#define wifi_ssid ""
#define wifi_password ""

#define mqtt_server "192.168.2.139"
#define mqtt_user "your_username"
#define mqtt_password "your_password"
//ADC_MODE(ADC_VCC);  //measure voltage
WiFiClient espClient;
PubSubClient client(espClient);

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  30        /* Time ESP32 will go to sleep (in seconds) */

#define ANALOG_PIN_0 36

void setup() {
  Serial.begin(115200);
  Serial.println("A0="+String(A0));
  pinMode(BUILTIN_LED, OUTPUT);
  
  adc1_config_width(ADC_WIDTH_12Bit);
  //adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_6db);
  adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_11 );  //ADC_ATTEN_DB_11 = 0-3,6V
  
  pinMode(ADC1_CHANNEL_0, INPUT);
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
    if (client.connect("ESP8266Client")) {
    // if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
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


long lastMsg = 0;

int counter=0;

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

String path = "/esp/"+String((int)ESP.getEfuseMac(),HEX);

  long now = millis();
  if (now - lastMsg > 1000) {

    lastMsg = now;
     client.publish(String(path+"/time").c_str(), String(now).c_str(), true);
//     client.publish(String(path+"/soundlevel").c_str(), String(analogRead(ANALOG_PIN_0)).c_str(), true);
     client.publish(String(path+"/soundlevel").c_str(), String(adc1_get_voltage( ADC1_CHANNEL_0 )).c_str(), true);
     

     if (counter++ > 60) {
        client.publish(String(path+"/status").c_str(), "SLEEP", true);
        delay(100); //must allow mqtt client to finish
        esp_deep_sleep_start();
     }
     
  }
}
