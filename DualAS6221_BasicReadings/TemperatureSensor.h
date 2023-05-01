
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

// From library manager
#include <Wire.h>                  // for I2c Comm to the sensors
#include "SparkFun_AS6212_Qwiic.h" // for communicating with the T sensor AS6221
#include "SensorFilters.h"
//#include "SoftFilters.h"          // for circular buffer for moving average and std dev
#include "SensorAddresses.h"


#define TOLERANCE_MC_PER_S 7.0 // in milli deg C/s, defines when we consider the temperature stable, increasing or decreasing


// Different possible states of "filteredTrendTemperature", default: T_UNDEF
#define T_STABLE    0
#define T_DECREASE  1
#define T_INCREASE  2
#define T_UNDEF     99

// Different possible states of "isResponding", default: S_ALIVE
// Updated for only at the start
#define S_ALIVE   1
#define S_MIA     2

// To check if the measurement is valid, if not, do not use it in the filters and calculations, , default: MEAS_INVALID
// Updated for every measurement
#define MEAS_VALID      1
#define MEAS_INVALID   0 


// -------------------------- Define  --------------------------



//
//// -------------------------- Struct (global) --------------------------
struct temperatureSensorData
{
  AS6212    sensor;
  byte      address                           = 0x00;
  uint8_t   isResponding                      = S_ALIVE;
  uint8_t   isMeasurementValid                = MEAS_VALID;
  float     rawCurrentTemperature             = 0.0;
  float     filteredPreviousTemperature       = 0.0;
  float     filteredDifferenceTemperature_mC  = 0.0;
  uint8_t   filteredTrendTemperature          = T_UNDEF;

  //Results of filters
  double avg;
  double var;
  
};

// After loooong hours of debuging on a Sat at 3 AM, you cannot put that in the struct 
MovingAverageFilter<double, double> movAvg[NBR_SENSORS](NUM_SAMPLES_MOVSTATS);
MovingVarianceFilter<double, double> movVar[NBR_SENSORS](NUM_SAMPLES_MOVSTATS);

// global structures
temperatureSensorData as6221Data[NBR_SENSORS];
//temperatureSensorData dataS2;







//END OF FILE
