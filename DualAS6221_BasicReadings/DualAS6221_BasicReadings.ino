
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
* Version       : 1.1 (finished the 2023-05-01)
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
* //"MovingAverageFloat.h"   https://github.com/pilotak/MovingAverageFloat
* //#include "Statistic.h" from Rob Tillaart https://github.com/RobTillaart/Statistic
* 
*
*
* TODO
* ----
* Use structure instead of single global variables <---------------
* Add (+1: is valid to add) to UART stream
* Put threshold values in #define (sanity check)
* Deal with invalid measurements: what are the exclusion rules? What are the consequences?
* Deal of when the averaging didn't work
* 
* Done
* -----
* Check sensor are replying, use "---" otherwise -- done
* for flag variables, use some #def for states -- done
* Change datarate: CR -- done
* Swap delay for HW timer ISR --- done
* Moving average + increase/decrease -- done
* Put operations in functions -- done
* STDDEV -- done
* Change separators for serial studio -- done
* Array of struct for modular nbr sensors (complex) -- done
*
* Potential
* ---------
* Swap arduino for ESP32C3
* Schnmitt trigger for temp changes
* Try alert mode
* CRC checksum, need to have a message as a str->char array
* 
* ========================================
*/


// -------------------------- Includes --------------------------


//Personal ones
#include "General.h"





// -------------------------- ISR functions  --------------------------
ISR(TIMER1_COMPA_vect)
{//timer1 interrupt 1Hz toggles pin 13 (LED)
	//generates pulse wave of frequency 5Hz/2 = 2.5Hz (takes two cycles for full wave- toggle high then toggle low)

	readSensor = 1; //set the flag to indicate the loop to do a sensor read

	if (toggleLED){
		digitalWrite(LED_BUILTIN,HIGH);
		toggleLED = 0;
	}
	else{
		digitalWrite(LED_BUILTIN,LOW);
		toggleLED = 1;
	}
}





// -------------------------- SetUp --------------------------
void setup()
{

	//cli();//stop interrupts

	setUpUART();
	setUpPins();
	setUpSensors();

	t_start = millis();

	Serial.println(EO_SETUP_MESSAGE);

	prepareHWTimerInterrupt();

	sei();//allow interrupts

}// END OF SET UP





// -------------------------- Loop --------------------------
void loop()
{

	if (readSensor == 1) // if the ISR flag is set then new teperature readings are ready 
	{

		readSensor  = 0; //reset the ISR flag

		// Step #1: get the values
		//----------------------------

		collectSensorValues();
		int cnt_sensors;
		for (cnt_sensors = 0; cnt_sensors < NBR_SENSORS; ++cnt_sensors)
		{
			as6221Data[cnt_sensors].rawCurrentTemperature = as6221Data[cnt_sensors].sensor.readTempC();
		}


		// Step #2: calculations
		//-------------------------
		performCalculations();


		// Step #3: send to computer
		//-------------------------
		sendDataSerial();
		


		// Step #4: prepare for next step
		//---------------------------------
		updatePrevValues();


	}
} // END OF LOOP




//-------------------------------------------------
void i2cQuickScan(void)
{
	byte error, address;
	int nDevices;

	Serial.println("Scanning I2C bus for devices");
	for(address = 1; address < 127; address++ ) 
	{
		// The i2c_scanner uses the return value of
		// the Write.endTransmisstion to see if
		// a device did acknowledge to the address.
		Wire.beginTransmission(address);
		error = Wire.endTransmission();

		if (error == 0)
		{
			Serial.print("I2C device found at address: ");
			printI2CAddress (address);
			Serial.println();

			nDevices++;
		}
		else if (error==4) 
		{
			Serial.print("Unknown error at address: ");
			printI2CAddress (address);
			Serial.println();
		}    
	}
	if (nDevices == 0)
	Serial.println("No I2C devices found :(");
	else
	Serial.println("Scan done!");
} // END OF FUNCTION




//-------------------------------------------------
void sendDataSerial(void)
{

	// Build the string + send via UART

	// Start of message
	Serial.print(SERIAL_SOM);
#ifdef PRINT_FOR_SERIAL_STUDIO
	Serial.print(SERIAL_SEPARATOR);
#endif

	// indicate the time
	long int currentTime_MS = millis();
	Serial.print(currentTime_MS-t_start);
	Serial.print(SERIAL_SEPARATOR);

	t_start = currentTime_MS; //reset prev time


	// Sensor struct - 5 fields (+1: is valid to add)
	//-------------------------------------------------

	int cnt_sensors;
	for (cnt_sensors = 0; cnt_sensors < NBR_SENSORS; ++cnt_sensors) 
	{

		// field #1: I2C address
		printI2CAddress(as6221Data[cnt_sensors].address);
		Serial.print(SERIAL_SEPARATOR); 

		// Check if we were able to access it earlier via I2C
		if (as6221Data[cnt_sensors].isResponding == S_ALIVE) // 
		{
			
			// field #2: Raw temperature
			Serial.print(as6221Data[cnt_sensors].rawCurrentTemperature, NBR_FLOAT_DISPLAY);
			Serial.print(SERIAL_SEPARATOR);

#ifdef DISPLAY_SANITY
			// field #2.5: Raw temperature valid?
			Serial.print(as6221Data[cnt_sensors].isMeasurementValid);
			Serial.print(SERIAL_SEPARATOR);

			Serial.print(as6221Data[cnt_sensors].movAvgSuccess);
			Serial.print(SERIAL_SEPARATOR);

			Serial.print(as6221Data[cnt_sensors].movVarSuccess);
			Serial.print(SERIAL_SEPARATOR);
#endif
			

			// field #3: Moving average on temperature
			Serial.print(as6221Data[cnt_sensors].avg, NBR_FLOAT_DISPLAY);
			Serial.print(SERIAL_SEPARATOR);

			// field #4: difference new average and old average
			Serial.print(as6221Data[cnt_sensors].filteredDifferenceTemperature_mC, NBR_FLOAT_DISPLAY);
			Serial.print(SERIAL_SEPARATOR);

			// field #: 
			Serial.print(as6221Data[cnt_sensors].filteredTrendTemperature);
			Serial.print(SERIAL_SEPARATOR);
			

			// field #5: Moving var on temperature
			Serial.print(as6221Data[cnt_sensors].var, NBR_FLOAT_DISPLAY);
			Serial.print(SERIAL_SEPARATOR);

			
		}

		else
		{
			for (int i=0; i<NBR_DISPLAY_FIELDS; i++)
			{   
				Serial.print(NO_DATA);
				Serial.print(SERIAL_SEPARATOR);
			}
		}
	} // END OF SENSOR LOOP


#ifdef ADD_CRC
	Serial.print("*"); // To indicate the CRC is going to follow
	Serial.print("32"); // Placeholder for the CRC
#endif

	Serial.println(SERIAL_EOM); 



}// END OF FUNCTION




//-------------------------------------------------
void prepareHWTimerInterrupt(void)
{

	//set timer1 interrupt at 5Hz (should really be 8Hz)
	TCCR1A = 0;// set entire TCCR1A register to 0
	TCCR1B = 0;// same for TCCR1B
	TCNT1  = 0;//initialize counter value to 0
	// set compare match register for 1hz increments
	OCR1A = 3124;// = (16*10^6) / (8*1024) - 1 (must be <65536)
	// turn on CTC mode
	TCCR1B |= (1 << WGM12);
	// Set CS10 and CS12 bits for 1024 prescaler
	TCCR1B |= (1 << CS12) | (1 << CS10);  
	// enable timer compare interrupt
	TIMSK1 |= (1 << OCIE1A);

}// END OF FUNCTION


//-------------------------------------------------
void checkSensors(void)
{

	// Check to see if the AS6221 are present on the bus
	// Calling begin() with no arguments = defaults (address:0x48, I2C-port:Wire)


	int cnt_sensors;
	for (cnt_sensors = 0; cnt_sensors < NBR_SENSORS; ++cnt_sensors)
	{
		// Display information about the current sensor
		Serial.print("AS6221 #");
		Serial.print(cnt_sensors+1);
		Serial.print(" (");
		printI2CAddress(as6221Data[cnt_sensors].address);
		Serial.print("): ");

		if (as6221Data[cnt_sensors].sensor.begin(as6221Data[cnt_sensors].address) == false)
		{
			// If there is a communication error
			Serial.println("failed to respond. Please check wiring and possibly the I2C address.");
			
			as6221Data[cnt_sensors].isResponding = S_MIA;   
		}
		else
		{
			// If no problems are detected
			Serial.println(" good to go");
			
			as6221Data[cnt_sensors].isResponding = S_ALIVE;   
		}
	}// END OF SENSOR LOOP

}// END OF FUNCTION



//-------------------------------------------------
void wakeupSensors(void)
{

	int cnt_sensors;
	for (cnt_sensors = 0; cnt_sensors < NBR_SENSORS; ++cnt_sensors) {

		// Check if we were able to access it earlier via I2C
		if (as6221Data[cnt_sensors].isResponding == S_ALIVE) {


			//WAKE UP
			//-------
			// check to see if the sensor might be in sleep mode (maybe from a previous arduino example)
			if (as6221Data[cnt_sensors].sensor.getSleepMode() == true) 
			{
				Serial.print("Sensor #");
				Serial.print(cnt_sensors+1);
				Serial.print(" (");
				printI2CAddress(as6221Data[cnt_sensors].address);
				Serial.print(") was asleep, waking up now...");
				as6221Data[cnt_sensors].sensor.sleepModeOff();
				delay(150); // wait for it to wake up
				Serial.println("done!");
			} // END IF SLEEP


			//DEFAULT SETTINGS
			//----------------
			as6221Data[cnt_sensors].sensor.setDefaultSettings(); // return to default settings 
			// in case they were set differenctly by a previous example

			//ALARM/THRESHOLD
			//----------------
			as6221Data[cnt_sensors].sensor.setTHighC(32); // set high threshhold
			as6221Data[cnt_sensors].sensor.setTLowC(23); // set low threshhold

			Serial.print("\tThighF: ");
			Serial.print(as6221Data[cnt_sensors].sensor.getTHighF(), 2);
			Serial.print("\tTlowF: ");
			Serial.println(as6221Data[cnt_sensors].sensor.getTLowF(), 2); // no getTLowC function

		}
	}

}// END OF FUNCTION



//-------------------------------------------------
void setSensorsRate (void)
{

	Serial.print("Changing sensors data rate...");

	int cnt_sensors;
	for (cnt_sensors = 0; cnt_sensors < NBR_SENSORS; ++cnt_sensors)
	{
		as6221Data[cnt_sensors].sensor.setConversionCycleTime(AS6212_CONVERSION_CYCLE_TIME_250MS); // 4Hz
	}

	// Other possible rates
	//----------------------
	// AS6212_CONVERSION_CYCLE_TIME_125MS // 8Hz
	// AS6212_CONVERSION_CYCLE_TIME_250MS // 4Hz
	// AS6212_CONVERSION_CYCLE_TIME_1000MS // 1Hz
	// AS6212_CONVERSION_CYCLE_TIME_4000MS // 1 time every 4 seconds

	Serial.println("done!");

}// END OF FUNCTION



//-------------------------------------------------
void setUpUART (void)
{

	Serial.begin(USB_BAUDRATE);
	delay(10);
	Serial.println(SO_SKETCH_MESSAGE);
	Serial.println("Dual AS6221 - Basic Readings");

}// END OF FUNCTION



//-------------------------------------------------
void setUpPins (void)
{

	Serial.print("Setting up pins...");
	pinMode(LED_BUILTIN, OUTPUT);
	Serial.println("done!");


}// END OF FUNCTION



//-------------------------------------------------
void printI2CAddress (byte addressToDisplay)
{

	Serial.print("0x");
	if (addressToDisplay<16)
	{ 
		Serial.print("0");
	}
	Serial.print(addressToDisplay,HEX);


}// END OF FUNCTION



//-------------------------------------------------
void allocateSensorAddress (void)
{

	int cnt_sensors;
	for (cnt_sensors = 0; cnt_sensors < NBR_SENSORS; ++cnt_sensors)
	{
		as6221Data[cnt_sensors].address = i2cAddresses[cnt_sensors];
	}


}// END OF FUNCTION


//-------------------------------------------------
void setUpSensors (void)
{

	// Allocate sensor addresses
	allocateSensorAddress();

	// Start the I2C comm
	Wire.begin();

	// Scan
	i2cQuickScan();

	// Check +wakeup
	checkSensors();

	setSensorsRate();

}// END OF FUNCTION



//-------------------------------------------------
void collectSensorValues (void)
{
	int cnt_sensors;
	for (cnt_sensors = 0; cnt_sensors < NBR_SENSORS; ++cnt_sensors)
	{
		// Check if we were able to access it earlier via I2C
		if (as6221Data[cnt_sensors].isResponding == S_ALIVE) 
		{
			as6221Data[cnt_sensors].rawCurrentTemperature = as6221Data[cnt_sensors].sensor.readTempC();

			// Check if measurement was valid
			if  (as6221Data[cnt_sensors].rawCurrentTemperature > 13 && as6221Data[cnt_sensors].rawCurrentTemperature < 36) // Placeholder
			{
				as6221Data[cnt_sensors].isMeasurementValid == MEAS_VALID;
			}
			else
			{
				as6221Data[cnt_sensors].isMeasurementValid == MEAS_INVALID;
			}
			
		}
	}// END OF SENSOR LOOP
	
}// END OF FUNCTION



//-------------------------------------------------
void updatePrevValues(void)
{

	int cnt_sensors;
	for (cnt_sensors = 0; cnt_sensors < NBR_SENSORS; ++cnt_sensors) 
	{
		// Check if we were able to access it earlier via I2C AND the raw meas was valid
		if (as6221Data[cnt_sensors].isResponding == S_ALIVE && as6221Data[cnt_sensors].isMeasurementValid == MEAS_VALID && as6221Data[cnt_sensors].movAvgSuccess == MEAS_VALID) 
		{
			as6221Data[cnt_sensors].filteredPreviousTemperature = as6221Data[cnt_sensors].avg;
		}
	} // END OF SENSOR LOOP

}// END OF FUNCTION


//-------------------------------------------------
void performCalculations(void)
{

	float temp_1 = 0.0;
	float temp_2 = 0.0;

	// Check if we were able to access it earlier via I2C AND the raw meas was valid
	if (as6221Data[0].isResponding == S_ALIVE && as6221Data[0].isMeasurementValid == MEAS_VALID) 
	{
		// Step 1: moving average
		//-----------------------
		temp_1 = as6221Data[0].rawCurrentTemperature;
		temp_2 = as6221Data[0].avg;
		if ( movAvg_0.push(&temp_1, &temp_2) ) 
		{
			// Then SUCCESS
			as6221Data[0].avg = temp_2;
			as6221Data[0].movAvgSuccess = MEAS_VALID;
		}
		else
		{
			// the averaging didn't work
			as6221Data[0].movAvgSuccess = MEAS_INVALID;
		}
		// Step 2: moving var
		//-----------------------
		temp_2 = as6221Data[0].var;
		if ( movVar_0.push(&temp_1, &temp_2) ) 
		{
			// Then SUCCESS
			as6221Data[0].var = temp_2*1000;
			as6221Data[0].movVarSuccess = MEAS_VALID;
		}
		else
		{
			// the variance calculation didn't work
			as6221Data[0].movVarSuccess = MEAS_INVALID;
		}

	} // END OF SANITY CHECK





	// Check if we were able to access it earlier via I2C AND the raw meas was valid
	if (as6221Data[1].isResponding == S_ALIVE && as6221Data[1].isMeasurementValid == MEAS_VALID) 
	{
		// Step 1: moving average
		//-----------------------
		temp_1 = as6221Data[1].rawCurrentTemperature;
		temp_2 = as6221Data[1].avg;
		if ( movAvg_1.push(&temp_1, &temp_2) ) 
		{
			// Then SUCCESS
			as6221Data[1].avg = temp_2;
			as6221Data[1].movAvgSuccess = MEAS_VALID;
		}
		else
		{
			// the averaging didn't work
			as6221Data[1].movAvgSuccess = MEAS_INVALID;
		}
		// Step 2: moving var
		//-----------------------
		temp_2 = as6221Data[1].var;
		if ( movVar_1.push(&temp_1, &temp_2) ) 
		{
			// Then SUCCESS
			as6221Data[1].var = temp_2*1000;
			as6221Data[1].movVarSuccess = MEAS_VALID;
		}
		else
		{
			// the variance calculation didn't work
			as6221Data[1].movVarSuccess = MEAS_INVALID;
		}

	} // END OF SANITY CHECK



	// TODO: continue from 3 to 8

	// Part #2: diff and trend
	int cnt_sensors;
	for (cnt_sensors = 0; cnt_sensors < NBR_SENSORS; ++cnt_sensors) 
	{

		// Check if we were able to access it earlier via I2C AND the raw meas was valid
		if (as6221Data[cnt_sensors].isResponding == S_ALIVE && as6221Data[cnt_sensors].isMeasurementValid == MEAS_VALID && as6221Data[cnt_sensors].movAvgSuccess == MEAS_VALID) 
		{


			// Step 3: diff
			as6221Data[cnt_sensors].filteredDifferenceTemperature_mC = (as6221Data[cnt_sensors].avg - as6221Data[cnt_sensors].filteredPreviousTemperature)*1000.0;



			// Step 4: trend
			//    as6221Data[cnt_sensors].filteredTrendTemperature          = T_UNDEF;

			if (abs(as6221Data[cnt_sensors].filteredDifferenceTemperature_mC) < TOLERANCE_MC_PER_S)
			{
				// then we can consider the temperature is stable
				as6221Data[cnt_sensors].filteredTrendTemperature  = T_STABLE;
			}
			else // if the temperature difference (from movmean) is becoming bigger, check direction
			{
				if (as6221Data[cnt_sensors].filteredDifferenceTemperature_mC > 0.0) // increasing
				{
					as6221Data[cnt_sensors].filteredTrendTemperature = T_INCREASE;
				}
				else // decreasing
				{
					as6221Data[cnt_sensors].filteredTrendTemperature = T_DECREASE;
				}
			}
			
		} // END OF SANITY CHECK
	} // END OF SENSOR LOOP


}// END OF FUNCTION



//END OF FILE
