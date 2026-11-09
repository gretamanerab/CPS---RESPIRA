# Respira – Stepper Motor Circuit

## Motor

**28BYJ-48 5V Stepper Motor**

**Driver:** ULN2003 Stepper Motor Driver Module

The stepper motor drives the mechanical bubble-forming mechanism.

The Arduino sends a sequence of digital control signals to the ULN2003 driver, which energises the motor coils in sequence to produce rotational movement.

## Electrical Connections

| ULN2003 Pin | Arduino Nano Every |
|---|---|
| IN1 | D8 |
| IN2 | D9 |
| IN3 | D10 |
| IN4 | D11 |
| GND (-) | Common GND |
| VCC (+) | External regulated 5V supply |

The 28BYJ-48 motor connects directly to the white 5-pin connector on the ULN2003 board.

## Circuit Diagram

```text
       ARDUINO NANO EVERY

       D8  ---------------- IN1
       D9  ---------------- IN2
       D10 ---------------- IN3
       D11 ---------------- IN4
                              |
                         ULN2003 DRIVER
                              |
       +5V EXTERNAL -------- VCC
       GND ----------------- GND
                              |
                         MOTOR OUTPUT
                              |
                         28BYJ-48
                       STEPPER MOTOR
