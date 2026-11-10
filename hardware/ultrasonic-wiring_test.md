# Test 4: two HC-SR04 ultrasonic sensors on Arduino Nano

Sketch: `test4_ultrasonic_nano.ino`

Board: Tools > Board > Arduino AVR Boards > Arduino Nano. Serial Monitor at 115200 baud.

The Nano and the HC-SR04 both work at 5V, so every wire goes straight in, no resistors.

## Parts

1. 2x HC-SR04
2. 8 jumper wires (4 per sensor)
3. a breadboard (so both sensors can share 5V and GND)

## Wiring

| HC-SR04 pin | Sensor 1 goes to | Sensor 2 goes to |
|---|---|---|
| VCC | Nano 5V | Nano 5V |
| Trig | Nano D2 | Nano D4 |
| Echo | Nano D3 | Nano D5 |
| GND | Nano GND | Nano GND |

## Reading the chest (less noise)

The sketch takes 5 pings per sensor, keeps the middle one, and smooths it.

Open Tools > Serial Plotter to see the breathing wave.

If a sensor shows -1.00 it has no good reading yet.

Setup that matters more than code:

1. Tape a flat, stiff card (about 10 x 10 cm cardboard) on the chest. Clothes swallow sound; a flat card reflects it.
2. Fix the sensors on a stand 15 to 30 cm away, pointing straight at the card. Never hold them in your hand.
3. Keep the two sensors apart (for example one on chest, one on belly) and not facing each other.

Settings in PART 2: SMOOTHING 0.3 is the default. Lower (0.15) = smoother but slower. Higher (0.5) = faster but noisier.

## Checks if it prints -1.00

1. VCC is on 5V, not on 3V3.
2. Trig and Echo are not swapped (Trig on D2/D4, Echo on D3/D5).
3. Nothing is in front of the sensor closer than 2 cm or farther than 60 cm (the sketch ignores anything farther), and the target is not soft fabric (it absorbs sound).
