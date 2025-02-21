#include "commands.h"
#include "globals.h"

// Displays the serial menu with available commands
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

// Reads the state of a specified pin
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

// Reads the value of a specified ADC channel and converts it to voltage
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

// Sets the PWM duty cycle for a specified pin
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

// Toggles the state of a specified pin
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

// Sets the baud rate for serial communication
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

// Writes a value to a specified EEPROM address
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

// Reads all values from EEPROM and prints them
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

// Reads a value from a specified EEPROM address
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

// Handles EEPROM read commands, either a specific address or all addresses
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

// Displays system information
void sysinfo() 
{
    Serial.println("");
    Serial.println("System Information");
    Serial.println("Software Firmware Version 1.0");
    Serial.print("Baud Rate Set to: "); Serial.println(baud);
    Serial.println("");
    flag = 1;
}

// Processes serial commands and executes the corresponding functions
void serial_commands() 
{
    inputString.toLowerCase();
    inputString.trim();
    flag = 0;
  
    if (inputString == "?") { serial_menu(); command_message ="Serial Menu"; change = true;}
    if (inputString == "reset") { softReset(); }
    if (inputString == "re") { eprom_routine(); command_message ="Read EEPROM"; change = true;}
    if (inputString == "we") { write_eprom(); command_message ="Write EEPROM"; change = true;}
    if (inputString == "sb") { set_baud(); command_message ="Set Baud"; change = true;}
    if (inputString == "tp") { toggle_pin(); command_message ="Toggle Pin"; change = true;}
    if (inputString == "pwm") { pwm_pin(); command_message ="PWM Pin"; change = true;}
    if (inputString == "ra") { read_adc(); command_message ="Read ADC"; change = true;}
    if (inputString == "rp") { read_pin(); command_message = "Read Pin"; change = true;}
    if (inputString == "si") { sysinfo(); command_message ="System Information"; change = true;}
    if (inputString == "") { flag = 1; }
  
    if (flag == 0) { Serial.println("Invalid Command");command_message ="Invalid Command"; change = true; }
}
