#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <keys.h>

#define LED_GREEN 17
#define LED_RED 16

const char* LB = "-------------------------------";

WiFiClient wiFiClient;
PubSubClient pubSubClient(wiFiClient);
long lastMessage = 0;
char msg[100];
int value = 0;

bool isGreen = true;
bool isRed = false;

void ledGreen() {
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, LOW);
}

void ledRed() {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, HIGH);
}

void callback(char* topic, byte* message, unsigned int lenght) {
  String messageTemp;
  for (size_t i = 0; i < lenght; i++) {
    messageTemp += (char) message[i];
  }

  Serial.print("Message on Topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(messageTemp);

  if (messageTemp == "red") {
    isRed = !isRed;
  } else if (messageTemp == "green") {
    isGreen = !isGreen;
  }
  
  
}

void setupWifi() {
  delay(12);
  Serial.println(LB);
  Serial.print("Connecting to: ");
  Serial.println(SSID);

  WiFi.begin(SSID, PWD);

  while (WiFi.status() != WL_CONNECTED){
    delay(420);
    Serial.print(">");
  }

  Serial.println();
  Serial.println("Connected to WiFi!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void setupPubSub() {
  pubSubClient.setServer(MQTT_SERVER, 1883);
  pubSubClient.setCallback(callback);
}

void reconnectToPubSub() {
  while(!pubSubClient.connected()) {
    Serial.println("MQTT Connecting...");
    if (pubSubClient.connect("ESP32Client")) {
      Serial.println("Connected to MQTT!");
      pubSubClient.subscribe("esp32/output");
    } else {
      Serial.println("Failed to connect to MQTT");
      Serial.print("State: ");
      Serial.println(pubSubClient.state());
      Serial.println("Trying again in 5 seconds...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  setupWifi();
  setupPubSub();

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  delay(1500);
}

void loop() {

  if (!pubSubClient.connected()){
    reconnectToPubSub();
  }
  pubSubClient.loop();

  if (isGreen) {
    digitalWrite(LED_GREEN, LOW);
  } else {
    digitalWrite(LED_GREEN, HIGH);
  }

  if (isRed) {
    digitalWrite(LED_RED, LOW);
  } else {
    digitalWrite(LED_RED, HIGH);
  }
  
}