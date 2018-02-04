/* Example for analogContinuousReadDifferential

*/

#include <ADC.h>

ADC *adc = new ADC(); // adc object

#define REFPIN  10

float signal1;
float gain = 0.01;

float deriv=0;
float deriv2=0;

float oldsignal=0;
float oldderiv=0;


const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average
int value = 0;
int prevvalue = 0;
  int latestreading=0;
void setup() {

//setup my averager
 
  pinMode(LED_BUILTIN, OUTPUT);


  pinMode(A10, INPUT); //Diff Channel 0 Positive
  pinMode(A11, INPUT); //Diff Channel 0 Negative

//This is just fancy PWM
 // pinMode(REFPIN, OUTPUT);
  analogWriteResolution(12);  // analogWrite at 12 bit can take value 0 to 4095, or 4096 for high
 // analogWrite(REFPIN,  2000 );
    //This is proper DAC // Good use was 20 and no resolutionchange // 4000 and resolution was good
 analogWrite(A14,(int)4090);
  

 // analogWrite(A14,(unit16_t)(4095.0*(value - min)/(max - min)));


  ///// ADC0 ////
//   reference can be ADC_REFERENCE::REF_3V3, ADC_REFERENCE::REF_1V2 (not for Teensy LC) or ADC_REFERENCE::REF_EXT.
  adc->setReference(ADC_REFERENCE::REF_1V2, ADC_0); // change all 3.3 to 1.2 if you change the reference to 1V2

//GOOD COMBO IS Averaging = 1600, CONV = LOW, SAMPLING = HIGH
//GOOD COMBO IS Av = 100, CONV = MED, SAMP = HIGH
  adc->setAveraging(32); // set number of averages // can be 0, 4, 8, 16 or 32
  adc->setResolution(16); // set bits of resolution

  // it can be any of the ADC_CONVERSION_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED_16BITS, HIGH_SPEED or VERY_HIGH_SPEED
  // see the documentation for more information
  // additionally the conversion speed can also be ADACK_2_4, ADACK_4_0, ADACK_5_2 and ADACK_6_2,
  // where the numbers are the frequency of the ADC clock in MHz and are independent on the bus speed.
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_LOW_SPEED); // change the conversion speed
  // it can be any of the ADC_MED_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED or VERY_HIGH_SPEED
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_LOW_SPEED); // change the sampling speed

  // always call the compare functions after changing the resolution!
  //adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_0), 0, ADC_0); // measurement will be ready if value < 1.0V
  //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_0)/3.3, 2.0*adc->getMaxValue(ADC_0)/3.3, 0, 1, ADC_0); // ready if value lies out of [1.0,2.0] V


  adc->startContinuous(A10, ADC_0);
  //adc->startContinuousDifferential(A10, A11, ADC_0); 
  //adc->enablePGA (2,ADC_0) ; //can be 1, 2, 4, 8, 16, 32 or 64 // just gives me a flat 0

  //Start it off with an average calibration
 for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = (uint16_t)adc->analogReadContinuous(ADC_0); 
      total = total + readings[thisReading];
  }
  average = total / numReadings;
  Serial.begin(9600);


  delay(500);
}


float adcflo = 4096.0;
void loop() {
//adcflo=adcflo-3;
// analogWrite(A14,(int)adcflo);
 

  value = (uint16_t)adc->analogReadContinuous(ADC_0); // the unsigned is necessary for 16 bits, otherwise values larger than 3.3/2 V are negative!


//Try a method where detections are not Added to the average
  deriv=average-value; //old average minus latest reading

//if(deriv<20){ // only calculate the average during resting state? noise level of 20
//Average Stuff

   // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  latestreading = value;
  readings[readIndex] = latestreading;
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  
//update the new average
  average = total / numReadings;

//}
  int diffvalueAVE = average - value; // latest average - latest reading

 
 // deriv2=deriv-oldderiv;
 //     oldderiv=deriv;
 // oldsignal=average;


//  Serial.print(adcflo*10);
//Serial.print(",");
//
//  Serial.print(37000);
//Serial.print(",");
  Serial.println(value);
 
    
 
  //  Serial.println(deriv);

   // Serial.println(average);

   //Serial.println(diffvalueAVE);

  //Visualization
  if (abs(diffvalueAVE) > 40) {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else {
    digitalWrite(LED_BUILTIN, LOW);

  }

  prevvalue = value;

  delay(10);

}

void adc0_isr(void) {
  adc->analogReadContinuous(ADC_0);
  digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN)); // Toggle the led
}


// RESULTS OF THE TEST Teesny 3.x
// Measure continuously a voltage divider.
// Measurement pin A9 (23). Clock speed 96 Mhz, bus speed 48 MHz.

//
//  Using ADC_LOW_SPEED (same as ADC_VERY_LOW_SPEED) for sampling and conversion speeds
// ADC resolution     Measurement frequency                 Num. averages
//     16  bits            81 kHz                               1
//     12  bits            94 kHz                               1
//     10  bits            94 kHz                               1
//      8  bits           103 kHz                               1

//     16  bits             2.5 kHz                              32
//     12  bits             2.9 kHz                              32
//     10  bits             2.9 kHz                              32
//      8  bits             3.2 kHz                              32

//
//  Using ADC_MED_SPEED for sampling and conversion speeds
// ADC resolution     Measurement frequency                 Num. averages
//     16  bits           193 kHz                               1
//     12  bits           231 kHz                               1
//     10  bits           231 kHz                               1
//      8  bits           261 kHz                               1

//     10  bits            58 kHz                               4 (default settings) corresponds to about 17.24 us

//
//  Using ADC_HIGH_SPEED (same as ADC_HIGH_SPEED_16BITS) for sampling and conversion speeds
// ADC resolution     Measurement frequency                 Num. averages
//     16  bits           414 kHz                               1
//     12  bits           500 kHz                               1
//     10  bits           500 kHz                               1
//      8  bits           571 kHz                               1
//
//      8  bits           308 kHz                               1 ADC_VERY_LOW_SPEED sampling
//      8  bits           387 kHz                               1 ADC_LOW_SPEED sampling
//      8  bits           480 kHz                               1 ADC_MED_SPEED sampling
//      8  bits           632 kHz                               1 ADC_VERY_HIGH_SPEED sampling

//
//  Using ADC_VERY_HIGH_SPEED for sampling and conversion speeds
//  This conversion speed is over the limit of the specs! (speed=24MHz, limit = 18 MHz for res<16 and 12 for res=16)
// ADC resolution     Measurement frequency                 Num. averages
//     16  bits           888 kHz                               1
//     12  bits          1090 kHz                               1
//     10  bits          1090 kHz                               1
//      8  bits          1262 kHz                               1


// At 96 Mhz (bus at 48 MHz), 632 KHz is the fastest we can do within the specs, and only if the sample's impedance is low enough.


// RESULTS OF THE TEST Teensy LC
// Measure continuously a voltage divider.
// Measurement pin A9 (23). Clock speed 48 Mhz, bus speed 24 MHz.

//
//  Using ADC_VERY_LOW_SPEED for sampling and conversion speeds
// ADC resolution     Measurement frequency                 Num. averages
//     16  bits            33.3 kHz                               1
//     12  bits            37.5 kHz                               1
//     10  bits            37.5 kHz                               1
//      8  bits            40.5 kHz                               1

//     16  bits             1.04 kHz                             32
//     12  bits             1.2 kHz                              32
//     10  bits             1.2 kHz                              32
//      8  bits             1.3 kHz                              32

//
//  ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED_16BITS, ADC_HIGH_SPEED and ADC_VERY_HIGH_SPEED are the same for Teensy 3.x and LC,
//  except for a very small amount that depends on the bus speed and not on the ADC clock (which is the same for those speeds).
//  This difference corresponds to 5 bus clock cycles, which is about 0.1 us.
//
//  For 8 bits resolution, 1 average, ADC_MED_SPEED sampling speed the measurement frequencies for the different ADACK are:
//  ADC_ADACK_2_4       106.8 kHz
//  ADC_ADACK_4_0       162.6 kHz
//  ADC_ADACK_5_2       235.1 kHz
//  ADC_ADACK_6_2       263.3 kHz
//  For Teensy 3.x the results are similar but not identical for two reasons: the bus clock plays a small role in the total time and
//  the frequency of this ADACK clock is actually quite variable, the values are the typical ones, but in the electrical datasheet
//  it says that they can range from +-50% their values approximately, so every Teensy can have different frequencies.
