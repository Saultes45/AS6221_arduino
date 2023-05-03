#define nominal_resistance 10000       //Nominal resistance at 25⁰C
#define nominal_temeprature 25   // temperature for nominal resistance (almost always 25⁰ C)

#define beta 3950  // The beta coefficient or the B value of the thermistor (usually 3000-4000) check the datasheet for the accurate value.
#define Rref 10000   //Value of  resistor used for the voltage divider



// Calculate NTC resistance
average = 1023 / average - 1;
average = Rref/ average;
Serial.print("Thermistor resistance ");
Serial.println(average);

float temperature;
temperature = average / nominal_resistance;     // (R/Ro)
temperature = log(steinhart);                  // ln(R/Ro)
temperature /= beta;                   // 1/B * ln(R/Ro)
temperature += 1.0 / (nominal_temeprature + 273.15); // + (1/To)
temperature = 1.0 / temperature;                 // Invert
temperature -= 273.15;                         // convert absolute temp to C
Serial.print("Temperature ");
Serial.print(temperature);
Serial.println(" *C");
