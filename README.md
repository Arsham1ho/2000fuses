# 2000fuses

Embedded systems coursework and competition code for an Arduino Mega based two-motor robot.

The repository is organised by lab session, ending with the code used in the final
line/distance competition (best recorded result: **4.3 cm**).

## Contents

| Path | Description |
| --- | --- |
| `lab-august-31/week36/` | Week 36 lab — first sketches |
| `lab-september-7/week37/` | Week 37 lab |
| `lab-september-21/joystick/` | Reading a 2-axis analog joystick and driving the motors |
| `lab-september-21/encoder_isr/` | Wheel encoders handled with pin-change interrupts |
| `Week40/Week40.ino` | Final competition sketch (joystick + encoders + LCD) |

## Hardware

- Arduino Mega 2560
- 16x2 character LCD in 4-bit mode (`LiquidCrystal`), pins `rs=37, en=36, d4..d7=35..32`
- Analog joystick on `A8` / `A9` with the click on digital pin `19`
  (note: the X and Y pins are deliberately swapped for ergonomics)
- Two DC motors via a dual H-bridge — direction on pins `7` / `8`, PWM on `9` / `10`
- Two wheel encoders on interrupt pins `2` and `3`

Pin assignments differ between labs; check the `const int` block at the top of each sketch.

## Building

Open the `.ino` file for the session you want in the Arduino IDE, select
**Arduino Mega 2560** as the board, and upload. The only external dependency is
`LiquidCrystal`, which ships with the IDE.
