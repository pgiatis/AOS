#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <SPI.h>														// Include SPI communications library
#include <SD.h>															// Include SD Card library
#include <EEPROM.h>												  // Include EEPROM library for handling EEPROM communications
#include <WiFi.h>
#include <HTTPClient.h>
#include <esp_wifi.h>
#include <Wire.h>												  // Include WIRE library for handling I2C communications
#include <PubSubClient.h>
#include "commands.h"
#include "USB.h"
#include "Wifi_MQTT.h" 
#include <ArduinoJson.h>

extern const char* ssid;
extern const char* password;
extern const char* mqtt_server;

extern WiFiClient espClient;
extern PubSubClient client;
extern unsigned long lastMsg;
#define MSG_BUFFER_SIZE (50)
#define MQTT_PORT 1883

extern char msg[MSG_BUFFER_SIZE];
extern int value;

extern unsigned long last_print_time;

extern void (*softReset) (void);

extern int inByte;

extern String inputString;
extern String prompt;
extern String command_message;

extern boolean stringComplete;

extern boolean change;

extern byte flag;
extern byte done;
extern long baud;
extern byte valid;

//function prototypes
void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);
void USB();
void inputString_module();
void reset_serial_no_prompt();
void reset_serial();
void serial_accum();
void spaces(int num);
void serial_menu();
void read_pin();
void read_adc();
void pwm_pin();
void toggle_pin();
void set_baud();
void write_eprom();
void eprom_all();
void eprom_rd();
void eprom_routine();
void sysinfo();
void serial_commands();
String getMacAddress();
void messageReceived(String &topic, String &payload);
void mqttConnect();
void mqttService();
void SendInfoCallback();
void SendInfoMQTT();


#endif // GLOBALS_H
