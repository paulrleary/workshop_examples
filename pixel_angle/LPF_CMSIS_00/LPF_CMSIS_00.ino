#include "arm_math.h"
#include "math_helper.h"
#include "arm_fir_data.h"
/* ----------------------------------------------------------------------
** Macro Defines
** ------------------------------------------------------------------- */
#define TEST_LENGTH_SAMPLES  320
#define SNR_THRESHOLD_F32    140.0f
#define BLOCK_SIZE            320
#define NUM_TAPS              29

/* -------------------------------------------------------------------
   The input signal and reference output (computed with MATLAB)
   are defined externally in arm_fir_lpf_data.c.
   ------------------------------------------------------------------- */
extern float32_t testInput_f32_1kHz_15kHz[TEST_LENGTH_SAMPLES];
extern float32_t refOutput[TEST_LENGTH_SAMPLES];
/* -------------------------------------------------------------------
   Declare Test output buffer
   ------------------------------------------------------------------- */
static float32_t lp_testOutput[TEST_LENGTH_SAMPLES];
static float32_t hp_testOutput[TEST_LENGTH_SAMPLES];
/* -------------------------------------------------------------------
   Declare State buffer of size (numTaps + blockSize - 1)
   ------------------------------------------------------------------- */
static float32_t lp_firStateF32[BLOCK_SIZE + NUM_TAPS - 1];
static float32_t hp_firStateF32[BLOCK_SIZE + NUM_TAPS - 1];
/* ----------------------------------------------------------------------
** FIR Coefficients buffer generated using fir1() MATLAB function.
** fir1(28, 6/24)
** ------------------------------------------------------------------- */
const float32_t firCoeffs32[NUM_TAPS] = {
  -0.0018225230f, -0.0015879294f, +0.0000000000f, +0.0036977508f, +0.0080754303f, +0.0085302217f, -0.0000000000f, -0.0173976984f,
  -0.0341458607f, -0.0333591565f, +0.0000000000f, +0.0676308395f, +0.1522061835f, +0.2229246956f, +0.2504960933f, +0.2229246956f,
  +0.1522061835f, +0.0676308395f, +0.0000000000f, -0.0333591565f, -0.0341458607f, -0.0173976984f, -0.0000000000f, +0.0085302217f,
  +0.0080754303f, +0.0036977508f, +0.0000000000f, -0.0015879294f, -0.0018225230f
};
/** b = fir1(48,[10/24, 20/24]) */
const float32_t firCoeffBP[NUM_TAPS] = {
 -0.00066425f,  0.00327798f, -0.00000000f, -0.00763330f,  0.00294323f, -0.00351830f,  0.03009171f, -0.01859058f,
 -0.03400055f,  0.01132976f, -0.00000000f,  0.16258044f, -0.20703268f, -0.14626412f,  0.41571676f, -0.14626412f,
 -0.20703268f,  0.16258044f, -0.00000000f,  0.01132976f, -0.03400055f, -0.01859058f,  0.03009171f, -0.00351830f,
 0.00294323f, -0.00763330f, -0.00000000f,  0.00327798f, -0.00066425f
  };
  
/*   Global variables for FIR LPF Example
   ------------------------------------------------------------------- */
uint32_t blockSize = BLOCK_SIZE;
uint32_t numBlocks = TEST_LENGTH_SAMPLES / BLOCK_SIZE;
float32_t  snr;
int ledState = LOW;
String datastr; // string holder

uint32_t i;
arm_fir_instance_f32 lpS;
arm_fir_instance_f32 hpS;
arm_status status;
float32_t  *inputF32, *lp_outputF32, *hp_outputF32;

void setup() {
  Serial.begin(2000000);
  delay(100);
  pinMode(13, OUTPUT);
  /* Initialize input and output buffer pointers */

  inputF32 = &testInput_f32_1kHz_15kHz[0];
  lp_outputF32 = &lp_testOutput[0];
  hp_outputF32 = &hp_testOutput[0];
  /* Call FIR init function to initialize the instance structure. */
  arm_fir_init_f32(&lpS, NUM_TAPS, (float32_t *)&firCoeffs32[0], &lp_firStateF32[0], blockSize);
    arm_fir_init_f32(&hpS, NUM_TAPS, (float32_t *)&firCoeffBP[0], &hp_firStateF32[0], blockSize);
  /* ----------------------------------------------------------------------
    ** Call the FIR process function for every blockSize samples
    ** ------------------------------------------------------------------- */
  for (i = 0; i < numBlocks; i++)
  {
    arm_fir_f32(&lpS, inputF32 + (i * blockSize), lp_outputF32 + (i * blockSize), blockSize);
  }

    for (i = 0; i < numBlocks; i++)
  {
    arm_fir_f32(&hpS, inputF32 + (i * blockSize), hp_outputF32 + (i * blockSize), blockSize);
  }
}

void loop() {

  /* ----------------------------------------------------------------------
  ** Compare the generated output against the reference output computed
  ** in MATLAB.
  ** ------------------------------------------------------------------- */
  snr = arm_snr_f32(&refOutput[0], &lp_testOutput[0], TEST_LENGTH_SAMPLES);
  if (snr < SNR_THRESHOLD_F32)
  {
    status = ARM_MATH_TEST_FAILURE;
    ledState = HIGH;
  }
  else
  {
    status = ARM_MATH_SUCCESS;
    ledState = LOW; // blink if successful
  }
//    Serial.print("SNR = ");Serial.println(snr);
//    Serial.print("ARM_MATH_STATUS = ");Serial.println(status);

  datastr = "";

  /* here is a cute way of transposing FFT/xCorr data to be plotetd as a frame
    it also shows how to blend multiple signals*/
  for ( i = 0; i < TEST_LENGTH_SAMPLES - 1; i++)
  {
    datastr += (String(*(lp_outputF32 + i)) + ",");
    datastr += (String(*(hp_outputF32 + i)) + ",");
    datastr += (String(*(inputF32 + i)) + '\n');
  }
  Serial.println(datastr);
  Serial.println();

  /* Blink LED if successful */
  digitalWrite(13, ledState);
  delay(2);
  ledState = LOW; // blink if successful
  digitalWrite(13, ledState);
  /* ----------------------------------------------------------------------
  ** Loop here if the signal does not match the reference output.
  ** ------------------------------------------------------------------- */
  if ( status != ARM_MATH_SUCCESS)
  {
    while (1);
  }
//  while (1);                             /* main function does not return */

}
