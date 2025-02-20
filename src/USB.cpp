#include "USB.h"
#include "globals.h"

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

void spaces(int num) 
{
    for (int i = 0; i< num; i++)
    {
  Serial.println();
    }
}