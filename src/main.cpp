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
#include <SPI.h>														// Include SPI communications library
#include <SD.h>															// Include SD Card library
#include <EEPROM.h>												  // Include EEPROM library for handling EEPROM communications
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>
#include <AsyncTCP.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <esp_wifi.h>
#include "echoData.h"

AsyncWebServer server(80);

const char* ssid = "petropower";           // Your WiFi SSID
const char* password = "";                 // Your WiFi Password

unsigned long last_print_time = millis();

void (*softReset) (void) = 0;							 // Declare Software Reset Function @ address 0 ... This "Software" resets the microcontroller

int inByte= -1;													   // incoming byte from serial RX

String inputString = "";							     // a string to hold incoming data
String prompt ="AOS>";									 	 // String to hold prompt

boolean stringComplete = false;						 // whether the string is complete

boolean valid = false;										 // check to see if something is valid

int baud = 0;

void print_blank_spaces(int spaces) 
{
  for (int i = 0; i < spaces; i++)
  {
   SerialPrintln();
  }
}

void serial_menu() 
{
  print_blank_spaces(1);
  SerialPrintln("***********************************");
  SerialPrintln("********** AOS Commands ***********");
  SerialPrintln("***********************************");
  SerialPrintln("?     = This menu");
  SerialPrintln("RE    = Read EEPROM");
  SerialPrintln("WE    = Write EEPROM");
  SerialPrintln("SB    = Set Baud Rate");
  SerialPrintln("TP    = Toggle Pin");
  SerialPrintln("PWM   = Pulse width modulate Pin");
  SerialPrintln("RA    = Read ADC Channel");
  SerialPrintln("RP    = Read Pin");
  SerialPrintln("SI    = System information");
  SerialPrintln("RESET = Reset Microcontroller");
  SerialPrintln("RSSI  = Read WiFi Signal Strength");
  print_blank_spaces(1);
  valid = true;
}

void serial_accum() 
{
  char inChar = (char)Serial.read();

  if (inChar == 8 && inputString.length() > 0) 
  {
    SerialWrite(inChar);
    SerialPrint(" ");
    SerialWrite(8);
    inputString = inputString.substring(0, inputString.length() - 1);
  }

  if (inChar == 13) 
  {
    SerialWrite(inChar);
    SerialWrite(char(10));
    stringComplete = true;
  }

  if (inChar != 13 && inChar != 8) 
  {
    SerialWrite(inChar);
    inputString += inChar;
  }
}

void reset_serial() 
{
  inputString = "";
  SerialPrint(prompt);
  stringComplete = false;
}

void reset_serial_no_prompt() 
{
  inputString = "";
  stringComplete = false;
}

void inputString_module() 
{
  boolean complete = false;
  inputString = "";

  while (complete != true) 
  {
    WebSerial.onMessage([&](uint8_t *data, size_t len) 
    {
      inputString = String((char*)data);
      SerialPrintln(inputString);
      stringComplete = true;
      complete = true;
    });

    while (Serial.available() > 0) 
    {
      serial_accum();
      if (stringComplete == true) 
      {
        complete = true;
      }
    }
  }
}

void read_pin() 
{
  byte done = 0;
  reset_serial_no_prompt();
  print_blank_spaces(1);
  SerialPrint("Please Select Pin To Read:");
  inputString_module();
  int value = inputString.toInt();
  value = digitalRead(value);
  SerialPrint("Read: "); 
  SerialPrintln(String(value)); 
  SerialPrintln();
  reset_serial_no_prompt();
  valid = true;
}

void read_adc() 
{
  byte done = 0;
  reset_serial_no_prompt();
  print_blank_spaces(1);
  SerialPrint("Please Select ADC Channel To Read:");
  inputString_module();
  int sensorValue = analogRead(inputString.toInt());
  float voltage = sensorValue * (5.0 / 1023.0);
  SerialPrint("ADC Value Read: "); SerialPrint(String(sensorValue));
  SerialPrint(" Converted to Voltage: "); SerialPrintln(String(voltage, 2));
  SerialPrintln();
  reset_serial_no_prompt();
  valid = true;
}

void pwm_pin() 
{
  byte pin = 0;
  byte state = 0;
  reset_serial_no_prompt();
  print_blank_spaces(1);
  SerialPrint("Please Select Pin To PWM:");
  inputString_module();
  inputString.toLowerCase();
  pin = inputString.toInt();
  reset_serial_no_prompt();
  print_blank_spaces(1);
  SerialPrint("Please enter duty cycle (0-255):");
  inputString_module();
  inputString.toLowerCase();
  state = inputString.toInt();
  analogWrite(pin, state);
  print_blank_spaces(1);
  reset_serial_no_prompt();
  valid = true;
}
void toggle_pin() 
{
  byte pin = 0;
  byte state = 0;
  reset_serial_no_prompt();
  SerialPrintln();
  SerialPrint("Please Select Pin To Toggle:");
  inputString_module();
  inputString.toLowerCase();
  pin = inputString.toInt();
  reset_serial_no_prompt();
  SerialPrintln();
  SerialPrint("Please enter 1 for High or 0 for Low:");
  inputString_module();
  inputString.toLowerCase();
  state = inputString.toInt();
  digitalWrite(pin, state);
  SerialPrint();
  reset_serial_no_prompt();
  valid = true;
}

void set_baud() 
{
  byte check = 0;
  reset_serial_no_prompt();
  SerialPrintln();
  SerialPrintln("1) 1200");
  SerialPrintln("2) 2400");
  SerialPrintln("3) 4800");
  SerialPrintln("4) 9600");
  SerialPrintln("5) 14400");
  SerialPrintln("6) 19200");
  SerialPrintln("7) 38400");
  SerialPrintln("8) 115200");
  SerialPrintln();
  SerialPrint("Please select(1-8):");
  while (check < 1) 
  {
    inputString_module();
    inputString.toLowerCase();
    if (inputString.toInt() < 1 || inputString.toInt() > 8) 
    {
      SerialPrintln("Invalid selection");
      SerialPrint("Please select(1-8):");
      check = 0;
      reset_serial_no_prompt();
    } 
    else 
    {
      check = 1;
    }
  }
  EEPROM.write(1, inputString.toInt());
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
  }
  SerialPrintln();
  reset_serial_no_prompt();
  valid = true;
}

void write_eprom() 
{
  int ADDRESS = 0;
  byte VAL = 0;
  reset_serial_no_prompt();
  SerialPrintln();
  SerialPrint("EEPROM Address:");
  inputString_module();
  inputString.toLowerCase();
  ADDRESS = inputString.toInt();
  reset_serial_no_prompt();
  SerialPrintln();
  SerialPrint("EEPROM Value:");
  inputString_module();
  inputString.toLowerCase();
  VAL = inputString.toInt();
  EEPROM.write(ADDRESS, VAL);
  SerialPrintln();
  reset_serial_no_prompt();
  valid = true;
}

void eprom_all() 
{
  SerialPrintln(); 
  SerialPrintln("Reading ALL");
  byte VAL;
  for (int i = 0; i <= 4096; i++) 
  {
    VAL = EEPROM.read(i);
    SerialPrint("EEPROM Address: "); 
    SerialPrint(String(i)); 
    SerialPrint("\t"); 
    SerialPrint("Value: "); 
    SerialPrint(String(VAL)); 
    SerialPrintln("");
    delay(100);
  }
  SerialPrintln();
}

void eprom_rd() 
{
  byte VAL;
  SerialPrintln();
  int a = inputString.toInt();
  VAL = EEPROM.read(a);

  SerialPrint("EEPROM Address: ");
  SerialPrint(String(a));
  SerialPrint("\t");
  SerialPrint("Value: ");
  SerialPrint(String(VAL));
  print_blank_spaces(2);

  valid = true;
}

void eprom_routine() 
{
  reset_serial_no_prompt();
  SerialPrintln();
  SerialPrint("EEPROM Address (0-4096 or all):");
  inputString_module();
  inputString.toLowerCase();

  if (inputString == "all") 
  {
    eprom_all();
  } 
  else 
  {
    eprom_rd();
  }
  reset_serial_no_prompt();
  valid = true;
}

void sysinfo() 
{
  SerialPrintln();
  SerialPrintln("System Information");
  SerialPrintln("Software Firmware Version 1.0");
  SerialPrint("Baud Rate Set to: "); 
  SerialPrintln(String(baud));
  SerialPrintln();
  valid = true;
}

void read_wifi_signal() 
{
  long rssi = WiFi.RSSI();
  SerialPrint("WiFi Signal Strength (RSSI): ");
  SerialPrintln(String(rssi) + " dBm");
  valid = true;
}

void serial_commands() 
{
  inputString.toLowerCase();
  inputString.trim();
  valid = false;

  if (inputString == "?") { serial_menu(); }
  if (inputString == "reset") { softReset(); }
  if (inputString == "re") { eprom_routine(); }
  if (inputString == "we") { write_eprom(); }
  if (inputString == "sb") { set_baud(); }
  if (inputString == "tp") { toggle_pin(); }
  if (inputString == "pwm") { pwm_pin(); }
  if (inputString == "ra") { read_adc(); }
  if (inputString == "rp") { read_pin(); }
  if (inputString == "si") { sysinfo(); }
  if (inputString == "rssi") { read_wifi_signal(); }
  if (inputString == "") { valid = true; }

  if (valid != true) { SerialPrintln("Invalid Command"); }
}

void USB() 
{
  while (Serial.available() > 0) 
  {
    serial_accum();
  }

  if (stringComplete == true) 
  {
    serial_commands();
    reset_serial();
  }
}
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
    default: baud = 9600; EEPROM.write(1, 4);
  }

  baud = 115200;
  Serial.begin(baud);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid);

  if (WiFi.waitForConnectResult() != WL_CONNECTED) 
  {
    SerialPrintln("WiFi Failed!");
    return;
  }

  SerialPrint("IP Address: ");
  SerialPrintln(WiFi.localIP().toString());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) 
  {
    request->send(200, "text/plain", "Access webserial interface at http://" + WiFi.localIP().toString() + "/webserial");
  });

  WebSerial.begin(&server);

  WebSerial.onMessage([&](uint8_t *data, size_t len) 
  {
    //Serial.printf("Received %u bytes from WebSerial: ", len);
    //Serial.write(data, len);
    inputString = String((char*)data);
    stringComplete = true;
    serial_commands();
    reset_serial();
    //SerialPrintln("");
    //WebSerial.println("Received Data...");
    //String d = "";
    //for (size_t i = 0; i < len; i++) 
    //{
      //d += char(data[i]);
    //}
    //WebSerial.println(d);
  });

  server.begin();
  print_blank_spaces(1);
  SerialPrintln("AOS Serial Interface");
  SerialPrint("Baud Rate Set to:"); 
  SerialPrintln(String(baud));
  serial_menu();
  SerialPrint(prompt);
}

void loop() 
{
  USB();
  //WebSerial.loop();
}