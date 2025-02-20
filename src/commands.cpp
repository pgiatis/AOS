#include "commands.h"
#include "globals.h"

void serial_menu() 
{
  spaces(1);
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
  Serial.println("SI    = System information");
  Serial.println("RESET = Reset Microcontroller");
  spaces(1);
  flag = 1;
}

void read_pin() 
{
    byte done = 0;
    reset_serial_no_prompt();
    spaces(1);
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
    spaces(1);
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
    spaces(1);
    Serial.write("Please Select Pin To PWM:");
    inputString_module();
    inputString.toLowerCase();
    pin = inputString.toInt();
  
    reset_serial_no_prompt();
    spaces(1);
    Serial.write("Please enter duty cycle (0-255):");
    inputString_module();
    inputString.toLowerCase();
    state = inputString.toInt();
  
    analogWrite(pin, state);
    spaces(1);
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