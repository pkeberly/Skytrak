/*
  Skytrak PWM module replacement.  Paul Keberly  (PKeberly@yahoo.com)

  This code should replace the two custom Skytrak modules that create a PWM signal to controls 
  the 4 Apitech Pulsar boom solenoids in our old Skytrak.  The datasheet says to modulate 
  the solenoids at 33Hz between 40% & 90%, we'll see how that goes.

  I picked off the potentiometer trim values around the time the microswitches close in
  an attempt to linearize the controls and much as possible, the pots are offset a bit.  
  I dropped the pot centertap wiring and went right to a simple inline sweep.  This allowed 
  the trim values to match the joystick sweep and align with the switches better.

  I'm running this on a HiLetgo Nano V3.0 ATmega328Pa from Amazon.  The PWM outputs are 400W MOS transistors
  also from Amazon.  The solenoids need to be rewired as the new PWM devices are low side drive.  I know not
  as safe as high side drivers but I'm desperate to get this thing going again.  The ignition key removes power 
  from the solenoids so there is still a touch of safety.

  June 26th 2023 PWK:  First pass to get the PWM values going.
  

 
*/
#define DEBUG 1
#define NUMB_AXIS 2
#define BOOM_DWN_UP 0
#define BOOM_RET_EXT 1
const int minPWM = 12;               //minimum 40% PWM according to the Pulsar datasheet, changed to 5%
const int maxPWM = 255;              //maximum 90% PWM according to the Pulsar datasheet, changed to 100%
int joyValue[NUMB_AXIS] = { 0, 0 };  // value read from the joystick pot (analog in)
int pwmValue[NUMB_AXIS] = { 0, 0 };  // value output to the PWM (analog out)
const int trimVal[NUMB_AXIS][4] = {
  { 200, 380, 480, 755 },  // boom DWN (min-max), boom UP (min-max)
  { 252, 475, 580, 780 }   //boom RET (min-max), boom EXT (min-max)
};

// These constants won't change. They're used to give names to the pins used:
const int analogInPin_BOOM_DWN_UP = A0;    // Analog input pin that the boom UP/DWN potentiometer is attached to
const int analogInPin_BOOM_RET_EXT = A1;   // Analog input pin that the boom RET/EXT potentiometer is attached to
const int analogOutPin_BOOM_DWN_UP = 9;    // Analog output pin that controls the UP/DWN solenoids
const int analogOutPin_BOOM_RET_EXT = 10;  // Analog output pin that controls the RET/EXT solenoids



void setup() {
  // initialize serial communications at 19200 bps for debugging:
  Serial.begin(19200);
  TCCR1B = TCCR1B & B11111000 | B00000101;  // for PWM frequency of 30.64 Hz
  analogWrite(analogOutPin_BOOM_DWN_UP, 0);
  analogWrite(analogOutPin_BOOM_RET_EXT, 0);
}

void loop() {
  // read both of the joystick analog input values:
  joyValue[BOOM_DWN_UP] = analogRead(analogInPin_BOOM_DWN_UP);
  joyValue[BOOM_RET_EXT] = analogRead(analogInPin_BOOM_RET_EXT);

  // Convert both Joy values to PWM outputs
  for (int joy = 0; joy < NUMB_AXIS; joy++) {
    if (joyValue[joy] < trimVal[joy][1]) {  //Boom DOWN
      pwmValue[joy] = map(joyValue[joy], trimVal[joy][1], trimVal[joy][0], minPWM, maxPWM);
    } else if (joyValue[joy] > trimVal[joy][2]) {  //Boom UP
      pwmValue[joy] = map(joyValue[joy], trimVal[joy][2], trimVal[joy][3], minPWM, maxPWM);
    } else {
      pwmValue[joy] = minPWM;
    }
  }


  // change the analog out value:
  analogWrite(analogOutPin_BOOM_DWN_UP, pwmValue[BOOM_DWN_UP]);
  analogWrite(analogOutPin_BOOM_RET_EXT, pwmValue[BOOM_RET_EXT]);

  // print the results to the Serial Monitor:
  if (DEBUG) {
    Serial.print("raw ");
    Serial.print(joyValue[BOOM_DWN_UP]);
    Serial.print("\t");
    Serial.print(joyValue[BOOM_RET_EXT]);
    Serial.print("\t PWM ");
    Serial.print(pwmValue[BOOM_DWN_UP]);
    Serial.print("\t");
    Serial.println(pwmValue[BOOM_RET_EXT]);
  }

  // wait 2 milliseconds before the next joy for the analog-to-digital
  // converter to settle after the last reading:
  delay(5);
}
