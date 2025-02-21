#include "USB.h"
#include "globals.h"

// Main USB function to handle serial input and commands
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

// Accumulates characters from the serial input into a string
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

// Resets the serial input string and prints the prompt
void reset_serial() 
{
  inputString = "";
  Serial.print(prompt);
  stringComplete = false;
}

// Resets the serial input string without printing the prompt
void reset_serial_no_prompt() 
{
  inputString = "";
  stringComplete = false;
}

// Handles input string accumulation in a module until completion
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

// Prints a specified number of newline characters
void spaces(int num) 
{
    for (int i = 0; i< num; i++)
    {
  Serial.println();
    }
}