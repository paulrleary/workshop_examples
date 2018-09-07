/* 
 *  Example for sampling into memory buffers, and computing FFTs
*/

#include <ADC.h>
#include <arm_math.h>  //Important math library, which will be used frequently in this course.  reference: http://www.keil.com/pack/doc/cmsis/DSP/html/modules.html

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

float *samples_a0_mag;    //These will store our FFT magnitudes
float *samples_a1_mag;

// x-Correlation math
float32_t *xcorr_output;  //  sclaed x-correlation
float32_t *xcorr_temp;    //  unscaled x-correlation
float32_t *power_a0;      //  power of samples_a0
float32_t *power_a1;      //  power of samples_a1
float32_t *pwr_a0;        //  power of samples_a0
float32_t *pwr_a1;        //  power of samples_a1
float32_t *peakResult;    //  peak of x_correlation plot
uint32_t *peakIndex;      //  index of the peak

float *freqs;             // stores an array of frequences for better visual plotting 

IntervalTimer samplingTimer;

elapsedMicros time;

void setup() {

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(readPin1, INPUT);
    pinMode(readPin2, INPUT);

    Serial.begin(2000000);

    initialize_adc(); 
  
    allocate_memory();

    create_frequency_vec();

    //This initializes our fft object: http://www.keil.com/pack/doc/cmsis/DSP/html/group__ComplexFFT.html#gac9565e6bc7229577ecf5e090313cafd7
    arm_cfft_radix4_init_f32(&fft, BUFFER_SIZE, 0, 1); 
    
    samplingBegin();  
}


ADC::Sync_result result;

void loop() {
  String datastr = "";
  if (samplingIsDone())
      { 
        /* x-Correlation*/
        crossCorrelate();
        // print xcorr data to serial_plotter
        bool printXCORR = 0;
        if(printXCORR) {
          datastr = printXCorr();
        }
        // print xcorr data to python stream
        bool printXCORR2python = 1;
        if(printXCORR2python) {
          datastr = pythonXCorr();
        }

        /* global print out*/
        Serial.println(datastr);
        /*while we generally discourage use of delays, we need to put something here to slow the serial printing, or we overwhelm the line.*/
        delay(50);
        
        /* keep sampling*/
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
  samples_a0 = (float*)malloc(2*4*BUFFER_SIZE);
  samples_a1 = (float*)malloc(2*4*BUFFER_SIZE);    
 
  samples_a0_mag = (float*)malloc(4*BUFFER_SIZE);
  samples_a1_mag = (float*)malloc(4*BUFFER_SIZE);  

  freqs = (float*)malloc(4*BUFFER_SIZE);  

  xcorr_output = (float32_t*)malloc(2 * sizeof(float32_t) * BUFFER_SIZE-1);
  xcorr_temp = (float32_t*)malloc(2 * sizeof(float32_t) * BUFFER_SIZE-1);
  power_a0 = (float32_t*)malloc(sizeof(float32_t));
  power_a1 = (float32_t*)malloc(sizeof(float32_t));
  pwr_a0 = (float32_t*)malloc(sizeof(float32_t));
  pwr_a1 = (float32_t*)malloc(sizeof(float32_t));
  peakResult = (float32_t*)malloc(sizeof(float32_t));
  peakIndex = (uint32_t*)malloc(sizeof(uint32_t));
}

///////////////////////////////////////////////////////////////////////////////
// Frequency Indexing FUNCTIONS
///////////////////////////////////////////////////////////////////////////////
     
void create_frequency_vec()
{
  float df = SAMPLE_FREQ_HZ/BUFFER_SIZE;
  
  for(int i = 1; i<=BUFFER_SIZE; i++){
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
  
  *(samples_a0+2*sample_index) = scale*((uint16_t)result.result_adc0 - pedestal);
  *(samples_a1+2*sample_index) = scale*((uint16_t)result.result_adc1 - pedestal);

  *(samples_a0+2*sample_index+1) = 0.0;
  *(samples_a1+2*sample_index+1) = 0.0;

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
// X-correlation FUNCTIONS
///////////////////////////////////////////////////////////////////////////////
void crossCorrelate()
{
  /* xCorr A2&A3 and normalization of centered data*/ 
  /* x-Correlation*/
  arm_correlate_f32(samples_a0, BUFFER_SIZE, samples_a1, BUFFER_SIZE, xcorr_temp); 
//  arm_correlate_f32(samples_a0, BUFFER_SIZE, samples_a1, BUFFER_SIZE, xcorr_output); 
  /* x-Corr normalization by the power of signals
    http://www.keil.com/pack/doc/CMSIS/DSP/html/group__power.html
    http://www.keil.com/pack/doc/CMSIS/DSP/html/group__SQRT.html
    http://www.keil.com/pack/doc/CMSIS/DSP/html/group__scale.html
    pwr_ax - hold results of intermediate calculations of power
    xcorr_temp - is an unscaled version of cross correlation */
  
  arm_power_f32 (samples_a0, BUFFER_SIZE, power_a0);
  arm_sqrt_f32 (*power_a0, pwr_a0);
  arm_power_f32 (samples_a1, BUFFER_SIZE, power_a1);
  arm_sqrt_f32 (*power_a1, pwr_a1);
  arm_scale_f32 (xcorr_temp, 1. / (*pwr_a0 * (*pwr_a1)), xcorr_output, 2 * BUFFER_SIZE);

  /* Peak finding*/
  arm_max_f32(xcorr_output, 2 * BUFFER_SIZE, peakResult, peakIndex);
}

String pythonXCorr()
{  // reset the string
        String tmpstr = "";
        tmpstr += (String(2*BUFFER_SIZE)+","+String(SAMPLE_FREQ_HZ)+",");
        tmpstr += (String(*peakIndex)+","+String(*peakResult));
          for (int i = 0; i < 2 * BUFFER_SIZE - 1; i++){
            tmpstr += (","+String(*(xcorr_output + i)));
          }// end of for
        return tmpstr;
  }// end of printXcorr

String printXCorr()
{          // reset the string
          String tmpstr = "";
          for (int i = 0; i < 2 * BUFFER_SIZE - 1; i++)
          {
            tmpstr += (String(*(xcorr_output + i)) + ",");
            /* adding Peak index*/
            if (i == (*peakIndex - 1))
              tmpstr += ((String)(*peakResult) + ',');
            else
              tmpstr += ((String)(0) + ',');
            /* add the index of x-corr center*/
            if (i == (BUFFER_SIZE - 1))
              tmpstr += ((String)(*peakResult) + '\n');
            else if (i == BUFFER_SIZE)
              tmpstr += ((String)(-*peakResult) + '\n');
            else
              tmpstr += ((String)(0) + '\n' );
          }// end of for
          return tmpstr;
  }// end of printXcorr


