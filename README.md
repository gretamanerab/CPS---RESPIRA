# CPS-RESPIRA ADD LINKS TO FOLDERS AND EXPLAIN WHATS INSIDE

Cyber Physical Systems 2026/27

An interactive breathing installation mediated by machine learning

<p align="center">
<img width="1774" height="887" alt="b450ae89-d12b-4606-b47b-9cfa1736ba06" src="https://github.com/user-attachments/assets/c5c660ae-0b7d-4db3-932c-b5157c072c5e" />
</p>

Team members: Dario Cosenza, Greta Manera Bertizzolo, Selma Danesi-Vold.    
Imperial College London &amp; Royal College of Art, Innovation Design Engineering

## Project Overview  
We present "Respira", an interactive installation in which real soap bubbles breathe with visitors. The aim is to gradually lead users to about 6 breaths per minute with longer exhales, a breathing pace associated with changes in cardiovascular regulation and heart rate variability.(Russo et al., 2017).    
Through this gradual transition from mirroring to pacing,"Respira" investigates whether a responsive, non-verbal interaction can support self-regulation and create a moment of calm. The project explores the relationship between breathing, stress regulation, and human-machine synchronization.


## Repository Structure

This repository documents the research, design, fabrication, electronics, software development, and testing of RESPIRA. The project is organized into the following directories:

### Documentation

**[docs/](docs/)** — Project documentation covering the design process, mechanical assembly, fabrication techniques, and conceptual development.

- [Mechanical Assembly](docs/Mechanical-Assembly.md) — Assembly and construction of the physical mechanism.
- [Acrylic Heat Bending](docs/Acrylic-Heat-Bending.md) — Fabrication of the acrylic support structure using laser cutting and heat bending.
- [Hand Sketches](docs/Hand-Sketches.md) — Early design explorations and concept sketches.
- [Research](docs/Research.md) — Background research informing the design and development of RESPIRA.
- [Research](docs/Material-Renders.md) — Renders of final product with different materials based on CAD files

### Mechanical Design

**[mechanical/](mechanical/)** — CAD models, technical drawings, and digital fabrication files used to manufacture the physical components.

- [3D CAD Model](mechanical/Model/BreathDevice.step) — STEP model of the breathing device.
- [Technical Drawings](mechanical/Technical%20Drawings/) — Individual component drawings in PDF format, including the base, bobbin, cable holders, and mechanical parts.
- [Laser Cutting](mechanical/Laser%20Cutting/) — Vector files for laser-cutting components.
- [Acrylic Stand](mechanical/Laser%20Cutting/acrylic_stand.ai) — Adobe Illustrator file used to laser-cut the acrylic support structure.

### Hardware and Electronics

**[hardware/](hardware/)** — Electronic components, sensor configurations, wiring documentation, and hardware testing.

- [Bill of Materials](hardware/Bills_of_Materials_BOM.md) — Components and materials used in the project.
- [Sensor Setup](hardware/sensor-setup/sensor-setup.md) — Sensor configurations, mounting, and photographs.
- [Thermistor Wiring](hardware/thermistor-wiring_test.md) — Wiring and testing of the breathing thermistors.
- [Ultrasonic Sensor Wiring](hardware/ultrasonic-wiring_test.md) — Wiring and testing of the ultrasonic sensors.
- [LED Ring Wiring](hardware/led-ring-wiring_test.md) — LED ring connections and test documentation.
- [Arduino Pin Reference](hardware/arduino_pins.jpeg) — Arduino pinout reference image.

### Firmware

**[firmware/](firmware/)** — Arduino firmware and supporting documentation for the integrated device.

- [BreathDevice.ino](firmware/BreathDevice.ino) — Arduino sketch for the breathing device.
- [Firmware Map](firmware/firmware-map.png) — Firmware architecture diagram.

### Processing

**[processing/](processing/)** — Processing sketches for communication between Arduino and the machine learning system.

- [Final Processing Code](processing/final_code/WekinatorProcessingArduino.pde) — Processing sketch for the integrated system.
- [Training Code](processing/training/wekinator_training_processing.pde) — Processing sketch used during machine learning development.

### Machine Learning

**[ml/](ml/)** — Wekinator project files, model configuration, and recorded data.

- [Wekinator Project](ml/WekiRespira/current/WekiRespira.wekproj) — Wekinator project configuration.
- [Trained Model](ml/WekiRespira/current/models/model0.xml) — Saved machine learning model.
- [Training Data](ml/WekiRespira/current/data.csv) — Recorded dataset.
- [Machine Learning Documentation](ml/WekiRespira/current/ml_info.md) — Information about the machine learning implementation.

### Testing and Prototyping

**[tests/](tests/)** — Standalone experiments and component tests conducted during development.

- [Arduino and Processing Communication](tests/COMarduinoProcessing/) — Serial communication tests.
- [LED Ring Tests](tests/LedsTest/) — LED ring functionality and rainbow animation tests.
- [Stepper Motor Tests](tests/StepperTest/) — Stepper motor functionality tests.
- [Thermistor Tests](tests/test_thermistor/) — Thermistor sensor tests.
- [Ultrasonic Sensor Tests](tests/test_ultrasonic/) — Ultrasonic distance sensor tests.

### Archived Experiments

**[archive/](archive/)** — Earlier mechanical concepts and experiments retained as part of the design development process.

- [Motorized Iris Diaphragm Test](archive/Motorized-Iris-Diaphragm_test.md) — Documentation of an earlier iris mechanism experiment.
- [Iris Acrylic File 1](archive/iris-diaphragm-acrylic-file1.svg) — Laser-cutting geometry.
- [Iris Acrylic File 2](archive/iris-diaphragm-acrylic-file2.svg) — Additional laser-cutting geometry.

### Media

**[media/](media/)** — Project photographs, visual documentation, and video material.

- [Project Hero Image](media/hero-shot.jpg) — Photograph of the completed installation.


---

## 01. Intent and Objectives

Breathing is one of the fundamental mechanisms of the human body.

In 2018, 74% of UK adults surveyed by the Mental Health Foundation reported having felt so stressed in the preceding year that they were overwhelmed or unable to cope (Mental Health Foundation, 2018). Stress is a natural physiological response, but the demands of modern life can trigger stress extending to reactions to inaccurate threats or the absence of threats. When prolonged or repeatedly activated, stress can negatively affect both well-being and overall quality of life. Our project's purpose is to combat this through re-establishing healthier breathing patterns.

Faced with stress, the human body activates its sympathetic nervous system, releasing adrenaline, leading to a faster heartbeat and faster breathing. (Russo et al., 2017) Specifically, breathing shortens in frequency and decreases in depth. In turn, these physiological changes reinforce the experience of stress, creating a feedback loop between the psychological state of stress and the bodily experience of stress. In this way, breathing can be regarded as both a signal of stress and a tool against it- increasing physiological stress (Masaoka and Homma) and lowering adrenaline release (Zaccaro et al. 2018; Balban et al. 2023). “Respira” aims to make breathing visible and oﬀer a slower alternative rhythm. We aim to create an installation that firstly synchronises bubble formation with each visitor’s breathing pattern. This offers an externally grounded visual repertoire. Unlike many physiological processes, breathing occupies a unique position between the autonomic and voluntary nervous systems. Although it occurs largely unconsciously, it can also be consciously regulated. Thus, the bubble formation gradually slows, encouraging visitors to follow its pace. Our objective is to lead visitors to 6 breaths per minute, as this has been described as lowering adrenaline release (Zaccaro et al. 2018; Balban et al. 2023). This project utilizes breathing as an accessible means of influencing physiological arousal and supporting the body's transition towards a more relaxed state.

Listed Objectives:
1. Make breathing visible through a physical bubble
2. Model Performance: Classification performance using recordings held back from training. Explore machine learning as a mediator between physiological input and mechanical output
3. Breathing Guidance: Investigating stress regulation through breathing interventions
4. User Experience: Investigate if a non-verbal interaction with bubbles is intuitive to visitors. Self- reported calmness before and after interaction with Respira.

## 02. System Architecture

The user experience is divided into three stages:
1. Notice  
Visitors begin by putting breathing sensors on. As visitors first engage, they notice that the project expands as a function of their own breathing, leading to a bubble formation copying their own breathing. This could make abnormal breathing, such as one aﬀected by stress, evident, without using screens or numbers.
3. Mirror  
Moving exactly with them, the bubble becomes their breath. “Respira” first establishes a sense of correspondence, allowing the visitor and the installation to move together.
5. Guide  
Once synchronisation is established, the bubbles gradually slow their rhythm, inviting visitors to follow. The installation gradually adjusts towards approximately six breaths per minute. The pace decreases incrementally over several breathing cycles, guiding the visitor towards slower breathing without abrupt changes. If they lose the rhythm, it waits.

<p align="center">
  <img
    src="https://github.com/user-attachments/assets/39026a5c-734e-4baa-a5da-608e88a69c74"
    alt="Respira user experience diagram"
    width="300"
  >
</p>

Respira connects physiological sensing, machine learning, and mechanical movement through a real-time software pipeline. The system architecture processes visitors through the following steps:
1. Breathing input (airflow sensor)
2. Arduino
3. Signal processing
4. Machine learning (Wekinator)
5. State transition processing (Idle / Mirror / Guide / Release)
6. Arduino
7. Servo mechanical movement
8. Physical bubble output

<p align="center">
<img width="2894" height="194" alt="mermaid-diagram copy" src="https://github.com/user-attachments/assets/afbae2d6-e5a3-4c79-b5ca-ddbff9c2bee0" />
</p>

####   02. 1 Primary Input: Oral Airflow

Visitors breathe through a disposable, custom-designed interface. A differential pressure sensor measures the pressure difference across the restriction at 50 Hz, providing a signal from which airflow direction, breathing rate, and relative breath depth can be estimated. This mouthpiece-based approach draws on respiratory measurement methods described by Masaoka and Homma (2008).

####   02. 2 Primary Output: Mechanical Movement and Bubble formation

The primary output is a mechanical movement in the mechanism that results in a real soap bubble formation. This movement is mediated by a machine learning algorithm. The mechanism consists of three individual rings connected in an arrangement that allows them to move relative to one another. A servo motor mounted at the side of the assembly drives this movement, creating an angular displacement between the rings. As the rings move apart, the soap film is stretched across the changing geometry, enabling the formation of a bubble. The servo motor controls the movement of the rings, translating the system's output signals into physical motion. This allows the mechanical output to respond to the visitor's breathing pattern.

## 03. Conceptual Context

"Respira" explores how an internal human rhythm can be mediated by a machine. Rather than presenting breathing through numerical data or a screen, Respira makes an otherwise invisible bodily process tangible through the expansion and contraction of a bubble.

We use soap bubbles as a physical representation of an individual's breathing. The bubbles grow and shrink to match the participant's lungs. Soap bubbles were selected for several key reasons: Embodiment, fragility, and intuitiveness.

A bubble shows clear volumes in one of the most natural configurations: a sphere. In this way, breaths can be illustrated as filling and emptying compared to a lung. Bubbles can be here regarded as an embodiment of air.

Soap bubbles are both ephemeral and delicate. Their usually short existence inspires us to pay attention to and care for them. Moreover, rushed breathing can lead to bubbles bursting, which implies that slow breathing can keep them alive. Coupled with mechanics, we believe bubbles can inspire visitors in an inherently gentle and fragile interaction.

Lastly, one of the pillars of modern design theory discusses how design can be regarded as good if it is intuitive. We believe bubbles are familiar and tangible, and that their movement can be understood without screens and numbers. This allows our interaction to unfold through observation of only a couple of breaths: thus, an intuitive choice.

<p align="center">
  <img width="607" height="405" alt="Respira project image" src="https://github.com/user-attachments/assets/e70be18c-e150-40ff-b537-b2bd3fe55ae8" />
</p>


## 05. Mechanical Design and Fabrication

The mechanical system translates digital control signals into physical movement, enabling the expansion and contraction of soap films in response to breathing patterns.

The development process included concept sketching, CAD modelling, mechanical prototyping, digital fabrication, and physical assembly.

### 05.1 Concept Development

Early concepts explored different geometries and mechanical approaches to forming and manipulating soap bubbles.

- [Hand Sketches](docs/Hand-Sketches.md)
- [Archived Iris Diaphragm Experiment](archive/Motorized-Iris-Diaphragm_test.md)

### 05.2 CAD Modelling

The mechanical components were designed using CAD software and prepared for fabrication.

- [Complete STEP Model](mechanical/Model/BreathDevice.step)
- [Component Technical Drawings](mechanical/Technical%20Drawings/)

### 05.3 Fabrication

The physical structure combines 3D-printed components, a laser-cut and heat-bent acrylic support, mechanical components, and electronics.

The fabrication process included:

1. Preparing housing geometries in CAD.
2. Exporting and 3D printing the upper and lower housing components using PLA.
3. Assembling the printed components with the electronic components.
4. Preparing an Adobe Illustrator file for laser cutting the acrylic support.
5. Fabricating a curved metal form using a sheet metal rolling machine.
6. Heat bending the acrylic around the metal form.
7. Installing string around the bobbin.
8. Completing the electrical wiring.
9. Hand sewing fabric onto the upper mechanism.
10. Adding the soap and water mixture to the lower component.

Detailed documentation:

- [Mechanical Assembly](docs/Mechanical-Assembly.md)
- [Acrylic Heat Bending](docs/Acrylic-Heat-Bending.md)
- [Acrylic Laser-Cutting File](mechanical/Laser%20Cutting/acrylic_stand.ai)



## 06. Hardware

RESPIRA integrates electronic sensing, microcontroller-based processing, motor actuation, and LED illumination.

The hardware documentation includes component specifications, wiring configurations, and individual component tests.

### 06.1 Components

The complete component inventory is documented in the [Bill of Materials](hardware/Bills_of_Materials_BOM.md).

### 06.2 Sensors

- [Sensor Setup](hardware/sensor-setup/sensor-setup.md)
- [Thermistor Wiring](hardware/thermistor-wiring_test.md)
- [Ultrasonic Sensor Wiring](hardware/ultrasonic-wiring_test.md)

### 06.3 Actuation and Lighting

- [Stepper Motor Tests](tests/StepperTest/)
- [LED Ring Wiring](hardware/led-ring-wiring_test.md)
- [LED Ring Tests](tests/LedsTest/)

### 06.4 Electrical Documentation

- [Arduino Pin Reference](hardware/arduino_pins.jpeg)
- [Firmware Architecture](firmware/firmware-map.png)

Electrical schematics and power-budget calculations should be added when verified.

## 07. Software

*need to double check information below*

####   07. 1 Software Pipeline

1. The Arduino reads the incoming sensor measurements.
2. Sensor data is transmitted to Processing through serial communication.
3. Processing filters the incoming signal and calculates relevant breathing features.
4. Processed features are transmitted to Wekinator using OSC.
5. Wekinator classifies the incoming data and returns its output.
6. Processing interprets the model output and determines the appropriate mechanical response.
7. Commands are transmitted to the Arduino, which controls the motors.

####   07. 2 Running the Software 

1. Connect the Arduino to the computer.
2. Open the Arduino sketch in Arduino IDE.
3. Upload the sketch to the Arduino.
4. Open the Processing project.
5. Configure the correct serial port and communication settings.
6. Open the Wekinator project and load the trained model.
7. Start the Processing sketch.
8. Check that the input data, model output and mechanical response are functioning
correctly.


#### 07.3 Source Code

The project uses Arduino, Processing, and Wekinator to connect sensor measurements with mechanical movement.

The source files are organized into the following directories:

- [Firmware](firmware/) — Arduino code for the integrated breathing device.
- [Processing](processing/) — Communication, signal processing, and machine learning integration.
- [Machine Learning](ml/) — Wekinator project configuration, trained model, and data.
- [Tests](tests/) — Individual hardware and communication tests.

**Main project files:**

- [Arduino Firmware](firmware/BreathDevice.ino)
- [Processing Integration](processing/final_code/WekinatorProcessingArduino.pde)
- [Wekinator Project](ml/WekiRespira/current/WekiRespira.wekproj)
- [Trained Model](ml/WekiRespira/current/models/model0.xml)

**Component testing:**

- [LED Ring Test](tests/LedsTest/LedRingTest.ino)
- [LED Rainbow Test](tests/LedsTest/LedRingRainbowTest.ino)
- [Stepper Motor Test](tests/StepperTest/MotorTest.ino)
- [Thermistor Test](tests/test_thermistor/test_thermistors_nano.ino)
- [Ultrasonic Test](tests/test_ultrasonic/test_ultrasonic.ino)


## 08. Machine Learning

Machine learning is used to interpret breathing-related input and determine how the installation
should respond.

####   08. 1 Training Data

The original training approach involved recording breathing sessions from the three team members. The proposed dataset includes:

1. Calm breathing.
2. Moderate breathing.
3. Stressed or accelerated breathing.

####   08. 2 Final Model Configuration 

1. Model type: [Insert]
2. Input features: [Insert]
3. Output classes: [Insert]
4. Number of training examples: [Insert]
5. Training configuration: [Insert]

####   08. 3 Model Evaluation

*[Insert results from testing the trained model.] If possible, include: Confusion matrix, Classification accuracy, Performance on previously unseen recordings, Observations of misclassification or unstable predictions.*

The model's performance is limited by the size and diversity of its training dataset. Training on a small number of participants may limit its ability to generalise to visitors with different breathing patterns. With additional resources, the dataset could be expanded to include more participants, a wider range of breathing patterns and more varied physiological measurements. Additional sensor inputs could also improve the model's ability to distinguish between different breathing behaviors.

Alternative machine learning approaches could be investigated, including more advanced classification models capable of handling temporal patterns in physiological data. However, more complex models would require larger datasets and additional computational resources.

## 09. Safety and Ethics

1. Data.  
Recordings use anonymous IDs; visitor data is processed live and not saved.
2. Health.  
3. Not a medical device. Visitors are seated, the pattern has no breath holds, and a
note tells them to stop if dizzy.
4. Hygiene and safety.
Soap and water are covered and kept away from the electronics,
which run on an external supply.

####   09. 1 MSDS (Material Safety Data Sheet)

The bubble-forming solution consists of ordinary green liquid washing-up soap diluted with water. Water is used as the diluting medium. No specialised chemical reagents are intentionally added to the bubble solution. The SDS identifies potential eye irritation (H319) and recommends avoiding direct contact, using appropriate eye protection and protective gloves when handling the concentrated product. It also recommends storing the detergent in its original, sealed packaging in a cool, dry environment. The diluted soap solution should be handled carefully to prevent splashing and spills. Any spilled solution should be cleaned promptly to prevent slipping hazards.

####   09. 2 RA (Risk Assessment) 

*Hygiene risks associated with the shared breathing interface, Cross-contamination between
users*

## 10. References

## References

Balban, M. Y., et al. “Brief Structured Respiration Practices Enhance Mood and Reduce Physiological Arousal.” *Cell Reports Medicine* 4, no. 1 (2023): 100895. [https://doi.org/10.1016/j.xcrm.2022.100895](https://doi.org/10.1016/j.xcrm.2022.100895).

Bhamla, M. S., C. Chai, M. A. Álvarez-Valenzuela, J. Tajuelo, and G. G. Fuller. “Interfacial Mechanisms for Stability of Surfactant-Laden Films.” *PLOS ONE* 12, no. 5 (2017): e0175753. [https://doi.org/10.1371/journal.pone.0175753](https://doi.org/10.1371/journal.pone.0175753).

“Breathing Metrics for Stress Detection and Guided Breathing.” Background Research and Sensor Comparison. Claude Artifact. [https://claude.ai/artifact/WgDdvaBqwFZTfhzFZooab7](https://claude.ai/artifact/WgDdvaBqwFZTfhzFZooab7).

Cantat, I., S. Pertant, C. Raufaste, and E. Rio. “Capillary Drainage in Horizontal Soap Films: Theoretical Review and Experimental Illustrations.” *Annual Review of Fluid Mechanics* 58 (2026): 111–38. [https://doi.org/10.1146/annurev-fluid-100224-111138](https://doi.org/10.1146/annurev-fluid-100224-111138).

Cohen, C., B. Darbois Texier, E. Reyssat, J. H. Snoeijer, D. Quéré, and C. Clanet. “On the Shape of Giant Soap Bubbles.” *Proceedings of the National Academy of Sciences* 114, no. 10 (2017): 2515–19. [https://doi.org/10.1073/pnas.1612188114](https://doi.org/10.1073/pnas.1612188114).

“Development and Pilot Test of a Virtual Reality Respiratory Biofeedback Approach.” (2020). [https://doi.org/10.1007/s10484-020-09468-x](https://doi.org/10.1007/s10484-020-09468-x).

Fiebrink, Rebecca. *Wekinator*. Software and project website. [https://www.wekinator.org/](https://www.wekinator.org/).

Franco, José Tomás. “Frei Otto Experimenting with Soap Bubbles.” *ArchDaily*, 2015. [https://www.archdaily.com/609541/video-frei-otto-experimenting-with-soap-bubbles](https://www.archdaily.com/609541/video-frei-otto-experimenting-with-soap-bubbles).

Grassmann, M., et al. “Respiratory Changes in Response to Cognitive Load: A Systematic Review.” *Neural Plasticity* 2016 (2016): 8146809. [https://doi.org/10.1155/2016/8146809](https://doi.org/10.1155/2016/8146809).

Homma, I., and Y. Masaoka. “Breathing Rhythms and Emotions.” *Experimental Physiology* 93, no. 9 (2008): 1011–21. [https://doi.org/10.1113/expphysiol.2008.042424](https://doi.org/10.1113/expphysiol.2008.042424).

Kakehi, Y., et al. *Air on Air*. Ars Electronica Festival, 2020.

Kawashima, R., and Y. Kakehi. *Anima – Breathing Life Into a Light*. Material Experience Design Laboratory, University of Tokyo, 2018.

Laborde, S., et al. “Effects of Voluntary Slow Breathing on Heart Rate and Heart Rate Variability: A Systematic Review and a Meta-Analysis.” *Neuroscience & Biobehavioral Reviews* 138 (2022): 104711. [https://doi.org/10.1016/j.neubiorev.2022.104711](https://doi.org/10.1016/j.neubiorev.2022.104711).

Marteau, T. M., and H. Bekker. “The Development of a Six-Item Short Form of the State Scale of the STAI.” *British Journal of Clinical Psychology* 31, no. 3 (1992): 301–6. [https://doi.org/10.1111/j.2044-8260.1992.tb00997.x](https://doi.org/10.1111/j.2044-8260.1992.tb00997.x).

Masaoka, Y., and I. Homma. “Anxiety and Respiratory Patterns: Their Relationship during Mental Stress and Physical Load.” *International Journal of Psychophysiology* 27, no. 2 (1997): 153–59. [https://doi.org/10.1016/S0167-8760(97)00052-4](https://doi.org/10.1016/S0167-8760(97)00052-4).

OpenStax. *University Physics, Volume 3: Interference in Thin Films*. Rice University, 2016. [https://openstax.org/books/university-physics-volume-3/pages/3-4-interference-in-thin-films](https://openstax.org/books/university-physics-volume-3/pages/3-4-interference-in-thin-films).

Pasquet, M., F. Boulogne, F. Restagno, and E. Rio. “Lifetime of Vertical Giant Soap Films: Role of the Relative Humidity and Film Dimensions.” *Soft Matter* 20 (2024): 2374–80. [https://doi.org/10.1039/D3SM01629C](https://doi.org/10.1039/D3SM01629C).

Préve, D., and A. Saa. “Doughnut-Shaped Soap Bubbles.” *Physical Review E* 92 (2015): 042402. [https://doi.org/10.1103/PhysRevE.92.042402](https://doi.org/10.1103/PhysRevE.92.042402).

Pugh, R. J. “Soap Bubbles and Thin Films.” In *Bubble and Foam Chemistry*, 84–111. Cambridge: Cambridge University Press, 2016. [https://doi.org/10.1017/CBO9781316106938.004](https://doi.org/10.1017/CBO9781316106938.004).

Russo, M. A., D. M. Santarelli, and D. O’Rourke. “The Physiological Effects of Slow Breathing in the Healthy Human.” *Breathe* 13, no. 4 (2017): 298–309. [https://doi.org/10.1183/20734735.009817](https://doi.org/10.1183/20734735.009817).

Schmidt, P., et al. “Introducing WESAD, a Multimodal Dataset for Wearable Stress and Affect Detection.” In *Proceedings of the 2018 ACM International Conference on Multimodal Interaction (ICMI 2018)* (2018). [https://doi.org/10.1145/3242969.3242985](https://doi.org/10.1145/3242969.3242985).

Shao, R., I. S. C. Man, and T. M. C. Lee. “The Effect of Slow-Paced Breathing on Cardiovascular and Emotion Functions: A Meta-Analysis and Systematic Review.” *Mindfulness* 15 (2024): 1–18.

Venkat, R. B., et al. “Development and Preliminary Evaluation of a Prototype Audiovisual Biofeedback Device Incorporating a Patient-Specific Guiding Waveform.” *Physics in Medicine & Biology* 53 (2008): N197. [https://doi.org/10.1088/0031-9155/53/11/N01](https://doi.org/10.1088/0031-9155/53/11/N01).

Zaccaro, A., et al. “How Breath-Control Can Change Your Life: A Systematic Review on Psycho-Physiological Correlates of Slow Breathing.” *Frontiers in Human Neuroscience* 12 (2018): 353. [https://doi.org/10.3389/fnhum.2018.00353](https://doi.org/10.3389/fnhum.2018.00353).
