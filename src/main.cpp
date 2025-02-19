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
//#include <Wire.h>												  // Include WIRE library for handling I2C communications

AsyncWebServer server(80);

const char* ssid = "petropower"; // Your WiFi SSID
const char* password = ""; // Your WiFi Password

unsigned long last_print_time = millis();

void (*softReset) (void) = 0;							  // Declare Software Reset Function @ address 0 ... This "Software" resets the microcontroller

int inByte= -1;													   // incoming byte from serial RX

String inputString = "";							     // a string to hold incoming data
String prompt ="AOS>";									 	 // String to hold prompt

boolean stringComplete = false;						 // whether the string is complete

byte flag = 0;														 // Declare Error handling flag
byte done = 0;														 // Declare done flag
long baud = 0;														 // Initialize Baud Rate Variable
byte valid = 0;														 //check to see if something is valid

void spaces() 
{
  Serial.println('\n');
}

void serial_menu() 
{
  spaces();
  Serial.println("***********************************");
  Serial.println("********** AOS Commands ***********");
  Serial.println("***********************************");
  Serial.println("?     = This menu");
  Serial.println("RE    = Read EEPROM");
  Serial.println("WE    = Write EEPROM");
  Serial.println("SB    = Set Baud Rate");
  Serial.println("TP    = Toggle Pin");
  Serial.println("PWM   = Pulse width module Pin");
  Serial.println("RA    = Read ADC Channel");
  Serial.println("RP    = Read Pin");
  Serial.println("DSD   = Directory SD Card");
  Serial.println("RSD   = Read SD Card");
  Serial.println("WSD   = Write SD Card");
  Serial.println("ESD   = Erase SD Card File");
  Serial.println("SI    = System information");
  Serial.println("RESET = Reset Microcontroller");
  spaces();
  flag = 1;
}

void serial_accum() 
{
  char inChar = (char)Serial.read();

  if (inChar == 8 && inputString.length() > 0) 
  {
    Serial.write(inChar);
    Serial.write(" ");
    Serial.write(8);
    inputString = inputString.substring(0, inputString.length() - 1);
  }
  if (inChar == 13) 
  {
    Serial.write(inChar);
    Serial.write(char(10));
    stringComplete = true;
  }

  if (inChar != 13 && inChar != 8) 
  {
    Serial.write(inChar);
    inputString += inChar;
  }
}

void reset_serial() 
{
  inputString = "";
  Serial.print(prompt);
  stringComplete = false;
}

void reset_serial_no_prompt() 
{
  inputString = "";
  stringComplete = false;
}

void inputString_module() 
{
  done = 0;
  inputString = "";

  while (done < 1) 
  {
    while (Serial.available() > 0) 
    {
      serial_accum();
      if (stringComplete == true) 
      {
        done = 1;
      }
    }
  }
}

void read_pin() 
{
  byte done = 0;
  reset_serial_no_prompt();
  spaces();
  Serial.write("Please Select Pin To Read:");
  inputString_module();
  int value = inputString.toInt();
  value = digitalRead(value);
  Serial.print("Read: "); Serial.println(value); Serial.println();
  reset_serial_no_prompt();
  flag = 1;
}

void read_adc() 
{
  byte done = 0;
  reset_serial_no_prompt();
  spaces();
  Serial.write("Please Select ADC Channel To Read:");
  inputString_module();
  int sensorValue = analogRead(inputString.toInt());
  float voltage = sensorValue * (5.0 / 1023.0);
  Serial.print("ADC Value Read: "); Serial.print(sensorValue);
  Serial.print(" Converted to Voltage: "); Serial.println(voltage);
  Serial.println();
  reset_serial_no_prompt();
  flag = 1;
}

void pwm_pin() 
{
  byte pin = 0;
  byte state = 0;
  reset_serial_no_prompt();
  spaces();
  Serial.write("Please Select Pin To PWM:");
  inputString_module();
  inputString.toLowerCase();
  pin = inputString.toInt();

  reset_serial_no_prompt();
  spaces();
  Serial.write("Please enter duty cycle (0-255):");
  inputString_module();
  inputString.toLowerCase();
  state = inputString.toInt();

  analogWrite(pin, state);
  spaces();
  reset_serial_no_prompt();
  flag = 1;
}
void toggle_pin() 
{
  byte pin = 0;
  byte state = 0;
  reset_serial_no_prompt();
  Serial.println("");
  Serial.write("Please Select Pin To Toggle:");
  inputString_module();
  inputString.toLowerCase();
  pin = inputString.toInt();

  reset_serial_no_prompt();
  Serial.println("");
  Serial.write("Please enter 1 for High or 0 for Low:");
  inputString_module();
  inputString.toLowerCase();
  state = inputString.toInt();

  digitalWrite(pin, state);
  Serial.println("");
  reset_serial_no_prompt();
  flag = 1;
}

void set_baud() 
{
  byte check = 0;
  reset_serial_no_prompt();
  Serial.println(" ");
  Serial.println("1) 1200");
  Serial.println("2) 2400");
  Serial.println("3) 4800");
  Serial.println("4) 9600");
  Serial.println("5) 14400");
  Serial.println("6) 19200");
  Serial.println("7) 38400");
  Serial.println("8) 115200");
  Serial.println("");
  Serial.write("Please select(1-8):");
  while (check < 1) 
  {
    inputString_module();
    inputString.toLowerCase();
    if (inputString.toInt() < 1 || inputString.toInt() > 8) 
    {
      Serial.println("Invalid selection");
      Serial.write("Please select(1-8):");
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
  Serial.println();
  reset_serial_no_prompt();
  flag = 1;
}



void write_eprom() 
{
  int ADDRESS = 0;
  byte VAL = 0;
  reset_serial_no_prompt();
  Serial.println("");
  Serial.write("EEPROM Address:");
  inputString_module();
  inputString.toLowerCase();
  ADDRESS = inputString.toInt();
  reset_serial_no_prompt();

  Serial.println();
  Serial.write("EEPROM Value:");
  inputString_module();
  inputString.toLowerCase();
  VAL = inputString.toInt();

  EEPROM.write(ADDRESS, VAL);
  Serial.println("");
  reset_serial_no_prompt();
  flag = 1;
}

void eprom_all() 
{
  Serial.println(); Serial.println("Reading ALL");
  byte VAL;
  for (int i = 0; i <= 4096; i++) 
  {
    VAL = EEPROM.read(i);
    Serial.print("EEPROM Address: "); Serial.print(i); Serial.print("\t"); Serial.print("Value: "); Serial.print(VAL); Serial.println();
    delay(100);
  }
  Serial.println();
}

void eprom_rd() 
{
  byte VAL;
  Serial.println();
  int a = inputString.toInt();
  VAL = EEPROM.read(a);

  Serial.print("EEPROM Address: ");
  Serial.print(a);
  Serial.print("\t");
  Serial.print("Value: ");
  Serial.print(VAL);
  Serial.println();
  Serial.println();

  flag = 1;
}

void eprom_routine() 
{
  reset_serial_no_prompt();
  Serial.println("");
  Serial.write("EEPROM Address (0-4096 or all):");
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
  flag = 1;
}

void sysinfo() 
{
  Serial.println("");
  Serial.println("System Information");
  Serial.println("Software Firmware Version 1.0");
  Serial.print("Baud Rate Set to: "); Serial.println(baud);
  Serial.println("");
  flag = 1;
}

void serial_commands() 
{
  inputString.toLowerCase();
  flag = 0;

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
  if (inputString == "") { flag = 1; }

  if (flag == 0) { Serial.println("Invalid Command"); }
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
    Serial.println("WiFi Failed!");
    return;
  }

  Serial.print("IP Address: ");
  Serial.printf("%s", WiFi.localIP().toString().c_str());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) 
  {
    request->send(200, "text/plain", "Hi! This is WebSerial demo. You can access webserial interface at http://" + WiFi.localIP().toString() + "/webserial");
  });

  WebSerial.begin(&server);

  WebSerial.onMessage([&](uint8_t *data, size_t len) 
  {
    Serial.printf("Received %u bytes from WebSerial: ", len);
    Serial.write(data, len);
    inputString = String((char*)data);
    stringComplete = true;
    serial_commands();
    Serial.println("");
    WebSerial.println("Received Data...");
    String d = "";
    for (size_t i = 0; i < len; i++) 
    {
      d += char(data[i]);
    }
    WebSerial.println(d);
  });

  server.begin();
  spaces();
  Serial.println("AOS Serial Interface");
  Serial.print("Baud Rate Set to:"); Serial.println(baud);
  serial_menu();
  Serial.print(prompt);
}

void loop() 
{
  USB();
  WebSerial.loop();
}