# CPS-RESPIRA
Cyber Physical Systems 2026/27

An interactive breathing installation mediated by machine learning

Team members: Dario Cosenza, Greta Manera Bertizzolo, Selma Danesi-Vold.    
Imperial College London &amp; Royal College of Art, Innovation Design Engineering

## Project Overview  
We present "Respira", an interactive installation in which real soap bubbles breathe with visitors. The aim is to gradually lead users to about 6 breaths per minute with longer exhales, a breathing pace associated with changes in cardiovascular regulation and heart rate variability.(Russo et al., 2017).    
Through this gradual transition from mirroring to pacing,"Respira" investigates whether a responsive, non-verbal interaction can support self-regulation and create a moment of calm. The project explores the relationship between breathing, stress regulation, and human-machine synchronization.

#### *clear photo of the finished installation, this is a placeholder*

---

## Table of Contents
1. [Intent and Objectives](#01-intent-and-objectives)
2. [System Architecture](#02-system-architecture)
3. [Conceptual Context](#03-conceptual-context)
4. [Research](#04-research)
5. [Mechanical Design and Fabrication](#05-mechanical-design-and-fabrication)
6. [Hardware](#06-hardware)
7. [Software](#07-software)
8. [Machine Learning](#08-machine-learning)
9. [Safety and Ethics](#09-safety-and-ethics)
10. [References](#10-references)

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
As visitors first engage, they notice that the bubble formation copies their own breathing. This could make abnormal breathing, such as one aﬀected by stress, evident, without using screens or numbers.
3. Mirror  
Moving exactly with them, the bubble becomes their breath. “Respira” first establishes a sense of correspondence, allowing the visitor and the installation to move together.
5. Guide  
Once synchronisation is established, the bubbles gradually slow their rhythm, inviting visitors to follow. The installation gradually adjusts towards approximately six breaths per minute. The pace decreases incrementally over several breathing cycles, guiding the visitor towards slower breathing without abrupt changes. If they lose the rhythm, it waits.

Respira connects physiological sensing, machine learning, and mechanical movement through a real-time software pipeline. The system architecture processes visitors through the following steps:
1. Breathing input (airflow sensor)
2. Arduino
3. Signal processing
4. Machine learning (Wekinator)
5. State transition processing (Idle / Mirror / Guide / Release)
6. Arduino
7. Servo mechanical movement
8. Physical bubble output

#### *clear diagram here showcasing system architecture*

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

## 04. Research
## 05. Mechanical Design and Fabrication
## 06. Hardware
## 07. Software
## 08. Machine Learning
## 09. Safety and Ethics
## 10. References
