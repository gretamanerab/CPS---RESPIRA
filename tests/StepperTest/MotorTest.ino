/*
  MOTOR TEST  -  Arduino Nano Every

  Tests ONLY the motors (no sensors, no LED ring). Same pins and the same directions as BreathDevice.

  HOW TO USE  (Serial Monitor or Processing, 115200 baud, type the letter and press Enter)
    1    choose motor 1
    2    choose motors 2+3   (they share pins, so they always move together)
    0    choose motor 0      (the one that locks the air)
    +    move the chosen motor DOWN by STEP_SIZE steps (the same direction as opening)
    -    move the chosen motor UP by STEP_SIZE steps   (the same direction as closing)
    +++  three letters in a row = three moves

  WHAT TO CHECK
    Every motor turns, and + goes the same way as the opening of the device on all of them.
    If one motor turns the wrong way, its pins are wired in the wrong order on the breadboard.

  WIRING  (IN1..IN4 = the 4 input pins on each ULN2003 board, wired in normal order on the breadboard)
    Motor 1      IN1 -> D6    IN2 -> D7    IN3 -> D8    IN4 -> D9
    Motor 0      IN1 -> D10   IN2 -> D11   IN3 -> D12   IN4 -> D13
    Motors 2+3   IN1 -> A2    IN2 -> A3    IN3 -> A4    IN4 -> A5   (board 2 AND board 3 both on these 4 pins)
    Every ULN2003 board:  + -> power bank 5V rail,  - -> power bank GND rail. Power bank GND joined to a Nano GND pin.

  Needs the library "AccelStepper".
*/

#include <AccelStepper.h>                                             // The AccelStepper library moves the motors with a smooth speed-up and slow-down.

// PINS

AccelStepper motor23(AccelStepper::HALF4WIRE, A5, A3, A4, A2);        // Motors 2+3 share pins A2-A5. The pins are listed backwards, exactly as in BreathDevice, so + means "down" here too.
AccelStepper motor1 (AccelStepper::HALF4WIRE,  6,  8,  7,  9);        // Motor 1 is connected to pins D6-D9 (written IN1, IN3, IN2, IN4: the normal order for the 28BYJ-48 in code).
AccelStepper motor0 (AccelStepper::HALF4WIRE, 10, 12, 11, 13);        // Motor 0, the one that locks the air, is connected to pins D10-D13.

// SETTINGS

const long  STEP_SIZE = 250;                                          // Each + or - moves the motor by 250 steps (the same as calibration in BreathDevice). 4096 steps = 1 full turn.
const float SPEED     = 400;                                          // The top speed for this test, in steps per second (slow and safe).
const float ACCEL     = 500;                                          // How quickly the motor speeds up and slows down.
const long  BAUD_RATE = 115200;                                       // The speed of the USB connection, the same as BreathDevice.

// VARIABLES

int chosen = 1;                                                       // Which motor the + and - letters move: 1, 2 (meaning motors 2+3) or 0. It starts with motor 1.

// HELPERS

AccelStepper& chosenMotor() {                                         // This function gives back the motor that is chosen right now.
  if (chosen == 2) return motor23;                                    // 2 means motors 2+3.
  if (chosen == 1) return motor1;                                     // 1 means motor 1.
  return motor0;                                                      // Anything else (0) means motor 0.
}                                                                     // End of chosenMotor.

void printChosen() {                                                  // This function sends the name of the chosen motor (no ":" so Processing only shows it).
  Serial.print(chosen == 2 ? ">> MOTORS 2+3" : (chosen == 1 ? ">> MOTOR 1" : ">> MOTOR 0"));   // It picks the right name for 2, 1 or 0.
}                                                                     // End of printChosen.

// SETUP + LOOP

void setup() {                                                        // setup() runs once, when the Arduino is switched on.
  Serial.begin(BAUD_RATE);                                            // The USB connection is started.
  motor23.setMaxSpeed(SPEED);  motor23.setAcceleration(ACCEL);        // Motors 2+3 get the test speed and acceleration.
  motor1.setMaxSpeed(SPEED);   motor1.setAcceleration(ACCEL);         // Motor 1 gets the same.
  motor0.setMaxSpeed(SPEED);   motor0.setAcceleration(ACCEL);         // Motor 0 gets the same.
  Serial.println(">> MOTOR TEST - type 1 2 or 0 to choose a motor, + or - to move it");   // The instructions are shown once.
  printChosen();                                                      // The motor chosen at the start is shown...
  Serial.println(" chosen");                                          // ...followed by the word "chosen".
}                                                                     // End of setup.

void loop() {                                                         // loop() runs again and again.
  if (Serial.available() == 0) return;                                // If nothing was typed, nothing happens this time.
  char letter = Serial.read();                                        // One typed letter is read.

  if (letter == '1' || letter == '2' || letter == '0') {              // A 1, 2 or 0 chooses a motor.
    chosen = letter - '0';                                            // The character '2' becomes the number 2.
    printChosen();                                                    // The new choice is shown...
    Serial.println(" chosen");                                        // ...followed by the word "chosen".
  }                                                                   // End of choosing.

  if (letter == '+' || letter == '-') {                               // A + or - moves the chosen motor.
    long steps = STEP_SIZE;                                           // A + moves STEP_SIZE steps down...
    if (letter == '-') steps = -STEP_SIZE;                            // ...and a - moves the same amount up.
    chosenMotor().move(steps);                                        // The chosen motor is told to move those steps from where it is.
    chosenMotor().runToPosition();                                    // The code waits here until it has arrived (fine in a test, nothing else happens).
    printChosen();                                                    // The motor's name is shown...
    Serial.print(" at ");                                             // ...the word "at"...
    Serial.print(chosenMotor().currentPosition());                    // ...its position in steps since power-on...
    Serial.println(" steps");                                         // ...and the word "steps".
  }                                                                   // End of moving.
}                                                                     // End of loop.
