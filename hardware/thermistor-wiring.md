# Thermistor wiring: Arduino Nano

How the two breathing thermistors are wired to the Arduino Nano. The test code that reads them is in the `tests` folder.

## Thermistor type

**2 × 100K NTC 3950 thermistors**, glass bead on a 1 m cable.

* **100K**: it measures 100,000 ohms at 25 °C.
* **NTC** (negative temperature coefficient): when it gets warmer, its resistance goes down.
* **3950**: the beta value, which the code uses to convert the reading into degrees.

One sits about 1 cm below a nostril, and the other about 2 cm in front of the lips, both held in a 3D printed holder. Warm air breathed out makes the reading rise, and cooler air breathed in makes it fall.

## Parts

| Part | How many | Notes |
|---|---|---|
| 100K NTC 3950 thermistor | 2 | One for the nose, one for the mouth. The two wires have no plus or minus. |
| 100k Ω resistor | 2 | One per thermistor. Colour bands: brown, black, black, orange, brown |
| Arduino Nano (classic, ATmega328P) | 1 | Sits in the breadboard |
| Breadboard | 1 | From the Elegoo kit |
| Male to male jumper wires | about 6 | |
| USB data cable | 1 | Powers the Nano and carries the readings to the laptop |

## How it works

The Nano cannot measure resistance, only voltage. So each thermistor is paired with a fixed 100k resistor to make a **voltage divider**: the thermistor goes from 5V to a middle point, and the resistor goes from that middle point to GND. The Nano reads the voltage at the middle point. When the thermistor warms up, its resistance drops and the voltage at the middle point rises.

```
Nano 5V  ──► RED RAIL ──( nose thermistor )──┬──► Nano A0
                                             │
Nano GND ──► BLUE RAIL ──[ 100k resistor ]───┘

Nano 5V  ──► RED RAIL ──( mouth thermistor )─┬──► Nano A1
                                             │
Nano GND ──► BLUE RAIL ──[ 100k resistor ]───┘
```

## Step by step

**Unplug the USB cable before changing any wire.**

Push the Nano into the left end of the breadboard so it straddles the middle gap. Every Nano pin is then joined to the free holes in the same column, so wires go into those holes. On the breadboard, holes a to e in one numbered column are joined together. The rail with the red line is the RED RAIL (power), and the rail with the blue line is the BLUE RAIL (ground).

### Power

| Step | Wire | From | To |
|---|---|---|---|
| W1 | Red | a hole next to the Nano's **5V** pin | **RED RAIL** |
| W2 | Black | a hole next to the Nano's **GND** pin | **BLUE RAIL** |

### Nose thermistor (column 30)

| Step | Part | One end | Other end |
|---|---|---|---|
| W3 | Thermistor wire 1 | **RED RAIL** | |
| W4 | Thermistor wire 2 | column 30, hole a | |
| W5 | 100k resistor | column 30, hole b | **BLUE RAIL** |
| W6 | Jumper wire | column 30, hole c | a hole next to the Nano's **A0** pin |

### Mouth thermistor (column 36)

| Step | Part | One end | Other end |
|---|---|---|---|
| W7 | Thermistor wire 1 | **RED RAIL** | |
| W8 | Thermistor wire 2 | column 36, hole a | |
| W9 | 100k resistor | column 36, hole b | **BLUE RAIL** |
| W10 | Jumper wire | column 36, hole c | a hole next to the Nano's **A1** pin |

Check every wire against the tables, then plug in the USB cable.

## Pin summary

| Nano pin | Connected to |
|---|---|
| 5V | RED RAIL, the top of both thermistors |
| GND | BLUE RAIL, the bottom of both 100k resistors |
| A0 | Nose thermistor middle point |
| A1 | Mouth thermistor middle point |

## Troubleshooting

| What you see | Likely cause | Fix |
|---|---|---|
| A reading is stuck and never moves | Loose wire or missing resistor | Check that thermistor's column against the table |
| Degrees show something silly, like −40 or 150 | Thermistor and resistor swapped, or the resistor is not 100k | Thermistor to the RED RAIL, resistor to the BLUE RAIL; check the colour bands |
| The lines barely move | The beads are too far from the air | Move them closer to the nostril and lips |
