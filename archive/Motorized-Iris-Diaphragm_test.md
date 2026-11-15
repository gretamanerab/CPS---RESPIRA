
# Motorized Iris Diaphragm, Development & Testing

**Component:** Motorized Iris Diaphragm  
**Status:** Discontinued, not integrated into final prototype  
**Fabrication methods:** Laser cutting and FDM 3D printing  
**Design scale:** Ø 140 mm circular mechanism

---

## 1. Project Context

During the development of Respira, we initially explored the possibility of incorporating a motorized iris diaphragm to regulate airflow into the bubble-generation system. The bubbles are currently created by pulling apart a string-based mechanism, stretching the soap, but does not control the longevity of membrane existence. We wanted to introduce an additional mechanism that would enable oxygen supply to be given or cut off, which is a vital part of bubbles being popped or not.

The proposed solution was a motorized iris diaphragm, inspired by the mechanical aperture systems used in camera lenses. By adjusting the opening of the diaphragm, we aimed to regulate the airflow reaching the bubble membrane and introduce another level of control into the bubble-generation process.

The mechanism would ideally be integrated into the circular frame assembly and controlled by a motor.

## 2. Design Intent

The main objective was to develop a compact, lightweight and motorized aperture mechanism that could open and close reliably.

The design had several requirements:

- **Maximum aperture:** Achieve the largest possible central opening relative to the Ø 140 mm outer diameter. This is directly correlated to the size of the bubbles, so we wanted this as large as possible.
- **Controlled movement:** Allow the aperture to open and close through a single motorized input.
- **Mechanical reliability:** Ensure that all blades moved simultaneously and consistently.
- **Minimal weight:** Reduce the number of components and moving parts. Because the rings are being pulled by our own mechanism, introducing more unnecessary weight could compromise the structural integrity of the product as a whole. 
- **Minimal thickness:** Minimize the number of structural layers to keep the assembly as thin as possible for aesthetic purposes.
- **Integration:** Fit within the circular frame and work alongside the existing string-based membrane system.

We explored several mechanical approaches to achieve the desired movement.

---


## 3. Initial Concepts & Mechanical Exploration

Initial sketches were used to explore the geometry and mechanical arrangement before translating the concepts into digital models for fabrication. A key design question was how many blades to use. We explored different configurations, particularly eight-blade and sixteen-blade arrangements.

Fewer blades offered the possibility of a simpler, lighter assembly with no overlapping components. However, more blades allowed for a larger central opening due to the resting state of blades overlapping, while increasing weight due to assembled items. 

Below some images illustrating the proposed mechanism can be seen:

<p align="center">
<img width="524" height="450" alt="reference 2" src="https://github.com/user-attachments/assets/99776e1d-200f-4365-8f2a-eca7f471ddb8" />
</p>

<p align="center">
<img width="512" height="288" alt="0c0d0855dc725e84498566885cee18b5" src="https://github.com/user-attachments/assets/bd46eed7-21bc-4e46-96b5-d5c7da36e6ff" />
</p>

### Initial Sketches

The following sketches document the early mechanical exploration and conceptual development of the iris mechanism.

<p align="center">
</p>

## 4. Mechanical Development

We developed and fabricated several iterations of the iris mechanism, exploring different blade geometries, actuation methods and gear configurations.

Following the initial sketches, the concepts were developed further using Adobe Illustrator and Fusion 360.

Adobe Illustrator was used to prepare the two-dimensional geometry for laser cutting, while Fusion 360 was used to develop and refine the three-dimensional mechanical assemblies.

These digital models formed the basis for the subsequent fabrication and testing iterations.

The development focused on three main approaches.

### 4.1 Iteration 01 — Laser-Cut Iris with Eight Blades

**Fabrication:** Laser cutting  
**Blade material:** 2 mm sheet material  
**Blade count:** 8  
**Design approach:** Rotational actuation through a unified gear mechanism

The first approach used eight individually laser-cut blades arranged radially around a central aperture.

Each blade was designed to rotate around a fixed pivot point. The intention was to connect the blades through a common rotating mechanism, allowing the entire aperture to open and close through a single rotational input.

The components were designed in Adobe Illustrator and laser-cut from 2 mm sheet material.

**Testing and observed issues:**

Two main problems emerged during testing:

1. **Blade interference and insufficient clearance**

   As the mechanism opened, the blades overlapped. However, the available clearance between the overlapping components was insufficient to allow them to slide and rotate freely. When attempting to close the aperture again, the blades would interfere with one another and become stuck.

2. **Failure of the unified actuation mechanism**

   The unified gear mechanism did not successfully engage all of the individual blade latches. As a result, rotating the central mechanism did not consistently transfer movement to every blade.

   We found that the blades had to be moved individually rather than being actuated together through a single input.

   The intended mechanical coupling therefore did not function as expected.

**Outcome:**

Although the laser-cut components allowed us to explore the eight-blade geometry, the mechanism did not achieve synchronized movement or reliable opening and closing.

The design would have required further investigation into the relationship between the blade pivots, drive-ring geometry, latch positions and clearances.

### 4.2 Iteration 02 — 3D-Printed Iris with Sixteen Blades

**Fabrication:** FDM 3D printing  
**Material:** PLA (Polylactic Acid)
**Blade count:** 16  
**Design approach:** Rotating mechanism with thin interconnected blades

The second approach explored a higher blade count using a 3D-printed iris mechanism.

The intention was to achieve a more continuous aperture by distributing the opening and closing movement across sixteen thinner blades.

Unlike the first iteration, this mechanism was able to rotate and transfer movement to the blade assembly. However, the movement was not consistently distributed across all sixteen blades.

**Testing and observed issues:**

1. **Blade jamming**

   The blades were extremely thin, approximately 0.2 mm, making them difficult to fabricate reliably using FDM 3D printing.

   Some of the printed components failed during fabrication and had to be reprinted multiple times.

2. **Inconsistent blade movement**

   Although the mechanism could rotate, not all sixteen blades moved simultaneously. Some blades would move while others remained stuck or failed to follow the movement.

3. **Mechanical tolerances**

   The small clearances between the thin blades and their surrounding components made the mechanism sensitive to friction, alignment and fabrication inconsistencies.

   Even small variations in the printed geometry could affect the movement of the interconnected components.

**Outcome:**

The sixteen-blade configuration demonstrated that rotational actuation was possible, but the mechanism did not achieve reliable synchronized movement.

The high number of blades, combined with their very small thickness and the fabrication limitations of FDM printing, made this approach difficult to develop further.

### 4.3 Iteration 03 — 3D-Printed Iris with Eight Blades and Top-Driven Actuation

**Fabrication:** FDM 3D printing  
**Blade count:** 8  
**Design approach:** Top-driven blade movement

A further approach was developed by a team member, returning to an eight-blade configuration but using a different actuation principle.

Rather than relying on the same rotational blade arrangement, this design explored a mechanism in which the blades were pushed from above to produce the opening and closing movement.

The aim was to simplify the interaction between the moving components and achieve a more reliable aperture.

**Testing and observed issues:**

The mechanism did not operate as intended.

One suspected issue was the relationship between the blade angles and the physical thickness of the printed components.

The geometric arrangement appeared to create interference when accounting for the actual material thickness, rather than treating the blades as infinitely thin surfaces.

This potentially affected the clearance between adjacent blades and the available range of movement.

However, the exact geometric cause was not isolated during testing.

**Outcome:**

The eight-blade top-driven configuration also failed to achieve reliable opening and closing.

Further geometric adjustments and tolerance testing would have been necessary to establish whether the actuation principle could work.

---

## 5. Testing Overview

Across the development process, we fabricated four physical prototype iterations, exploring different blade counts, geometries and actuation configurations.

The following table summarizes the main approaches and their observed limitations.

| Configuration | Fabrication | Blade count | Main issue |
|---|---|---:|---|
| Laser-cut iris | Laser cutting, 2 mm material | 8 | Blade overlap, insufficient clearance and unsuccessful unified gear coupling |
| Rotating iris | FDM 3D printing | 16 | Extremely thin 0.2 mm blades, inconsistent movement and jamming |
| Top-driven iris | FDM 3D printing | 8 | Suspected geometric interference related to blade angles and material thickness |
| Additional prototype / revision | To be documented | — | Further iteration during mechanical development |

**Photographic documentation**

The photograph below shows several of the fabricated iris components and their different mechanical arrangements.

![Iris mechanism prototype testing](images/iris-mechanism-testing.jpg)

The prototypes were evaluated primarily through physical assembly and manual testing of their opening and closing movements.

The main criteria were:

- Whether all blades could move together.
- Whether the mechanism could achieve a sufficiently large central aperture.
- Whether the blades could return to their original position without jamming.
- Whether the actuation mechanism could transfer movement consistently.
- Whether the assembly could remain lightweight and mechanically compact.

Despite several iterations, none of the tested configurations achieved the level of reliability required for integration into Respira.

---

## 6. Design Decision

Following the prototyping and testing process, we decided to discontinue the motorized iris diaphragm and remove it from the final Respira assembly.

The decision was primarily driven by the available project timeframe and the amount of additional mechanical development required.

Each approach introduced different challenges:

- The laser-cut eight-blade design had difficulties with blade interference and the transfer of movement through the unified gear mechanism.
- The sixteen-blade 3D-printed design suffered from fabrication limitations, extremely thin components and inconsistent blade movement.
- The alternative eight-blade, top-driven design introduced additional geometric challenges related to blade angles and material thickness.

Achieving a reliable mechanism would have required further iterations, potentially including redesigned blade geometries, revised pivot positions, adjusted clearances and a more carefully engineered actuation system.

Given the remaining development time and the need to prioritize the core bubble-generation system, we decided that continuing to refine the iris mechanism was no longer feasible within the project schedule.

The iris was therefore excluded from the final prototype.

---

## 7. Reflection and Technical Learnings

The development of the iris mechanism highlighted the difference between a mechanically plausible concept and a mechanism that can operate reliably once fabricated.

One of the main challenges was coordinating multiple moving components through a single actuation input. Although the individual blades and circular geometries appeared feasible in CAD, their physical interaction introduced problems that were not immediately apparent during the design stage.

Several important considerations emerged:

**1. Mechanical coupling and synchronized movement**

A central rotating mechanism needs to transfer movement consistently to every blade. The geometry and positioning of the individual connections are critical. A unified drive component alone does not guarantee synchronized movement if the individual blade connections are not correctly constrained.

**2. Tolerances and material thickness**

The physical thickness of the blades significantly affects the available clearance and the geometry of their movement. Overlapping blade configurations require careful consideration of component spacing, pivot positions and potential interference throughout the entire opening and closing cycle.

**3. Fabrication constraints**

The sixteen-blade configuration demonstrated the limitations of fabricating extremely thin moving components through FDM 3D printing. At approximately 0.2 mm thickness, the blades were difficult to print consistently and were particularly sensitive to fabrication errors.

**4. Balancing aperture size, complexity and weight**

The exploration also highlighted a design trade-off between the number of blades, the size of the central aperture, mechanical complexity and overall weight. Reducing the number of blades can simplify the assembly, but requires careful consideration of blade geometry and coverage. Increasing the number of blades can produce a more continuous aperture, but also introduces additional moving components and potential friction points.

Ultimately, the exploration was discontinued, but it provided valuable insight into the mechanical constraints of integrating a motorized aperture into the Respira system.

It also reinforced the importance of testing moving assemblies early in the design process, particularly when several components must operate together through a shared mechanical input.

---

## 8. Source Files and Fabrication Documentation

The following files document the design and fabrication process:

- **Adobe Illustrator:** Laser-cut blade and component geometry.
- **Fusion 360:** 3D models of the iris mechanism and its mechanical components.
- **Prototype photographs:** Physical documentation of the different fabricated iterations.

These files provide a record of the mechanical exploration, including the alternative configurations that were tested but ultimately not integrated into the final Respira prototype.
