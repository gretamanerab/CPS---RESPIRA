
/*
  LED RING RAINBOW TEST - Arduino Nano Every

  RESPIRA - Initial LED colour exploration

  Tests a continuous rotating rainbow animation using
  the WS2812B addressable LED ring.

  This is an independent visual experiment, separate
  from the final breathing-feedback firmware.

  HARDWARE
    Board: Arduino Nano Every
    LED type: WS2812B
    LED count: 45
    Data pin: A1

  WIRING
    LED DIN -> 220-470 ohm resistor -> Nano A1
    LED 5V -> External regulated 5V supply
    LED GND -> External supply GND
    Nano GND -> External supply GND

  LIBRARY
    Adafruit NeoPixel
*/

#include <Adafruit_NeoPixel.h>

// PIN CONFIGURATION

#define LED_PIN A1
#define NUM_LEDS 45

// ANIMATION SETTINGS

#define SPEED 40

const int LED_BRIGHTNESS = 60;

// INITIALISE LED RING

Adafruit_NeoPixel ring(
  NUM_LEDS,
  LED_PIN,
  NEO_GRB + NEO_KHZ800
);

uint32_t offset = 0;

// SETUP

void setup() {
  ring.begin();
  ring.setBrightness(LED_BRIGHTNESS);
  ring.clear();
  ring.show();
}

// CONTINUOUS RAINBOW ANIMATION

void loop() {

  for (int i = 0; i < NUM_LEDS; i++) {

    uint32_t color = ring.gamma32(
      ring.ColorHSV(
        offset + (i * 65536UL / NUM_LEDS)
      )
    );

    ring.setPixelColor(i, color);
  }

  ring.show();

  offset += 512;

  delay(SPEED);
}
