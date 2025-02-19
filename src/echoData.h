#ifndef ECHODATA_H
#define ECHODATA_H

#include <WebSerial.h>

void echoToWebSerial(const String &message);
void echoToWebSerial(const char *message);
void echoToWebSerial(char c);
void echoToWebSerial(byte b);
void echoToWebSerial(int num);
void echoToWebSerial(float num);

void SerialPrintln(String message);
void SerialWrite(byte message);
void SerialPrint(String message);

#endif // ECHODATA_H
