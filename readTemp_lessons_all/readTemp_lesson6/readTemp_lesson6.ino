/*
 * Part of series:
 * Introduction to Arduino for Scientists
 * Developed by Dr. Paul Leary, and Dr. Natalie Low

  Lesson 6 - Feedback loop with changeable setpopint
  
  Instead of specifying the setpoint in the code itself, we can use a
  Because we don't have to edit and re-upload the code each time we want to change the setpoint

  Here we will use a POTENTIOMETER (in the form of a knob) to adjust the temperature setpoint. 
  A potentiometer is a adjustable voltage divider (see: https://en.wikipedia.org/wiki/Potentiometer). 
  Recall from Lesson 3 that our thermistor is also set up in a voltage divider circuit.
  Therefore, we will handle the output from the potentiometer in a similar way to how we handled the 
  signal from the thermistor. 
  
  The key difference is that the potentiometer output does not 'naturally' correspond to any temperature.
  Instead, we will define the range of potential temperature setpoints that we want to be able to set
  using the knob, and 'map' this temperature range onto the range of potentiometer outputs.

 */

// Here we define our pin numbers as global variables
int temperaturePin = A0; 
int knobPin = A2; // like the thermistor pin, we use an analog pin to read the potentiometer
int fanPin = 4;



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


   // Here we burst sample and average the readings from the potentiometer
   // This is exactly the same as what we did for the themistor in Lesson 2
   int knob_analog_avg = read_temp_avg(knobPin, 20);
   
   // Here we convert the averaged reading from the potentiometer to a temperature setpoint
   float setpoint_C = knob2temp_C(knob_analog_avg);

  
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

// These are the functions used in the loop() block
// FUNCTION declaration has form: RETURN TYPE  function_name((TYPE) INPUT ARGUMENTS)

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


float knob2temp_C(int temp_analog){

  // Here we define the range (minimum and maximum values) of potential temperature setpoints
  float temp_min = 20;
  float temp_max = 36;

  // Here we map the range of temperature setpoints onto the range of potential digital values from the ADC
  // For the Arduino's built-in 10-bit ADC, this is a range of 1023 
  float set_temp_C = temp_min + ((temp_max - temp_min)/1023)*temp_analog;
  return set_temp_C;
  
}
