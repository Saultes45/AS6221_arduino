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






//#define PRINT_FOR_HUMANS
#define PRINT_FOR_SERIAL_STUDIO

#ifdef PRINT_FOR_HUMANS
#define SERIAL_SEPARATOR "\t" 
#define SERIAL_SOM ""
#define SERIAL_EOM ""
#define NBR_FLOAT_DISPLAY   6
#define ADD_CRC // define if we should calculate and return a CRC of the message to the PC
#define NO_DATA "---"
//#define DISPLAY_SANITY
#else
#define SERIAL_SEPARATOR "," 
#define SERIAL_SOM "$"
#define SERIAL_EOM "*/"
#define NBR_FLOAT_DISPLAY   6
#define ADD_CRC // define if we should calculate and return a CRC of the message to the PC
#define NO_DATA "---"
//#define DISPLAY_SANITY // not yet implemented on Serial Studio
#endif


#define NBR_DISPLAY_FIELDS 5

//END OF FILE
