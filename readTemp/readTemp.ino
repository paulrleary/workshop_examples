int temperaturePin = A0;

int knobPin = A2;

int fanPin = 4;

void setup() {

  pinMode(fanPin, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  
//  int temperature_analog_avg = analogRead(temperaturePin);

  int temperature_analog_avg = read_temp_avg(temperaturePin, 20);

  float temperature_resistance_ohms = calculate_resistance(temperature_analog_avg, 10000.0, 5.0);


  float temperature_C = calculate_temperature_C(temperature_resistance_ohms);

  int knob_analog_avg = read_temp_avg(knobPin, 20);

  float setpoint_C = knob2temp_C(knob_analog_avg);

  bool fanState = (temperature_C > setpoint_C);

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

  digitalWrite(fanPin, fanState);
 
  delay(1000);

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
  
  float temp_resistance = (resolution_conversion*series_resistance_ohms*(float)temp_analog)/(V_in-(resolution_conversion*(float)temp_analog));
  
  return temp_resistance; 
  
  
}

float calculate_temperature_C(float temp_resistance){

  float temp_C = (1/(((0.003354016434681)-((0.0002531645569620253)*log(10000)))+((0.0002531645569620253)*log(temp_resistance))))-273.15;

  return temp_C;

}

float knob2temp_C(int temp_analog){
  float temp_min = 20;
  float temp_max = 30;

  float set_temp_C = temp_min + ((temp_max - temp_min)/1024)*temp_analog;
  return set_temp_C;
}

