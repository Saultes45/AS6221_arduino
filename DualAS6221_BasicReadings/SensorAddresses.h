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


// THE FOLLOWING will be replaced
//#define T_SENSOR_1_ADDRS 0x48
//#define T_SENSOR_1_ADDRS 0x45
//#define T_SENSOR_2_ADDRS 0x44
//#define T_SENSOR_3_ADDRS 0x45 // Unused
//#define T_SENSOR_4_ADDRS 0x44 // Unused
//#define T_SENSOR_5_ADDRS 0x45 // Unused
//#define T_SENSOR_6_ADDRS 0x44 // Unused
//#define T_SENSOR_7_ADDRS 0x45 // Unused
//#define T_SENSOR_8_ADDRS 0x44 // Unused

// Declare a const array of I2C addresses
//const uint8_t i2cAddresses[] = {0x44, 0x45, 0x46, 0x47, 0x48, 0x50}; // They should all be different, order dosen't matter
const uint8_t i2cAddresses[] = {0x48, 0x50}; // They should all be different, order dosen't matter


#define NBR_SENSORS 2



//END OF FILE
