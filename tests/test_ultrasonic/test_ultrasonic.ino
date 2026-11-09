// =====================================================================
// TEST 4 : TWO HC-SR04 ULTRASONIC SENSORS ON THE CHEST  :  ARDUINO NANO SKETCH
// =====================================================================
// What this sketch does, in plain words:
//   1. It pings sensor 1 and sensor 2, one after the other, 5 times each.
//   2. For each sensor it keeps the MIDDLE value of the 5 (the "median").
//      This throws away the crazy jumps that ultrasonic sensors often give.
//   3. It smooths that value a little more, so small shaking disappears
//      and only the slow movement of breathing stays.
//   4. It prints both distances about 10 times every second.
//
// One line looks like this (made for the Serial Plotter):
//   sensor1:23.41,sensor2:57.88
// Open Tools > Serial Plotter to SEE the chest go up and down as a wave.
//
// Board to choose in Arduino IDE:  Tools > Board > Arduino AVR Boards > Arduino Nano
// Processor (if upload fails, try the other one):  ATmega328P  or  ATmega328P (Old Bootloader)
// Speed to choose in Serial Monitor or Serial Plotter:  115200 baud
//
// The Nano works at 5V, like the HC-SR04, so no resistors are needed.
// =====================================================================


// ---------------------------------------------------------------------
// PART 1 : WHICH PIN IS EACH SENSOR CONNECTED TO
// ---------------------------------------------------------------------

const int TRIG_1_PIN = 2;   // sensor 1 Trig goes to the pin labelled D2
const int ECHO_1_PIN = 3;   // sensor 1 Echo goes to the pin labelled D3
const int TRIG_2_PIN = 4;   // sensor 2 Trig goes to the pin labelled D4
const int ECHO_2_PIN = 5;   // sensor 2 Echo goes to the pin labelled D5


// ---------------------------------------------------------------------
// PART 2 : SETTINGS YOU ARE ALLOWED TO CHANGE
// ---------------------------------------------------------------------

const int PINGS_PER_READING = 5;               // how many pings we take for each sensor before keeping the middle one (use an odd number)
const float SMOOTHING = 0.3;                   // how much a new reading counts: 1.0 = no smoothing, 0.1 = very smooth but slow
const float MIN_DISTANCE_CM = 2.0;             // readings closer than this are wrong, so we ignore them
const float MAX_DISTANCE_CM = 60.0;            // readings farther than this are not the chest, so we ignore them
const unsigned long ECHO_TIMEOUT_US = 6000;    // wait at most 6000 microseconds for an echo (about 1 metre away, plenty for a chest)
const int PAUSE_BETWEEN_PINGS_MS = 10;         // wait 10 milliseconds between pings, so old echoes have died out
const float SPEED_OF_SOUND_CM_PER_US = 0.0343; // sound travels 0.0343 centimetres every microsecond (at about 20 degrees C)


// ---------------------------------------------------------------------
// PART 3 : VARIABLES (boxes that remember numbers while the sketch runs)
// ---------------------------------------------------------------------

float pings1[PINGS_PER_READING];   // a list that keeps the 5 pings of sensor 1
float pings2[PINGS_PER_READING];   // a list that keeps the 5 pings of sensor 2
float smooth1 = -1;                // the smoothed distance of sensor 1 (-1 means "no value yet")
float smooth2 = -1;                // the smoothed distance of sensor 2 (-1 means "no value yet")


// ---------------------------------------------------------------------
// PART 4 : SETUP (runs one time when the Nano turns on)
// ---------------------------------------------------------------------

void setup() {                          // start of setup
  Serial.begin(115200);                 // open the USB connection at 115200 baud
  pinMode(TRIG_1_PIN, OUTPUT);          // sensor 1 Trig is an output: the Nano talks to the sensor
  pinMode(ECHO_1_PIN, INPUT);           // sensor 1 Echo is an input: the Nano listens to the sensor
  pinMode(TRIG_2_PIN, OUTPUT);          // sensor 2 Trig is an output
  pinMode(ECHO_2_PIN, INPUT);           // sensor 2 Echo is an input
  digitalWrite(TRIG_1_PIN, LOW);        // make sure sensor 1 Trig starts at LOW (quiet)
  digitalWrite(TRIG_2_PIN, LOW);        // make sure sensor 2 Trig starts at LOW (quiet)
  delay(500);                           // wait half a second so the sensors can wake up
}                                       // end of setup


// ---------------------------------------------------------------------
// PART 5 : A HELPER THAT PINGS ONE SENSOR ONE TIME
// ---------------------------------------------------------------------
// It gives back the distance in centimetres, or -1 if the reading is no good.

float pingOnce(int trigPin, int echoPin) {                 // start of the helper
  digitalWrite(trigPin, LOW);                              // set Trig LOW first, to start from a clean state
  delayMicroseconds(2);                                    // keep it LOW for 2 microseconds
  digitalWrite(trigPin, HIGH);                             // set Trig HIGH: this tells the sensor "send a ping now"
  delayMicroseconds(10);                                   // keep it HIGH for 10 microseconds (the sensor needs at least 10)
  digitalWrite(trigPin, LOW);                              // set Trig LOW again: the ping is sent
  unsigned long echoTime = pulseIn(echoPin, HIGH, ECHO_TIMEOUT_US);   // measure how long Echo stays HIGH (in microseconds)
  float distance = echoTime * SPEED_OF_SOUND_CM_PER_US / 2;           // time to distance, divided by 2 because the sound goes there and back
  if (distance < MIN_DISTANCE_CM) {                        // too close (this also catches 0, which means "no echo")
    return -1;                                             // give back -1 to say "bad reading"
  }                                                        // end of the "too close" check
  if (distance > MAX_DISTANCE_CM) {                        // too far, so it is not the chest
    return -1;                                             // give back -1 to say "bad reading"
  }                                                        // end of the "too far" check
  return distance;                                         // the reading is good: give it back
}                                                          // end of the helper


// ---------------------------------------------------------------------
// PART 6 : A HELPER THAT FINDS THE MIDDLE VALUE OF A LIST
// ---------------------------------------------------------------------
// It puts the good readings in order from small to big, then takes the middle one.
// It gives back -1 if all the readings were bad.

float middleValue(float list[]) {                          // start of the helper
  float good[PINGS_PER_READING];                           // a new list for the good readings only
  int goodCount = 0;                                       // how many good readings we found
  for (int i = 0; i < PINGS_PER_READING; i++) {            // look at every reading in the list
    if (list[i] > 0) {                                     // a good reading is bigger than 0 (bad ones are -1)
      good[goodCount] = list[i];                           // copy the good reading into the new list
      goodCount = goodCount + 1;                           // we have one more good reading
    }                                                      // end of the "good reading" check
  }                                                        // end of looking at every reading
  if (goodCount == 0) {                                    // if no reading was good
    return -1;                                             // give back -1 to say "no value"
  }                                                        // end of the "no good readings" check
  for (int a = 0; a < goodCount; a++) {                    // sort the list: repeat once for every reading
    for (int b = 0; b < goodCount - 1; b++) {              // walk through the list, comparing neighbours
      if (good[b] > good[b + 1]) {                         // if the left one is bigger than the right one
        float temporary = good[b];                         // remember the left one
        good[b] = good[b + 1];                             // move the right one to the left
        good[b + 1] = temporary;                           // put the remembered one on the right
      }                                                    // end of the swap
    }                                                      // end of walking through the list
  }                                                        // end of sorting
  int middle = goodCount / 2;                              // find the position in the middle of the sorted list
  return good[middle];                                     // give back the middle value
}                                                          // end of the helper


// ---------------------------------------------------------------------
// PART 7 : A HELPER THAT SMOOTHS A VALUE
// ---------------------------------------------------------------------
// It mixes the new value with the old smoothed value, so the line moves gently.

float smoothValue(float oldSmooth, float newValue) {       // start of the helper
  if (newValue < 0) {                                      // the new value is bad
    return oldSmooth;                                      // keep the old value, do not change anything
  }                                                        // end of the "bad value" check
  if (oldSmooth < 0) {                                     // there is no old value yet (the very first time)
    return newValue;                                       // just start from the new value
  }                                                        // end of the "first time" check
  float result = oldSmooth + SMOOTHING * (newValue - oldSmooth);   // move the old value a little bit towards the new one
  return result;                                           // give back the smoothed value
}                                                          // end of the helper


// ---------------------------------------------------------------------
// PART 8 : LOOP (runs again and again forever)
// ---------------------------------------------------------------------

void loop() {                                                   // start of loop
  for (int i = 0; i < PINGS_PER_READING; i++) {                 // do this 5 times
    pings1[i] = pingOnce(TRIG_1_PIN, ECHO_1_PIN);               // ping sensor 1 and keep the result
    delay(PAUSE_BETWEEN_PINGS_MS);                              // wait so the sound of sensor 1 fades away
    pings2[i] = pingOnce(TRIG_2_PIN, ECHO_2_PIN);               // ping sensor 2 and keep the result
    delay(PAUSE_BETWEEN_PINGS_MS);                              // wait so the sound of sensor 2 fades away
  }                                                             // end of the 5 pings
  float middle1 = middleValue(pings1);                          // keep the middle value of sensor 1
  float middle2 = middleValue(pings2);                          // keep the middle value of sensor 2
  smooth1 = smoothValue(smooth1, middle1);                      // smooth sensor 1
  smooth2 = smoothValue(smooth2, middle2);                      // smooth sensor 2
  Serial.print("sensor1:");                                     // print the label for sensor 1
  Serial.print(smooth1, 2);                                     // print sensor 1 with 2 numbers after the dot
  Serial.print(",sensor2:");                                    // print a comma and the label for sensor 2
  Serial.print(smooth2, 2);                                     // print sensor 2 with 2 numbers after the dot
  Serial.println();                                             // finish the line, so the next one starts underneath
}                                                               // end of loop
