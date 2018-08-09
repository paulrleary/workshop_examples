/*
 * Part of series:
 * Introduction to Arduino for Scientists
 * Developed by Dr. Paul Leary, and Dr. Natalie Low
 */


// This section stores the GLOBAL VARIABLES, which are variables that are GLOBAL in SCOPE
// Global variables are accessible to any section of the program

int temperaturePin = A0; //DECLARE a global variable to store the pin number of the pin we will use to read temperature.

void setup() {
  //The setup() block is a requirement of ANY Arduino program (even if it is left empty).
  //The setup() block is the first set of commands to run, and the block runs only once, before entering loop()
  

  Serial.begin(9600);  //Open a line to the SERIAL MONITOR at 9600 BAUD rate (bits/second)
}

void loop() {
  //DECLARE a LOCAL variable (LOCAL in SCOPE), a variable accessible only to the current function, here loop(). 
  // This variable, of type INT (integer), stores the raw ANALOG value of our current temperarure
  int temperature_analog = analogRead(temperaturePin);  

  //Here we simply print our data to the Serial Monitor.  For command reference, see: https://www.arduino.cc/reference/en/language/functions/communication/serial/
  Serial.print("Temperature = ");
  Serial.print(temperature_analog);
  Serial.println();

  //And last, we delay for 500 ms.  Otherwise these commands would execute as fast as the Arduino could possibly run.
  delay(500);

}


