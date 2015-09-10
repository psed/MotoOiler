 #include "constants.h"
 #include <EEPROM.h>

 String consoleCommand;
 
 int rotations;
 int rotationsToValveOpening;
 int valveOpenTime;

 void setup()
 {
   Serial.begin(9600);
   initConfigsFromEEPROM();
   attachInterrupt(0, hallSensorInterrupted, RISING);
   rotations = 0;

   pinMode(valvePin, OUTPUT);
   digitalWrite(valvePin, LOW);
   
   printConfig();
 }

 void loop()
 {
   if (rotations >= rotationsToValveOpening)
   {
     openValve();
     rotations = 0;
   }
 }
 
 // Opens valve for a current set up time
 void openValve()
 {
     digitalWrite(valvePin, HIGH);
     delay(valveOpenTime);
     digitalWrite(valvePin, LOW);
 }
 
 // Opens valve for a specified time
 void openValve(int time)
 {
     digitalWrite(valvePin, HIGH);
     delay(time);
     digitalWrite(valvePin, LOW);
 }

 // Each time hall sensor detects a magnetic field, 
 // this interruption method executing, rotation 
 // counter increasing
 void hallSensorInterrupted()
 {
   rotations++;
 }
 
 // Method reads values from EEPROM after each 
 // oiler launch and after configuration update
 // via bluetooth
 void initConfigsFromEEPROM()
 {
   rotationsToValveOpening = EEPROMReadInt(rotationsEEPROMAddress);
   valveOpenTime = EEPROMReadInt(valveOpenTimeEEPROMAddress);
 }

 void saveAndUpdateConfigsFromEEPROM(int rotations, int valveTime)
 {
   EEPROMWriteInt(rotationsEEPROMAddress, rotations);
   EEPROMWriteInt(valveOpenTimeEEPROMAddress, valveTime);
   Serial.println("New parameters were saved successfully.");
   initConfigsFromEEPROM();
 }

 // Method checks new commands on serial port and stores
 // received values to eeprom
 void serialEvent()
 {
    while (Serial.available()) {
      char c = Serial.read();
      delay(10);
      consoleCommand += c;
      if (consoleCommand.endsWith(";"))
      {
        processCommand(consoleCommand);
        consoleCommand = "";
      }
   }  
 }
 
 // Parsing and processing commands
 bool processCommand(String command)
 {
   if (command == "config;")
   {
     printConfig();
   } 
   else
   if (command == "open;")
   {
     openValve();
   }
   else 
   if (command.indexOf("(") != -1)
   {
     int openningBracePos = command.indexOf("(");
     int closingBracePos = command.indexOf(")");
     openValve(command.substring(openningBracePos +1 , closingBracePos).toInt());
   }
   else
   if (command.indexOf("/") != -1)
   {
     int separatorPos = command.indexOf("/");
     String rotations = command.substring(0, separatorPos);
     String valveTime = command.substring(separatorPos + 1, command.length() - 1);
     saveAndUpdateConfigsFromEEPROM(rotations.toInt(), valveTime.toInt());
   }
 }
 
 void printConfig()
 {
   Serial.print(rotationsToValveOpening);Serial.print("/");Serial.print(valveOpenTime);Serial.println(";");
 }
 
 void EEPROMWriteInt(int p_address, int p_value)
 {
   byte lowByte = ((p_value >> 0) & 0xFF);
   byte highByte = ((p_value >> 8) & 0xFF);

   EEPROM.write(p_address, lowByte);
   EEPROM.write(p_address + 1, highByte);
 }
  
 unsigned int EEPROMReadInt(int p_address)
 {
   byte lowByte = EEPROM.read(p_address);
   byte highByte = EEPROM.read(p_address + 1);
   return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
 }
 

