int temperaturePin = A0;

void setup() {

  pinMode(temperaturePin, INPUT);
}

void loop() {
  
//  int temperature_analog_avg = analogRead(temperaturePin);

  int temperature_analog_avg = read_temp_avg(temperaturePin, 20);

  float temperature_resistance_ohms = calculate_resistance(temperature_analog_avg, 10000.0, 5.0);

  float temperature_C = calculate_temperature_C(temperature_resistance_ohms, 10000.0);

  Serial.print("Temperature = ");
  Serial.print(temperature_C);
//  Serial.write(char(176));
  Serial.write(0xC2);
  Serial.write(0xB0);
  Serial.println("C");


  delay(500);

}

int read_temp_avg(int pin, int samples){
    int T_avg ;

    for (int i = 1; i<=samples; i++){
      T_avg += analogRead(pin);
    }

    T_avg /= samples;

    return T_avg;
}

float calculate_resistance(int temp_analog, float series_resistance_ohms, float V_in){

//  float resTemp = (0.000125*tSens[2]*(float)avgTemp)/(5-(0.000125*(float)avgTemp));
  
  float temp_resistance = (0.000125*series_resistance_ohms*(float)temp_analog)/(V_in-(0.000125*(float)temp_analog));

  return temp_resistance; 
  
  
}

float calculate_temperature_C(float temp_resistance, float series_resistance_ohms){
  
//  currentTemp = (1/(tSens[0]+(tSens[1]*log(resTemp))))-273.15;

  float temp_C = (1/(series_resistance_ohms+(3950*log(temp_resistance))))-273.15;

  return temp_C;


}

//avgTemp is just the average value of 20 ADC readings for the thermistor.
//The first line is calculating the resistance of the thermistor from the ADC/voltage reading. tSens[2] is the value of the second resistor in the voltage divider circuit.
//The second line calculates temperature based on the resistance and the B parameter equation 1/T = 1/T0 + (1/B)ln(R/R0), 
//which is linearized to [1/T] = [1/T0 - (1/B)lnR0] +(1/B) [lnR], so I did a linear calibration of [1/T] against [lnR] using the Denny lab water baths. tSens[0] and tSens[1] in the second line correspond to the constant [1/T0 - (1/B)lnR0] and the slope (1/B) of the linearized equation.

