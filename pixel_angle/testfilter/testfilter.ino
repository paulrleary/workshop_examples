#include <ADC.h>
#include <arm_math.h>  //Important math library, which will be used frequently in this course.  reference: http://www.keil.com/pack/doc/cmsis/DSP/html/modules.html
#include <Adafruit_NeoPixel.h>

#include "firCoeffs.h"

const int readPin1 = A2;
const int readPin2 = A3;
const int adcResolution = 12;
int pedestal;

ADC *adc = new ADC(); // adc object

int BUFFER_SIZE = 256;  
int SAMPLE_FREQ_HZ = 44800;

float *samples_a0;      //  samples od a0 channel
float *samples_a1;      //  samples of q1 channel
int sample_index;

arm_cfft_radix4_instance_f32 fft; //This is the fft object from arm_math.h which we will use to compute FFTs.  Please see 
arm_fir_instance_f32 bpS;


float *samples_a0_mag;    //These will store our FFT magnitudes
float *samples_a1_mag;

float *samples_a0_filt;
float *samples_a1_filt;
 
float *samples_a0_mag_filt;
float *samples_a1_mag_filt;

float *freqs;
int freq_idx;

int N = 1;

IntervalTimer samplingTimer;

void setup(){
	pinMode(LED_BUILTIN, OUTPUT);
    pinMode(readPin1, INPUT);
    pinMode(readPin2, INPUT);

    Serial.begin(500000);

    initialize_adc(); 
    allocate_memory();

    create_frequency_vec();

    freq_idx = get_frequency_index(1000);

     arm_cfft_radix4_init_f32(&fft, BUFFER_SIZE, 0, 1); 
    arm_fir_init_f32(&bpS, NUM_TAPS,(float32_t*)&firCoeffs[0],&bp_firState[0],BLOCK_SIZE);

    samplingBegin();  
  
}

ADC::Sync_result result;

void loop(){
   String datastr = "";
  if (samplingIsDone())
  {
    bpFilter();
    
    for(int i=0; i<BUFFER_SIZE; i++){
      Serial.println(String(*(samples_a0+i))+","+ String(*(samples_a0_filt+i))+",");
    }


      samplingBegin();
  }
	
}

///////////////////////////////////////////////////////////////////////////////
// ADC Initialization FUNCTIONS
///////////////////////////////////////////////////////////////////////////////

void initialize_adc()
{
  int M = 2;
  
  ///// ADC0 ////
  adc->setAveraging(M,ADC_0); // set number of averages
  adc->setResolution(adcResolution,ADC_0); // set bits of resolution
  // it can be any of the ADC_CONVERSION_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED_16BITS, HIGH_SPEED or VERY_HIGH_SPEED
  // see the documentation for more information
  // additionally the conversion speed can also be ADACK_2_4, ADACK_4_0, ADACK_5_2 and ADACK_6_2,
  // where the numbers are the frequency of the ADC clock in MHz and are independent on the bus speed.
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED,ADC_0); // change the conversion speed
  // it can be any of the ADC_MED_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED or VERY_HIGH_SPEED
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED,ADC_0); // change the sampling speed

  ////// ADC1 /////
  adc->setAveraging(M, ADC_1); // set number of averages
  adc->setResolution(adcResolution, ADC_1); // set bits of resolution
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED, ADC_1); // change the conversion speed
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED, ADC_1); // change the sampling speed
  // Read the values in the loop() with readSynchronizedContinuous()
  adc->startSynchronizedContinuous(readPin1, readPin2);
  //
 pedestal = (int)(pow(2, adcResolution)/2); 
}

///////////////////////////////////////////////////////////////////////////////
// Memory Allocation FUNCTIONS
///////////////////////////////////////////////////////////////////////////////

void allocate_memory()
{
  samples_a0 = (float*)malloc(N*4*BUFFER_SIZE);
  samples_a1 = (float*)malloc(N*4*BUFFER_SIZE);    
 
  samples_a0_mag = (float*)malloc(4*BUFFER_SIZE);
  samples_a1_mag = (float*)malloc(4*BUFFER_SIZE);  

  freqs = (float*)malloc(4*BUFFER_SIZE);  

  samples_a0_filt = (float*)malloc(N*4*BUFFER_SIZE);
  samples_a1_filt = (float*)malloc(N*4*BUFFER_SIZE);    
 
  samples_a0_mag_filt = (float*)malloc(4*BUFFER_SIZE);
  samples_a1_mag_filt = (float*)malloc(4*BUFFER_SIZE); 

  // xcorr_output = (float32_t*)malloc(2 * sizeof(float32_t) * BUFFER_SIZE-1);
  // xcorr_temp = (float32_t*)malloc(2 * sizeof(float32_t) * BUFFER_SIZE-1);
  // power_a0 = (float32_t*)malloc(sizeof(float32_t));
  // power_a1 = (float32_t*)malloc(sizeof(float32_t));
  // pwr_a0 = (float32_t*)malloc(sizeof(float32_t));
  // pwr_a1 = (float32_t*)malloc(sizeof(float32_t));
  // peakResult = (float32_t*)malloc(sizeof(float32_t));
  // peakIndex = (uint32_t*)malloc(sizeof(uint32_t));
}

///////////////////////////////////////////////////////////////////////////////
// Frequency Indexing FUNCTIONS
///////////////////////////////////////////////////////////////////////////////
     
void create_frequency_vec()
{
  float df = SAMPLE_FREQ_HZ/BUFFER_SIZE;
  
  for(int i = 0; i<BUFFER_SIZE; i++){
    *(freqs + i) = df*i;
  }
}

int get_frequency_index(float input_freq)
{ 

  float32_t* freq_temp;
  float32_t minval;
  uint32_t idx;
  
  freq_temp  =  (float*)malloc(4*BUFFER_SIZE);
  
  arm_copy_f32(freqs, freq_temp, BUFFER_SIZE);

  arm_offset_f32(freq_temp, -1*input_freq, freq_temp, BUFFER_SIZE);
  arm_abs_f32(freq_temp, freq_temp, BUFFER_SIZE);
  arm_min_f32(freq_temp, BUFFER_SIZE, &minval, &idx);
  // Serial.println(*(freqs+idx+1));
  // for (int i = 0; i<BUFFER_SIZE; i++){
  //   Serial.print(*(freqs +i));
  //   Serial.print(",");
  // }
  // Serial.println();
  free(freq_temp);

  return idx;
   
}

///////////////////////////////////////////////////////////////////////////////
// ADC SAMPLING FUNCTIONS
///////////////////////////////////////////////////////////////////////////////

void samplingBegin() 
{
  // Reset sample buffer position and start callback at necessary rate.
  sample_index = 0;
  samplingTimer.begin(samplingCallback, 1000000/SAMPLE_FREQ_HZ); //run function "samplingCallback" continuosly at intervals of 1000000/SAMPLE_RATE_HZ microseconds. Uses IntervalTimer object linked at https://www.pjrc.com/teensy/td_timing_IntervalTimer.html
}

void samplingCallback() 
{
  dual_sample();

  float scale = 1.0;
  
  *(samples_a0+N*sample_index) = scale*((uint16_t)result.result_adc0 - pedestal);
  *(samples_a1+N*sample_index) = scale*((uint16_t)result.result_adc1 - pedestal);

  *(samples_a0+N*sample_index+1) = 0.0;
  *(samples_a1+N*sample_index+1) = 0.0;

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

///////////////////////////////////////////////////////////////////////////////
// Filter FUNCTIONS
///////////////////////////////////////////////////////////////////////////////

void bpFilter(){

  for(int i=0; i<BLOCK_SIZE; i++){
    arm_fir_f32(&bpS, samples_a0+N*i, samples_a0_filt+N*i, N*BLOCK_SIZE);
    arm_fir_f32(&bpS, samples_a1+N*i, samples_a1_filt+N*i, N*BLOCK_SIZE);
  }
}

