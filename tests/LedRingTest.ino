/*
  LED RING TEST  -  Arduino Nano Every

  Tests ONLY the LED ring (no motors, no sensors). Same pin, brightness and colours as BreathDevice.

  It repeats 5 steps forever. The Serial Monitor or Processing (115200 baud) shows which step is running:
    1. LED 0 RED, LED 1 GREEN, LED 2 BLUE   -> where the ring starts, which way it counts, and if the colours are right
    2. ALL LEDs dim white                   -> every LED works? (a broken LED stops all the LEDs after it)
    3. TOP half BLUE, BOTTOM half PURPLE    -> the nose / mouth halves (uses TOP_LED)
    4. the white spinning bar, 2 rounds     -> the opening / closing animation
    5. both halves slowly brighter, then darker, 2 times -> the breathing light, at the real speed (4 s from off to full)

  WIRING
    Ring DI (data IN, not DO!) -> 220-470 ohm resistor -> Nano A1
    Ring 5V  -> BOTTOM red rail  (power bank 5V)
    Ring GND -> BOTTOM blue rail (power bank GND)
    BOTTOM blue rail joined to a Nano GND pin (one wire)  <- without this the ring does nothing or flickers

  Needs the library "Adafruit NeoPixel".
*/

#include <Adafruit_NeoPixel.h>                                        // The Adafruit NeoPixel library controls the LED ring.

// PINS

const int LED_PIN   = A1;                                             // The LED ring's data wire is connected to pin A1, as in BreathDevice.
const int LED_COUNT = 45;                                             // The ring has 45 LEDs.
Adafruit_NeoPixel ring(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);     // This creates the LED ring object the code talks to.

// SETTINGS

const int   LED_BRIGHTNESS = 60;                                      // The ring's overall brightness, from 0 to 255, the same as BreathDevice (kept low to save current).
const int   TOP_LED        = 0;                                       // The number of the LED at the very top of the ring, the same as BreathDevice. Step 1 helps to find it.
const float LED_STEP       = 0.0125;                                  // How much brighter or darker the breathing light gets every 50 ms, the same as BreathDevice.
const long  BAUD_RATE      = 115200;                                  // The speed of the USB connection, the same as BreathDevice.

// HELPERS  (the same LED functions as BreathDevice)

void setRing(float top, float bottom) {                               // This function lights the top half blue (nose) and the bottom half purple (mouth), each from 0 to 1.
  for (int i = 0; i < LED_COUNT; i++) {                               // It goes through all 45 LEDs, one by one.
    int d = (i - TOP_LED + LED_COUNT) % LED_COUNT;                    // How far round the ring this LED is from the top LED.
    bool topHalf = d <= LED_COUNT / 4 || d >= LED_COUNT - LED_COUNT / 4;   // The LED is in the top half if it is within 11 LEDs of the top, on either side.
    if (topHalf) ring.setPixelColor(i, ring.Color(0, 0, top * 255));  // A top LED gets blue, as bright as the top value.
    else         ring.setPixelColor(i, ring.Color(bottom * 150, 0, bottom * 255));   // A bottom LED gets purple, as bright as the bottom value.
  }                                                                   // End of going through the LEDs.
  ring.show();                                                        // The new colours are sent to the ring all at once.
}                                                                     // End of setRing.

void ringOff() {                                                      // This function switches every LED off.
  ring.clear();                                                       // All LEDs are set to off...
  ring.show();                                                        // ...and the ring is updated.
}                                                                     // End of ringOff.

// SETUP + LOOP

void setup() {                                                        // setup() runs once, when the Arduino is switched on.
  Serial.begin(BAUD_RATE);                                            // The USB connection is started.
  ring.begin();                                                       // The LED ring is started.
  ring.setBrightness(LED_BRIGHTNESS);                                 // Its overall brightness is limited.
  ringOff();                                                          // It starts with all LEDs off.
}                                                                     // End of setup.

void loop() {                                                         // loop() runs the 5 steps, then starts again.

  // ----- STEP 1: where the ring starts, which way it counts, the colours -----
  Serial.println(">> STEP 1 - LED 0 red, LED 1 green, LED 2 blue");   // The step is announced (no ":" so Processing only shows it).
  ringOff();                                                          // All LEDs are switched off first.
  ring.setPixelColor(0, ring.Color(255, 0, 0));                       // LED 0 is set to red: this is where the ring starts.
  ring.setPixelColor(1, ring.Color(0, 255, 0));                       // LED 1 is set to green: this shows which way the numbers go.
  ring.setPixelColor(2, ring.Color(0, 0, 255));                       // LED 2 is set to blue. If red and green look swapped, the colour order is wrong.
  ring.show();                                                        // The three LEDs light up.
  delay(4000);                                                        // They stay on for 4 s.

  // ----- STEP 2: every LED works? -----
  Serial.println(">> STEP 2 - all LEDs dim white");                   // The step is announced.
  for (int i = 0; i < LED_COUNT; i++) ring.setPixelColor(i, ring.Color(80, 80, 80));   // Every LED is set to a dim white.
  ring.show();                                                        // They all light up. A dark gap means a broken LED or a bad joint there.
  delay(3000);                                                        // They stay on for 3 s.

  // ----- STEP 3: the nose and mouth halves -----
  Serial.println(">> STEP 3 - top half blue (nose), bottom half purple (mouth)");   // The step is announced.
  setRing(1.0, 1.0);                                                  // Both halves at full brightness. The blue half should be at the top of the device.
  delay(4000);                                                        // They stay on for 4 s.

  // ----- STEP 4: the opening / closing animation -----
  Serial.println(">> STEP 4 - spinning bar (opening and closing animation)");   // The step is announced.
  for (int pos = 0; pos < LED_COUNT * 2; pos++) {                     // The bar moves 90 times: two full rounds of 45 LEDs.
    ring.clear();                                                     // All LEDs are switched off...
    for (int i = 0; i < 5; i++) ring.setPixelColor((pos + i) % LED_COUNT, ring.Color(255, 255, 255));   // ...then five LEDs in a row, starting at pos, are lit white.
    ring.show();                                                      // The ring shows the bar.
    delay(40);                                                        // The bar moves on every 40 ms, the same speed as in BreathDevice.
  }                                                                   // End of the two rounds.

  // ----- STEP 5: the breathing light, at the real speed -----
  Serial.println(">> STEP 5 - breathing light (brighter = breathing in, darker = breathing out)");   // The step is announced.
  for (int breath = 0; breath < 2; breath++) {                        // Two breaths are shown.
    for (float light = 0; light <= 1.0; light = light + LED_STEP) {   // Breathing in: the light goes from off to full, one LED_STEP at a time...
      setRing(light, light);                                          // ...on both halves...
      delay(50);                                                      // ...every 50 ms, so it takes 4 s, as in BreathDevice.
    }                                                                 // End of breathing in.
    for (float light = 1.0; light >= 0; light = light - LED_STEP) {   // Breathing out: the light goes from full back to off...
      setRing(light, light);                                          // ...on both halves...
      delay(50);                                                      // ...at the same speed.
    }                                                                 // End of breathing out.
  }                                                                   // End of the two breaths.

  ringOff();                                                          // All LEDs are switched off...
  delay(1000);                                                        // ...for 1 s before the steps start again.
}                                                                     // End of loop.
