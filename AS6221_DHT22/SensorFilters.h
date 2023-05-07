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


#include "SoftFilters.h"          // for circular buffer for moving average and std dev


#define NUM_SAMPLES_MOVSTATS 20



// TODO: find a better way to do this. Max of 8 sensor on 1 I2C line
MovingAverageFilter<float, float> movAvg_0(NUM_SAMPLES_MOVSTATS);
MovingVarianceFilter<float, float> movVar_0(NUM_SAMPLES_MOVSTATS);

MovingAverageFilter<float, float> movAvg_1(NUM_SAMPLES_MOVSTATS);
MovingVarianceFilter<float, float> movVar_1(NUM_SAMPLES_MOVSTATS);

MovingAverageFilter<float, float> movAvg_2(NUM_SAMPLES_MOVSTATS);
MovingVarianceFilter<float, float> movVar_2(NUM_SAMPLES_MOVSTATS);

MovingAverageFilter<float, float> movAvg_3(NUM_SAMPLES_MOVSTATS);
MovingVarianceFilter<float, float> movVar_3(NUM_SAMPLES_MOVSTATS);

MovingAverageFilter<float, float> movAvg_4(NUM_SAMPLES_MOVSTATS);
MovingVarianceFilter<float, float> movVar_4(NUM_SAMPLES_MOVSTATS);

MovingAverageFilter<float, float> movAvg_5(NUM_SAMPLES_MOVSTATS);
MovingVarianceFilter<float, float> movVar_5(NUM_SAMPLES_MOVSTATS);

MovingAverageFilter<float, float> movAvg_6(NUM_SAMPLES_MOVSTATS);
MovingVarianceFilter<float, float> movVar_6(NUM_SAMPLES_MOVSTATS);

MovingAverageFilter<float, float> movAvg_7(NUM_SAMPLES_MOVSTATS);
MovingVarianceFilter<float, float> movVar_7(NUM_SAMPLES_MOVSTATS);




//END OF FILE
