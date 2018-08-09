/*
 * Part of series:
 * Introduction to Arduino for Scientists
 * Developed by Dr. Paul Leary, and Dr. Natalie Low
 * 
 * Lesson 2 -  Burst sampling a sensor 
 *  
 */

int temperaturePin = A0; 

void setup() {
 
  Serial.begin(9600);  
  
}

void loop() {

//  int temperature_analog = analogRead(temperaturePin);  

   // Suppose our temperature was "noisy", (https://en.wikipedia.org/wiki/Noise_(signal_processing))
   // We may decide to cope with this by BURST SAMPLING, and taking the average of an ENSEMBLE of samples
   // So, we create the FUNCTION read_temp_avg(ARGUMENTS), to do precisely this.
   int temperature_analog_avg = read_temp_avg(temperaturePin, 20);
  
  Serial.print("Temperature = ");
  Serial.print(temperature_analog_avg);
  Serial.println();
  
  delay(500);

}

// These are the functions used in the loop() block
// FUNCTION declaration has form: RETURN TYPE  function_name((TYPE) INPUT ARGUMENTS)

int read_temp_avg(int pin, int samples){

    //This function takes as INPUT ARGUMENTS the pin which we want to burst sample, and how many samples we want to average over. 
    
    int T_avg ;

    //See: https://www.arduino.cc/reference/en/language/structure/control-structure/for/
    //For explanation of the "for()" control stucture
    for (int i = 1; i<=samples; i++){
      T_avg += analogRead(pin);
    }

    T_avg /= samples;

    //Here, we specify the variable (if any) we want to RETURN ("spit out" of our function)
    return T_avg;


    //Check your understanding: what is the SCOPE of variables: pin, samples? 
    //What about T_avg?
    
}


