#include "echoData.h"

void echoToWebSerialLn(const String &message) 
{
  WebSerial.println(message);
}

// Overload for char array
void echoToWebSerialLn(const char *message) 
{
  WebSerial.println(message);
}

// Overload for integer
void echoToWebSerialLn(int num) 
{
  WebSerial.println(num);
}

// Overload for float
void echoToWebSeriaLn(float num) 
{
  WebSerial.println(num);
}

// Overload for char array
void echoToWebSerial(const char *message) 
{
  WebSerial.print(message);
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

void echoToWebSerial(const String &message) 
{
  WebSerial.print(message);
}

void SerialPrintln(String message) 
{
  echoToWebSerialLn(message);
  Serial.println(message);
}

void SerialPrintln() 
{
  String message = "";
  echoToWebSerialLn(message);
  Serial.println(message);
}

void SerialWrite(byte message) 
{
  echoToWebSerial(message);
  Serial.write(message);
}

void SerialWrite() 
{
  String message = "";
  echoToWebSerial(message);
  Serial.write(message.c_str());
}

void SerialPrint(String message) 
{
  echoToWebSerial(message);
  Serial.print(message);
}

void SerialPrint() 
{
  String message = "";
  echoToWebSerial(message);
  Serial.print(message);
}
