#include <ESP8266WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>


#define wifi_ssid ""
#define wifi_password ""

#define mqtt_server "192.168.2.139"
#define mqtt_user "your_username"
#define mqtt_password "your_password"
//ADC_MODE(ADC_VCC);  //measure voltage
WiFiClient espClient;
PubSubClient client(espClient);


void setup() {
  Serial.begin(115200);

  pinMode(BUILTIN_LED, OUTPUT);
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);


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

  
  String path = "/esp/"+String(ESP.getChipId(),HEX);

  long now = millis();
  if (now - lastMsg > 1000) {

    lastMsg = now;
     client.publish(String(path+"/time").c_str(), String(now).c_str(), true);
     client.publish(String(path+"/soundlevel").c_str(), String(analogRead(0)).c_str(), true);
     

     if (counter++ > 60) {
        client.publish(String(path+"/status").c_str(), "SLEEP", true);
        delay(100); //must allow mqtt client to finish
        ESP.deepSleep(60000000, WAKE_RF_DEFAULT); //microseconds.  600000000 is 10 minutes
     }
     
  }
}
