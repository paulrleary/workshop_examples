/*
 * Part of series:
 * Introduction to Arduino for Scientists
 * Developed by Dr. Paul Leary, and Dr. Natalie Low

  Lesson 5 - Combining a sensor with an actuator to create a feedback loop
  
  Now that we have successfully obtained temperature values from our thermistor, we can start to
  do something with those values. One useful thing to do with these sensor readings is to compare
  them with a specified temperature setpoint, and if the measured temperature rises above this
  setpoint, we can get the Arduino to turn on an actuator.

  For our purposes, an ACTUATOR is something that acts on the physical environment, in response to
  a digital signal from the Arduino, which we will produce using the function digitalWrite(). Examples
  of actuators can include lights, pumps, solenoid valves, motors, heaters, chillers, or the fans that
  we are using in this example. An actuator that acts to bring the sensor value back toward the setpoint
  (e.g., a fan to lower the temperature) forms part of a feedback or regulatory loop. 


 */

// Here we define our pin numbers as global variables
int temperaturePin = A0; 
int fanPin = 4; // we use a digital pin to turn on/off the fan



void setup() { 

  // Open a line to the serial monitor at 9600 baud
  // Note that serial communication uses digital pins 0 and 1, so we can't use them for other things
  // while serial communication is happening
  Serial.begin(9600);  

  // Define the fanPin as an digital output pin
  // This will allow us to use the digitalWrite() function to control the fan from this pin 
  pinMode(fanPin, OUTPUT);

}



void loop() {

   // Burst sample and average the sample values (Lesson 2)
   int temperature_analog_avg = read_temp_avg(temperaturePin, 20);

   // Convert averaged digital values from the ADC to a resistance value (Lesson 3)
   float temperature_resistance_ohms = calculate_resistance(temperature_analog_avg, 10000.0, 5.0);

   // Convert resistance to temperature (Lesson 4)
   float temperature_C = calculate_temperature_C(temperature_resistance_ohms);

   // We define a temperature setpoint, above which we want our fan to turn on
    float setpoint_C = 30;
     
   // We define a BOOLEAN (true/false) variable to compare the current temperature to our setpoint
   // fanState is TRUE if current temperature is higher than the setpoint (i.e., we want to turn on the fan and cool down!)  
   bool fanState = (temperature_C > setpoint_C);


   // We define a STRING (an array of characters) to use in the serial output
   // It will report to us whether the fan is on or off
   
   String fanString = "Off";

   if(fanState){
    fanString = "On";
    }

  Serial.print("Temperature = ");
  Serial.print(temperature_C);
  Serial.write(0xC2);
  Serial.write(0xB0);
  Serial.print("C ");

  Serial.print("Setpoint = ");
  Serial.print(setpoint_C);
  Serial.write(0xC2);
  Serial.write(0xB0);
  Serial.print("C");

  Serial.print("  Fan = ");
  Serial.print(fanString);
  Serial.println();

  // Turn the fan on or off, based on the fanState variable that we defined earlier
  digitalWrite(fanPin, fanState);

  delay(1000);

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
    
    float temp_voltage = 0.004887586*(float)temp_analog;

    float temp_resistance = (series_resistance_ohms*temp_voltage)/(V_in-temp_voltage);

  // float temp_resistance = (resolution_conversion*series_resistance_ohms*(float)temp_analog)/(V_in-(resolution_conversion*(float)temp_analog));
  
  return temp_resistance; 
}


float calculate_temperature_C(float temp_resistance){

  // Convert resistance to temperature
  
  float temp_C = (1/(((0.003354016434681)-((0.0002531645569620253)*log(10000)))+((0.0002531645569620253)*log(temp_resistance))))-273.15;

  return temp_C;

}
  
