/*
  WiFi Blink

  Turns an LED on and off by commands received through MQTT

*/

#include <WiFi.h>
#include <PubSubClient.h>
#include "Constants.h"
WiFiClient wifi_client;
PubSubClient mqtt_client(wifi_client);

// function is executed when a new message arrives
void mqtt_callback(char* topic, byte* message, unsigned int length) {
  Serial.print("message arrived on topic: ");
  Serial.println(topic);
  Serial.print("message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // if a message is received on the topic, you check if the message is either "on" or "off".
  // changes the output state according to the message
  if (String(topic) == led_mqtt_topic) {
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(led_pin, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(led_pin, LOW);
    }
  }
}

// setup for wifi and mqtt
void setup_connections() {
  Serial.printf("connecting to %s", wifi_ssid);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(2000);
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
  }
  Serial.println("");
  Serial.println("connected.");
  Serial.print("ip address: ");
  Serial.println(WiFi.localIP());
  pinMode(led_pin, OUTPUT);
  mqtt_client.setServer(mqtt_server_address, mqtt_server_port);
  mqtt_client.setCallback(mqtt_callback);
}

// connects to mqtt and subscribes to topics
void reconnect_mqtt() {
  int counter = 0;
  while (!mqtt_client.connected()) {
    if (counter==5) ESP.restart();
    counter+=1;
    Serial.printf("connecting to mqtt server at %s", mqtt_server_address);
    Serial.println("");

    if (mqtt_client.connect(mqtt_client_name)) {
      Serial.println("connected.");
      mqtt_client.subscribe(led_mqtt_topic);
    } else {
      Serial.print("failed, rc = ");
      Serial.println(mqtt_client.state());
      delay(5000);
    }
  }
}

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  Serial.println("");
  setup_connections();
}

// the loop function runs over and over again forever
void loop() {
  if (!mqtt_client.connected()) reconnect_mqtt();
  mqtt_client.loop();
  delay(500);
}
