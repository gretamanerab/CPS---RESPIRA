# Machine learning

This folder holds everything about the breathing classifier: the data, the trained model and how well it works.

## 1. Overview
Wekinator classifies a visitor's breathing into 3 classes so the bubble knows which corrective pattern to play.

| Class | Meaning |
|---|---|
| 1 | Too fast |
| 2 | Too slow |
| 3 | Erratic (stressed) |

## 2. Folder structure
| Item | Contents |
|---|---|
| `wekinator-project/` | Saved Wekinator project folder: trained kNN model, its configuration and all recorded examples (open the .wekproj file) |
| `[data].csv` | All 4987 training examples exported from Wekinator (7 inputs + class) |

## 3. Inputs (features)
The Nano calculates 7 features and Processing sends them to Wekinator (OSC port 6448, /wek/inputs):
1. **bpm**: breaths per minute
2. **irregular**: how uneven the breath timing is
3. **depth**: size of each breath
4. **ratio**: inhale vs exhale length
5. **chest**: chest movement (ultrasonic sensor)
6. **belly**: belly movement (ultrasonic sensor)
7. **bellyShare**: how much of the breath comes from the belly

Wekinator sends the predicted class back to Processing on port 12000.

## 4. Training data
Recorded by: Greta Manera Bertizzolo only (single person, demo version)

Total examples: 4987 across 16 recordings

Per class: [7] too fast, [4] too slow, [3] erratic

How: I breathed on purpose in each style for around [30] seconds while recording, with the thermistors under the nose and over the mouth and the ultrasonic sensors pointed at chest and belly.

## 5. Processed datasets
No separate processed dataset: the Arduino Nano already turns the raw sensor signals into the 7 features before they reach Wekinator, so the .csv file is the data the model was trained on.

## 6. Model configuration
Type: Classifier, 1 output, 3 classes

Algorithm: k Nearest Neighbour (k = [1])

Why: we first tried Dynamic Time Warping on the raw signals, but it was too slow for live use. A classifier on 7 summary features reacts quickly enough for the installation.

## 7. Limitations and next steps
1. Trained on one person only, so the model has learned my breathing, not breathing in general.
2. Next step: record 5 to 10 people of different body types and ages, each breathing in all 3 styles.
3. Test on people who were not in the training data, to see if it works on a new visitor at the installation.

Other ideas: add a 4th "calm" class so the bubble does not correct someone who already breathes well; the thermistors react slowly to very fast breathing.

## 8. How to reproduce
1. Upload the Nano sketch from `firmware/` to the Arduino Nano.
2. Run the Processing sketch from `processing/`.
3. Open the .wekproj in `wekinator-project/` with Wekinator and press Run.

