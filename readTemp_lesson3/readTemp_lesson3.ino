/*
 * Part of series:
 * Introduction to Arduino for Scientists
 * Developed by Dr. Paul Leary, and Dr. Natalie Low
 * 
 * Lesson 3 - Converting a digital reading from a sensor to a physical sensor property (resistance)
 * 
 * 
 */

int temperaturePin = A0; 

void setup() {
 
  Serial.begin(9600);  

}

void loop() {

//  int temperature_analog = analogRead(temperaturePin);  

   int temperature_analog_avg = read_temp_avg(temperaturePin, 20);

   // We have seen that the above command does produce a number which responds to temperature
   // However its units are meaningless.  
   // This number is simply a DIGITAL number which corresponds to the ANALOG voltage on our pin. (https://en.wikipedia.org/wiki/Analog-to-digital_converter)
   // The possible values of this number are dictated by the RESOLUTION (here, 10 bit) of our ADC
   // And by the VOLTAGE RANGE this resolution corresponds to (here, 0-5 V)

   // To convert this number to something physically useful, we do two calculations:
   // Firstly, we convert the digital number to the analog voltage on the pin, using the known ADC resolution and voltage range
   // See: https://learn.sparkfun.com/tutorials/analog-to-digital-conversion/relating-adc-value-to-voltage
   // Secondly, we use this voltage to calculate the resistance of the thermistor by applying Ohm's law to the voltage divider circuit 
   // See: https://en.wikipedia.org/wiki/Voltage_divider
   // For our circuit, we have a fixed resistor (10K) as our R1 and the thermistor (a variable resistor) as R2
   

   float temperature_resistance_ohms = calculate_resistance(temperature_analog_avg, 10000.0, 5.0);

  
  Serial.print("Thermistor = ");
  Serial.print(temperature_resistance_ohms);
  Serial.print(" ohms");
  Serial.println();
  
  delay(500);

}


// These are the functions used in the loop() block
//FUNCTION declaration has form: RETURN TYPE  function_name((TYPE) INPUT ARGUMENTS)

int read_temp_avg(int pin, int samples){

    //This function takes as INPUT ARGUMENTS the pin which we want to burst sample, and how many samples we want to average over. 
    
    int T_avg ;

    for (int i = 1; i<=samples; i++){
      T_avg += analogRead(pin);
    }

    T_avg /= samples;

    return T_avg;
    
}


float calculate_resistance(int temp_analog, float series_resistance_ohms, float V_in){
  
    //This function takes as INPUT ARGUMENTS the digital value from the ADC (here, we will use the averaged value from our burst sample)
    // As well as the resistance value of the fixed resistor R1, and the voltage across the entire voltage divider circuit

    // Here we calculate the ADC pin voltage from the digital value we have.
    
    float temp_voltage = 0.004887586*(float)temp_analog;

    // Now we calculate the resistance of the thermistor

    float temp_resistance = (series_resistance_ohms*temp_voltage)/(V_in-temp_voltage);
  
  return temp_resistance; 
  
  
}
