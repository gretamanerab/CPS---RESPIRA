
# LED Ring Wiring and Rainbow Animation Test: Arduino Nano

This document describes the wiring, setup, and initial testing of the addressable LED ring used in RESPIRA.

The initial test implemented a continuous rainbow animation to evaluate individual LED control, colour transitions, brightness, and animation speed.

The Arduino test sketch is stored separately in the `tests/LedsTest` folder.

## LED Type

**WS2812B individually addressable RGB LEDs**, arranged in a circular ring.

Each LED contains a built-in control chip, allowing its colour and brightness to be controlled independently through a single data connection.

- **WS2812B:** Individually addressable RGB LED with an integrated driver.
- **RGB:** Each LED combines red, green, and blue light to produce different colours.
- **5V:** Nominal operating voltage.
- **DIN:** Data input receiving instructions from the Arduino.
- **GND:** Ground connection.

The rainbow test was configured for 32 LEDs. The physical LED count should be checked against the actual ring before running the code.

## Parts

| Part | Quantity | Notes |
|---|---|---|
| WS2812B LED ring | 1 | Addressable RGB LED assembly |
| Arduino Nano | 1 | Controls the LED animation |
| Jumper wires | 3 | Power, ground, and data |
| Breadboard | 1 | For temporary connections |
| USB data cable | 1 | Programming and power for the Arduino |
| 330–470 Ω resistor | 1 | Recommended in series with the data line |
| 1000 µF capacitor | 1 | Recommended across LED power and ground |
| Regulated 5V power supply | 1 | Must support the LED current requirement |

## Wiring

The WS2812B LED ring requires three main connections: power, ground, and data.

| LED Ring Connection | Arduino / Power Connection | Function |
|---|---|---|
| 5V | Regulated 5V supply | Powers the LEDs |
| GND | Supply GND and Nano GND | Common ground |
| DIN | Nano D6 | Receives animation data |

The data wire must connect to DIN, not DOUT.

### Wiring Diagram

```text
Arduino Nano                 WS2812B LED Ring

D6 ───[330–470 Ω]──────────► DIN

GND ───────────────────────► GND
                              │
5V Power Supply ────────────► 5V
                              │
Power Supply GND ───────────► GND

1000 µF capacitor:
Between LED 5V (+) and GND (-)
```

For a 32-LED test, the theoretical maximum LED current can approach 1.92 A when all LEDs display full-brightness white.

The LED ring should therefore use an appropriately rated external 5V supply rather than drawing high current through the Arduino Nano.

The Arduino and LED power supply must share a common ground.

## Step by Step

**Disconnect power before changing any wiring.**

### 1. Prepare the Arduino

1. Connect the Arduino Nano to the computer using a USB data cable.
2. Open the Arduino IDE.
3. Select the correct Arduino Nano board and port.
4. Install the Adafruit NeoPixel library through the Library Manager.

### 2. Connect the LED Ring

1. Connect the LED ring's DIN pad to Arduino digital pin D6 through a 330–470 Ω resistor.
2. Connect the LED ring's GND pad to the external power supply ground.
3. Connect Arduino GND to the same ground.
4. Connect the LED ring's 5V pad to the regulated 5V supply.
5. Connect a 1000 µF capacitor across the LED supply terminals, observing polarity.

Check all connections before applying power.

### 3. Upload the Rainbow Test

Open the Arduino sketch:

`LedRingTest.ino`

The test uses the Adafruit NeoPixel library to assign colours to individual LEDs.

The animation continuously shifts the hue values around the ring, producing a rotating rainbow effect.

### Test Parameters

| Parameter | Value |
|---|---|
| LED type | WS2812B |
| Data pin | D6 |
| Configured LED count | 32 |
| Brightness | 80 / 255 |
| Animation delay | 40 ms |
| Animation type | Continuous rainbow |
| Colour order | GRB |

These parameters describe the initial rainbow test rather than the final RESPIRA lighting behaviour.

## Expected Results

When the sketch is uploaded and the LED ring is powered:

1. The LEDs illuminate in different colours.
2. The colours form a continuous rainbow around the circular arrangement.
3. The rainbow gradually rotates as the hue values change.
4. The animation repeats continuously.

This test evaluates the basic functionality of the addressable LEDs before integration with RESPIRA's breathing feedback system.

## Troubleshooting

| Problem | Possible Cause | Solution |
|---|---|---|
| No LEDs illuminate | Missing power or incorrect wiring | Check 5V, GND, and DIN |
| Only some LEDs illuminate | Incorrect LED count or data interruption | Verify the LED count and connections |
| LEDs flicker unexpectedly | Unstable power or ground connection | Check the power supply and common ground |
| Colours appear incorrect | Incorrect RGB colour order | Check NEO_GRB configuration |
| Animation does not move | Sketch not running correctly | Re-upload the Arduino sketch |
| LEDs reset or behave unpredictably | Insufficient power supply current | Use an appropriately rated 5V supply |

## Integration into RESPIRA

The LED ring was investigated as a visual feedback component for RESPIRA.

The initial rainbow animation demonstrated individual LED control and smooth colour transitions.

Subsequent firmware development explored how lighting could be integrated into the breathing interaction, including visual feedback during different phases of the experience.

The rainbow animation was an independent hardware test and does not represent the final lighting behaviour of the device.

## Related Files

- [Rainbow LED Test](../tests/LedsTest/LedRingTest.ino)
- [Main Arduino Firmware](../firmware/BreathDevice.ino)
- [Bill of Materials](Bills_of_Materials_BOM.md)
```
