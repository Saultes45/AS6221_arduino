/* ========================================
*
* Copyright Veriphi, 2023
* All Rights Reserved
* UNPUBLISHED, LICENSED SOFTWARE.
*
* Metadata
* Written by    : Nathanaël Esnault
* Verified by   : Nathanaël Esnault
* Creation date : 2023-04-29
* Version       : 1.1 (finished the 2023-04-29)
* Modifications :
* Known bugs    :
*
*
* Possible Improvements
*
* Notes
*
* 
* ========================================
*/


// -------------------------- Includes --------------------------

//#include "Arduino.h"
#include "TemperatureSensor.h"
#include "FrameDefinition.h"

// -------------------------- Define  --------------------------

#define USB_BAUDRATE 230400
#define SO_SKETCH_MESSAGE "++++++++++++++++++++++++++++++++++++++++++++++++++++++"
#define EO_SETUP_MESSAGE "-------------------------------------------------------"


// -------------------------- Global variables  --------------------------


//ISR variables
boolean toggleLED = 0;
volatile boolean readSensor = 0;

// Timing check
long int t_start = millis();


// -------------------------- Function declaration  --------------------------
void i2cQuickScan                   (void);
void sendDataSerial                 (void);
void prepareHWTimerInterrupt        (void);
void checkSensors                   (void);
void wakeupSensors                  (void);
void setUpUART                      (void);
void setUpPins                      (void);
void setSensorsRate                 (void);
void printI2CAddress                (byte addressToDisplay);
void allocateSensorAddress          (void);
void setUpSensors                   (void);
void collectSensorValues            (void);
void updatePrevValues               (void);
void performCalculations            (void);






//END OF FILE
