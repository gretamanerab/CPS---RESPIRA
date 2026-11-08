// =====================================================================
// THERMISTOR TEST  :  NOSE AND MOUTH BREATH TEMPERATURE
// BOARD            :  ARDUINO NANO 
// ---------------------------------------------------------------------
// WHAT THIS TEST IS FOR
// ---------------------------------------------------------------------
// A thermistor is a tiny resistor that changes with temperature.
// When it gets WARMER its resistance goes DOWN.
// When it gets COOLER its resistance goes UP.
//
// Air you breathe OUT is warm (about 30 to 34 degrees).
// Air you breathe IN is room air (about 18 to 24 degrees).
// So a thermistor under your nose warms up a little on every breath out,
// and cools down a little on every breath in.
//
// This test shows you that small change, for the nose and for the mouth,
// as moving lines in the Serial Plotter.
//
// ---------------------------------------------------------------------
// WHAT YOU SHOULD SEE
// ---------------------------------------------------------------------
// Two lines in the Serial Plotter:
//   noseChange  : the nose temperature change, in TENTHS of a degree
//   mouthChange : the mouth temperature change, in TENTHS of a degree
// Both lines sit near 0 when nobody is breathing on them.
// Breathe out slowly on the nose thermistor: noseChange climbs (for example to 20,
// which means 2 degrees warmer). Breathe in: it comes back down.
// Breathing normally gives gentle waves, one wave per breath.
//
// Want to see the real temperature in degrees instead?
// Change  SHOW_DEGREES = false  to  SHOW_DEGREES = true  (it is in PART 2 below)
// and press Upload again. Room temperature should show as roughly 18 to 25.
// The degrees are only approximate. What matters is that the lines MOVE with your breath.
//
// ---------------------------------------------------------------------
// THE THERMISTORS
// ---------------------------------------------------------------------
// Type: 2 x 100K NTC 3950 thermistors (glass bead on a 1 m cable).
//   100K : it measures 100,000 ohms at 25 degrees.
//   NTC  : when it gets warmer, its resistance goes down.
//   3950 : the "beta" number, used in PART 5 to work out degrees.
// Nose thermistor on pin A0, mouth thermistor on pin A1.
// The wiring is explained in the hardware folder of this repository.
//
// ---------------------------------------------------------------------
// HOW TO RUN IT, STEP BY STEP
// ---------------------------------------------------------------------
//   1. Wire the thermistors (see the hardware folder) with the USB cable UNPLUGGED.
//   2. Plug the board into the Mac with the USB cable.
//   3. Click Tools > Board > Arduino AVR Boards > Arduino Nano, then Tools > Processor > ATmega328P
//      (if the upload fails, choose ATmega328P (Old Bootloader) instead: most Nano copies need it)
//   4. Click Tools > Port and choose /dev/cu.usbserial... or /dev/cu.wchusbserial... or /dev/cu.usbmodem...
//   5. Click the round arrow button at the top left (Upload).
//   6. Wait for "Done uploading" at the bottom of the window.
//   The Nano does not need any button to upload. If upload fails,
//   check Tools > Port again and make sure no Serial Monitor or
//   Processing sketch is using the cable.
//   7. Click Tools > Serial Plotter.
//   8. At the bottom right of the plotter, choose 115200 baud.
//   9. Breathe on the thermistors and watch the lines.
//  10. When you are done, CLOSE the Serial Plotter. Only one program can
//      use the USB cable at a time (Processing will need it later).
// =====================================================================


// ---------------------------------------------------------------------
// PART 1 : WHICH PIN IS EACH THERMISTOR CONNECTED TO
// ---------------------------------------------------------------------
// "const int" means: a whole number (int) that never changes (const).
// We give each pin a name, so the rest of the code is easier to read.
// We use A0 and A1. Only the pins whose name starts with A (A0 to A7)
// can measure a voltage on the Nano.

const int NOSE_PIN = A0;                        // the nose thermistor's middle point goes to pin A0
const int MOUTH_PIN = A1;                       // the mouth thermistor's middle point goes to pin A1


// ---------------------------------------------------------------------
// PART 2 : SETTINGS YOU ARE ALLOWED TO CHANGE
// ---------------------------------------------------------------------
// "float" means a number that can have decimals, like 0.3 or 100000.0
// "bool" means a yes/no value: true or false

const int HOW_MANY_READINGS = 16;               // we read each thermistor this many times in a row and take the average, to remove jitter
const float FIXED_RESISTOR = 100000.0;          // the resistor going to GND is 100k ohms (100000 ohms)
const float THERMISTOR_AT_25C = 100000.0;       // the thermistor measures 100k ohms at 25 degrees (that is the "100K" in its name)
const float BETA = 3950.0;                      // a number that describes how this thermistor reacts to heat (the "3950" in its name)
const float MAX_READING = 1023.0;            // the biggest number this board can give from a pin (10 bit: 0 to 1023)
const float SMOOTH_SPEED = 0.3;                 // how much jitter we remove: smaller = smoother but slower to react
const float RESTING_SPEED = 0.005;              // how fast the "room temperature" level follows: very small = very slow
const bool SHOW_DEGREES = false;                // false = show the change in tenths of a degree. true = show real degrees.
const int WAIT_MS = 50;                         // wait 50 milliseconds between measurements (so about 20 measurements every second)


// ---------------------------------------------------------------------
// PART 3 : VARIABLES (boxes that remember numbers while the board runs)
// ---------------------------------------------------------------------
// These are NOT const, because their values change all the time.

float noseSmooth = 0;                           // the nose temperature with the jitter removed
float noseResting = 0;                          // the slow "room temperature" level for the nose
float mouthSmooth = 0;                          // the mouth temperature with the jitter removed
float mouthResting = 0;                         // the slow "room temperature" level for the mouth
bool firstTime = true;                          // true only the very first time loop() runs


// ---------------------------------------------------------------------
// PART 4 : A HELPER RECIPE  :  read one pin several times and average
// ---------------------------------------------------------------------
// A "function" is a small recipe with a name. We write it once here,
// and later we only write its name, readAverage(NOSE_PIN), to use it.
// "float readAverage(int pinNumber)" means: this recipe needs a pin number,
// and it gives back a number with decimals.

float readAverage(int pinNumber) {              // start of the recipe
  float total = 0;                              // start a running total at zero
  for (int i = 0; i < HOW_MANY_READINGS; i = i + 1) {   // repeat the next line HOW_MANY_READINGS times (i counts 0, 1, 2 ...)
    total = total + analogRead(pinNumber);      // measure the voltage on the pin (0 to 1023) and add it to the total
  }                                             // end of the repeat
  float average = total / HOW_MANY_READINGS;    // divide the total by how many readings we took
  if (average < 1) {                            // a reading of 0 would break the maths in the next recipe (you cannot divide by 0)
    average = 1;                                // so we change it to 1 (a real 0 means a wire is loose anyway)
  }                                             // end of the "if"
  return average;                               // give the average back to whoever asked for it
}                                               // end of the recipe


// ---------------------------------------------------------------------
// PART 5 : A HELPER RECIPE  :  turn a reading into degrees Celsius
// ---------------------------------------------------------------------
// Step A: from the reading, work out the thermistor's resistance.
//         The thermistor and the 100k resistor share the 5 volts.
//         The reading tells us how the voltage is shared, so we can
//         work out the thermistor's resistance in ohms.
// Step B: from the resistance, work out the temperature with the
//         "beta formula" that thermistor makers give. You do not need
//         to understand the maths: it is split into small steps below.

float readingToCelsius(float reading) {                          // start of the recipe. It needs a reading and gives back degrees.
  float ohms = FIXED_RESISTOR * (MAX_READING - reading) / reading;   // step A: the thermistor's resistance in ohms
  float step1 = log(ohms / THERMISTOR_AT_25C);                    // step B1: compare it with the resistance at 25 degrees (log is a maths button on a calculator)
  float step2 = step1 / BETA;                                     // step B2: divide by the beta number
  float step3 = step2 + (1.0 / 298.15);                           // step B3: add 1 divided by 25 degrees in Kelvin (298.15)
  float kelvin = 1.0 / step3;                                     // step B4: flip it over to get the temperature in Kelvin
  float celsius = kelvin - 273.15;                                // step B5: Kelvin minus 273.15 gives Celsius
  return celsius;                                                 // give the temperature back
}                                                                 // end of the recipe


// ---------------------------------------------------------------------
// PART 6 : setup()  runs ONE time, when the board turns on
// ---------------------------------------------------------------------

void setup() {                                  // start of setup
  Serial.begin(115200);                         // open the USB connection to the Mac at a speed of 115200 (the plotter must use the same number)
  delay(500);                                   // wait half a second so everything settles after power on
}                                               // end of setup


// ---------------------------------------------------------------------
// PART 7 : loop()  runs AGAIN AND AGAIN, forever
// ---------------------------------------------------------------------

void loop() {                                   // start of loop

  // ---------- 1. MEASURE ----------
  float noseReading = readAverage(NOSE_PIN);    // read the nose thermistor (average of several readings)
  float mouthReading = readAverage(MOUTH_PIN);  // read the mouth thermistor (average of several readings)

  // ---------- 2. TURN INTO DEGREES ----------
  float noseCelsius = readingToCelsius(noseReading);     // nose temperature in degrees Celsius
  float mouthCelsius = readingToCelsius(mouthReading);   // mouth temperature in degrees Celsius

  // ---------- 3. THE VERY FIRST TIME, START EVERYTHING AT THE REAL VALUE ----------
  if (firstTime == true) {                      // is this the very first time loop runs?
    noseSmooth = noseCelsius;                   // start the smooth nose value at the real temperature
    noseResting = noseCelsius;                  // start the nose room level at the real temperature
    mouthSmooth = mouthCelsius;                 // start the smooth mouth value at the real temperature
    mouthResting = mouthCelsius;                // start the mouth room level at the real temperature
    firstTime = false;                          // from now on it is not the first time any more
  }                                             // end of the "if"

  // ---------- 4. SMOOTH ----------
  // Smoothing works like this:  smooth = smooth + SPEED * (new value - smooth)
  // Each time, the smooth value moves a little step towards the new value.
  noseSmooth = noseSmooth + SMOOTH_SPEED * (noseCelsius - noseSmooth);         // nose: remove jitter
  mouthSmooth = mouthSmooth + SMOOTH_SPEED * (mouthCelsius - mouthSmooth);     // mouth: remove jitter

  // ---------- 5. FOLLOW THE ROOM TEMPERATURE VERY SLOWLY ----------
  // Same formula, but with a tiny speed, so it ignores single breaths
  // and only follows the slow room temperature.
  noseResting = noseResting + RESTING_SPEED * (noseCelsius - noseResting);     // nose room level
  mouthResting = mouthResting + RESTING_SPEED * (mouthCelsius - mouthResting); // mouth room level

  // ---------- 6. WHAT IS LEFT IS THE BREATH ----------
  float noseChange = (noseSmooth - noseResting) * 10.0;      // nose: difference from room level, times 10 = tenths of a degree
  float mouthChange = (mouthSmooth - mouthResting) * 10.0;   // mouth: difference from room level, times 10 = tenths of a degree

  // ---------- 7. SEND TO THE SERIAL PLOTTER ----------
  // The plotter wants:  name:value,name:value  and then a new line.
  if (SHOW_DEGREES == true) {                   // did you ask for real degrees?
    Serial.print("noseC:");                     // write the label "noseC:" (C for Celsius)
    Serial.print(noseCelsius);                  // write the nose temperature
    Serial.print(",");                          // write a comma to separate the two lines
    Serial.print("mouthC:");                    // write the label "mouthC:"
    Serial.print(mouthCelsius);                 // write the mouth temperature
    Serial.println();                           // finish the line (the plotter draws one new point)
  } else {                                      // otherwise (SHOW_DEGREES is false)
    Serial.print("noseChange:");                // write the label "noseChange:"
    Serial.print(noseChange);                   // write the nose change in tenths of a degree
    Serial.print(",");                          // write a comma to separate the two lines
    Serial.print("mouthChange:");               // write the label "mouthChange:"
    Serial.print(mouthChange);                  // write the mouth change in tenths of a degree
    Serial.println();                           // finish the line (the plotter draws one new point)
  }                                             // end of the if / else

  // ---------- 8. WAIT A MOMENT ----------
  delay(WAIT_MS);                               // wait 50 milliseconds, then loop starts again from the top
}                                               // end of loop
