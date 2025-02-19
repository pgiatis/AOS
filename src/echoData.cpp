#include "echoData.h"

void echoToWebSerial(const String &message) 
{
  WebSerial.println(message);
}

// Overload for char array
void echoToWebSerial(const char *message) 
{
  WebSerial.println(message);
}

// Overload for single character
void echoToWebSerial(char c) 
{
  WebSerial.print(c);
}

// Overload for byte
void echoToWebSerial(byte b) 
{
  WebSerial.print(b);
}

// Overload for integer
void echoToWebSerial(int num) 
{
  WebSerial.println(num);
}

// Overload for float
void echoToWebSerial(float num) 
{
  WebSerial.println(num);
}

void SerialPrintln(String message) 
{
  echoToWebSerial(message);
  Serial.println(message);
}

void SerialWrite(byte message) 
{
  echoToWebSerial(message);
  Serial.write(message);
}

void SerialPrint(String message) 
{
  echoToWebSerial(message);
  Serial.print(message);
}
