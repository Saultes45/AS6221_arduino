
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
* Version       : 1.1 (finished the 2023-04-25)
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
* Array of struct for modular nbr sensors (complex)
* CRC checksum, need to have a message as a str->char array
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
* Change separator for seial studio -- done
*
* Potential
* ---------
* Swap arduino for ESP32C3
* Schnmitt trigger for temp changes
* Try alert mode
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


  setUpUART();
  setUpPins();


  // Allocate sensor addresses
    int cnt_sensors;
    for (cnt_sensors = 0; cnt_sensors < NBR_SENSORS; ++cnt_sensors)
    {
      as6221Data[cnt_sensors].address = i2cAddresses[cnt_sensors];
    }



  
  //cli();//stop interrupts
  Wire.begin();
  i2cQuickScan();
  checkSensors();

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

    readSensor  = 0; //reste the ISR flag

    // Step #1: get the values
    //----------------------------

    float sensorRawValue1 = sensor1.readTempC();
    float sensorRawValue2 = sensor2.readTempC();

    int cnt_sensors;
    for (cnt_sensors = 0; cnt_sensors < NBR_SENSORS; ++cnt_sensors)
    {
      as6221Data[cnt_sensors].rawCurrentTemperature = as6221Data[cnt_sensors].sensor.readTempC();
    }


    // Step #2: calculations
    //-------------------------

//    int cnt_sensors;
//    for (cnt_sensors = 0; cnt_sensors < NBR_SENSORS; ++i)
//    {
//      as6221Data[cnt_sensors].filteredPreviousTemperature
//      as6221Data[cnt_sensors].filteredDifferenceTemperature_mC
//      as6221Data[cnt_sensors].filteredPreviousTemperature
//      as6221Data[cnt_sensors].filteredTrendTemperature
//
//      as6221Data[cnt_sensors].avg
//      as6221Data[cnt_sensors].var
//      
//    }


    // Step #3: send to computer
    //-------------------------

    sendDataSerial(sensorRawValue1, sensorRawValue2);

    


    // Step #4: prepare for next step
    //---------------------------------

    //  delay(119);
  }
} // END OF LOOP




//-------------------------------------------------
void i2cQuickScan(void)
{
  byte error, address;
  int nDevices;

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
} // END OF FUNCTION




//-------------------------------------------------
void sendDataSerial(float raw_1, float raw_2)
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



  // Sensor #1 - 5 fields
  //----------------------


  // S1 - field #1: I2C address
  Serial.print("0x");
  if (T_SENSOR_1_ADDRS<16)
  { 
    Serial.print("0");
  }
  Serial.print(T_SENSOR_1_ADDRS,HEX);
  Serial.print(SERIAL_SEPARATOR); 

  if (isRespondingSensor1 == S_ALIVE)
  { 
    // S1 - field #2: Raw temperature
    Serial.print(raw_1, NBR_FLOAT_DISPLAY);
    Serial.print(SERIAL_SEPARATOR);

    // S1 - field #3: Moving average on temperature
    if (movAvg_1.push(&raw_1, &avg_1)) 
    {
      Serial.print(avg_1, NBR_FLOAT_DISPLAY);
    }
    else
    {
      Serial.print(NO_DATA);
    }
    Serial.print(SERIAL_SEPARATOR);


    // S1 - field #4: difference new avaerage and old average
    diffSensor1_mC = (avg_1 - oldSensor1) * 1000; // conversion from C to mC
    Serial.print(diffSensor1_mC, NBR_FLOAT_DISPLAY);
    Serial.print(SERIAL_SEPARATOR);

    // S1 - field #5: direction of the temperature difference
    if (abs(diffSensor1_mC) < TOLERANCE_MC_PER_S)
    {
      // then we can consider the temperature is stable
      globalTrendSensor1 = T_STABLE;
    }
    else // if the temperature difference (from movmean) is becoming bigger, check direction
    {
      if (diffSensor1_mC > 0.0) // increasing
      {
        globalTrendSensor1 = T_INCREASE;
      }
      else // decreasing
      {
        globalTrendSensor1 = T_DECREASE;
      }
    }
    Serial.print(globalTrendSensor1);
    Serial.print(SERIAL_SEPARATOR);
    

    // S1 - field #5: Moving standard deviation
    if (movVar_1.push(&raw_1, &var_1)) 
    {
      Serial.print(var_1*1000, NBR_FLOAT_DISPLAY); // conversion from C to mC
    }
    else
    {
      Serial.print(NO_DATA);
    }
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


#ifdef PRINT_FOR_SERIAL_STUDIO
Serial.print("0"); // dummy for debug
Serial.print(SERIAL_SEPARATOR); // dummy for debug
#endif

  // Sensor #2 - 5 fields
  //----------------------

    // S2 - field #1: I2C address
  Serial.print("0x");
  if (T_SENSOR_2_ADDRS<16)
  { 
    Serial.print("0");
  }
  Serial.print(T_SENSOR_2_ADDRS,HEX);
  Serial.print(SERIAL_SEPARATOR); 

  if (isRespondingSensor2 == S_ALIVE)
  { 
    // S2 - field #2: Raw temperature
    Serial.print(raw_2, NBR_FLOAT_DISPLAY);
    Serial.print(SERIAL_SEPARATOR);

    // S2 - field #3: Moving average on temperature
    if (movAvg_2.push(&raw_2, &avg_2)) 
    {
      Serial.print(avg_2, NBR_FLOAT_DISPLAY);
    }
    else
    {
      Serial.print(NO_DATA);
    }
    Serial.print(SERIAL_SEPARATOR);


    // S2 - field #4: difference new avaerage and old average
    diffSensor2_mC = (avg_2 - oldSensor2) * 1000; // conversion from C to mC
    Serial.print(diffSensor2_mC, NBR_FLOAT_DISPLAY);
    Serial.print(SERIAL_SEPARATOR);

    // S1 - field #5: direction of the temperature difference
    if (abs(diffSensor2_mC) < TOLERANCE_MC_PER_S)
    {
      // then we can consider the temperature is stable
      globalTrendSensor2 = T_STABLE;
    }
    else // if the temperature difference (from movmean) is becoming bigger, check direction
    {
      if (diffSensor2_mC > 0.0) // increasing
      {
        globalTrendSensor2 = T_INCREASE;
      }
      else // decreasing
      {
        globalTrendSensor2 = T_DECREASE;
      }
    }
    Serial.print(globalTrendSensor2);
    Serial.print(SERIAL_SEPARATOR);
    

    // S1 - field #5: Moving standard deviation
    if (movVar_2.push(&raw_2, &var_2)) 
    {
      Serial.print(var_2*1000, NBR_FLOAT_DISPLAY); // conversion from C to mC
    }
    else
    {
      Serial.print(NO_DATA);
    }
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






#ifdef ADD_CRC
  Serial.print("*"); // To indicate the CRC is going to follow
  Serial.print("32"); // Placeholder for the CRC
#endif



  Serial.println(SERIAL_EOM); 

  oldSensor1 = avg_1;
  oldSensor2 = avg_2;


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

  // Check to see if AS6221 is present on the bus
  // Note, here we are calling begin() with no arguments = defaults (address:0x48, I2C-port:Wire)



  if (sensor1.begin(T_SENSOR_1_ADDRS) == false)
  {
    Serial.println("AS6221 #1 failed to respond. Please check wiring and possibly the I2C address.");
    isRespondingSensor1 = S_MIA;   
  }
  else
  {
    Serial.println("AS6221 #1 is online!");
  }







  if (sensor2.begin(T_SENSOR_2_ADDRS) == false)
  {
    Serial.println("AS6221 #2 failed to respond. Please check wiring and possibly the I2C address."); 
    isRespondingSensor2 = S_MIA;  
  }
  else
  {
    Serial.println("AS6221 #2 is online!");
  }




}// END OF FUNCTION



//-------------------------------------------------
void wakeupSensors(void)
{


  // SENSOR #1
  if (isRespondingSensor1 == S_ALIVE)
  {
    // check to see if the sensor might be in sleep mode (maybe from a previous arduino example)
    if (sensor1.getSleepMode() == true)
    {
      Serial.println("Sensor #1 was asleep, waking up now");
      sensor1.sleepModeOff();
      delay(150); // wait for it to wake up
    }

    sensor1.setDefaultSettings(); // return to default settings 
    // in case they were set differenctly by a previous example

    sensor1.setTHighC(32); // set high threshhold
    sensor1.setTLowC(23); // set low threshhold

    Serial.print("\tThighF: ");
    Serial.print(sensor1.getTHighF(), 2);
    Serial.print("\tTlowF: ");
    Serial.println(sensor1.getTLowF(), 2); // no getTLowC function
  }






  // SENSOR #2
  if (isRespondingSensor2 == S_ALIVE)
  {
    // check to see if the sensor might be in sleep mode (maybe from a previous arduino example)
    if (sensor2.getSleepMode() == true)
    {
      Serial.println("Sensor #2 was asleep, waking up now");
      sensor2.sleepModeOff();
      delay(150); // wait for it to wake up
    }
    sensor2.setDefaultSettings(); // return to default settings 
    // in case they were set differenctly by a previous example

    sensor2.setTHighC(32); // set high threshhold
    sensor2.setTLowC(23); // set low threshhold

    Serial.print("\tThighF: ");
    Serial.print(sensor2.getTHighF(), 2);
    Serial.print("\tTlowF: ");
    Serial.println(sensor2.getTLowF(), 2); // no getTLowC function
  }





}// END OF FUNCTION



//-------------------------------------------------
void setSensorsRate (void)
{
  sensor1.setConversionCycleTime(AS6212_CONVERSION_CYCLE_TIME_250MS); // 4Hz
  sensor2.setConversionCycleTime(AS6212_CONVERSION_CYCLE_TIME_250MS); // 4Hz

  // sensor.setConversionCycleTime(AS6212_CONVERSION_CYCLE_TIME_125MS); // 8Hz
  // sensor.setConversionCycleTime(AS6212_CONVERSION_CYCLE_TIME_250MS); // 4Hz
  // sensor.setConversionCycleTime(AS6212_CONVERSION_CYCLE_TIME_1000MS); // 1Hz
  // sensor.setConversionCycleTime(AS6212_CONVERSION_CYCLE_TIME_4000MS); // 1 time every 4 seconds
  
}// END OF FUNCTION



//-------------------------------------------------
void setUpUART (void)
{

  Serial.begin(USB_BAUDRATE);
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





//END OF FILE
