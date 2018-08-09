/*
 * Part of series:
 * Introduction to Arduino for Scientists
 * Developed by Dr. Paul Leary, and Dr. Natalie Low
 */

int temperaturePin = A0; 

void setup() {
 
  Serial.begin(9600);  

}

void loop() {

//  int temperature_analog = analogRead(temperaturePin);  


   // Burst sample and average the sample values
   int temperature_analog_avg = read_temp_avg(temperaturePin, 20);

   // Convert averaged digital values from the ADC to a resistance value
   float temperature_resistance_ohms = calculate_resistance(temperature_analog_avg, 10000.0, 5.0);

   // We know that the thermistor's resistance varies predictably with temperature
   // This can be approximated using the B-parameter equation: 1/T = 1/To + (1/B)*ln(R/Ro)
   // See: https://en.wikipedia.org/wiki/Thermistor
   // Where T is the temperature, R is the resistance, To = 298.15 K (25 deg C, room temperature)
   // B and Ro are known properties of the thermistor (in this case, B = 3950, Ro = 10K)

   float temperature_C = calculate_temperature_C(temperature_resistance_ohms);



  Serial.print("Temperature = ");
  Serial.print(temperature_C);
  Serial.write(0xC2);
  Serial.write(0xB0);
  Serial.print("C ");
  
//  Serial.print("Thermistor = ");
//  Serial.print(temperature_resistance_ohms);
//  Serial.print(" ohms");
//  Serial.println();
  
  delay(500);

}

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
    
    float temp_voltage = (5/1023)*(float)temp_analog;

    float temp_resistance = (series_resistance_ohms*temp_voltage)/(V_in-temp_voltage);

  // float temp_resistance = (resolution_conversion*series_resistance_ohms*(float)temp_analog)/(V_in-(resolution_conversion*(float)temp_analog));
  
  return temp_resistance; 
}


float calculate_temperature_C(float temp_resistance){

  // Convert resistance to temperature
  
  float temp_C = (1/(((0.003354016434681)-((0.0002531645569620253)*log(10000)))+((0.0002531645569620253)*log(temp_resistance))))-273.15;

  return temp_C;

}
  
