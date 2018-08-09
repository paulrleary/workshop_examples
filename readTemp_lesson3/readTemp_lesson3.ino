/*
 * Part of series:
 * Introduction to Arduino for Scientists
 * Developed by Dr. Paul Leary, and Dr. Natalie Low
 */

int temperaturePin = A0; 

void setup() {
 
  Serial.begin(9600);  

void loop() {

//  int temperature_analog = analogRead(temperaturePin);  

   int temperature_analog_avg = read_temp_avg(temperaturePin, 20);

   // We have seen that the above command does produce a number which responds to temperature
   // However its units are meaningless.  
   // This number is simply a DIGITAL number which corresponds to the ANALOG voltage on our pin. (https://en.wikipedia.org/wiki/Analog-to-digital_converter)
   // The possible values of this number are dictated by the RESOLUTION (here, 10 bit) of our ADC, and the VOLTAGE RANGE this resolution corresponds to. 

   // To convert this number to something physically useful, we apply Ohm's law and get the resistance by solving for R2
   
   float temperature_resistance_ohms = calculate_resistance(temperature_analog_avg, 10000.0, 5.0);

  
  Serial.print("Thermistor = ");
  Serial.print(temperature_resistance_ohms);
  Serial.print(" ohms");
  Serial.println();
  
  delay(500);

}

//FUNCTION declaration has form: RETURN TYPE  function_name((TYPE) INPUT ARGUMENTS)

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


