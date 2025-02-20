#include "globals.h"

unsigned long last_print_time = millis();

void (*softReset) (void) = 0; // Declare Software Reset Function @ address 0

int inByte = -1; // incoming byte from serial RX

String inputString = ""; // a string to hold incoming data
String prompt = "AOS>"; // String to hold prompt

boolean stringComplete = false; // whether the string is complete

byte flag = 0; // Declare Error handling flag
byte done = 0; // Declare done flag
long baud = 0; // Initialize Baud Rate Variable
byte valid = 0; // check to see if something is valid