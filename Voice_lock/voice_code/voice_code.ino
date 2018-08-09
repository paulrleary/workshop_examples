#include <ADC.h>
#include <arm_math.h>

IntervalTimer samplingTimer;
elapsedMicros time;

int pedestal = 32767+9000;
int BUFFER_SIZE = 7500;
int SAMPLE_FREQ_HZ = 5000;

const uint8_t ANALOG_COUNT = 2;
int StartPIN = A2; // select ADC channels
int EndPIN = A3;

//unsigned N; // 2^6=64 "window size" of the moving average filter; DOES NOT affect theh sampling delay
int resolution = 16; //Teensy - 16 bit
int M = 32; // number of samples in burst averaging; DOES affect the sampling delay

float *samples_rec;
float *samples_check;

float *xcor_sig;

float xcor_max;
uint32_t xcor_max_idx;

float *mags_a0;
float *mags_a1;
int sample_index;

arm_cfft_radix4_instance_f32 fft;

ADC *adc = new ADC(); 
ADC::Sync_result result;
uint16_t estimate[2] = {};

int recButton = 31;
int checkButton = 32;

void setup() {
  Serial.begin(500000);

  pinMode(recButton, INPUT_PULLUP);
  pinMode(checkButton, INPUT_PULLUP);
  
  initialize_adc();

  allocate_memory();

  arm_cfft_radix4_init_f32(&fft, BUFFER_SIZE, 0, 1); 

//  samplingBegin();

}

void loop() {
  
  if (!digitalRead(checkButton))
      {
        delay(250);
        Serial.print("Recording Check Code...      ");
    sample_index = 0;
    int prevtime = time;
        while(!(sample_index >= BUFFER_SIZE)){
          if((time - prevtime) >= (1000000/(SAMPLE_FREQ_HZ))){
//            Serial.println(millis());
            prevtime = time;
            samplingCallbackCheck();
          }
        }
        Serial.println("Finished Recording.");
        Serial.println();
        normalize_vector(samples_check, BUFFER_SIZE);

        get_correlation();
        
        Serial.print("Correlation: ");
        Serial.println(xcor_max);

        if(xcor_max >= 2000){
          Serial.println("Code recognized!");
        }
        else{
          Serial.println("INTRUDER!!!");
        }
        Serial.println();
//        Serial.print("Correlation: ");
//        Serial.println(xcor_max);
//        Serial.print("check = [");
//        for (int i=0; i<BUFFER_SIZE; i++){
//          Serial.print(*(samples_check+i));
//          Serial.print(",");
//        }
//        Serial.println("];");
      }
//        noInterrupts();
//
//        arm_cfft_radix4_f32(&fft, samples_a1);
//        arm_cmplx_mag_f32(samples_a1, mags_a1, BUFFER_SIZE);
//
//        String datastr = "";
//        datastr += (String(BUFFER_SIZE) + "," + String(SAMPLE_FREQ_HZ));
//
//        for(int i = 1; i<=BUFFER_SIZE/2; i++){
//          datastr += ("," +String(*(mags_a1+i)));
//        }
//        Serial.println(datastr);
//
//        interrupts();
//        samplingBegin();
        
      
if (!digitalRead(recButton))
  { delay(250);
    Serial.print("Recording Passcode...      ");
    sample_index = 0;
    int prevtime = time;
        while(!(sample_index >= BUFFER_SIZE)){
          if((time - prevtime) >= (1000000/(SAMPLE_FREQ_HZ))){
//            Serial.println(millis());
            prevtime = time;
            samplingCallbackRec();
          }
        }
        Serial.println("Finished Recording.");
        Serial.println();
        
        normalize_vector(samples_rec, BUFFER_SIZE);
        
//        Serial.print("code = [");
//        for (int i=0; i<BUFFER_SIZE; i++){
//          Serial.print(*(samples_rec+i));
//          Serial.print(",");
//        }
//        Serial.println("];");
  }
}

///////////////////////////////////////////////////////////////////////////////
// ADC SAMPLING FUNCTIONS
////////////////////////////////////////////////////////////////////////////////
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
  samples_rec = (float*)malloc(4*BUFFER_SIZE);
  samples_check = (float*)malloc(4*BUFFER_SIZE);

  xcor_sig = (float*)malloc(2*4*BUFFER_SIZE);
  
  mags_a0 = (float*)malloc(4*BUFFER_SIZE);
  mags_a1 = (float*)malloc(4*BUFFER_SIZE);
  
}

//void samplingBegin() 
//{
//  // Reset sample buffer position and start callback at necessary rate.
//  sample_index = 0;
//  samplingTimer.begin(samplingCallback, 1000000/SAMPLE_FREQ_HZ); //run function "samplingCallback" continuosly at intervals of 1000000/SAMPLE_RATE_HZ microseconds. Uses IntervalTimer object linked at https://www.pjrc.com/teensy/td_timing_IntervalTimer.html
//}

void samplingCallbackRec() 
{

  dual_sample();

  *(samples_rec+sample_index) = 0.1*((uint16_t)result.result_adc0 - pedestal); 
//  *(samples_a1+sample_index) = 0.1*((uint16_t)result.result_adc1 - pedestal);

//  *(samples_a0+2*sample_index-1) = 0.0; 
//  *(samples_a1+2*sample_index-1) = 0.0;
  
  sample_index += 1;
  
//  if (sample_index >= BUFFER_SIZE) {
////    samplingTimer.end();
//    sample_index=0;
//  }
}

void samplingCallbackCheck() 
{

  dual_sample();

  *(samples_check+sample_index) = 0.1*((uint16_t)result.result_adc0 - pedestal); 
//  *(samples_a1+sample_index) = 0.1*((uint16_t)result.result_adc1 - pedestal);

//  *(samples_a0+2*sample_index-1) = 0.0; 
//  *(samples_a1+2*sample_index-1) = 0.0;
  
  sample_index += 1;
  
//  if (sample_index >= BUFFER_SIZE) {
////    samplingTimer.end();
//    sample_index=0;
//  }
}

void dual_sample()
{  // read 2 analog channels and preprocess
  
  result = adc->readSynchronizedContinuous();
  
  result.result_adc0 = (uint16_t)result.result_adc0;
  result.result_adc1 = (uint16_t)result.result_adc1;
  
}
void normalize_vector(float* vector, int vector_length)
{
  float mean;
  float rms;

  arm_mean_f32(vector, vector_length, &mean);
  arm_offset_f32(vector, -1*mean, vector, vector_length);

  arm_rms_f32(vector, vector_length, &rms);
  arm_scale_f32(vector, 1/rms, vector, vector_length);
  
  
}

void get_correlation(){
  arm_correlate_f32(samples_rec, BUFFER_SIZE, samples_check, BUFFER_SIZE, xcor_sig);
  arm_max_f32(xcor_sig, 2*BUFFER_SIZE, &xcor_max, &xcor_max_idx);
}
boolean samplingIsDone() 
{
  return sample_index >= BUFFER_SIZE;
}
