#include <ADC.h>
#include <arm_math.h>

IntervalTimer samplingTimer;

int pedestal = 32767+9000;
int BUFFER_SIZE = 256;
int SAMPLE_FREQ_HZ = 5000;

const uint8_t ANALOG_COUNT = 2;
int StartPIN = A2; // select ADC channels
int EndPIN = A3;

unsigned N; // 2^6=64 "window size" of the moving average filter; DOES NOT affect theh sampling delay
int resolution = 16; //Teensy - 16 bit
int M = 32; // number of samples in burst averaging; DOES affect the sampling delay

float *samples_a0;
float *samples_a1;

float *mags_a0;
float *mags_a1;
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

void setup() {
  Serial.begin(2000000);

  initialize_adc();

  allocate_memory();

  arm_cfft_radix4_init_f32(&fft, BUFFER_SIZE, 0, 1); 

//  create_xcor_time_vec();

//  angle = 90;

  samplingBegin();

}

void loop() {
  if (samplingIsDone())
      {
        noInterrupts();

        arm_cfft_radix4_f32(&fft, samples_a1);
        arm_cmplx_mag_f32(samples_a1, mags_a1, BUFFER_SIZE);

        String datastr = "";
        datastr += (String(BUFFER_SIZE) + "," + String(SAMPLE_FREQ_HZ));
//        Serial.print(BUFFER_SIZE);
//        Serial.print(",");
//        Serial.print(SAMPLE_FREQ_HZ);
//        Serial.print(",");
//        char c;
//        String datastr = "";
        for(int i = 1; i<=BUFFER_SIZE/2; i++){
          datastr += ("," +String(*(mags_a1+i)));
//          Serial.print(",");
        }
        Serial.println(datastr);

         interrupts();
         samplingBegin();
        
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
//  const int max_fft_size = 1024;
    
  samples_a0 = (float*)malloc(2*4*BUFFER_SIZE);
  samples_a1 = (float*)malloc(2*4*BUFFER_SIZE);

//  xcor_sig = (float*)malloc(2*4*BUFFER_SIZE);
//  xcor_time_s = (float*)malloc(2*4*BUFFER_SIZE);
//
//  xcor_maxvalue = (float*)malloc(4);
//  xcor_maxindex = (uint32_t*)malloc(4);

  mags_a0 = (float*)malloc(4*BUFFER_SIZE);
  mags_a1 = (float*)malloc(4*BUFFER_SIZE);
//
//  virtual_sig = (float*)malloc(4*max_fft_size);
//
//  sampletimes = (float*)malloc(4*max_fft_size);
    
}

void samplingBegin() 
{
  // Reset sample buffer position and start callback at necessary rate.
  sample_index = 0;
  samplingTimer.begin(samplingCallback, 1000000/SAMPLE_FREQ_HZ); //run function "samplingCallback" continuosly at intervals of 1000000/SAMPLE_RATE_HZ microseconds. Uses IntervalTimer object linked at https://www.pjrc.com/teensy/td_timing_IntervalTimer.html
}

void samplingCallback() 
{

  dual_sample();

  *(samples_a0+2*sample_index) = 0.1*((uint16_t)result.result_adc0 - pedestal); 
  *(samples_a1+2*sample_index) = 0.1*((uint16_t)result.result_adc1 - pedestal);

  *(samples_a0+2*sample_index-1) = 0.0; 
  *(samples_a1+2*sample_index-1) = 0.0;
  
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

boolean samplingIsDone() 
{
  return sample_index >= BUFFER_SIZE;
}
