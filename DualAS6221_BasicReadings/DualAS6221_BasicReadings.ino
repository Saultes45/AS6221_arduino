
/* ========================================
*
* Copyright Veriphi, 2023
* All Rights Reserved
* UNPUBLISHED, LICENSED SOFTWARE.
*
* Metadata
* Written by    : Nathanaël Esnault
* Verified by   : Nathanaël Esnault
* Creation date : 2023-04-25
* Version       : 1.0 (finished the 2023-04-25)
* Modifications :
* Known bugs    :
*
*
* Possible Improvements
*
* Notes
*
*
* Ressources (Boards + Libraries Manager)
* //SparkFun_AS6212_Qwiic.h: Click here to get the library: http://librarymanager/All#SparkFun_AS6212
*
*
* TODO
*
* ========================================
*/


// -------------------------- Includes --------------------------
#include "SparkFun_AS6212_Qwiic.h" 
#include <Wire.h>

// -------------------------- Define  --------------------------
#define USB_BAUDRATE 230400
#define SO_SKETCH_MESSAGE "++++++++++++++++++++++++++++++++++++++++++++++++++++++"
#define EO_SETUP_MESSAGE "-------------------------------------------------------"


// -------------------------- Global variables  --------------------------
AS6212 sensor1;
AS6212 sensor2;


void setup()
{

  Serial.begin(USB_BAUDRATE);
  Serial.println(SO_SKETCH_MESSAGE);
  Serial.println("Dual AS6221 - Basic Readings");

  Wire.begin();

  // Check to see if AS6212 Qwiic is present on the bus
  // Note, here we are calling begin() with no arguments = defaults (address:0x48, I2C-port:Wire)
  if (sensor.begin() == false)
  {
    Serial.println("AS6221 failed to respond. Please check wiring and possibly the I2C address. Freezing...");
    while (1);      
  }; 

  
  Serial.println(SO_SKETCH_MESSAGE);
}

void loop(){
  tempC = sensor.readTempC();
  tempF = sensor.readTempF();

  Serial.println();
  Serial.print("Temperature (°C): ");
  Serial.print(tempC, 6);							//Reads out 6 characters of the temperature float
  Serial.print("\tTemperature (°F): ");
  Serial.println(tempF, 6);							//Reads out 6 characters of the temperature float
  
  delay(1000);
}
