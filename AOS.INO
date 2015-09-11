/*

Arduino OS (AOS)- Written By Petros Z Giatis

This Software Allows the user to use the computer like a computer operating system.

The following OS commands are supported

- Read / Write to various EEPROM addresses
- Read ADC Ports
- Reset System


EEPROM Address MAP

Location        Description
--------        ----------
1               Baud Rate : 1)1200 2)2400 3)4800 4)9600 5)14400 6)19200 7)38400 8)115200
*/
#include <SPI.h>														// Include SPI communications library
#include <SD.h>															// Include SD Card library
#include <EEPROM.h>												  // Include EEPROM library for handling EEPROM communications

//#include <Wire.h>												  // Include WIRE library for handling I2C communications

void (*softReset) (void) = 0;							  // Declare Software Reset Function @ address 0 ... This "Software" resets the microcontroller


File myFile;															 // set up variables using the SD utility library functions:

Sd2Card card;															 // For SD Card
SdVolume volume;													 // For SD Card
SdFile root;															 // For SD Card
const int chipSelect = 4;									 // For SD Card

int inByte= -1;													   // incoming byte from serial RX

String inputString = "";							     // a string to hold incoming data
String prompt ="AOS>";									 	 // String to hold prompt

boolean stringComplete = false;						 // whether the string is complete

byte flag = 0;														 // Declare Error handling flag
byte done = 0;														 // Declare done flag
long baud = 0;														 // Initialize Baud Rate Variable
byte valid = 0;														 //check to see if something is valid
void setup()
{
// pinMode(53, OUTPUT);										 // Need to have 53 as output on Mega
if (!SD.begin(chipSelect))								 // Check for SD card
{Serial.println("SD Card initialization failed!");}						// Print that the SD Card failed to initialize
  
switch (EEPROM.read(1))													  // Read EEPROM location 1
{
 case 1:baud = 1200; break;												// Set Baud Rate to 1200 baud
 case 2:baud = 2400; break;												// Set Baud Rate to 2400 baud
 case 3:baud = 4800; break;												// Set Baud Rate to 4800 baud
 case 4:baud = 9600; break;												// Set Baud Rate to 9600 baud
 case 5:baud = 14400; break;											// Set Baud Rate to 14400 baud
 case 6:baud = 19200; break;											// Set Baud Rate to 19200 baud
 case 7:baud = 38400; break;											// Set Baud Rate to 38400 baud
 case 8:baud = 115200; break;											// Set Baud Rate to 115200 baud
 default:baud = 9600;EEPROM.write(1,4);						// If none of these conditions are met then Set Baud Rate to 9600 baud
}
 Serial.begin(baud);													// USB serial Port start with baud rate
 spaces();																// Add three line feeds 
 Serial.println("AOS Serial Interface");								// Introductory Message
 Serial.print("Baud Rate Set to:"); Serial.println(baud);				// Displays the baud Rate That was selected
 serial_menu();															// Show the user the available commands
 Serial.print(prompt);													// Initial Prompt
}   

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////  Main Program Loop/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
 USB();																	//SCAN USB Port
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void USB()
{
 while (Serial.available() > 0)											// Scan serial port for activity
 { 
  serial_accum();														// Accumulate serial data
 }
      
 if (stringComplete == true)											// Checks to see if we have completed text
 {
 serial_commands();														// Go To serial commands sub
 reset_serial();														// Reset data once I am done doing something with it
 }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void serial_commands() //Check serial command inputs
{
 inputString.toLowerCase();												// Converts the inputstring to lowercase - easier to handle in if then statements
 
 flag = 0;																// Set good flag to zero

 if (inputString == "?"){serial_menu();}								// Displays Menu Commands
        
 if (inputString == "reset"){softReset();}								// Resets uC
        
 if (inputString == "re"){eprom_routine();}								// Read From EEPROM
       
 if (inputString == "we"){write_eprom();}								// Write To EEPROM
        
 if (inputString == "sb"){set_baud();}									// Set Baud Rate
        
 if (inputString == "dsd"){directory_sd();}								// directory SD card
        
 if (inputString == "rsd"){read_sd_file();}								// Read SD card
        
 if (inputString == "wsd"){write_sd_file();}							// Write SD card
        
 if (inputString == "esd"){delete_sd_file();}							// Erase SD card File
        
 if (inputString == "tp"){toggle_pin();}								// Toggle Pin

 if (inputString == "pwm"){pwm_pin();}									// PWM Pin
        
 if (inputString == "ra"){read_adc();}									// Read ADC voltage
        
 if (inputString == "rp"){read_pin();}									// Read pin
       
 if (inputString == "si"){sysinfo();}									// Displays System Information
        
 if (inputString == ""){flag = 1;}										// If carriage return is just pressed

 if (flag == 0){Serial.println("Invalid Command");}						// If entry is not valid
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void spaces()// Adding Spaces in the serial port
{
 Serial.println('\n');													// Line Feed
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void serial_menu()//Main Serial Menu
{
 spaces();																// Prints some blank spaces
 
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
 Serial.println("DSD   = Directory SD Card");
 Serial.println("RSD   = Read SD Card");
 Serial.println("WSD   = Write SD Card");
 Serial.println("ESD   = Erase SD Card File");
 Serial.println("SI    = System information");
 Serial.println("RESET = Reset Microcontroller"); 

 spaces();																// Print some spaces
 flag = 1;																// Set flag to 1
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void serial_accum() // This function accumulated Serial input data for reading back
{
 char inChar = (char)Serial.read();										// Flush serial data into variable inChar

 if (inChar == 8 && inputString.length() > 0)							// Check for backspace
 {
  Serial.write(inChar);													// Echo Data Back to serial port
  Serial.write(" ");													// Echo Blank Data Back To Serial Port
  Serial.write(8);														// Echo backspace Back To Serial Port
  inputString = inputString.substring(0, inputString.length() - 1);		// Subtract character from string object     
 }
 if (inChar == 13)                                                      // Check for carriage return
 {
  Serial.write(inChar);                                                 // Echo Data Back to serial port
  Serial.write(char(10));                                               // Write a line feed and set stringComplete to true
  stringComplete = true;                                                // stringComplete becomes True
 }
    
 if (inChar != 13 && inChar != 8)                                       // If both carriage return and backspace weren't pressed then do the following
 {
  Serial.write(inChar);                                                 // Echo Data Back to serial port
  inputString += inChar;                                                // Accumulate Data into inputString
  }                         
}   
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void reset_serial() // Clear Serial output with prompt
{
 inputString = "";                                                      // clear the string:
 Serial.print(prompt);                                                  // Print prompt
 stringComplete = false;                                                // Reset 
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void reset_serial_no_prompt() //Clear Serial output with no prompt
{
 inputString = "";                                                      // clear the string:
 stringComplete = false;                                                // Reset 
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void read_pin() // Read Arduino Pin
{          
 byte done = 0;															// Declare done variable
 reset_serial_no_prompt();												// Reset Serial port input variable without re-displaying prompt
 spaces();																// Prints some blank spaces
 Serial.write("Please Select Pin To Read:");							// Print to serial port
 inputString_module();													// Collect serial data
 int value =inputString.toInt();										// Convert data to integer
 value = digitalRead(value);											// Declare Pin read variable
 Serial.print("Read: "); Serial.println(value); Serial.println();		// Print out value to serial port
 reset_serial_no_prompt();												// Reset Serial port input variable without re-displaying prompt
 flag = 1;																// This is a valid command Flag
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void read_adc() // Read ADC channel on Arduino Pin
{          
 byte done = 0;															// Declare done variable
 reset_serial_no_prompt();												// Reset Serial port input variable without re-displaying prompt
 spaces();																// Prints some blank spaces
 Serial.write("Please Select ADC Channel To Read:");					// Print to serial port
 inputString_module();													// Collect serial data
 int sensorValue = analogRead(inputString.toInt());						// Declare ADC read variable and convert data to integer
 float voltage = sensorValue * (5.0 / 1023.0);							// Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
 Serial.print("ADC Value Read: "); Serial.print(sensorValue);			// print out the value you read:
 Serial.print(" Converted to Voltage: "); Serial.println(voltage);		// print out the value you read:
 Serial.println();														// print out the value you read:
 reset_serial_no_prompt();												// Reset Serial port input variable without re-displaying prompt
 flag = 1;																// This is a valid command Flag
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void pwm_pin() // PWM Arduino Pin
{          
 byte pin = 0;															// Declare Microcontroller PIN variable
 byte state = 0;														// Declare Microcontroller State variable
 reset_serial_no_prompt();												// Reset Serial port input variable without re-displaying prompt
 spaces();																// Prints some blank spaces
 Serial.write("Please Select Pin To PWM:");								// print out the value you read:
 inputString_module();													// Collect user entry
 inputString.toLowerCase();												// Converts the inputstring to lowercase - easier to handle in if then statements
 pin = inputString.toInt();												// Set PIN variable to inputString (Serial Data)
      
 reset_serial_no_prompt();												// Reset Serial port input variable without re-displaying prompt
 spaces();																// Prints some blank spaces
 Serial.write("Please enter duty cycle (0-255):");						// print out the value you read:
 inputString_module();													// Collect user entry
 inputString.toLowerCase();												// Converts the inputstring to lowercase - easier to handle in if then statements
 state = inputString.toInt();											// Set the state of the microcontroller pin  
           
 analogWrite(pin, state);												// PWM microcontroller pin
 spaces();																// Prints some blank spaces
 reset_serial_no_prompt();												// Reset Serial port input variable without re-displaying prompt
 flag = 1;																// This is a valid command Flag
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void inputString_module() // Capture data from serial port
{
 done = 0;																// Reset Done to zero
 inputString = "";														// Clear inputString
 
 while (done < 1)														// While not done
 {
  while (Serial.available() > 0)										// Scan serial port for activity
  { 
   serial_accum();														// Accumulate Serial Data
   if (stringComplete == true)											// Checks to see if we have completed text
   {
    done = 1;															// Set done flag
   }
  }
 } 
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void toggle_pin() // Toggle Arduino Pin
{          
 byte pin = 0;															// Set pin to zero
 byte state = 0;														// Set state to zero
 reset_serial_no_prompt();
 Serial.println("");
 Serial.write("Please Select Pin To Toggle:");
 inputString_module();
 inputString.toLowerCase();												// Converts the inputstring to lowercase - easier to handle in if then statements
 pin = inputString.toInt();												// Set PIN variable to inputString (Serial Data)
 
 reset_serial_no_prompt();
 Serial.println("");
 Serial.write("Please enter 1 for High or 0 for Low:");
 inputString_module();
 inputString.toLowerCase();												// Converts the inputstring to lowercase - easier to handle in if then statements
 state = inputString.toInt();											// Set the state of the microcontroller pin  
           
 digitalWrite(pin, state);   // sets the LED on
 Serial.println("");
 reset_serial_no_prompt();
 flag = 1; // This is a valid command Flag
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void set_baud() // Set USB Baud Rate
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
           while (check <1)
           {
           inputString_module();
           inputString.toLowerCase();                                         // Converts the inputstring to lowercase - easier to handle in if then statements
           if (inputString.toInt()<1 ||inputString.toInt() > 8)// Invalid condition
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
            //Serial.println(inputString);
                EEPROM.write(1,inputString.toInt());                        // If none of these conditions are met then Set Baud Rate to 9600 bau  
                switch (EEPROM.read(1))
                {
                  case 1:baud = 1200; break;                                    // Set Baud Rate to 1200 baud
                  case 2:baud = 2400; break;                                    // Set Baud Rate to 2400 baud
                  case 3:baud = 4800; break;                                    // Set Baud Rate to 4800 baud
                  case 4:baud = 9600; break;                                    // Set Baud Rate to 9600 baud
                  case 5:baud = 14400; break;                                   // Set Baud Rate to 14400 baud
                  case 6:baud = 19200; break;                                   // Set Baud Rate to 19200 baud
                  case 7:baud = 38400; break;                                   // Set Baud Rate to 38400 baud
                  case 8:baud = 115200; break;                                  // Set Baud Rate to 115200 baud
                }
  
           Serial.println();
           reset_serial_no_prompt();
           flag = 1; // This is a valid command Flag
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void eprom_routine() // Read from EEPROM address(s)
{          
           reset_serial_no_prompt();
           Serial.println("");
           Serial.write("EEPROM Address (0-4096 or all):");
           inputString_module();
           inputString.toLowerCase();                                         // Converts the inputstring to lowercase - easier to handle in if then statements
            
            if (inputString == "all")
            {
            eprom_all();
            }
            else
            {
            eprom_rd(); 
            }          
           reset_serial_no_prompt();
           flag = 1; // This is a valid command Flag
       }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void write_eprom() // Write to EEPROM address
{
           int ADDRESS = 0;
           byte VAL = 0;
           reset_serial_no_prompt();
           Serial.println("");
           Serial.write("EEPROM Address:");
           inputString_module();
           inputString.toLowerCase();                                         // Converts the inputstring to lowercase - easier to handle in if then statements
           ADDRESS = inputString.toInt();                                         // Set PIN variable to inputString (Serial Data)
           reset_serial_no_prompt();
           
           Serial.println();
           Serial.write("EEPROM Value:");
           inputString_module();
           inputString.toLowerCase();                                         // Converts the inputstring to lowercase - easier to handle in if then statements
           VAL = inputString.toInt();                                         // Set PIN variable to inputString (Serial Data)
     
           EEPROM.write(ADDRESS, VAL);
           Serial.println("");
           reset_serial_no_prompt(); 
                     flag = 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void eprom_all() // Serial Print all EEPROM Values
{
  Serial.println();Serial.println("Reading ALL"); // Print Blank Line and Process
  byte VAL;
  for (int i=0; i <= 4096; i++)
  {
  VAL = EEPROM.read(i);
  Serial.print("EEPROM Address: ");  Serial.print(i);  Serial.print("\t");  Serial.print("Value: ");  Serial.print(VAL);  Serial.println();
  delay(100);// Wait 100mS
  }
  Serial.println(); // Print Space
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void eprom_rd() // Serial Print EPPROM Values
{
  byte VAL;
  Serial.println();
  int a = inputString.toInt(); // Convert Decimal String to number
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void sysinfo() // Displays system configuration information
{
  Serial.println("");
           
  Serial.println("System Information");
           
  Serial.println("Software Firmware Version 1.0");                                                           // Prints Firmware Version
           
  Serial.print("Baud Rate Set to: " );Serial.println(baud);                                                // Displays the devices communication baud Rate 
           
  Serial.println("");
           
  flag = 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void check_sd() // Check SD card for proper insertion
{
  valid = 0;
//   if (!SD.begin(chipSelect)) {
//    Serial.println("initialization failed!");
//    valid = 1;
//    return;
 // }
// Initializes card and suggestion for failure.
   if (!card.init(SPI_HALF_SPEED, chipSelect)) 
   {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card is inserted?");
    Serial.println("* Is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    valid = 1;
    return;
   } 
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void directory_sd() // Directory SD Card
{
  check_sd();
  if (valid == 1){return;}
  Serial.println("Directory SD Card");

   // print the type of card
  Serial.print("\nCard type:");
  switch (card.type()) 
  {
    case SD_CARD_TYPE_SD1:
      Serial.print("SD1 ");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.print("SD2 ");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.print("SDHC ");
      break;
    default:
      Serial.print("Unknown ");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) 
  {
   Serial.println();
   Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
   return;
  }
  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print(" Volume:FAT");
  Serial.print(volume.fatType(), DEC);
  //Serial.println();

  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize *= 512;                            // SD card blocks are always 512 bytes
 // Serial.print("Volume size (bytes): ");
  //Serial.println(volumesize);
  //Serial.print("Volume size (Kbytes): ");
  volumesize /= 1024;
  //Serial.println(volumesize);
  Serial.print(" Size (Mbytes):");
  volumesize /= 1024;
  Serial.println(volumesize);


  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);

  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);
  Serial.println();
  flag = 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void delete_sd_file()// Delete file on SD socket
{
  check_sd(); // Check SD Card
  if (valid == 1){return;}

  directory_sd();
  Serial.print("Enter File Name:");
  reset_serial_no_prompt();

  inputString_module();
  char filename[50];                                           // Character Array to hold String class function
  inputString.toCharArray(filename, 50); // Fill in inputString with inputString data
  SD.remove(filename);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void read_sd_file()// Read file on SD socket
{
  check_sd(); // Check SD Card
  if (valid == 1){return;}

  directory_sd();
  Serial.print("Enter File Name:");
  reset_serial_no_prompt();

  inputString_module();
  char filename[50];                                           // Character Array to hold String class function
  inputString.toCharArray(filename, 50); // Fill in inputString with inputString data
  
  myFile = SD.open(filename);

  if (myFile) 
  {
    Serial.print(filename);
    Serial.println(" contains:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  }
  // if the file isn't open, pop up an error:
  else 
  {
    Serial.print("error opening ");Serial.println(filename);
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void write_sd_file()// Write file on SD socket
{
  check_sd(); // Check SD Card
  if (valid == 1){return;}

  directory_sd();
  Serial.print("Enter File Name:");
  reset_serial_no_prompt();

  inputString_module();
  char filename[50];                                           // Character Array to hold String class function
  inputString.toCharArray(filename, 50); // Fill in inputString with inputString data

  myFile = SD.open(filename,FILE_WRITE);
  
  Serial.print("Enter text:");
  reset_serial_no_prompt();

  inputString_module();

   if (myFile) 
   {
    Serial.print("Writing to:");Serial.println(filename);
    myFile.println(inputString);
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else 
  {
    // if the file didn't open, print an error:
    Serial.print("error opening:");Serial.println(filename);
  }
}
/////////////////////////////////////////////////////////////////////////////// End Of Program //////////////////////////////////////////////////////////////////////////////////////////
