/* ========================================
*
* Copyright Veriphi, 2024
* All Rights Reserved
* UNPUBLISHED, LICENSED SOFTWARE.
*
* Metadata
* Written by    : Nathanaël Esnault
* Verified by   : Nathanaël Esnault
* Creation date : 202-03-21
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

// DAC IC: DAC8532
// SPI
// 16-bit
// For the T controller: Vref = 3.3V
// For the sample temperature/drug/liquid/ detector board: Vref = 5.0V

// Selects which AS6221's temperature value will be used 
// to be converted and simulated as
// a thermistor via DAC0 (SPI) 
// check this variable: i2cAddresses
#define SOURCE_AS6221_ID 0 

// V divider
#define RDIV  10000
#define V0    3.3

// Thermistor-to-emulate properties
#define R0    10000
#define BETA 3380
#define T0_DEG 25
#define KELVIN 273.15


#include "DAC8552.h"
#include <math.h> // for dac calculations


//  HW SPI
DAC8552 mydac(10);




//END OF FILE