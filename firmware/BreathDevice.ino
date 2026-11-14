  /*
  BREATHING DEVICE

  CALIBRATION -> OPENING -> LISTENING -> EXERCISE -> CLOSING -> CALIBRATION (ready for the next person)

  CALIBRATION: + / - move the motor, n saves it as closed (0) and goes to the next one. Order: motor 1 -> motors 2+3 -> motor 0. After the last n the opening starts. After a closing the motors are already at 0, so n three times is enough. The letters come from Processing (the only program on the port), typed in its window.

  OPENING    motors 2+3 go down as fast as they can. Motor 1 starts at the same moment and goes half the distance at half the speed, so they ARRIVE TOGETHER. Then motor 0 locks air in the top head (9 deg). The LED ring spins white.

  LISTENING  15 s. The device copies the person, breathing in  -> motors move BREATH_SPEED steps every second (2 s in = 2000 steps, 8 s in = 8000) CHEST breathing -> motors 2+3 AND motor 1 move.  BELLY -> only motors 2+3. LEDs: top half = nose (blue), bottom half = mouth (purple). They get brighter while breathing in, darker while breathing out. The sensor data goes to Processing / Wekinator 20 times a second. Wekinator answers with ONE number 1, 2 or 3. We take it once, then stop listening to it.

  EXERCISE   Each case = just the goal pattern (the data). The transition (in loop, EXERCISE) goes from the person's own rhythm to the goal, over TRANSITION_BREATHS breaths, then holds the goal for the case's hold breaths.

  CLOSING    the opening backwards: motor 0 unlocks, then everything goes back up to 0. Then CALIBRATION restarts.

 PROCESSING -> ARDUINO  (115200 baud, same letters as before)
    CALIBRATION  = + / -   move the motor being calibrated down / up (CAL_STEP steps) only, n save this motor as closed (0), next motor
    CLOSING = c close now, from wherever the motors are during a session
    LISTENING = 1 2 3   Wekinator's number only once after LISTENING, then ignored.

 ARDUINO -> PROCESSING
    20 lines a second during LISTENING and EXERCISE, the 7 numbers for the Wekinator classifier: bpm:18.20,irregular:0.080,depth:9.50,ratio:0.850,chest:3.10,belly:4.20,bellyShare:0.580
*/

#include <AccelStepper.h>                                             // The AccelStepper library lets the motors move smoothly while the rest of the code keeps running.
#include <Adafruit_NeoPixel.h>                                        // The Adafruit NeoPixel library controls the LED ring.

// PINS

AccelStepper motor23(AccelStepper::HALF4WIRE, A5, A3, A4, A2);        // Motors 2+3 share pins A2-A5, so they always move together. The pins are listed backwards so that a positive number means "down" for every motor.
AccelStepper motor1 (AccelStepper::HALF4WIRE,  6,  8,  7,  9);        // Motor 1 is connected to pins D6-D9.
AccelStepper motor0 (AccelStepper::HALF4WIRE, 10, 12, 11, 13);        // Motor 0, the one that locks the air, is connected to pins D10-D13.

const int NOSE_PIN   = A0;                                            // The nose thermistor is read on pin A0.
const int MOUTH_PIN  = A7;                                            // The mouth thermistor is read on pin A7.
const int CHEST_TRIG = 2, CHEST_ECHO = 3;                             // The chest ultrasonic sensor sends its sound on D2 and listens for the echo on D3.
const int BELLY_TRIG = 4, BELLY_ECHO = 5;                             // The belly ultrasonic sensor sends its sound on D4 and listens for the echo on D5.

const int LED_PIN   = A1;                                             // The LED ring's data wire is connected to pin A1.
const int LED_COUNT = 45;                                             // The ring has 45 LEDs.
Adafruit_NeoPixel ring(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);     // This creates the LED ring object the code talks to.

// SETTINGS

// ----- motors -----
const float MAX_SPEED    = 1000;                                      // The fastest the motors ever turn, in steps per second (the limit of the 28BYJ-48).
const float OPEN_ACCEL   = 2000;                                      // How quickly the motors speed up and slow down while opening and closing.
const float FOLLOW_ACCEL = 5000;                                      // A quicker speed-up used while following a breath, so the motors can change direction fast.
const float MOTOR1_SHARE = 0.5;                                       // Motor 1 always moves half as far, half as fast and with half the acceleration of motors 2+3.

const long OPEN_POS_23 = 18432;                                       // Where motors 2+3 stop when the device is open: 18432 steps, which is 4.5 turns.
const long OPEN_POS_1  = (OPEN_POS_23 * MOTOR1_SHARE);                // Motor 1's open position is half of that, so both arrive at the same time.
const long LOCK_POS_0  = 102;                                         // Motor 0 turns 102 steps (9 degrees) to lock the air in.
const long CAL_STEP    = 250;                                         // During calibration, each + or - moves the motor by 250 steps.

// ----- breathing -> motors (direct) -----
const float BREATH_SPEED     = 1000;                                  // While breathing in or out, the motors move 1000 steps every second (2 s of breath = 2000 steps).
const int   BREATH_DIRECTION = -1;                                    // -1 means breathing in moves the motors back towards closed. Changing it to +1 flips the direction.
const long  MAX_BREATH_STEPS = 12000;                                 // The motors are never allowed to move more than 12000 steps away from the open position.

// ----- phases -----
const float LISTEN_SECONDS     = 15;                                  // Listening lasts 15 seconds.
const float CLASS_WAIT_SECONDS = 5;                                   // The exercise waits at most 5 seconds for Wekinator's number before it gives up and closes.
const int   TRANSITION_BREATHS = 6;                                   // The exercise takes 6 breaths to go from the person's rhythm to the goal.

// ----- reading the breath -----
const float SENSOR_SMOOTHING = 0.3;                                   // How strongly the thermistor reading is smoothed: a smaller number is calmer but slower.
const unsigned long CHECK_EVERY_MS = 300;                             // Every 300 ms the temperature is compared with the one from 300 ms before.
const float MIN_CHANGE       = 0.03;                                  // If the temperature changed less than 0.03 degrees in that time, the person is holding their breath.
const float LED_STEP         = 0.0125;                                // How much brighter or darker an LED half gets every 50 ms (80 steps = from off to full in 4 s).
const int   THERMISTOR_READINGS = 64;                                 // Each thermistor is read 64 times in a row and averaged (the same as the firmware Wekinator was trained on).

// ----- the 7 numbers for Wekinator (calculated like the firmware the model was trained on) -----
const int   SPIKE_LIST        = 5;                                    // The last 5 temperatures of each thermistor are kept; the middle one is used, which throws away sudden spikes.
const float DATA_SMOOTHING    = 0.1;                                  // The slower smoothing used for Wekinator's numbers (about half a second).
const float ROOM_SPEED        = 0.005;                                // How slowly the "room temperature" follows (about 10 s), so only the breath is left.
const float MIN_BREATH_SIZE   = 2.0;                                  // The smallest up-and-down of nose + mouth that counts as a breath (2 = 0.2 degrees).
const float BREATH_SIZE_SHARE = 0.3;                                  // A turn only counts if the signal comes back 30% of a normal breath.
const unsigned long SHORTEST_BREATH_MS = 1000;                        // "Breaths" shorter than 1 s are ignored.
const unsigned long LONGEST_BREATH_MS  = 30000;                       // "Breaths" longer than 30 s are ignored (that is a pause).

// ----- ultrasonics (chest / belly) -----
const float US_SMOOTHING     = 0.05;                                  // How strongly the ultrasonic distance is smoothed (the same as the trained firmware).
const float MAX_JUMP_CM      = 2.0;                                   // A breath never moves the body more than 2 cm in one reading, so a bigger jump is treated as an error...
const int   JUMPS_BEFORE_ACCEPT = 6;                                  // ...unless it stays 6 readings in a row: then the person really moved.
const float REST_SPEED       = 0.003;                                 // How slowly the "resting position" of the body follows the distance (very slowly, so breaths are ignored).
const float LEVEL_SPEED      = 0.01;                                  // How quickly the "how much it is moving" number follows the movement (about 5 s).
const float CHEST_MIN_MM     = 1.0;                                   // The chest has to move at least 1 mm to count as chest breathing.

// ----- LED ring -----
const int LED_BRIGHTNESS = 60;                                        // The ring's overall brightness, from 0 to 255. It is kept low because 45 LEDs at full white draw about 2.7 A.
const int TOP_LED        = 0;                                         // The number of the LED that sits at the very top of the ring.

// ----- Processing -----
const long BAUD_RATE = 115200;                                        // The speed of the USB connection to Processing.
const unsigned long SEND_EVERY_MS = 50;                               // The sensors are read and one line is sent to Processing every 50 ms (20 times a second).

// VARIABLES

enum Phase { CALIBRATION, OPENING, LISTENING, EXERCISE, CLOSING };    // The five phases of a session, given names so the code reads like words.
Phase phase = CALIBRATION;                                            // The phase the device is in right now. It starts in calibration.
bool  phaseStarted = false;                                           // false means the current phase has not set itself up yet; its "first pass" block is still to come.
unsigned long phaseStart = 0;                                         // The time (in ms since power-on) when the current phase started.
bool  secondMove = false;                                             // Opening and closing are two moves each: false during the 1st move, true during the 2nd.

// Calibration
int  calMotor = 1;                                                    // Which motor is being calibrated: 1, 2 (meaning motors 2+3) or 0.
long calTotal = 0;                                                    // How many steps this motor has been moved during calibration.

// One thermistor: everything we know about it
struct Thermistor {                                                   // A "box" that holds everything the code needs to know about one thermistor.
  int pin;                                                            // The pin the thermistor is connected to.
  bool started;                                                       // false until the first reading, so the smoothing can start from the real temperature.
  float smooth;                                                       // The averaged and smoothed temperature, in degrees.
  float before;                                                       // The smoothed temperature at the last check, 0.3 s ago.
  unsigned long lastCheck;                                            // The time of the last check.
  float change;                                                       // How many degrees the temperature changed since the last check. Below 0 means it is getting cooler.
  int direction;                                                      // +1 = breathing in, -1 = breathing out, 0 = holding.
  float light;                                                        // How bright this sensor's half of the LED ring is, from 0 (off) to 1 (full).
  bool dataStarted;                                                   // false until the very first reading of the Wekinator part (it is never restarted, like in the trained firmware).
  float spikes[SPIKE_LIST];                                           // The last 5 temperatures, for the spike remover.
  int spikeSlot;                                                      // Where the next temperature goes in that list.
  float dataSmooth;                                                   // The slowly smoothed temperature for Wekinator.
  float room;                                                         // The very slow "room temperature".
  float clean;                                                        // The breath that is left: (dataSmooth - room) in tenths of a degree. Up = warmer = breathing out.
};                                                                    // End of the thermistor box.
Thermistor nose  = { NOSE_PIN };                                      // The nose thermistor's box. Only the pin is filled in; everything else starts at 0.
Thermistor mouth = { MOUTH_PIN };                                     // The mouth thermistor's box.

// One ultrasonic sensor
const int PINGS = 7;                                                  // Each ultrasonic sensor remembers its last 7 measurements (the same as the trained firmware).
struct Ultrasonic {                                                   // A "box" that holds everything the code needs to know about one ultrasonic sensor.
  int trig, echo;                                                     // The pin that sends the sound and the pin that hears the echo.
  float pings[PINGS];                                                 // The last 7 measured distances in cm (-1 means a bad measurement).
  int slot;                                                           // Where in that list the next measurement goes.
  float smooth;                                                       // The smoothed distance in cm (-1 means there is no good value yet).
  float rest;                                                         // The slow "resting" distance of the body, in cm.
  float level;                                                        // How many mm the body has been moving lately. This is the number the rest of the code uses.
  int jumps;                                                          // How many sudden jumps in a row were ignored.
};                                                                    // End of the ultrasonic box.
Ultrasonic chest = { CHEST_TRIG, CHEST_ECHO, {-1,-1,-1,-1,-1,-1,-1}, 0, -1, -1, 0, 0 };   // The chest sensor's box. The values fill the items in the same order as in the box above.
Ultrasonic belly = { BELLY_TRIG, BELLY_ECHO, {-1,-1,-1,-1,-1,-1,-1}, 0, -1, -1, 0, 0 };   // The belly sensor's box, filled the same way.
bool pingChestNext = true;                                            // The two sensors take turns: true means the chest measures next, false means the belly.

// Chest or belly (from the ultrasonics)
float bellyShare = 0.5;                                               // The belly's part of all the movement: 0 = only the chest moves, 1 = only the belly moves. Sent to Wekinator.
bool  usingChest = false;                                             // true when the person breathes with the chest; then motor 1 moves too.

// The breath now
int   breathDirection = 0;                                            // The breath right now: +1 in, -1 out, 0 holding (from whichever thermistor changes more).
float breathSteps = 0;                                                // How many steps the motors are away from the open position right now.

// Measuring the breaths (for Wekinator: the same breath finder as the trained firmware)
bool  lookingForPeak = true;                                          // true = waiting for a TOP of nose + mouth (end of breathing out), false = waiting for a BOTTOM (end of breathing in).
float highestSoFar = -999999;                                         // The highest value since the last bottom...
unsigned long highestTime = 0;                                        // ...and when it happened.
float lowestSoFar = 999999;                                           // The lowest value since the last top...
unsigned long lowestTime = 0;                                         // ...and when it happened.
unsigned long lastPeakTime = 0;                                       // When the last top happened (0 = none yet).
unsigned long lastTroughTime = 0;                                     // When the last bottom happened (0 = none yet).
float lastTroughValue = 0;                                            // How low the last bottom was.
float lastInhaleMs = 0;                                               // How long the latest inhale lasted, in ms.
float typicalBreathSize = 0;                                          // The size of a normal breath of this person (sent as "depth").
int   breathsFound = 0;                                               // How many whole breaths have been measured for this person.
float avgPeriodMs = 0;                                                // The person's average breath length, in ms.
float avgRatio = 1.0;                                                 // The person's average inhale time divided by exhale time (sent as "ratio").
float irregularity = 0;                                               // How uneven the breath lengths are: 0 = all the same.
// Wekinator / exercise
int   wekinatorClass = 0;                                             // The last number (1, 2 or 3) received from Wekinator. 0 means none yet.
int   exerciseClass  = 0;                                             // The number the exercise locked in and is using. 0 means no case chosen yet.
float goalIn, goalOut;                                                // The goal pattern from the chosen case: seconds breathing in and out.
bool  goalChest, goalInNose, goalOutNose;                             // From the chosen case: chest or belly, and nose or mouth for breathing in and out.
int   holdBreaths;                                                    // From the chosen case: how many breaths to stay at the goal before closing.
float inStep, outStep;                                                // How many seconds the inhale and exhale change after each breath of the transition.
float guideIn = 3, guideOut = 3;                                      // The pattern's current inhale and exhale, in seconds.
bool  guideInhaling = false;                                          // true while the pattern is breathing in, false while it is breathing out.
float guideTime = 0;                                                  // How many seconds the pattern's current inhale or exhale has lasted.
float exhaleSpeed = 0;                                                // The speed (steps per second) that brings the motors back to open in exactly guideOut seconds.
int   guideBreaths = 0;                                               // How many breaths the pattern has done.

// Clocks
unsigned long lastPing = 0, lastSend = 0, lastPlan = 0, lastRing = 0; // The last time an ultrasonic was read, a line was sent, the motors were planned, and the LED bar moved.
int ringPos = 0;                                                      // Where the white spinning bar on the LED ring is.


// HELPERS  (only the functions used in more than one moment)

// ----- phases -----

void setPhase(Phase p) {                                              // This function moves the device to another phase. It is used by every phase change.
  phase = p;                                                          // The current phase becomes the one that was passed in.
  phaseStarted = false;                                               // The new phase has not set itself up yet; it will do that on its next pass.
}                                                                     // End of setPhase.

// ----- motors -----

void setMotorSpeeds(float accel) {                                    // This function gives all motors the same top speed and the acceleration passed in.
  motor23.setMaxSpeed(MAX_SPEED);                 motor23.setAcceleration(accel);                  // Motors 2+3 get the full top speed and the full acceleration.
  motor1.setMaxSpeed(MAX_SPEED * MOTOR1_SHARE);   motor1.setAcceleration(accel * MOTOR1_SHARE);    // Motor 1 gets half of both, so it takes the same time to cover half the distance.
  motor0.setMaxSpeed(MAX_SPEED);                  motor0.setAcceleration(accel);                   // Motor 0 gets the full top speed and the full acceleration.
}                                                                     // End of setMotorSpeeds.

void runMotors() {                                                    // This function lets each motor take one step if it is time. Without it, nothing moves.
  motor23.run();                                                      // Motors 2+3 take a step if they need one.
  motor1.run();                                                       // Motor 1 takes a step if it needs one.
  motor0.run();                                                       // Motor 0 takes a step if it needs one.
}                                                                     // End of runMotors.

bool motorsArrived() {                                                // This function answers: have all three motors reached their targets? Used in opening and closing.
  return motor23.distanceToGo() == 0 && motor1.distanceToGo() == 0 && motor0.distanceToGo() == 0;   // true only when none of the three has any steps left to go.
}                                                                     // End of motorsArrived.

void moveBreath(int direction, float inSpeed, float outSpeed, bool chestToo, float seconds) {   // This function moves the motors along with a breath: direction +1 in, -1 out, 0 hold. chestToo decides if motor 1 joins in.
  if (direction > 0) breathSteps += inSpeed * seconds;                // Breathing in: the motors move further from open by speed x time.
  if (direction < 0) breathSteps -= outSpeed * seconds;               // Breathing out: the motors move back towards open by speed x time.
  breathSteps = constrain(breathSteps, 0, MAX_BREATH_STEPS);          // The value is kept between the open position (0) and the safety limit.

  long steps = BREATH_DIRECTION * (long)breathSteps;                  // The number of steps, turned the right way round by BREATH_DIRECTION.
  motor23.moveTo(OPEN_POS_23 + steps);                                // Motors 2+3 are sent to the open position plus those steps.
  motor1.moveTo(OPEN_POS_1 + (chestToo ? (long)(steps * MOTOR1_SHARE) : 0));   // Motor 1 moves half as far, but only for chest breathing; otherwise it stays at its open position.
}                                                                     // End of moveBreath.

// ----- LED ring -----

void setRing(float top, float bottom) {                               // This function lights the top half blue (nose) and the bottom half purple (mouth), each from 0 to 1.
  for (int i = 0; i < LED_COUNT; i++) {                               // It goes through all 45 LEDs, one by one.
    int d = (i - TOP_LED + LED_COUNT) % LED_COUNT;                    // How far round the ring this LED is from the top LED.
    bool topHalf = d <= LED_COUNT / 4 || d >= LED_COUNT - LED_COUNT / 4;   // The LED is in the top half if it is within 11 LEDs of the top, on either side.
    if (topHalf) ring.setPixelColor(i, ring.Color(0, 0, top * 255));  // A top LED gets blue, as bright as the nose value.
    else         ring.setPixelColor(i, ring.Color(bottom * 150, 0, bottom * 255));   // A bottom LED gets purple, as bright as the mouth value.
  }                                                                   // End of going through the LEDs.
  ring.show();                                                        // The new colours are sent to the ring all at once.
}                                                                     // End of setRing.

void spinRing() {                                                     // This function makes a white bar spin round the ring during opening and closing.
  if (millis() - lastRing < 40) return;                               // The bar only moves every 40 ms; if it is too early, nothing happens.
  lastRing = millis();                                                // The time of this move is remembered.
  ring.clear();                                                       // All LEDs are switched off first.
  for (int i = 0; i < 5; i++) ring.setPixelColor((ringPos + i) % LED_COUNT, ring.Color(255, 255, 255));   // Five LEDs in a row, starting at ringPos, are lit white.
  ring.show();                                                        // The ring shows the new picture.
  ringPos = (ringPos + 1) % LED_COUNT;                                // The bar starts one LED further next time, going back to 0 after the last LED.
}                                                                     // End of spinRing.

// ----- sensors (one function, used for two sensors each) -----

void readThermistor(Thermistor &t) {                                  // This function reads one thermistor (nose or mouth) and decides if the person is breathing in, out or holding.
  analogRead(t.pin);                                                  // The first reading after switching pins can be wrong, so it is read once and thrown away.
  delayMicroseconds(200);                                             // A short pause lets the reading settle.
  float total = 0;                                                    // The sum of the readings starts at 0.
  for (int i = 0; i < THERMISTOR_READINGS; i++) {                     // The pin is read 64 times in a row.
    total += analogRead(t.pin);                                       // Each reading (0 to 1023) is added to the sum.
    runMotors();                                                      // The motors keep stepping in between, so they do not stutter.
  }                                                                   // End of the 16 readings.
  float reading = constrain(total / THERMISTOR_READINGS, 1, 1022);    // The average of the readings, kept away from 0 and 1023 so the maths below never breaks.
  float ohms = 100000.0 * (1023.0 - reading) / reading;               // The thermistor's resistance, worked out from the reading and the 100k resistor next to it.
  float kelvin = 1.0 / (log(ohms / 100000.0) / 3950.0 + 1.0 / 298.15);   // The "beta formula" for a 100K 3950 thermistor turns the resistance into a temperature in Kelvin.
  float temperature = kelvin - 273.15;                                // Kelvin minus 273.15 gives degrees Celsius.

  // ----- for Wekinator: the same cleaning as the trained firmware -----
  if (t.dataStarted == false) {                                       // On the very first reading ever...
    for (int i = 0; i < SPIKE_LIST; i++) t.spikes[i] = temperature;   // ...the spike list is filled with this temperature,
    t.dataSmooth = temperature;                                       // the slow smoothing starts here,
    t.room = temperature;                                             // and so does the room temperature.
    t.dataStarted = true;                                             // This only happens once (it is not restarted for each person).
  }                                                                   // End of the very first reading.
  t.spikes[t.spikeSlot] = temperature;                                // The new temperature goes into the list of the last 5...
  t.spikeSlot = (t.spikeSlot + 1) % SPIKE_LIST;                       // ...and the next one will replace the oldest.
  float sorted[SPIKE_LIST];                                           // A copy of the list, to sort.
  for (int i = 0; i < SPIKE_LIST; i++) sorted[i] = t.spikes[i];       // Each temperature is copied.
  for (int a = 0; a < SPIKE_LIST; a++) {                              // The copy is sorted from small to big...
    for (int b = 0; b < SPIKE_LIST - 1; b++) {                        // ...by comparing neighbours...
      if (sorted[b] > sorted[b + 1]) {                                // ...and if the left one is bigger...
        float temporary = sorted[b];                                  // ...it is remembered,
        sorted[b] = sorted[b + 1];                                    // the right one moves left,
        sorted[b + 1] = temporary;                                    // and the remembered one goes right.
      }                                                               // End of the swap.
    }                                                                 // End of one walk.
  }                                                                   // End of sorting.
  float middle = sorted[SPIKE_LIST / 2];                              // The middle one: a single spike can never be the middle.
  t.dataSmooth = t.dataSmooth + DATA_SMOOTHING * (middle - t.dataSmooth);   // Slow smoothing: 10% towards the new value each time.
  t.room = t.room + ROOM_SPEED * (middle - t.room);                   // The room temperature follows very slowly.
  t.clean = (t.dataSmooth - t.room) * 10.0;                           // What is left is the breath, in tenths of a degree. Up = warmer = breathing out.

  // ----- for the motors and LEDs: the simple direction -----

  if (t.started == false) {                                           // On the very first reading there is nothing to compare with yet...
    t.smooth = temperature;                                           // ...so the smoothed value starts at the real temperature,
    t.before = temperature;                                           // the "0.3 s ago" value starts there too,
    t.lastCheck = millis();                                           // the 0.3 s clock starts now,
    t.started = true;                                                 // and from now on it is no longer the first reading.
  }                                                                   // End of the first reading.

  t.smooth = t.smooth + SENSOR_SMOOTHING * (temperature - t.smooth);  // 1. The smoothed value moves 30% of the way towards the new temperature, which removes jitter.

  if (millis() - t.lastCheck >= CHECK_EVERY_MS) {                     // 2. Every 0.3 s the temperature is compared with the one from 0.3 s before.
    t.change = t.smooth - t.before;                                   // How much it changed: below 0 is cooler, above 0 is warmer.
    t.before = t.smooth;                                              // The value now is saved for the next comparison.
    t.lastCheck = millis();                                           // The time of this check is saved.

    if (t.change < -MIN_CHANGE) {                                     // Clearly cooler air on the sensor...
      t.direction = 1;                                                // ...means the person is breathing IN.
    } else if (t.change > MIN_CHANGE) {                               // Clearly warmer air on the sensor...
      t.direction = -1;                                               // ...means the person is breathing OUT.
    } else {                                                          // Hardly any change...
      t.direction = 0;                                                // ...means the person is HOLDING their breath.
    }                                                                 // End of choosing the direction.
  }                                                                   // End of the 0.3 s check.

  if (t.direction == 1)  t.light = t.light + LED_STEP;                // 3. While breathing in, this sensor's LED half gets a little brighter.
  if (t.direction == -1) t.light = t.light - LED_STEP;                // While breathing out, it gets a little darker. While holding, it stays the same.
  t.light = constrain(t.light, 0.0, 1.0);                             // The brightness is kept between off (0) and full (1).
}                                                                     // End of readThermistor.

void readUltrasonic(Ultrasonic &u) {                                  // This function measures one ultrasonic sensor (chest or belly) and updates how much that part of the body is moving.
  digitalWrite(u.trig, LOW);  delayMicroseconds(2);                   // 1. The trigger pin is set low for a moment so the next signal starts clean.
  digitalWrite(u.trig, HIGH); delayMicroseconds(10);                  // The trigger pin goes high for 10 microseconds: this tells the sensor to send a sound.
  digitalWrite(u.trig, LOW);                                          // The trigger pin goes low again; the sound is on its way.
  float ping = pulseIn(u.echo, HIGH, 6000) * 0.0343 / 2.0;            // The echo time is measured (max 6 ms) and turned into cm: sound travels 0.0343 cm per microsecond, there and back.
  if (ping < 2 || ping > 60) ping = -1;                               // Closer than 2 cm, further than 60 cm, or no echo at all: the measurement is marked as bad (-1).

  u.pings[u.slot] = ping;                                             // The new measurement is stored in the list of the last 7.
  u.slot = u.slot + 1;                                                // The next measurement will go in the next place.
  if (u.slot >= PINGS) u.slot = 0;                                    // After the last place it starts again at the first, replacing the oldest one.

  float good[PINGS];                                                  // 2. A new, empty list for the good measurements only.
  int n = 0;                                                          // How many good measurements have been found so far.
  for (int i = 0; i < PINGS; i++) {                                   // Each of the 7 stored measurements is looked at.
    if (u.pings[i] > 0) {                                             // If it is a good one (not -1)...
      good[n] = u.pings[i];                                           // ...it is copied into the good list...
      n = n + 1;                                                      // ...and counted.
    }                                                                 // End of the check.
  }                                                                   // End of looking at the measurements.
  if (n == 0) return;                                                 // If none were good, the function stops here and everything keeps its old value.
  for (int a = 0; a < n; a++) {                                       // The good measurements are sorted from small to big. This walk is repeated n times, which is always enough.
    for (int b = 0; b < n - 1; b++) {                                 // Each walk compares every pair of neighbours.
      if (good[b] > good[b + 1]) {                                    // If the left one is bigger than the right one...
        float temporary = good[b];                                    // ...the left one is remembered,
        good[b] = good[b + 1];                                        // the right one moves to the left,
        good[b + 1] = temporary;                                      // and the remembered one goes on the right.
      }                                                               // End of the swap.
    }                                                                 // End of one walk.
  }                                                                   // End of sorting.
  float cm = good[n / 2];                                             // The middle one is taken. A wrong measurement always ends up at one end, so it is ignored.

  if (u.smooth < 0) {                                                 // On the very first good distance...
    u.smooth = cm;                                                    // ...the smoothed distance starts there...
    u.rest = cm;                                                      // ...and so does the resting distance.
  }                                                                   // End of the first good distance.

  if (fabs(cm - u.smooth) > MAX_JUMP_CM) {                            // A sudden jump bigger than a breath can make...
    u.jumps = u.jumps + 1;                                            // ...is counted...
    if (u.jumps < JUMPS_BEFORE_ACCEPT) return;                        // ...and ignored as an error (a lost echo, a hand in the way)...
    u.smooth = cm;                                                    // ...unless it stayed 6 times: then the person really moved, so smooth...
    u.rest = cm;                                                      // ...and rest jump to the new place.
  }                                                                   // End of the jump check.
  u.jumps = 0;                                                        // A normal reading: the jump counter starts again.

  u.smooth = u.smooth + US_SMOOTHING * (cm - u.smooth);               // 3. The smoothed distance moves 5% towards the new one, which removes jitter but still follows each breath.
  u.rest = u.rest + REST_SPEED * (cm - u.rest);                       // 4. The resting distance follows very slowly (15-20 s), so it stays in the middle of the breaths.
  float moving = fabs(u.rest - u.smooth) * 10.0;                      // 5. How far the body is from rest right now, in mm. fabs removes the minus sign, so closer and further both count.
  u.level = u.level + LEVEL_SPEED * (moving - u.level);               // 6. The average amount of movement over the last few seconds, in mm.
}                                                                     // End of readUltrasonic.

// ----- breath + Wekinator (used in LISTENING and EXERCISE) -----

void onPeakFound(float peakValue, unsigned long peakTime) {           // This function runs when a TOP was found (breathing out ended). If a whole breath is complete, it updates the averages.
  if (lastPeakTime > 0 && lastTroughTime > lastPeakTime) {            // A whole breath = top, bottom, top: there was a top before, and a bottom after it.
    float periodMs = peakTime - lastPeakTime;                         // The whole breath lasted from the last top to this one.
    float exhaleMs = peakTime - lastTroughTime;                       // Breathing out lasted from the bottom to this top.
    float depth = peakValue - lastTroughValue;                        // The size of this breath: top minus bottom.
    if (periodMs >= SHORTEST_BREATH_MS && periodMs <= LONGEST_BREATH_MS) {   // Only breaths between 1 and 30 s count.
      float thisRatio = 1.0;                                          // Inhale time divided by exhale time (1 = equal, until both are known)...
      if (exhaleMs > 0 && lastInhaleMs > 0) thisRatio = lastInhaleMs / exhaleMs;   // ...worked out when both are known.
      if (breathsFound == 0) {                                        // The very first breath simply becomes the average:
        avgPeriodMs = periodMs;  irregularity = 0;  avgRatio = thisRatio;  typicalBreathSize = depth;   // length, unevenness, ratio and size.
      } else {                                                        // Every later breath moves the averages towards it:
        irregularity += 0.3 * (fabs(periodMs - avgPeriodMs) / avgPeriodMs - irregularity);   // how different this breath is from the average (30%),
        avgPeriodMs  += 0.3 * (periodMs - avgPeriodMs);               // the average breath length (30%),
        avgRatio     += 0.3 * (thisRatio - avgRatio);                 // the average inhale / exhale (30%),
        typicalBreathSize += 0.2 * (depth - typicalBreathSize);       // and the normal breath size (20%).
      }                                                               // End of updating the averages.
      breathsFound++;                                                 // One more whole breath was measured.
    }                                                                 // End of the length check.
  }                                                                   // End of the whole-breath check.
  lastPeakTime = peakTime;                                            // This top is remembered for the next breath.
}                                                                     // End of onPeakFound.

bool readBreathAndSend() {                                            // This function does all the sensor work and sends the 7 numbers to Processing. It returns true right after each 50 ms reading.
  unsigned long now = millis();                                       // The current time, in ms since power-on.

  if (now - lastPing >= 25) {                                         // Every 25 ms one ultrasonic sensor is measured.
    lastPing = now;                                                   // The time of this measurement is remembered.
    if (pingChestNext) readUltrasonic(chest); else readUltrasonic(belly);   // The chest and the belly take turns, so they never hear each other's echo.
    pingChestNext = !pingChestNext;                                   // Next time it is the other sensor's turn.

    float total = chest.level + belly.level;                          // All the movement of chest and belly together.
    if (total > 0.01) bellyShare = belly.level / total;               // If anything is moving, the belly's part of it is worked out (for Wekinator).
    else              bellyShare = 0.5;                               // If nothing is moving, it is called half and half.
    usingChest = bellyShare < 0.5 && chest.level > CHEST_MIN_MM;      // It counts as chest breathing if the chest moves more than the belly and more than 1 mm.
  }                                                                   // End of the 25 ms work.

  if (now - lastSend < SEND_EVERY_MS) return false;                   // If 50 ms have not passed yet, the function stops here and answers false.
  lastSend = now;                                                     // Otherwise the time of this reading is remembered, and the 50 ms work starts.

  // 1. read nose and mouth, use the one you breathe through
  readThermistor(nose);                                               // The nose thermistor is read.
  readThermistor(mouth);                                              // The mouth thermistor is read.
  Thermistor &used = fabs(nose.change) >= fabs(mouth.change) ? nose : mouth;   // The one whose temperature changes more is the one the person is breathing through.
  breathDirection = used.direction;                                   // Its direction becomes the breath direction: in, out or holding.

  // 2. find the breaths for Wekinator: tops and bottoms of nose + mouth together (the same as the trained firmware)
  float value = nose.clean + mouth.clean;                             // The breath signal: up = breathing out (warm), down = breathing in (cool).
  float turnSize = BREATH_SIZE_SHARE * typicalBreathSize;             // How far the signal must come back to count as a turn: 30% of a normal breath...
  if (turnSize < MIN_BREATH_SIZE) turnSize = MIN_BREATH_SIZE;         // ...but never less than the minimum, so tiny noise is never a breath.

  if (lookingForPeak) {                                               // Breathing out: the TOP is searched.
    if (value > highestSoFar) {                                       // A new highest point...
      highestSoFar = value;                                           // ...is remembered...
      highestTime = now;                                              // ...with its time.
    }                                                                 // End of the new highest point.
    if (value < highestSoFar - turnSize) {                            // The signal clearly came down again: the top is confirmed (breathing out ended there).
      onPeakFound(highestSoFar, highestTime);                         // One whole breath may be complete: the averages are updated.
      lookingForPeak = false;                                         // Now the BOTTOM is searched...
      lowestSoFar = value;                                            // ...starting from this value...
      lowestTime = now;                                               // ...and this time.
    }                                                                 // End of the top.
  } else {                                                            // Breathing in: the BOTTOM is searched.
    if (value < lowestSoFar) {                                        // A new lowest point...
      lowestSoFar = value;                                            // ...is remembered...
      lowestTime = now;                                               // ...with its time.
    }                                                                 // End of the new lowest point.
    if (value > lowestSoFar + turnSize) {                             // The signal clearly went up again: the bottom is confirmed (breathing in ended there).
      if (lastPeakTime > 0) lastInhaleMs = lowestTime - lastPeakTime;   // The inhale lasted from the last top to this bottom.
      lastTroughTime = lowestTime;                                    // When this bottom happened...
      lastTroughValue = lowestSoFar;                                  // ...and how low it was.
      lookingForPeak = true;                                          // Now the TOP is searched...
      highestSoFar = value;                                           // ...starting from this value...
      highestTime = now;                                              // ...and this time.
    }                                                                 // End of the bottom.
  }                                                                   // End of finding the breaths.

  // 3. send the 7 numbers to Processing / Wekinator
  float bpm = 0;                                                      // Breaths per minute start at 0 (no breaths measured yet).
  if (breathsFound > 0 && avgPeriodMs > 0) bpm = 60000.0 / max(avgPeriodMs, (float)(now - lastPeakTime));   // 60000 ms divided by the breath length. A long pause counts as a long breath, so bpm drops at once.
  Serial.print("bpm:");          Serial.print(bpm, 2);                // Number 1: breaths per minute.
  Serial.print(",irregular:");   Serial.print(irregularity, 3);       // Number 2: how uneven the breaths are.
  Serial.print(",depth:");       Serial.print(typicalBreathSize, 2);        // Number 3: how deep the breaths are.
  Serial.print(",ratio:");       Serial.print(avgRatio, 3);           // Number 4: inhale divided by exhale.
  Serial.print(",chest:");       Serial.print(chest.level, 2);        // Number 5: how much the chest moves (mm).
  Serial.print(",belly:");       Serial.print(belly.level, 2);        // Number 6: how much the belly moves (mm).
  Serial.print(",bellyShare:");  Serial.print(bellyShare, 3);         // Number 7: the belly's part of the movement.
  Serial.println();                                                   // The line is finished, so Processing knows a full set has arrived.
  return true;                                                        // The function answers true: a new reading was just made.
}                                                                     // End of readBreathAndSend.

// ----- calibration (used for + - and for n) -----

AccelStepper& calibratedMotor() {                                     // This function gives back the motor that is being calibrated now.
  if (calMotor == 2) return motor23;                                  // 2 means motors 2+3.
  if (calMotor == 1) return motor1;                                   // 1 means motor 1.
  return motor0;                                                      // Anything else (0) means motor 0.
}                                                                     // End of calibratedMotor.

void printCalMotor() {                                                // This function sends the name of the motor being calibrated to Processing.
  Serial.print(calMotor == 2 ? ">> MOTORS 2+3" : (calMotor == 1 ? ">> MOTOR 1" : ">> MOTOR 0"));   // It picks the right name for 2, 1 or 0.
}                                                                     // End of printCalMotor.


// SETUP + LOOP

void setup() {                                                        // setup() runs once, when the Arduino is switched on.
  Serial.begin(BAUD_RATE);                                            // The USB connection to Processing is started.
  pinMode(CHEST_TRIG, OUTPUT);  pinMode(CHEST_ECHO, INPUT);           // The chest sensor's pins: one sends, one listens.
  pinMode(BELLY_TRIG, OUTPUT);  pinMode(BELLY_ECHO, INPUT);           // The belly sensor's pins: one sends, one listens.
  ring.begin();                                                       // The LED ring is started.
  ring.setBrightness(LED_BRIGHTNESS);                                 // Its overall brightness is limited.
  setPhase(CALIBRATION);                                              // The device goes to calibration, which sets itself up on the first pass of loop().
}                                                                     // End of setup.

void loop() {                                                         // loop() runs again and again, thousands of times a second.
  unsigned long now = millis();                                       // The current time, in ms since power-on.
  char letter = 0;                                                    // The letter Processing sent this time (0 means none).
  if (Serial.available() > 0) letter = Serial.read();                 // If Processing sent something, one letter is read.

  switch (phase) {                                                    // Only the case of the current phase runs.

    // ---------- CALIBRATION: + - n from Processing ----------
    case CALIBRATION:                                                 // CALIBRATION: the motors are set to their closed position by hand, through Processing.
      if (phaseStarted == false) {                                    // FIRST PASS: calibration sets itself up.
        phaseStarted = true;                                          // This block will not run again.
        phaseStart = now;                                             // The time calibration started.
        calMotor = 1;                                                 // Motor 1 is calibrated first.
        calTotal = 0;                                                 // It has not been moved yet.
        setMotorSpeeds(OPEN_ACCEL);                                   // The motors get the soft acceleration.
        ring.clear();                                                 // All LEDs are switched off...
        ring.show();                                                  // ...and the ring is updated.
        Serial.println(">> CALIBRATION >> MOTOR 1");                  // Processing is told which motor is being calibrated.
      }                                                               // End of the first pass.

      if (letter == '+' || letter == '-') {                           // A + or - from Processing moves the motor a little.
        long steps = (letter == '+') ? CAL_STEP : -CAL_STEP;          // + means 250 steps one way, - means 250 steps the other way.
        calibratedMotor().move(steps);                                // The motor being calibrated is told to move those steps.
        calibratedMotor().runToPosition();                            // The code waits here until it has moved (fine, nothing else happens now).
        calTotal = calTotal + steps;                                  // The total for this motor is updated.
        printCalMotor();                                              // Processing is told the motor's name...
        Serial.print(" adjusted ");                                   // ...the word "adjusted"...
        Serial.print(calTotal);                                       // ...the total number of steps...
        Serial.println(" steps");                                     // ...and "steps".
      }                                                               // End of + / -.
      if (letter == 'n') {                                            // An n from Processing means: this motor is now in its closed position.
        calibratedMotor().setCurrentPosition(0);                      // Where the motor is now is saved as 0 (closed).
        calTotal = 0;                                                 // The counter starts again for the next motor.
        if (calMotor == 0) {                                          // If this was motor 0, the last one...
          setPhase(OPENING);                                          // ...calibration is done and the device starts opening.
        } else {                                                      // Otherwise it moves on to the next motor.
          if (calMotor == 1) {                                        // After motor 1...
            calMotor = 2;                                             // ...come motors 2+3.
          } else {                                                    // After motors 2+3...
            calMotor = 0;                                             // ...comes motor 0.
          }                                                           // End of choosing the next motor.
          Serial.print(">> CALIBRATION ");                            // Processing is told calibration goes on...
          printCalMotor();                                            // ...with which motor...
          Serial.println();                                           // ...and the line is finished.
        }                                                             // End of next motor.
      }                                                               // End of n.
      break;                                                          // End of this pass of calibration.

    // ---------- OPENING: 1. motors 2+3 and 1 down together  2. motor 0 locks (9 degrees) ----------
    case OPENING:                                                     // OPENING: the device opens, then locks the air.
      if (phaseStarted == false) {                                    // FIRST PASS = MOVE 1: motors 2+3 and 1 are sent to the open position.
        phaseStarted = true;                                          // This block will not run again.
        phaseStart = now;                                             // The time the opening started.
        secondMove = false;                                           // The opening is in its 1st move.
        setMotorSpeeds(OPEN_ACCEL);                                   // The motors get the soft acceleration.
        motor23.moveTo(OPEN_POS_23);                                  // Motors 2+3 are sent to the open position...
        motor1.moveTo(OPEN_POS_1);                                    // ...and motor 1 to its own, half as far. Both start now and arrive together.
        Serial.println(">> OPENING");                                 // Processing is told the opening has started.
      }                                                               // End of the first pass.

      if (letter == 'c') { setPhase(CLOSING); break; }                // A c from Processing closes the device straight away.
      runMotors();  spinRing();                                       // The motors take their steps and the white bar spins round the ring.
      if (motorsArrived()) {                                          // When all motors have reached their targets...
        if (secondMove == false) {                                    // ...and it was the 1st move (the device is open)...
          motor0.moveTo(LOCK_POS_0);                                  // ...MOVE 2 starts: motor 0 turns 9 degrees to lock the air.
          secondMove = true;                                          // The opening is now in its 2nd move.
        } else {                                                      // If the 2nd move is done (the air is locked)...
          setPhase(LISTENING);                                        // ...the device starts listening.
        }                                                             // End of move 1 / move 2.
      }                                                               // End of motors arrived.
      break;                                                          // End of this pass of the opening.

    // ---------- LISTENING: 15 s copying the person ----------
    case LISTENING:                                                   // LISTENING: for 15 s the device copies the person's breathing.
      if (phaseStarted == false) {                                    // FIRST PASS: everything starts fresh for this person.
        phaseStarted = true;                                          // This block will not run again.
        phaseStart = now;                                             // The 15 s count from here.
        lastPlan = now;                                               // The 10 ms motor clock starts from now.
        setMotorSpeeds(FOLLOW_ACCEL);                                 // The motors get the quick acceleration for following a breath.
        nose.started = mouth.started = false;                         // Both thermistors restart their smoothing from the current temperature.
        nose.direction = mouth.direction = 0;                         // No old breath direction is left over.
        nose.light = mouth.light = 0;                                 // Both LED halves start dark.
        chest.level = belly.level = 0;                                // Chest and belly movement start at 0.
        breathDirection = 0;  breathSteps = 0;                        // The breath starts at "holding" and the motors at the open position.
        lastPeakTime = lastTroughTime = 0;  lastInhaleMs = 0;         // No breath of this person has been found yet...
        breathsFound = 0;  typicalBreathSize = 0;  avgPeriodMs = 0;  avgRatio = 1.0;  irregularity = 0;   // ...and the averages start empty, so only this person counts.
        wekinatorClass = 0;                                           // No number from Wekinator yet.
        Serial.println(">> LISTENING");                               // Processing is told listening has started.
      }                                                               // End of the first pass.

      if (letter == 'c') { setPhase(CLOSING); break; }                // A c from Processing closes the device straight away.
      if (letter >= '1' && letter <= '3') wekinatorClass = letter - '0';   // If Wekinator's number arrives early, it is kept for the exercise ('2' - '0' = the number 2).
      runMotors();                                                    // The motors take a step if it is time.

      if (readBreathAndSend()) setRing(nose.light, mouth.light);      // The sensors are read and the data sent; after each 50 ms reading the LEDs show nose (top) and mouth (bottom).

      if (now - lastPlan >= 10) {                                     // Every 10 ms the motors follow the person.
        float seconds = (now - lastPlan) / 1000.0;                    // How much time passed since last time, in seconds (about 0.01).
        lastPlan = now;                                               // This time is remembered for the next 10 ms.
        moveBreath(breathDirection, BREATH_SPEED, BREATH_SPEED, usingChest, seconds);   // The motors move with the person's breath; motor 1 joins only for chest breathing.
      }                                                               // End of the 10 ms work.

      if (now - phaseStart >= LISTEN_SECONDS * 1000) setPhase(EXERCISE);   // After 15 s the device moves on to the exercise.
      break;                                                          // End of this pass of listening.

    // ---------- EXERCISE: Wekinator's number -> one of the 3 cases -> slow transition to its pattern ----------
    case EXERCISE: {                                                  // EXERCISE: the { } give this case its own space for new variables.

      // ----- 1. FIRST PASS -----
      if (phaseStarted == false) {                                    // FIRST PASS: the exercise sets itself up.
        phaseStarted = true;                                          // This block will not run again.
        phaseStart = now;                                             // The 5 s wait for Wekinator counts from here.
        exerciseClass = 0;                                            // No case has been chosen yet.
      }                                                               // End of the first pass.

      // ----- 2. CLOSE NOW? -----
      if (letter == 'c') { setPhase(CLOSING); break; }                // A c from Processing closes the device straight away.

      runMotors();                                                    // The motors take a step if it is time.
      bool newReading = readBreathAndSend();                          // The sensors keep reading and sending data; newReading is true right after a 50 ms reading.

      // ----- 3. WEKINATOR'S NUMBER (only until a case is chosen) -----
      if (exerciseClass == 0) {                                       // While no case has been chosen yet...
        if (letter >= '1' && letter <= '3') wekinatorClass = letter - '0';   // ...a 1, 2 or 3 from Wekinator is kept.

        if (wekinatorClass == 0) {                                    // If there is still no number, the device copies the person and keeps trying, for at most 5 s.
          if (newReading) setRing(nose.light, mouth.light);           // After each reading the LEDs show nose (top) and mouth (bottom).
          if (now - lastPlan >= 10) {                                 // Every 10 ms...
            float seconds = (now - lastPlan) / 1000.0;                // ...the time since last time is worked out (about 0.01 s)...
            lastPlan = now;                                           // ...this time is remembered...
            moveBreath(breathDirection, BREATH_SPEED, BREATH_SPEED, usingChest, seconds);   // ...and the motors follow the person.
          }                                                           // End of the 10 ms work.
          if (now - phaseStart >= CLASS_WAIT_SECONDS * 1000) setPhase(CLOSING);   // If 5 s pass with no number, the device closes.
          break;                                                      // Nothing below runs until a number has arrived.
        }                                                             // End of waiting.

        // ----- 4. NUMBER RECEIVED: choose the case, start the transition -----
        exerciseClass = wekinatorClass;                               // The number is locked for the whole exercise; later numbers are ignored.

        // THE CASES: one per Wekinator number, only the GOAL (data)
        //   goalIn / goalOut seconds in / out; goalChest true = chest (2+3 and 1), false = belly (only 2+3);
        //   goalInNose / goalOutNose true = nose (top LEDs), false = mouth (bottom LEDs); holdBreaths = breaths at the goal, then closing
        switch (exerciseClass) {                                      // The case that matches Wekinator's number is picked.
          case 1:                                                     // Case 1, TOO FAST: the person is slowed down, with a long exhale.
            goalIn = 3.5;  goalOut = 6.5;  goalChest = false;         // The goal is 3.5 s in and 6.5 s out, with the belly.
            goalInNose = true;  goalOutNose = true;   holdBreaths = 8;   // In and out through the nose; the goal is held for 8 breaths.
            break;                                                    // End of case 1.
          case 2:                                                     // Case 2, TOO SLOW: an even, steady rhythm.
            goalIn = 5.0;  goalOut = 5.0;  goalChest = false;         // The goal is 5 s in and 5 s out, with the belly.
            goalInNose = true;  goalOutNose = true;   holdBreaths = 8;   // In and out through the nose; the goal is held for 8 breaths.
            break;                                                    // End of case 2.
          case 3:                                                     // Case 3, ERRATIC: a steady rhythm with a slightly longer exhale.
            goalIn = 4.0;  goalOut = 6.0;  goalChest = false;         // The goal is 4 s in and 6 s out, with the belly.
            goalInNose = true;  goalOutNose = false;  holdBreaths = 8;   // In through the nose and out through the mouth; the goal is held for 8 breaths.
            break;                                                    // End of case 3.
        }                                                             // End of the cases.

        float personIn  = 2.0;                                        // The person's inhale is set to 2 s, in case listening measured no breath.
        float personOut = 3.0;                                        // The person's exhale is set to 3 s, for the same reason.
        if (breathsFound > 0) {                                       // If listening measured at least one whole breath...
          personIn  = constrain(avgPeriodMs * avgRatio / (1.0 + avgRatio) / 1000.0, 1.0, 8.0);   // ...their inhale is used instead: the breath length split by their in/out ratio (ms -> s, kept between 1 and 8 s)...
          personOut = constrain(avgPeriodMs / (1.0 + avgRatio) / 1000.0, 1.0, 8.0);   // ...and their exhale (the rest of the breath).
        }                                                             // End of the person's rhythm.

        guideIn  = personIn;                                          // The pattern's inhale starts at the person's inhale...
        guideOut = personOut;                                         // ...and its exhale at the person's exhale.
        inStep  = (goalIn  - personIn)  / TRANSITION_BREATHS;         // How much the inhale changes after each breath, e.g. (3.5 - 2) / 6 = 0.25 s.
        outStep = (goalOut - personOut) / TRANSITION_BREATHS;         // How much the exhale changes after each breath, e.g. (6.5 - 3) / 6 = 0.58 s.

        guideBreaths = 0;                                             // The pattern has not done any breaths yet.
        guideInhaling = false;                                        // The pattern starts with an EXHALE, to bring the motors back to the open position first.
        guideTime = 0;                                                // That exhale has lasted 0 s so far.
        exhaleSpeed = breathSteps / guideOut;                         // The speed that brings the motors back to open in exactly guideOut seconds.

        Serial.print(">> EXERCISE ");                                 // Processing is told the exercise has started...
        Serial.println(exerciseClass);                                // ...and which case, e.g. ">> EXERCISE 1".
      }                                                               // End of choosing the case.

      // ----- 5. THE TRANSITION: the motors follow a calculated pattern now, not the person -----
      if (newReading) {                                               // After each 50 ms reading the LEDs are updated.
        float light = constrain(breathSteps / (BREATH_SPEED * guideIn), 0.0, 1.0);   // The brightness is 0 at the open position and 1 at the end of the pattern's inhale.
        bool viaNose = guideInhaling ? goalInNose : goalOutNose;      // While breathing in the case's inhale setting is used, while breathing out its exhale setting.
        if (viaNose) setRing(light, 0);                               // Nose: the top half lights up and the bottom stays off.
        else         setRing(0, light);                               // Mouth: the bottom half lights up and the top stays off.
      }                                                               // End of the LEDs.

      if (now - lastPlan >= 10) {                                     // Every 10 ms the pattern moves on.
        float seconds = (now - lastPlan) / 1000.0;                    // How much time passed since last time, in seconds (about 0.01).
        lastPlan = now;                                               // This time is remembered for the next 10 ms.
        guideTime = guideTime + seconds;                              // The current inhale or exhale has lasted this much longer.

        if (guideInhaling) {                                          // If the pattern is breathing in...
          if (guideTime >= guideIn) {                                 // ...and the inhale has lasted long enough...
            guideInhaling = false;                                    // ...the pattern starts breathing out,
            guideTime = 0;                                            // the exhale starts at 0 s,
            exhaleSpeed = breathSteps / guideOut;                     // and the speed is set to bring the motors back to open in exactly guideOut seconds.
          }                                                           // End of inhale done.
        } else {                                                      // If the pattern is breathing out...
          if (guideTime >= guideOut) {                                // ...and the exhale has lasted long enough, one whole breath is done.
            guideInhaling = true;                                     // The pattern starts breathing in,
            guideTime = 0;                                            // the inhale starts at 0 s,
            guideBreaths = guideBreaths + 1;                          // and the breath is counted.
            if (guideBreaths <= TRANSITION_BREATHS) {                 // During the first 6 breaths (the transition)...
              guideIn  = guideIn  + inStep;                           // ...the next inhale is one step closer to the goal...
              guideOut = guideOut + outStep;                          // ...and so is the next exhale.
            }                                                         // End of the step.
          }                                                           // End of exhale done.
        }                                                             // End of in / out.

        if (guideInhaling) moveBreath( 1, BREATH_SPEED, 0, goalChest, seconds);   // While the pattern breathes in, the motors close at BREATH_SPEED.
        else               moveBreath(-1, 0, exhaleSpeed, goalChest, seconds);   // While it breathes out, the motors open at exhaleSpeed.

        if (guideBreaths >= TRANSITION_BREATHS + holdBreaths) setPhase(CLOSING);   // When the transition and the hold breaths are done, the device closes.
      }                                                               // End of the 10 ms work.
      break;                                                          // End of this pass of the exercise.
    }                                                                 // End of the exercise's own space.

    // ---------- CLOSING: 1. stop where you are, motor 0 unlocks  2. everything back up to 0 ----------
    case CLOSING:                                                     // CLOSING: the opening in reverse. Motor 0 unlocks first, then the device closes.
      if (phaseStarted == false) {                                    // FIRST PASS = MOVE 1: the motors stop where they are and motor 0 unlocks.
        phaseStarted = true;                                          // This block will not run again.
        phaseStart = now;                                             // The time the closing started.
        secondMove = false;                                           // The closing is in its 1st move.
        motor23.moveTo(motor23.currentPosition());                    // Motors 2+3 get "where they are now" as their target, so they slow down and stop.
        motor1.moveTo(motor1.currentPosition());                      // Motor 1 does the same.
        setMotorSpeeds(OPEN_ACCEL);                                   // The motors get the soft acceleration again.
        motor0.moveTo(0);                                             // Motor 0 turns back to 0, which unlocks the air.
        Serial.println(">> CLOSING");                                 // Processing is told the closing has started.
      }                                                               // End of the first pass.

      runMotors();  spinRing();                                       // The motors take their steps and the white bar spins round the ring.
      if (motorsArrived()) {                                          // Only when ALL motors have reached their targets (so nothing closes while motor 0 is still turning)...
        if (secondMove == false) {                                    // ...and it was the 1st move (motor 0 unlocked, the others stopped)...
          motor23.moveTo(0);                                          // ...MOVE 2 starts: motors 2+3 go back up to 0, closing the device...
          motor1.moveTo(0);                                           // ...and so does motor 1 (half the distance at half the speed, so they arrive together).
          secondMove = true;                                          // The closing is now in its 2nd move.
        } else {                                                      // If the 2nd move is done, the device is closed...
          setPhase(CALIBRATION);                                      // ...and it goes back to calibration, ready for the next person (n n n).
        }                                                             // End of move 1 / move 2.
      }                                                               // End of motors arrived.
      break;                                                          // End of this pass of the closing.
  }                                                                   // End of the phases.
}                                                                     // End of loop.
