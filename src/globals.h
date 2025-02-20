#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <EEPROM.h>												  // Include EEPROM library for handling EEPROM communications
#include <Wire.h>												  // Include WIRE library for handling I2C communications
#include "commands.h"
#include "USB.h"

extern unsigned long last_print_time;

extern void (*softReset) (void);

extern int inByte;

extern String inputString;
extern String prompt;

extern boolean stringComplete;

extern byte flag;
extern byte done;
extern long baud;
extern byte valid;

//function prototypes

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

#endif // GLOBALS_H