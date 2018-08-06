int temperaturePin = A0;

int knobPin = A2;

int fanPin = 4;

void setup() {

  pinMode(fanPin, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  
//  int temperature_analog_avg = analogRead(temperaturePin);
//Serial.println(temperature_analog_avg);
  int temperature_analog_avg = read_temp_avg(temperaturePin, 20);

  float temperature_resistance_ohms = calculate_resistance(temperature_analog_avg, 10000.0, 5.0);

//  Serial.println(temperature_resistance_ohms);

  float temperature_C = calculate_temperature_C(temperature_resistance_ohms);

  int knob_analog_avg = read_temp_avg(knobPin, 20);

  float setpoint_C = knob2temp_C(knob_analog_avg);

  bool fanState = (temperature_C > setpoint_C);

  String fanString = "Off";
  if(fanState){
    fanString = "On";
  }

  Serial.print("Temperature = ");
//   Serial.print(temperature_resistance_ohms);
  Serial.print(temperature_C);
////  Serial.write(char(176));
  Serial.write(0xC2);
  Serial.write(0xB0);
  Serial.print("C ");

    Serial.print("Setpoint = ");
//   Serial.print(temperature_resistance_ohms);
  Serial.print(setpoint_C);
////  Serial.write(char(176));
  Serial.write(0xC2);
  Serial.write(0xB0);
  Serial.print("C");

  Serial.print("  Fan = ");
//   Serial.print(temperature_resistance_ohms);
  Serial.print(fanString);
////  Serial.write(char(176));
//  Serial.write(0xC2);
//  Serial.write(0xB0);
  Serial.println();

  digitalWrite(fanPin, fanState);
  


//while(1){
//  
//}
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
  float resolution_conversion = 0.0048828125;
//Serial.println(resolution_conversion, 20);
//  float resTemp = (0.000125*tSens[2]*(float)avgTemp)/(5-(0.000125*(float)avgTemp));
//9.765625000000000e-04
//Serial.println(temp_analog);
//Serial.println((float)temp_analog);
  
  float temp_resistance = (resolution_conversion*series_resistance_ohms*(float)temp_analog)/(V_in-(resolution_conversion*(float)temp_analog));
  
  return temp_resistance; 
  
  
}

float calculate_temperature_C(float temp_resistance){

//  Serial.println(temp_resistance);
  
//  float temp_C = (1/(((1/298.15)-((1/3950)*log(10000)))+((1/3950)*log(temp_resistance))))-273.15;

  float temp_C = (1/(((0.003354016434681)-((0.0002531645569620253)*log(10000)))+((0.0002531645569620253)*log(temp_resistance))))-273.15;

//  float temp_C = (1/(series_resistance_ohms+(3950*log(temp_resistance))))-273.15;

  return temp_C;


}

float knob2temp_C(int temp_analog){
  float temp_min = 20;
  float temp_max = 30;

  float set_temp_C = temp_min + ((temp_max - temp_min)/1024)*temp_analog;
  return set_temp_C;
}

//avgTemp is just the average value of 20 ADC readings for the thermistor.
//The first line is calculating the resistance of the thermistor from the ADC/voltage reading. tSens[2] is the value of the second resistor in the voltage divider circuit.
//The second line calculates temperature based on the resistance and the B parameter equation 1/T = 1/T0 + (1/B)ln(R/R0), 
//which is linearized to [1/T] = [1/T0 - (1/B)lnR0] +(1/B) [lnR], so I did a linear calibration of [1/T] against [lnR] using the Denny lab water baths. tSens[0] and tSens[1] in the second line correspond to the constant [1/T0 - (1/B)lnR0] and the slope (1/B) of the linearized equation.

