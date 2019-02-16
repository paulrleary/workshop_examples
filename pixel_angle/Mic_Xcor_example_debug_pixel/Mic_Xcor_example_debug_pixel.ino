//#include <dhtnew.h>
#include <ADC.h>
#include <arm_math.h>
#include <Adafruit_NeoPixel.h>
#include <Servo.h>

//DHTNEW dht(32);

//float T;
//float Vs;

elapsedMicros time;

IntervalTimer dhtTimer;
IntervalTimer samplingTimer;

#define PIN 6
Adafruit_NeoPixel ring = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);


//int pedestal = 512;

//int pedestal = 32767+10000;
//int pedestal = 0;


/*
 * ADC CONFIGURATION VARIABLES
 */
//There are only a few pins that can be accessed by both ADCs: 16 (A2), 17 (A3), 34-36 (A10-A13):
const uint8_t ANALOG_COUNT = 2;
int StartPIN = A2; // select ADC channels
int EndPIN = A3;

/*
 * ADC SAMPLING PARAMETERS
 */
unsigned N; // 2^6=64 "window size" of the moving average filter; DOES NOT affect theh sampling delay
int resolution = 12; //Teensy - 16 bit
int M = 32; // number of samples in burst averaging; DOES affect the sampling delay
int pedestal = pow(2, resolution)/2;

float *samples_a0;
float *samples_a1;
int sample_index;

float *xcor_sig;
// float *xcor_samples;
float *xcor_time_s;

float *xcor_maxvalue;
uint32_t *xcor_maxindex;

float TDOA;
float angle;
//float angle_mem;

float D = .14; //Distance between mics, meters
float Vs = 343.721;

arm_cfft_radix4_instance_f32 fft;

ADC *adc = new ADC(); 
ADC::Sync_result result;
uint16_t estimate[2] = {};

int BUFFER_SIZE = 256;
int SAMPLE_FREQ_HZ = 40000;

int servoPin = 2;
int servoPosition;
int servoTime;
int servoUpdateTime = 1000000;
Servo myServo;

bool enableServo = 0;
//int pixel;
void setup() {
  Serial.begin(9600);
  delay(1000);

//  pinMode(

  
  initialize_adc();

  allocate_memory();

  create_xcor_time_vec();

  angle = 90;

  // arm_cfft_radix4_init_f32(&fft, BUFFER_SIZE, 0, 1);
  
//  dht.setTempOffset(0);
  
  
//  updateTemp();

//    samplingTimer.priority(0);
  
//  dhtTimer.begin(updateTemp, 1000000);

 samplingBegin();
  ring.begin();
  ring.show();

  
  }

  int color = ring.Color(0,50,0);
int pixel = 0;

void loop() {
   
  
 
  if (samplingIsDone())
      {
        

      noInterrupts();
      char address[16];
      String datastr;

      bool printalldata = 0;
      bool printaddresses = 0;
      bool printxcor = 0;
      bool printAngle = 1;
      bool print2analog =0;

      bool printa0 = 0;
      bool printa1 = 0;
      bool printffta0 = 0;
      
//      bool print2analog =0;
      bool printanalogaddresses = 0;

      bool printmax = 0;
      bool printmaxindex = 0;
      
       
      arm_correlate_f32(samples_a0, BUFFER_SIZE, samples_a1, BUFFER_SIZE, xcor_sig); //look more into allocatememory();
      
//      delay(100);
      
      getXcorMax();
//      Serial.println("hi");
      
      getTDOA();

//      float prevAngle = angle;
      calcAngle();
//      Serial.println(angle);
//
//      pixel = pixelAngle(phi);
//        ring.setPixelColor(prev_pixel, 0);
//        ring.setPixelColor(pixel,color);
//        ring.show()

        
       if(printmax){
        Serial.println(*xcor_maxvalue);
       }
       if(printmaxindex){
        Serial.println(*xcor_maxindex);
       }
      
//      Serial.println(TDOA);
//      Serial.println(time - t1);
//      int t1= time;
      if(printxcor){
        for (int i = 0; i<2*BUFFER_SIZE-1; i++){
        Serial.print(*(xcor_sig + i));
        Serial.print(",");
//          datastr += (String(*(samples_a0+i))+",");
      }
      Serial.println();
//      delay(100);
      }

      if(printAngle){
      int prev_pixel = pixel;
      pixel = pixelAngle(angle);
      Serial.println(String(prev_pixel)+","+String(pixel)+","+String(angle)+",");
      
        ring.setPixelColor(prev_pixel, 0);
        ring.setPixelColor(pixel,color);
        ring.show();
        delay(10);
      }
      /*
      if(print2analog)
      {
        datastr = "";
        for(int i=0;i<BUFFER_SIZE;i++){
                  datastr += (String(*(samples_a0+i))+",");
//                  datastr += (String(i)+",");
            }
        Serial.print(datastr);
        datastr = "";
        for(int i=0;i<BUFFER_SIZE;i++){
                  datastr += (String(*(samples_a1+i))+",");
//                  datastr += (String(i+2)+",");
            }
        Serial.println(datastr);
      }

       if(printanalogaddresses)
      {
//        datastr = "";
//        for(int i=0;i<BUFFER_SIZE;i++){
//                  sprintf(address, "%u", samples_a0+i);
//                  datastr += (address +",");
////                  datastr += (String(i)+",");
//            }
//        Serial.print(datastr);
//        datastr = "";
//        for(int i=0;i<BUFFER_SIZE;i++){
//                  sprintf(address, "%u", samples_a1+i);
//                  datastr += (address +",");
////                  datastr += (String(i+2)+",");
//            }
//        Serial.println(datastr);
      }
//      Serial.println(time - t1);
      
      if(printa0){
      
        for (int i = 0; i<BUFFER_SIZE-1; i++){
        Serial.print(*(samples_a0 + i));
        Serial.print(",");
//          datastr += (String(*(samples_a0+i))+",");
      }
      Serial.println();
      }
      
      if(printa1){
      
        for (int i = 0; i<BUFFER_SIZE-1; i++){
        Serial.print(*(samples_a1 + i));
        Serial.print(",");
//          datastr += (String(*(samples_a0+i))+",");
      }
      Serial.println();
      }


      if(printffta0){
        arm_cfft_radix4_f32(&fft, samples_a0);
        
        for (int i = 0; i<BUFFER_SIZE/2-1; i++){
        Serial.print(*(samples_a0 + i));
        Serial.print(",");
//          datastr += (String(*(samples_a0+i))+",");
      }
      Serial.println();
      }


      
      if(printalldata){       
      Serial.print("Signal A0 :");  
//      sprintf(address, "%u", samples_a0);
//      Serial.println(address);
  
      datastr = "";
      for (int i = 0; i<BUFFER_SIZE; i++){
        Serial.print(*(samples_a0 + i));
        Serial.print(",");
//          datastr += (String(*(samples_a0+i))+",");
      }
      Serial.println();

      Serial.print("Signal A1 :");  
//      sprintf(address, "%u", samples_a1);
//      Serial.println(address);
  
      datastr = "";
      for (int i = 0; i<BUFFER_SIZE; i++){
        Serial.print(*(samples_a1 + i));
        Serial.print(",");
//          datastr += (String(*(samples_a0+i))+",");
      }
      Serial.println();

      Serial.print("XCor Signal :");
//      sprintf(address, "%u", xcor_sig);
//      Serial.println(address);
      datastr = "";
      for (int i = 0; i<2*BUFFER_SIZE; i++){
        Serial.print(*(xcor_sig + i));
        Serial.print(",");
//          datastr += (String(*(samples_a0+i))+",");
      }
      Serial.println();
      }

      if(printaddresses){       
      Serial.print("Signal A0 address :");  
      sprintf(address, "%u", samples_a0);
      Serial.println(address);
  
      datastr = "";
      for (int i = 0; i<BUFFER_SIZE; i++){
        sprintf(address, "%u", samples_a0 + i);
        Serial.print(address);
        Serial.print(",");
//          datastr += (String(*(samples_a0+i))+",");
      }
      Serial.println();

      Serial.print("Signal A1 address :");  
      sprintf(address, "%u", samples_a1);
      Serial.println(address);
  
      datastr = "";
      for (int i = 0; i<BUFFER_SIZE; i++){
       sprintf(address, "%u", samples_a1 + i);
        Serial.print(address);
        Serial.print(",");
//          datastr += (String(*(samples_a0+i))+",");
      }
      Serial.println();

      Serial.print("XCor Signal address :");
      sprintf(address, "%u", xcor_sig);
      Serial.println(address);
      datastr = "";
      for (int i = 0; i<2*BUFFER_SIZE; i++){
        sprintf(address, "%u", xcor_sig+i);
        Serial.print(address);
        Serial.print(",");
//          datastr += (String(*(samples_a0+i))+",");
      }
      Serial.println();
      }
*/

        interrupts();
  samplingBegin();
      }


}

void getXcorMax()
{
 arm_max_f32(xcor_sig, 2*BUFFER_SIZE, xcor_maxvalue, xcor_maxindex); 
  // Serial.print(*xcor_maxvalue);
 // Serial.print(",");
 // Serial.print(*xcor_maxindex);
 // Serial.println();
//  Serial.println("hi");
// float Max = 0;
// int ind = 0;
// for(int i = 0;i<=2*BUFFER_SIZE-1;i++)
//   {
//    if(*(xcor_sig + i) > Max)
//    {
//     Max = *(xcor_sig + i);
//     ind = i;
//    }
//   }
//  Serial.print(Max);
//  Serial.print(",");
//  Serial.print(ind);
//  Serial.println();
}

void getTDOA()
{
  TDOA = *(xcor_time_s + *xcor_maxindex);
  
//  Serial.println(TDOA,6);
}

void calcAngle()
{ 
  const float Pi = 3.14159265359;

  float currentangle  = acos(TDOA*Vs/D)*180/Pi; // Need so spend more time with this calculation, and would prefer to use arm_math, but does not have acos.  Perhaps implement lookup table
//  Serial.println(currentangle);
  float prevangle = angle;
//  Serial.println(prevangle);
//  int shift = 2;
//angle = 90;
  float alpha = 0.1;
//Serial.println(alpha, 8);
//Serial.println(alpha*(currentangle - prevangle),8);
  angle = prevangle+alpha*(currentangle - prevangle);
//  Serial.println(prevangle+alpha*(currentangle - prevangle));

//  angle = currentangle;
if isnan(angle){
  angle = prevangle;
//  Serial.println(0);
}
//Serial.println(angle);
//  angle -= angle >> shift;
//  angle += currentangle >> shift;

//  angle = currentangle;
  

}


void initialize_adc()
{
  ///// ADC0 ////
  adc->setAveraging(M,ADC_0); // set number of averages
  adc->setResolution(resolution,ADC_0); // set bits of resolution
  // it can be any of the ADC_CONVERSION_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED_16BITS, HIGH_SPEED or VERY_HIGH_SPEED
  // see the documentation for more information
  // additionally the conversion speed can also be ADACK_2_4, ADACK_4_0, ADACK_5_2 and ADACK_6_2,
  // where the numbers are the frequency of the ADC clock in MHz and are independent on the bus speed.
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED,ADC_0); // change the conversion speed
  // it can be any of the ADC_MED_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED or VERY_HIGH_SPEED
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED,ADC_0); // change the sampling speed

  ////// ADC1 /////
  adc->setAveraging(M, ADC_1); // set number of averages
  adc->setResolution(resolution, ADC_1); // set bits of resolution
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED, ADC_1); // change the conversion speed
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED, ADC_1); // change the sampling speed
  // Read the values in the loop() with readSynchronizedContinuous()
  adc->startSynchronizedContinuous(StartPIN, EndPIN);
  //
}

void allocate_memory()
{
//  const int max_fft_size = 1024;
    
  samples_a0 = (float*)malloc(4*BUFFER_SIZE);
  samples_a1 = (float*)malloc(4*BUFFER_SIZE);

  xcor_sig = (float*)malloc(2*4*BUFFER_SIZE);
  xcor_time_s = (float*)malloc(2*4*BUFFER_SIZE);

  xcor_maxvalue = (float*)malloc(4);
  xcor_maxindex = (uint32_t*)malloc(4);

//  mags_a0 = (float*)malloc(4*max_fft_size);
//  mags_a1 = (float*)malloc(4*max_fft_size);
//
//  virtual_sig = (float*)malloc(4*max_fft_size);
//
//  sampletimes = (float*)malloc(4*max_fft_size);
    
}

void create_xcor_time_vec()
{
  float Ts = 1/(float)SAMPLE_FREQ_HZ; //convert sample frequency to period (s)
 
  float index = -1*(BUFFER_SIZE-1);
  String datastr = "";
  
  for(int i = 0;i<=2*BUFFER_SIZE-1;i++)
  {
   *(xcor_time_s + i) = index * Ts;
//   index +=1;
//   datastr+= String(i) + ",";
//   Serial.println(*(xcor_time_s + i));
//   delay(10);
//   Serial.print(index);
//   Serial.print(",");
//   delay(10);
  index +=1;

  }
//  Serial.println(datastr);
}

///////////////////////////////////////////////////////////////////////////////
// ADC SAMPLING FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void samplingBegin() 
{
  // Reset sample buffer position and start callback at necessary rate.
  sample_index = 0;
  samplingTimer.begin(samplingCallback, 1000000/SAMPLE_FREQ_HZ); //run function "samplingCallback" continuosly at intervals of 1000000/SAMPLE_RATE_HZ microseconds. Uses IntervalTimer object linked at https://www.pjrc.com/teensy/td_timing_IntervalTimer.html
}

void samplingCallback() 
{
//int pedestal = 32767; //subtract 2^15
//    int pedestal = 0; 
//  int t1 = time;
  dual_sample();
  
//  estimate[0] = approxRollingAverage(estimate[0], (uint16_t)result.result_adc0 + (uint16_t)*(virtual_sig+sample_index));
//  estimate[1] = approxRollingAverage(estimate[1], (uint16_t)result.result_adc1 + (uint16_t)*(virtual_sig+sample_index));
//  
//  *(samples_a0+2*sample_index) = estimate[0];
//  *(samples_a1+2*sample_index) = estimate[1];
// 
//  *(samples_a0+2*sample_index+1) = 0.0; 
//  *(samples_a1+2*sample_index+1) = 0.0;

  *(samples_a0+sample_index) = 0.1*((uint16_t)result.result_adc0 - pedestal);
  *(samples_a1+sample_index) = 0.1*((uint16_t)result.result_adc1 - pedestal);

//  Serial.println((uint16_t)result.result_adc0);

//  int t2 = time - t1;

//  indivsampleTET =  t2;
  sample_index += 1;
  
  if (sample_index > BUFFER_SIZE) {
    samplingTimer.end();
  }
}

void dual_sample()
{  // read 2 analog channels and preprocess
  
  result = adc->readSynchronizedContinuous();
  
  result.result_adc0 = (uint16_t)result.result_adc0;
  result.result_adc1 = (uint16_t)result.result_adc1;
  
}

//Modified IIR  filter
uint16_t approxRollingAverage (uint16_t avg, uint16_t input) {
  avg -= avg >> N;
  avg += input >> N;
  return avg;
}

boolean samplingIsDone() 
{
  return sample_index >= BUFFER_SIZE;
}


// void beginTempTimer()
// {
//  dhtTimer.begin(updateTemp, 1000000); 
// }

// void updateTemp(){
// //  int t = time;
//  dht.read();
//  T = dht.temperature;

// // Vs = sqrt(1.4*286.9*(273.15+T));
//  Vs = 331+(0.6*T);
//  // Serial.println(time - t);

//  // Serial.println(T);
//  // Serial.println(Vs);


// }

int pixelAngle(int angle){

  int pixel = (angle)*24/360+6;
  return pixel;
}

