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


// -------------------------- Global variables  --------------------------
AS6212 sensor1;
AS6212 sensor2;


uint8_t isRespondingSensor1 = S_ALIVE; // 0 = can't establish connection with sensor, 1 = connection ok
uint8_t isRespondingSensor2 = S_ALIVE;

float diffSensor1_mC = 0.0; // differerence between 2 iterations in milliC
float diffSensor2_mC = 0.0; // differerence between 2 iterations in milliC

float oldSensor1 = 0.0;
float oldSensor2 = 0.0;

uint8_t globalTrendSensor1  = T_UNDEF; // 0 = sensed temperature (after moving average) is stable, 1 = decrease, 2 = increase
uint8_t globalTrendSensor2  = T_UNDEF;


// -------------------------- Function declaration  --------------------------
void i2cQuickScan                   (void);
void sendDataSerial                 (float raw_1, float raw_2);
void prepareHWTimerInterrupt        (void);
void checkSensors                   (void);
void wakeupSensors                  (void);
void setUpUART                      (void);
void setUpPins                      (void);
void setSensorsRate                 (void);



//END OF FILE
