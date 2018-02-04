/* Example for analogRead
*  You can change the number of averages, bits of resolution and also the comparison value or range.
*/


#include <ADC.h>

const int readPin = A9; // ADC0
const int readPin2 = A2; // ADC1

ADC *adc = new ADC(); // adc object;


//Averaging Stuff
const int numReadings = 20;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
float average = 0;                // the average
int value = 0;
int prevvalue = 0;
float prevAve = 0;
int latestreading = 0;

float deriv1=0;
float prevDeriv1=0;
float deriv2=0;

//int value;
int value2;


void setup() {

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(readPin, INPUT);
    pinMode(readPin2, INPUT);

    pinMode(A10, INPUT); //Diff Channel 0 Positive
    pinMode(A11, INPUT); //Diff Channel 0 Negative


  //DAC This is just fancy PWM
  // pinMode(REFPIN, OUTPUT);
  analogWriteResolution(12);  // analogWrite value 0 to 4095, or 4096 for high
    // analogWrite(REFPIN,  2000 );

  //This is proper DAC // Good use was 20 and no resolutionchange // 4000 and resolution was good
  analogWrite(A14, (int)3000);

    Serial.begin(9600);

    Serial.println("Begin setup");

    ///// ADC0 ////
    // reference can be ADC_REFERENCE::REF_3V3, ADC_REFERENCE::REF_1V2 (not for Teensy LC) or ADC_REFERENCE::REF_EXT.
    //adc->setReference(ADC_REFERENCE::REF_1V2, ADC_0); // change all 3.3 to 1.2 if you change the reference to 1V2

    adc->setAveraging(32); // set number of averages
    adc->setResolution(16); // set bits of resolution

    // it can be any of the ADC_CONVERSION_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED_16BITS, HIGH_SPEED or VERY_HIGH_SPEED
    // see the documentation for more information
    // additionally the conversion speed can also be ADACK_2_4, ADACK_4_0, ADACK_5_2 and ADACK_6_2,
    // where the numbers are the frequency of the ADC clock in MHz and are independent on the bus speed.
    adc->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_LOW_SPEED); // change the conversion speed
    // it can be any of the ADC_MED_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED or VERY_HIGH_SPEED
    adc->setSamplingSpeed(ADC_SAMPLING_SPEED::LOW_SPEED); // change the sampling speed

    // always call the compare functions after changing the resolution!
    //adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_0), 0, ADC_0); // measurement will be ready if value < 1.0V
    //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_0)/3.3, 2.0*adc->getMaxValue(ADC_0)/3.3, 0, 1, ADC_0); // ready if value lies out of [1.0,2.0] V

    // If you enable interrupts, notice that the isr will read the result, so that isComplete() will return false (most of the time)
    //adc->enableInterrupts(ADC_0);

  adc->enablePGA  (8, ADC_0) ; //can be 1, 2, 4, 8, 16, 32 or 64 //  will amplify signals of less that 1.2V only in differential mode (see examples). Only for Teensy 3.1/3.2.


    Serial.println("End setup");

}


void loop() {


    value = adc->adc0->analogReadDifferential(A10, A11); // read a new value, will return ADC_ERROR_VALUE if the comparison is false.



    if(adc->adc0->fail_flag) {
        adc->adc0->printError();
    }
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

//LETS DO SOME CALCULUS

deriv1=average-prevAve;

deriv2=deriv1-prevDeriv1;


  //  digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

//Load variables for next round

prevAve=average;
prevvalue = value;
prevDeriv1=deriv1;

//DISPLAY INFO
   // Serial.println(value, DEC);

    // Serial.println(average);
      //  Serial.println(deriv1);
     Serial.println(deriv2);

    delay(7); //scrolling speed of information
}

// If you enable interrupts make sure to call readSingle() to clear the interrupt.
void adc0_isr() {
        adc->adc0->readSingle();
}

