#ifndef ECHODATA_H
#define ECHODATA_H

#include <WebSerial.h>

void echoToWebSerialLn(const String &message);
void echoToWebSerialLn(const char *message);
void echoToWebSerialLn(int num);
void echoToWebSeriaLn(float num);
void echoToWebSerial(const char *message);
void echoToWebSerial(char c);
void echoToWebSerial(byte b);
void echoToWebSerial(const String &message); 
void SerialPrintln(String message);
void SerialPrintln();
void SerialWrite(byte message);
void SerialWrite();
void SerialPrint(String message);
void SerialPrint();

#endif // ECHODATA_H
