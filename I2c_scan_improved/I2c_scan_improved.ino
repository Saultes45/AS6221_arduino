
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
*
*
* TODO
*
* ========================================
*/

// This sketch tests the standard 7-bit addresses
// Devices with higher bit address might not be seen properly.
//


// -------------------------- Includes --------------------------
#include <Wire.h> // for I2C

// -------------------------- Define  --------------------------
#define USB_BAUDRATE 9600
#define SO_SKETCH_MESSAGE "++++++++++++++++++++++++++++++++++++++++++++++++++++++"
#define EO_SETUP_MESSAGE "-------------------------------------------------------"


// -------------------------- SetUp --------------------------
void setup()
{

  
  Serial.begin(USB_BAUDRATE);
  while (!Serial);             // Leonardo: wait for serial monitor
  Serial.println(SO_SKETCH_MESSAGE);
  Serial.println("I2C Scanner");


  Wire.begin(); //Start the I2C on the arduino

  Serial.println(SO_SKETCH_MESSAGE);
}




// -------------------------- Loop --------------------------
void loop()
{
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++ ) 
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error==4) 
    {
      Serial.print("Unknown error at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found");
  else
    Serial.println("Scan done");

//while(1); // Wait here forever
  delay(5000);           // wait 5 seconds for next scan

}// END OF THE LOOP




//ENF OF FILE
