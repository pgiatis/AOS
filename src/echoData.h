#ifndef ECHODATA_H
#define ECHODATA_H

#include <WebSerial.h>

template <typename T>
inline void SerialPrint(T data) 
{
    String message = String(data);
    WebSerial.print(message);
    Serial.print(message);
}

inline void SerialPrint() 
{
    String message = "";
    WebSerial.print(message);
    Serial.print(message);
}

template <typename T>
inline void SerialPrintln(T data) 
{
    String message = String(data);
    WebSerial.println(message);
    Serial.println(message);
}

inline void SerialPrintln() 
{
    String message = "";
    WebSerial.println(message);
    Serial.println(message);
}

template <typename T>
inline void SerialWrite(T data) 
{
    uint8_t message = static_cast<uint8_t>(data);
    WebSerial.write(message);
    Serial.write(message);
}

inline void SerialWrite() 
{
    String message = "";
    WebSerial.print(message);
    Serial.write(message.c_str());
}

#endif // ECHODATA_H
