#ifndef WIFI_MQTT_H
#define WIFI_MQTT_H

#include <globals.h>

extern const char* ssid;
extern const char* password;
extern const char* mqtt_server;
extern WiFiClient espClient;
extern PubSubClient client;

void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void mqtt_loop();


#endif // WIFI_MQTT_H
