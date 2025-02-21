#ifndef WIFI_MQTT_H
#define WIFI_MQTT_H

#include <globals.h>

extern const char* ssid;
extern const char* password;
extern const char* mqtt_server;
extern WiFiClient espClient;
extern PubSubClient client;

String getMacAddress();
void setup_wifi();
void messageReceived(String &topic, String &payload);
void mqttConnect();
void mqttService();
void SendInfoCallback();
void SendInfoMQTT();
void callback(char* topic, byte* payload, unsigned int length);


#endif // WIFI_MQTT_H
