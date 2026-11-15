
# Mechanical Assembly
 
**Component:** Mechanical Assembly and Actuation System  
**Fabrication methods:** FDM 3D printing, acrylic laser cutting, metalworking, heat bending and hand sewing.
**Primary materials:** PLA, acrylic, string, mechanical fasteners  
**Actuation:** 4 × 28BYJ-48 5V stepper motors  
**Status:** Prototype development

---

## 1. Overview

The mechanical system consists of two circular frames connected by a central hinge assembly. The frames are suspended within a larger structure and actuated using stepper motors and a string-and-bobbin mechanism.

Through controlled winding and unwinding of the strings, the circular frames change their relative positions and orientations, expanding and contracting the soap membrane suspended between them.

This document outlines the fabrication, assembly and integration of the mechanical components.

**Fabrication process:**

1. Prepare the housing geometries in CAD.
2. Export the models for 3D printing, print the upper and lower housing components using PLA.
4. Assemble 3D printed components with electronic components
5. Creation of illustrator file for laser cutting acrylic holder
6. Creation of metal mold to prepare acrylic heat bending
7. Heat bending acrylic by warming to 190 and attaching it to metal structure
8. Putting string around bobbin
9. Electrical wiring
10. Hand sewing fabric on top part of mechanism
11. Putting soap and water mixture in lower component

## 2. Mechanical Components

The assembly consists of the following primary components:

| Component | Fabrication / Material | Function |
|---|---|---|
| Upper housing | 3D-printed PLA | Upper structural enclosure and mounting interface |
| Lower housing | 3D-printed PLA | Base structure and support |
| Circular frames | 3D-printed PLA | Support the soap membrane |
| Central hinges | Mechanical fasteners / hinge components | Connect the circular frames and allow relative rotation |
| Stepper motors | 4 × 28BYJ-48, 5V | Actuate the mechanism |
| Bobbins | Motor-mounted winding components | Wind and unwind the strings |
| Strings | Flexible tensile material | Transfer movement from motors to circular frames |
| Acrylic structure | Heat-bent acrylic | Structural support and enclosure |

Additional electronic components, including the Arduino, sensors, LED ring and motor drivers, are documented separately.

See: [Bill of Materials](../hardware/Bills_of_Materials_BOM.md)

---

## 3. Fabrication of Structural Components

### 3.1 Upper and Lower Housing

The upper and lower structural components were fabricated using FDM 3D printing with PLA filament.

The housings form the main supporting structure of Respira and provide the interfaces for integrating the mechanical system. The upper section accommodates the actuation system, while the lower section provides structural support, water and soap for the installation.

### CAD Files

The mechanical components of Respira were designed in CAD and exported as STEP files for fabrication and assembly. Individual Components can also be viewed within Technical Drawings folder. 

**3D Assembly Model:** [View BreathDevice STEP File](mechanical/BreathDevice.step)

**Fabrication process:**

1. Prepare the housing geometries in CAD.
2. Export the models for 3D printing.
3. Print the upper and lower housing components using PLA.
4. Remove any printing supports and excess material.

### 3.2 Circular Frames

Two circular frames were fabricated using FDM 3D printing with PLA.

These frames form the moving elements of the installation and provide the boundaries supporting the soap membrane. The frames were designed to rotate relative to one another through a central hinge connection.

**Fabrication process:**

1. Model the circular frames in CAD.
2. Include the required mounting points for the hinge assembly.
3. Export and 3D print both frames.
4. Remove supports and clean the printed surfaces.
5. Inspect the hinge connection points.
6. Test the alignment of the two frames before attaching the actuation strings.

---

## 4. Mechanical Assembly

### 4.1 Step 01 — Connecting the Circular Frames

The two circular frames were connected using hinges positioned at their adjoining edges.

**Assembly procedure:**

1. Position the two printed circular frames in their intended orientation.
2. Align the hinge mounting points.
3. Install the hinge components and associated fasteners.
5. Manually rotate the frames to inspect their available range of movement.

### 4.2 Step 02 — Installing the Stepper Motors

The mechanical system uses four 28BYJ-48 5V stepper motors.

The firmware distinguishes between the motors responsible for the main movement of the mechanism and the motor responsible for the locking and unlocking sequence.

Pictures of the mounted motors can be seen below

<p align="center">
 <img width="4032" height="3024" alt="IMG_8593" src="https://github.com/user-attachments/assets/6b2de9eb-e418-4fa0-9813-4a7ddac2189f" />
</p>

<p align="center">
 <img width="3024" height="4032" alt="IMG_8601" src="https://github.com/user-attachments/assets/053e28a2-b243-4c9b-8912-9148815f24e0" />

</p>

**Assembly procedure:**

1. Identify the motor mounting positions.
2. Position the stepper motors within the supporting structure.
4. Check that the motor output shafts can rotate freely.
6. Route the motor cables towards the electronic control system.

**Motor configuration:**

- **Motors 1, 2 and 3:** Coordinated mechanical movement.
- **Motor 0:** Locking and unlocking function during the opening and closing sequence.

The motors are controlled through the Arduino firmware, which coordinates their movement during calibration, opening, listening, breathing exercises and closing.

### 4.3 Step 03, Attaching the Bobbins

Bobbins were used to convert the rotational movement of the stepper motors into linear displacement of the strings. As the motor shaft rotates, the string winds around or unwinds from the bobbin, changing the effective string length. This allows the motors to control the position of the circular frames through tension.

**Assembly procedure:**

**Fabrication process:**

1. Prepare the housing geometries in CAD for the bobbins
2. Export the models for 3D printing.
3. Print using PLA.
2. Attach the bobbins to the corresponding motor output shafts.
3. Check that each bobbin is securely coupled to its motor.
4. Rotate the bobbins manually to verify their alignment.

### 4.4 Step 04, Installing the Strings

The string system transfers mechanical movement from the motors to the circular frames. By winding and unwinding the strings, the mechanism changes the position and orientation of the frames.

**Assembly procedure:**

1. Attach the strings to their corresponding bobbins.
2. Route each string through its intended path within the supporting structure.
3. Connect the opposite ends of the strings to the designated attachment points on the moving assembly.
4. Adjust the initial string lengths.
6. Manually test the winding and unwinding action.

**Design considerations:**

String tension is an important factor in the operation of the mechanism.

Insufficient tension may result in slack and delayed movement, while excessive tension may increase the load on the motors and mechanical joints. The string paths must also be arranged to avoid interference with the hinges, circular frames and other structural components.

### 4.5 Step 05, Integrating the Mechanism into the Housing

Following the assembly of the circular frames, hinges, motors and strings, the moving mechanism is integrated into the supporting structure.

**Assembly procedure:**

1. Position the connected circular frames within the structural assembly.
3. Check the clearance between the moving frames and the surrounding structure.
6. Secure the remaining structural connections.

The assembled mechanism should be able to transition between its contracted and expanded configurations without unintended collisions.

---

## 5. Acrylic Supporting Structure

In addition to the 3D-printed components, an acrylic supporting structure was fabricated using laser cutting and heat bending.

The acrylic was shaped to create the required curved structural geometry and provide support for the overall assembly. The process involved preparing the acrylic geometry, cutting the material and applying heat to form the desired curvature. In order to mold the acrylic, a metal piece was create. This mold provided a rigid, heat-resistant form around which the acrylic could be shaped.

Pictures from mold making are outlined below and can also be found here [Acrylic Heat-Bending and Fabrication](Acrylic-Heat-Bending.md)

<p align="center">
 <img width="3024" height="4032" alt="IMG_8575" src="https://github.com/user-attachments/assets/84d0b690-8fab-49e0-8fe2-ca7ce963e902" />
</p>

<p align="center">
 <img width="3024" height="4032" alt="IMG_8584" src="https://github.com/user-attachments/assets/07d843d9-1b3d-4e8a-84e9-8dd4d18a7193" />
</p>

A picture from heat bending can be below and can also be found here [Acrylic Heat-Bending and Fabrication](Acrylic-Heat-Bending.md)

<p align="center">
 <img width="4032" height="3024" alt="IMG_8587" src="https://github.com/user-attachments/assets/91f81844-a9ea-4226-a5d7-bd40ebfc89bf" />
</p>

**Assembly procedure:**

1. Creation of Illustrator File
2. Lazer Cutting 2mm acrylic sheet
3. Metal mold fabrication using a 

The acrylic fabrication process is documented separately to distinguish material processing from mechanical assembly.
**Related documentation:** [Acrylic Heat-Bending and Fabrication](Acrylic-Heat-Bending.md)

---
## 6. Textile Integratio

Fabric was hand-sewn onto the upper section of the mechanism. This introduced a textile element into the assembly alongside the rigid 3D-printed and acrylic components, due to the upper element being printed in two separate pieces which diminishes aesthetic value. 

---
## 7. Soap Solution Preparation

A mixture of soap and water was added to the lower component of the installation. The lower housing serves as the reservoir for the liquid solution used to generate the soap membrane.

The circular frames interact with the solution to create a suspended film that can subsequently be manipulated through mechanical movement.

A picture of the soap solution in the correct placement can be seen below

<p align="center">
 <img width="3853" height="2793" alt="IMG_8562" src="https://github.com/user-attachments/assets/de0d8c43-3818-470e-b9d7-81472fbb51da" />
</p>

---

## 8. Electronics Integration

The mechanical assembly operates through an Arduino-controlled system.

The electronic components include:

- Arduino microcontroller
- Four 28BYJ-48 5V stepper motors
- Stepper motor drivers
- WS2812B addressable LED ring
- Breathing sensors
- 5V power supply
- Connecting wires and prototyping boards

The motors provide the mechanical movement, while the sensors and firmware determine the timing of the actuation.

The LED system provides visual feedback during the interaction.

### 8.1 Firmware-Controlled Movement

The firmware is organized into five primary operational phases:

| Phase | Mechanical / System Behavior |
|---|---|
| Calibration | Establishes the reference positions of the motors |
| Opening | Moves the mechanism into its expanded configuration |
| Listening | Captures the participant's breathing rhythm |
| Exercise | Coordinates mechanical movement with breathing feedback |
| Closing | Returns the mechanism to its contracted configuration |

During the opening sequence, motors 1, 2 and 3 move towards their designated positions, followed by motor 0 performing the locking action.

During closing, motor 0 unlocks the mechanism before motors 1, 2 and 3 return to their initial positions.

The firmware also coordinates the mechanical movement with sensor data and the LED system.

### 8.2 Firmware Architecture

The diagram below illustrates the firmware structure, including the operational phases, motor functions, sensor acquisition and communication with Processing and Wekinator.

---

## 9. Assembly Testing and Calibration

Following mechanical assembly, the system requires testing to verify that the motors, strings and circular frames operate together.

### 9.1 Motor Calibration

The firmware includes a calibration phase to establish the motor reference positions.

This allows the system to coordinate the movement of the individual motors during subsequent phases.

Calibration should be completed before evaluating the full opening and closing sequence.

### 9.2 Opening and Closing Tests

The complete mechanical sequence should be evaluated by observing:

1. The transition from the contracted to the expanded configuration.
2. The coordination of the three main movement motors.
5. The return to the initial contracted configuration.

### 9.4 Breathing-Responsive Movement

The final stage of testing concerns the relationship between sensor input and mechanical movement.

The mechanism is intended to respond to the participant's respiratory rhythm, initially mirroring their breathing before introducing a guided pace.

The system should be evaluated for:

- Responsiveness to breathing input.
- Smoothness of mechanical movement.
- Consistency between breathing signals and frame displacement.
- Repeatability across multiple breathing cycles.
- Stability of the soap membrane during expansion and contraction.

---

## 10. Reflection

The assembly of Respira brings together additive manufacturing, mechanical joints, string-driven actuation and electronic control.

A central challenge is translating the rotational motion of several stepper motors into a coordinated physical movement that can represent the rhythm of human breathing.The string-and-bobbin system offers a way to transfer movement between the motors and the circular frames without requiring a direct rigid mechanical linkage across the entire structure. However, this approach introduces additional considerations relating to string tension, alignment, friction and motor synchronization.

The project demonstrates the importance of considering the mechanical, electronic and material systems together.

---

## 11. Related Documentation

- [Bill of Materials](Bills_of_Materials_BOM.md)
- [Acrylic Heat-Bending and Fabrication](Acrylic-Heat-Bending.md)
- [Iris Diaphragm Development](Iris-Mechanism-Motorized-Diaphragm_test.md)
