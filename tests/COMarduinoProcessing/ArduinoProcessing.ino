/*
  LED RING CLASS TEST  -  Arduino Nano Every

  The Arduino side of the Wekinator test. Use it together with the Processing sketch WekinatorMouseTest.

  Processing sends a number (1, 2 or 3, then a new line), exactly like the Wekinator bridge does for BreathDevice.
  This sketch colours the whole LED ring for that number:
    1 = RED      (in BreathDevice: too fast)
    2 = BLUE     (in BreathDevice: too slow)
    3 = GREEN    (in BreathDevice: erratic)
    0 = OFF
  Every time the number changes it answers with a message, for example ">> CLASS 2 BLUE" (no ":" so Processing only shows it).

  So: the ring changes colour when the cursor moves to another area = Processing -> Wekinator -> Processing -> Arduino all work.

  WIRING: the same as BreathDevice (ring DI -> A1, ring power from the power bank, grounds joined).
  Needs the library "Adafruit NeoPixel". After the test, upload BreathDevice again.
*/

#include <Adafruit_NeoPixel.h>                                        // The Adafruit NeoPixel library controls the LED ring.

// PINS

const int LED_PIN   = A1;                                             // The LED ring's data wire is connected to pin A1, as in BreathDevice.
const int LED_COUNT = 45;                                             // The ring has 45 LEDs.
Adafruit_NeoPixel ring(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);     // This creates the LED ring object the code talks to.

// SETTINGS

const int  LED_BRIGHTNESS = 60;                                       // The ring's overall brightness, from 0 to 255, the same as BreathDevice.
const long BAUD_RATE      = 115200;                                   // The USB speed, the same as BreathDevice and the Processing sketches.

// VARIABLES

int shownClass = -1;                                                  // The number the ring is showing now (-1 = nothing yet, so the first number always changes it).

// HELPERS

void fillRing(int red, int green, int blue) {                         // This function gives every LED the same colour.
  for (int i = 0; i < LED_COUNT; i++) {                               // It goes through all 45 LEDs...
    ring.setPixelColor(i, ring.Color(red, green, blue));              // ...and gives each one the colour.
  }                                                                   // End of going through the LEDs.
  ring.show();                                                        // The new colours are sent to the ring all at once.
}                                                                     // End of fillRing.

// SETUP + LOOP

void setup() {                                                        // setup() runs once, when the Arduino is switched on.
  Serial.begin(BAUD_RATE);                                            // The USB connection is started.
  ring.begin();                                                       // The LED ring is started.
  ring.setBrightness(LED_BRIGHTNESS);                                 // Its overall brightness is limited.
  fillRing(0, 0, 0);                                                  // It starts with all LEDs off.
  Serial.println(">> LED RING CLASS TEST READY");                     // Processing is told the test has started.
}                                                                     // End of setup.

void loop() {                                                         // loop() runs again and again.
  if (Serial.available() == 0) return;                                // If Processing sent nothing, nothing happens this time.
  char letter = Serial.read();                                        // One letter is read.
  if (letter < '0' || letter > '3') return;                           // Anything that is not 0, 1, 2 or 3 (like the new line) is ignored.

  int number = letter - '0';                                          // The character '2' becomes the number 2.
  if (number == shownClass) return;                                   // The same number as before: the ring already shows it.
  shownClass = number;                                                // The new number is remembered.

  if (number == 1) {                                                  // 1...
    fillRing(255, 0, 0);                                              // ...turns the ring RED.
    Serial.println(">> CLASS 1 RED");                                 // Processing is told.
  } else if (number == 2) {                                           // 2...
    fillRing(0, 0, 255);                                              // ...turns the ring BLUE.
    Serial.println(">> CLASS 2 BLUE");                                // Processing is told.
  } else if (number == 3) {                                           // 3...
    fillRing(0, 255, 0);                                              // ...turns the ring GREEN.
    Serial.println(">> CLASS 3 GREEN");                               // Processing is told.
  } else {                                                            // 0...
    fillRing(0, 0, 0);                                                // ...turns the ring OFF.
    Serial.println(">> CLASS 0 OFF");                                 // Processing is told.
  }                                                                   // End of choosing the colour.
}                                                                     // End of loop.
