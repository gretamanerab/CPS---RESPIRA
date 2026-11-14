// =====================================================================
// WEKINATOR MOUSE TEST  :  PROCESSING SKETCH
// =====================================================================
// Tests the whole chain with the mouse instead of the breathing sensors:
//
//   mouse position  --OSC 6448, /wek/inputs-->  Wekinator  --OSC 12000, /wek/outputs-->  this sketch  --USB-->  Arduino LED ring
//
// The window has 3 areas: 1 (left, red), 2 (middle, blue), 3 (right, green).
// The mouse position (2 numbers, x and y from 0 to 1) is sent to Wekinator 20 times a second.
// Wekinator's answer (1, 2 or 3) is sent to the Arduino, which colours the ring: 1 red, 2 blue, 3 green.
//
// HOW TO USE
//   1. Upload LedRingClassTest to the Arduino. Close the Serial Monitor and the other Processing sketches.
//   2. In Wekinator make a NEW project (not the breathing one):  2 inputs, port 6448,
//      1 output, classifier with 3 classes, sending to port 12000.
//   3. Press Run here. In Wekinator choose class 1, hold the mouse in area 1 and press Start Recording for a few
//      seconds, then do the same for class 2 in area 2 and class 3 in area 3. Press Train, then Run.
//   4. Move the mouse between the areas: Wekinator's answer and the ring should follow.
//
// KEY  d  switches DIRECT mode on and off: the area under the mouse goes straight to the Arduino, without Wekinator.
//         Useful to test only Processing -> Arduino.
//
// Library needed: oscP5.
// =====================================================================

import processing.serial.*;                                       // Processing's own tool for the USB connection.
import oscP5.*;                                                   // The oscP5 library speaks OSC, the language Wekinator uses.
import netP5.*;                                                   // A helper that comes with oscP5; it holds network addresses.


// ----- SETTINGS -----

String PORT_NAME           = "COM6";                              // The Arduino's port (Arduino IDE > Tools > Port). The console lists all ports at start.
int    BAUD_RATE           = 115200;                              // The USB speed, the same as the Arduino sketches.
String WEKINATOR_IP        = "127.0.0.1";                         // Wekinator runs on this same computer.
int    WEKINATOR_PORT      = 6448;                                // Wekinator listens for inputs on this port.
int    LISTEN_PORT         = 12000;                               // This sketch listens for Wekinator's answers on this port.
int    SEND_INPUTS_EVERY_MS = 50;                                 // The mouse position goes to Wekinator every 50 ms (20 times a second, like BreathDevice).
int    SEND_CLASS_EVERY_MS = 500;                                 // The number goes to the Arduino every 500 ms (twice a second, like the bridge).
int    CLASS_FRESH_MS      = 2000;                                // Only a Wekinator answer younger than 2 s is sent.
int    LOG_LINES           = 5;                                   // How many Arduino messages the window shows.


// ----- CONNECTIONS -----

Serial     arduino;                                               // The USB connection to the Arduino (null if the port could not be opened).
OscP5      osc;                                                   // The OSC connection; it listens on LISTEN_PORT.
NetAddress wekinator;                                             // Where Wekinator is: address and port.


// ----- WHAT IS GOING ON -----

boolean  directMode     = false;                                  // false = the Arduino gets Wekinator's answer, true = it gets the area under the mouse directly.
int      wekClass       = 0;                                      // Wekinator's newest answer: 0 = none yet, 1, 2 or 3.
int      lastAnswerTime = -100000;                                // When that answer arrived.
int      lastInputsSent = 0;                                      // When the mouse position was last sent to Wekinator.
int      inputsSent     = 0;                                      // How many times the mouse position was sent.
int      lastClassSent  = 0;                                      // When a number was last sent to the Arduino.
int      sentToArduino  = 0;                                      // The last number sent to the Arduino.
String[] log            = new String[LOG_LINES];                  // The last messages from the Arduino.


// =====================================================================
// setup()  runs once
// =====================================================================

void setup() {                                                    // Processing runs this once at the start.
  size(900, 500);                                                 // A window 900 pixels wide and 500 tall: three areas of 300 pixels.
  textSize(16);                                                   // The text is 16 pixels tall.
  for (int i = 0; i < LOG_LINES; i++) log[i] = "";                // The message list starts empty.

  osc = new OscP5(this, LISTEN_PORT);                             // The OSC connection starts listening on port 12000.
  wekinator = new NetAddress(WEKINATOR_IP, WEKINATOR_PORT);       // Wekinator's address is remembered.

  println("Serial ports on this computer:");                      // A title in the console...
  printArray(Serial.list());                                      // ...and every port, with its number.
  try {                                                           // Opening the port can fail...
    arduino = new Serial(this, PORT_NAME, BAUD_RATE);             // ...so it is tried here.
    arduino.bufferUntil('\n');                                    // serialEvent() runs every time a whole line has arrived.
    addLog("connected to the Arduino on " + PORT_NAME);           // Success is shown in the window.
  } catch (Exception e) {                                         // If it failed...
    addLog("could not open " + PORT_NAME + " - close the Serial Monitor, check PORT_NAME");   // ...what to do is shown.
  }                                                               // End of opening the port.
}                                                                 // End of setup.


// =====================================================================
// draw()  runs 60 times a second
// =====================================================================

void draw() {                                                     // Processing runs this again and again.
  sendMouseToWekinator();                                         // The mouse position goes to Wekinator (every 50 ms).
  sendNumberToArduino();                                          // The number goes to the Arduino (every 500 ms).
  drawScreen();                                                   // The window is drawn.
}                                                                 // End of draw.


// =====================================================================
// THE MOUSE
// =====================================================================

int areaUnderMouse() {                                            // This function answers: which area is the mouse in? 1, 2 or 3.
  int area = mouseX / (width / 3) + 1;                            // Each area is a third of the width: left = 1, middle = 2, right = 3.
  return constrain(area, 1, 3);                                   // The answer is kept between 1 and 3 (also at the very right edge).
}                                                                 // End of areaUnderMouse.

void sendMouseToWekinator() {                                     // This function sends the mouse position to Wekinator.
  if (millis() - lastInputsSent < SEND_INPUTS_EVERY_MS) return;   // It only runs every 50 ms.
  lastInputsSent = millis();                                      // The time of this send is remembered.
  OscMessage message = new OscMessage("/wek/inputs");             // A new message, with the address Wekinator expects.
  message.add((float) mouseX / width);                            // Input 1: the mouse's left-right position, from 0 to 1.
  message.add((float) mouseY / height);                           // Input 2: the mouse's up-down position, from 0 to 1.
  osc.send(message, wekinator);                                   // The message goes to Wekinator.
  inputsSent++;                                                   // One more message was sent.
}                                                                 // End of sendMouseToWekinator.


// =====================================================================
// WEKINATOR -> this sketch -> ARDUINO
// =====================================================================

void oscEvent(OscMessage incoming) {                              // oscP5 runs this by itself when a message arrives from Wekinator.
  if (incoming.checkAddrPattern("/wek/outputs") == false) return; // Only Wekinator's answers are used.
  int newClass = round(incoming.get(0).floatValue());             // The answer arrives as a decimal number (like 2.0) and is rounded.
  if (newClass < 1 || newClass > 3) return;                       // Only 1, 2 or 3 is used.
  wekClass = newClass;                                            // The answer is kept...
  lastAnswerTime = millis();                                      // ...together with the time it arrived.
}                                                                 // End of oscEvent.

void sendNumberToArduino() {                                      // This function sends 1, 2 or 3 to the Arduino, the same way the bridge does.
  if (arduino == null) return;                                    // Without an Arduino there is nobody to send to.
  if (millis() - lastClassSent < SEND_CLASS_EVERY_MS) return;     // It only runs every 500 ms.
  lastClassSent = millis();                                       // The time of this send is remembered.
  int number = 0;                                                 // 0 = nothing to show (the ring goes off).
  if (directMode) number = areaUnderMouse();                      // In DIRECT mode the area under the mouse is sent...
  else if (millis() - lastAnswerTime < CLASS_FRESH_MS) number = wekClass;   // ...otherwise Wekinator's answer, if it is fresh.
  arduino.write(number + "\n");                                   // The number goes to the Arduino as one digit and a new line, for example "2".
  sentToArduino = number;                                         // It is kept for the screen.
}                                                                 // End of sendNumberToArduino.


// =====================================================================
// ARDUINO -> this sketch
// =====================================================================

void serialEvent(Serial port) {                                   // Processing runs this by itself when a whole line arrived from the Arduino.
  String line = port.readStringUntil('\n');                       // The whole line is read.
  if (line == null) return;                                       // If nothing is there, the function stops.
  line = trim(line);                                              // Invisible characters at the start and end are removed.
  if (line.length() > 0) addLog("ARDUINO  " + line);              // The message is shown in the window (for example ">> CLASS 2 BLUE").
}                                                                 // End of serialEvent.


// =====================================================================
// KEYS
// =====================================================================

void keyPressed() {                                               // Processing runs this by itself when a key is pressed in this window.
  if (key == 'd' || key == 'D') {                                 // The d key...
    directMode = !directMode;                                     // ...switches DIRECT mode on or off.
    addLog(directMode ? "DIRECT mode: the area goes straight to the Arduino" : "WEKINATOR mode: Wekinator's answer goes to the Arduino");   // The new mode is shown.
  }                                                               // End of the d key.
}                                                                 // End of keyPressed.


// =====================================================================
// THE WINDOW
// =====================================================================

void addLog(String text) {                                        // This function adds one line to the messages in the window.
  for (int i = 0; i < LOG_LINES - 1; i++) log[i] = log[i + 1];    // Every line moves up by one (the oldest disappears)...
  log[LOG_LINES - 1] = text;                                      // ...and the new line goes at the bottom.
}                                                                 // End of addLog.

void drawScreen() {                                               // This function draws everything in the window.
  int w = width / 3;                                              // The width of one area.
  int area = areaUnderMouse();                                    // The area the mouse is in.
  color[] colours = { color(200, 40, 40), color(40, 80, 220), color(40, 170, 70) };   // The same colours as the ring: 1 red, 2 blue, 3 green.

  noStroke();                                                     // The areas have no outline.
  for (int i = 0; i < 3; i++) {                                   // The three areas are drawn, left to right.
    fill(colours[i], i + 1 == area ? 255 : 70);                   // The area under the mouse is bright, the others are dim.
    rect(i * w, 0, w, height);                                    // One area: a third of the window.
    fill(255);                                                    // White for the big number.
    textSize(90);                                                 // Big text...
    textAlign(CENTER, CENTER);                                    // ...centred...
    text(i + 1, i * w + w / 2, height / 2 - 60);                  // ...the number of the area.
  }                                                               // End of the areas.

  fill(0, 170);                                                   // A dark band at the bottom for the information.
  rect(0, height - 190, width, 190);                              // The band.
  textSize(16);                                                   // Normal text...
  textAlign(LEFT, BASELINE);                                      // ...from the left.
  fill(255);                                                      // White text.
  text("mouse in area " + area + "     mode: " + (directMode ? "DIRECT (key d)" : "WEKINATOR (key d)"), 20, height - 165);   // Where the mouse is and which mode is on.
  String answer = millis() - lastAnswerTime < CLASS_FRESH_MS ? str(wekClass) : "nothing (is Wekinator on Run?)";   // Wekinator's answer, or a hint if there is none.
  text("Wekinator says: " + answer + "     inputs sent: " + inputsSent, 20, height - 140);   // Wekinator's answer and how many positions were sent.
  text(arduino == null ? "Arduino: not connected" : "sent to the Arduino: " + sentToArduino, 20, height - 115);   // What the Arduino got.
  fill(200);                                                      // Light grey for the Arduino messages.
  for (int i = 0; i < LOG_LINES; i++) text(log[i], 20, height - 88 + i * 18);   // The last messages from the Arduino.
}                                                                 // End of drawScreen.
