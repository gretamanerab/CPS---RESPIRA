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

####   04. 1 Literary Review: Breathing and human physiological response to stress

Research by Homma and Masaoka (2008) explores the relationship between respiratory rhythms and emotional states, while Russo et al. (2017) review the physiological mechanisms associated with slow breathing. Research suggests that voluntarily slowing breathing can influence cardiovascular regulation, particularly heart rate variability (HRV).

HRV describes variations in the time intervals between consecutive heartbeats and is commonly used to investigate autonomic regulation. Russo et al. (2017) identify approximately six breaths per minute as a particularly relevant breathing frequency for cardiovascular effects. At this rate, respiratory and cardiovascular rhythms interact, potentially increasing heart rate variability.

Similarly, Shao et al. (2024) reviewed 31 studies involving 1,133 participants. Their findings indicated improvements in several cardiovascular measures following slow-paced breathing, although evidence for reductions in negative emotions was less conclusive.

#### Human Breathing patterns and Respira physiological targets

| Feature | Under stress | Goal |
|---|---|---|
| Breathing rate | Often increases | 6 breaths per minute |
| Inhale to exhale ratio | Exhale becomes relatively shorter | Longer exhalation than inhalation (e.g. 1:2) |
| Exhale duration | Often shortened | Extended with a gradual and controlled release |
| Breath-to-breath variability | May become irregular or altered | Consistent and rhythmic breathing |
| Breath depth | Shallower | Controlled |

####   04. 2 Body Mimetics and Biofeedback

Humans are mimetic beings, capable of synchronising body movement to external cues. This can be observed in everyday situations: people walking together may unconsciously adjust their pace to match one another, while listeners often tap their feet or move their bodies in time with music. These examples demonstrate how external rhythms can influence and coordinate human bodily movements to external cues.

Could this mechanism be utilized as a means to facilitate breathing rhythm transitions?

Biofeedback refers to the process of making physiological information available to an individual, allowing them to become more aware of and potentially regulate their bodily responses. Venkat et al. (2008), for example, developed an audiovisual respiratory biofeedback system that translated participants' respiratory waveforms into visual and auditory cues. Their study explored how different forms of feedback, including wave and bar representations, could support respiratory awareness and paced breathing.

Building on these principles, Respira explores the relationship between bodily synchronisation and respiratory biofeedback. Rather than immediately instructing visitors to change their breathing, the installation initially mirrors their existing rhythm before gradually introducing a slower pace. Through this transition from mirroring to pacing, Respira investigates whether an external, responsive stimulus can encourage visitors to adjust their breathing through natural processes of synchronisation, potentially supporting momentary stress regulation.

####   04. 3 Bubble Anatomy and Morphogenesis

A soap bubble is a thin liquid film consisting of a layer of water stabilised by surfactant molecules at its two air–water interfaces. Surfactants, commonly found in soap and detergents, are amphiphilic molecules composed of hydrophilic (water-attracting) heads and hydrophobic (water-repelling) tails.

<p align="center">
<img width="227" height="201" alt="Screenshot 2026-11-09 at 1 11 57 PM" src="https://github.com/user-attachments/assets/88f4be6f-997b-4745-9ab7-f9692270a047" />
</p>

At the air–water interface, these molecules orient themselves with their hydrophilic heads towards the water and their hydrophobic tails towards the surrounding air. This arrangement reduces the surface tension of water, allowing it to stretch into a thin, flexible film capable of enclosing air (Gibbs, Bubble and Foam Chemistry).

<p align="center">
<img width="582" height="307" alt="Screenshot 2026-11-09 at 1 13 36 PM" src="https://github.com/user-attachments/assets/54d06b51-c366-4650-bfac-63ef522146ce" />
</p>

Surface tension originates from the cohesive forces between water molecules. Molecules within the bulk liquid experience approximately balanced intermolecular forces, whereas molecules at the surface experience an imbalance, producing a tendency to minimise the surface area. The addition of surfactants modifies this behavior by reducing surface tension and introducing interfacial forces that help stabilize the film. When the film is locally stretched and its surfactant concentration changes, variations in surface tension generate Marangoni flows, which redistribute liquid and surfactants along the interface. This mechanism helps resist film thinning. and contributes to bubble stability (Interfacial Mechanisms for Stability of Surfactant-Laden Films, 2017; Cantat et al., 2026).

The spherical geometry of a free-floating soap bubble is a consequence of surface-energy minimisation. For a fixed volume of enclosed air, a sphere encloses the maximum volume with the minimum possible surface area. Since surface tension drives the film towards a configuration of lower surface energy, the bubble naturally adopts a spherical form. This makes soap bubbles an example of minimal-surface geometry, in which physical systems organize themselves into configurations that minimize energy (Préve & Saa, 2015; On the Shape of Giant Soap Bubbles, 2017). However, external forces such as gravity, airflow and contact with surrounding surfaces can deform this ideal geometry. A further example of minimal-surface geometry is the catenoid, a surface formed when a soap film spans two parallel, coaxial circular rings. The film adopts an hourglass-like geometry, narrowing towards the centre and widening towards the supporting rings. If the rings are too far apart, the catenoid becomes unstable and snaps into two separate disks. The critical separation for equal rings is approximately 0.663 times the ring diameter. For a spherical bubble, the two radii are equal. Soap bubbles are inherently unstable structures, and their lifetime depends on the physical and environmental conditions surrounding them. Over time, gravity causes liquid to drain towards the lower regions of the film, while evaporation progressively reduces its thickness. Air currents, mechanical contact and local variations in film thickness can further destabilise the structure, eventually causing it to rupture. The stability of a bubble is influenced by the composition of its solution, particularly its surfactant concentration and the presence of additives. Beyond their visual qualities, soap bubbles demonstrate how complex forms can emerge from relatively simple physical principles. Their geometry, movement, color and eventual rupture are not predetermined, but continuously shaped by the interaction between surface tension, gravity, fluid dynamics, light and environmental conditions. In this sense, the bubble can be understood as an example of nature's form-finding processes: a structure that continuously responds to its environment while seeking physical equilibrium.

####   04. 4 Bubble as a Design Informant through Time 

Several interactive art and design projects have explored soap bubbles as a medium for translating otherwise invisible phenomena into physical, temporary forms. Frei Otto was a German architect and structural engineer renowned for pioneering lightweight tensile architecture. His work explored how natural physical processes, such as soap bubbles, could inform the development of efficient architectural structures, minimizing material use while maximizing structural performance (Pritzker Architecture Prize, 2015). By suspending soap films within different wire and string boundaries, Otto observed how surface tension naturally generated minimal surfaces.

<p align="center">
<img width="540" height="393" alt="Screenshot 2026-11-09 at 1 16 55 PM" src="https://github.com/user-attachments/assets/4cabaf84-efd8-402b-9686-493fe3a64930" />
</p>

Two relevant precedents of more contemporary design practises utilizing bubbles are Thom Kubli's Black Hole Horizon (2012–2015) and Yasuaki Kakehi and collaborators' Air on Air (2020).

Thom Kubli, Black Hole Horizon (2012–2015)

<p align="center">
<img width="552" height="370" alt="Screenshot 2026-11-09 at 1 18 33 PM" src="https://github.com/user-attachments/assets/dc06f5d4-f2b0-46cf-964c-930dee23ed32" />
</p>

In Black Hole Horizon, sound is translated into the physical formation of soap bubbles. The bubbles grow in proportion to the duration of the sound, detach from the horns and float through the gallery before eventually bursting. This project demonstrates how an invisible phenomenon can be translated into a tangible, temporary physical form.

Yasuaki Kakehi and collaborators, Air on Air (2020)

<p align="center">
<img width="369" height="241" alt="Screenshot 2026-11-09 at 1 19 33 PM" src="https://github.com/user-attachments/assets/dcaed202-6dc6-427f-848f-8bad4afde62e" />
</p>

Air on Air connects participants in different locations through their breath. Participants blow into a microphone, transmitting their breath as data over the internet. A bubble-generating machine at a distant location then produces bubbles in response to the strength and duration of the transmitted breath. This project demonstrates how breathing can be captured as input and translated into a physical bubble-based output. Respira develops this relationship further by exploring not only the representation of breath, but also the possibility of influencing its rhythm through a responsive interaction.

## 05. Mechanical Design and Fabrication

The mechanical system translates digital control signals into physical movement.

*potentially sketches*
*CAD model needed here*
*potentially failed iris mechanism*
*Techical drawing needed*

## 06. Hardware

Materials used
*this needs to be updated*

## 07. Software
## 08. Machine Learning
## 09. Safety and Ethics
## 10. References
