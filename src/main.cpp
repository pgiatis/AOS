/*
Arduino OS (AOS)- Written By Petros Z Giatis

This Software Allows the user to use the computer like a computer operating system.

The following OS commands are supported

- Read / Write to various EEPROM addresses
- Read ADC Ports
- Reset System

EEPROM Address MAP

Location        Description
--------        ----------
1               Baud Rate : 1)1200 2)2400 3)4800 4)9600 5)14400 6)19200 7)38400 8)115200
*/

#include "globals.h"

void setup() 
{
  switch (EEPROM.read(1)) 
  {
    case 1: baud = 1200; break;
    case 2: baud = 2400; break;
    case 3: baud = 4800; break;
    case 4: baud = 9600; break;
    case 5: baud = 14400; break;
    case 6: baud = 19200; break;
    case 7: baud = 38400; break;
    case 8: baud = 115200; break;
    default: baud = 9600; EEPROM.write(1, 8);
  }

  baud = 115200;

  Serial.begin(baud);

  setup_wifi();

  client.setServer(mqtt_server, MQTT_PORT);

  client.setCallback(callback);

  mqttConnect();

  spaces(1);
  Serial.println("AOS Serial Interface");
  Serial.print("Baud Rate Set to:"); Serial.println(baud);
  serial_menu();
  Serial.print(prompt);
}

void loop() 
{
  USB(); // Use the USB function from the new library
  mqttService();                                                              // Handle MQTT data
}