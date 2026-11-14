// =====================================================================
// WEKINATOR BRIDGE  :  PROCESSING SKETCH
// =====================================================================
// A simple bridge between the Arduino (BreathDevice) and Wekinator, with a small Serial Monitor in the window:
//
//   Arduino  --USB-->  this sketch  --OSC port 6448, /wek/inputs-->   Wekinator   (the 7 numbers)
//   Arduino  <--USB--  this sketch  <--OSC port 12000, /wek/outputs-- Wekinator   (the answer 1, 2 or 3)
//
//   1. DATA lines from the Arduino (exactly 7 numbers, bpm:..,irregular:..,depth:..,ratio:..,chest:..,belly:..,bellyShare:..)
//      go straight to Wekinator, in that order, as its 7 inputs. Lines with a different number of values are ignored,
//      so the machine learning model always gets the same 7 inputs, for training and for running.
//   2. MESSAGE lines from the Arduino (they start with ">>" and have no ":") are shown in the window, like a Serial Monitor.
//   3. Wekinator's answer goes back to the Arduino as one digit, twice a second, while it is fresh.
//   4. Typing in the window works like the Serial Monitor: type (for example  +  -  n  c  or  +++ ) and press Enter.
//
// Wekinator project: CLASSIFIER, 7 inputs, 1 output with 3 classes.
// Library needed: oscP5 (Sketch > Import Library > Manage Libraries > oscP5).
// The Arduino IDE's Serial Monitor must be CLOSED, or this sketch cannot open the port.
// =====================================================================

import processing.serial.*;                                       // Processing's own tool for the USB connection.
import oscP5.*;                                                   // The oscP5 library speaks OSC, the language Wekinator uses.
import netP5.*;                                                   // A helper that comes with oscP5; it holds network addresses.


// ----- SETTINGS -----

String PORT_NAME           = "COM6";                              // The Arduino's port (Arduino IDE > Tools > Port). The console lists all ports at start.
int    BAUD_RATE           = 115200;                              // The USB speed. It must be the same as BAUD_RATE in BreathDevice.
String WEKINATOR_IP        = "127.0.0.1";                         // Wekinator runs on this same computer.
int    WEKINATOR_PORT      = 6448;                                // Wekinator listens for inputs on this port (its default).
int    LISTEN_PORT         = 12000;                               // This sketch listens for Wekinator's answers on this port (Wekinator's default).
int    INPUTS              = 7;                                   // Wekinator's model uses exactly 7 inputs.
int    SEND_CLASS_EVERY_MS = 500;                                 // The answer is sent to the Arduino every 500 ms (twice a second).
int    CLASS_FRESH_MS      = 2000;                                // Only an answer younger than 2 s is sent, so an old visitor's answer never reaches the Arduino.
int    LOG_LINES           = 9;                                   // How many message lines the window shows.


// ----- CONNECTIONS -----

Serial     arduino;                                               // The USB connection to the Arduino. It stays empty (null) if the port could not be opened.
OscP5      osc;                                                   // The OSC connection; it listens on LISTEN_PORT.
NetAddress wekinator;                                             // Where Wekinator is: address and port.


// ----- WHAT THE WINDOW SHOWS -----

String[] values         = new String[0];                          // The 7 "name:number" pieces of the last data line, for the screen.
int      linesSent      = 0;                                      // How many data lines went to Wekinator.
int      linesIgnored   = 0;                                      // How many data lines were ignored because they did not have 7 numbers.
int      currentClass   = 0;                                      // Wekinator's newest answer: 0 = none yet, 1, 2 or 3.
int      lastAnswerTime = -100000;                                // When that answer arrived (a long time ago at the start).
int      lastClassSent  = 0;                                      // When the answer was last sent to the Arduino.
String[] log            = new String[LOG_LINES];                  // The message lines shown in the window (the small Serial Monitor).
String   typed          = "";                                     // What is being typed in the window, before Enter is pressed.


// =====================================================================
// setup()  runs once, when Run is pressed
// =====================================================================

void setup() {                                                    // Processing runs this once at the start.
  size(700, 560);                                                 // A window 700 pixels wide and 560 tall is opened.
  textSize(16);                                                   // The text is 16 pixels tall.
  for (int i = 0; i < LOG_LINES; i++) log[i] = "";                // The message list starts empty.

  osc = new OscP5(this, LISTEN_PORT);                             // The OSC connection starts listening on port 12000.
  wekinator = new NetAddress(WEKINATOR_IP, WEKINATOR_PORT);       // Wekinator's address is remembered.

  println("Serial ports on this computer:");                      // A title in the console...
  printArray(Serial.list());                                      // ...and every port, with its number.

  try {                                                           // Opening the port can fail (wrong name, Serial Monitor still open)...
    arduino = new Serial(this, PORT_NAME, BAUD_RATE);             // ...so it is tried here.
    arduino.bufferUntil('\n');                                    // serialEvent() will run every time a whole line has arrived.
    addLog("connected to the Arduino on " + PORT_NAME);           // Success is shown in the window.
  } catch (Exception e) {                                         // If it failed...
    addLog("could not open " + PORT_NAME + " - close the Serial Monitor, check PORT_NAME");   // ...what to do is shown.
  }                                                               // End of opening the port.
}                                                                 // End of setup.


// =====================================================================
// draw()  runs 60 times a second
// =====================================================================

void draw() {                                                     // Processing runs this again and again.
  sendClassToArduino();                                           // Wekinator's answer is sent to the Arduino (when it is time).
  drawScreen();                                                   // The window is drawn.
}                                                                 // End of draw.


// =====================================================================
// ARDUINO -> this sketch -> WEKINATOR
// =====================================================================

void serialEvent(Serial port) {                                   // Processing runs this by itself when a whole line arrived from the Arduino.
  String line = port.readStringUntil('\n');                       // The whole line is read.
  if (line == null) return;                                       // If nothing is there, the function stops.
  line = trim(line);                                              // Invisible characters at the start and end are removed.
  if (line.length() == 0) return;                                 // An empty line is skipped.

  if (line.indexOf(':') < 0) {                                    // A line without ":" is a message, not data (for example ">> MOTOR 1 adjusted 250 steps")...
    addLog("ARDUINO  " + line);                                   // ...so it is shown in the window...
    println("ARDUINO  " + line);                                  // ...and in the console.
    return;                                                       // Nothing is sent to Wekinator.
  }                                                               // End of messages.

  String[] pairs = split(line, ',');                              // The data line is cut at every comma: "bpm:18.20", "irregular:0.080", ...
  if (pairs.length != INPUTS) {                                   // It must have exactly 7 numbers, the 7 inputs of the model...
    linesIgnored++;                                               // ...otherwise it is counted as ignored...
    return;                                                       // ...and not sent.
  }                                                               // End of the 7-numbers check.

  OscMessage message = new OscMessage("/wek/inputs");             // A new message for Wekinator, with the address it expects.
  for (int i = 0; i < INPUTS; i++) {                              // Each "name:number" piece is looked at, in order.
    String[] parts = split(pairs[i], ':');                        // "bpm:18.20" is cut into "bpm" and "18.20".
    if (parts.length != 2 || Float.isNaN(float(parts[1]))) {      // A broken piece means a broken line...
      linesIgnored++;                                             // ...so it is counted as ignored...
      return;                                                     // ...and not sent.
    }                                                             // End of the check.
    message.add(float(parts[1]));                                 // The number becomes Wekinator's input number i + 1.
  }                                                               // End of the pieces.

  osc.send(message, wekinator);                                   // All 7 numbers go to Wekinator in one message.
  values = pairs;                                                 // The pieces are kept for the screen.
  linesSent++;                                                    // One more line was sent.
}                                                                 // End of serialEvent.


// =====================================================================
// WEKINATOR -> this sketch -> ARDUINO
// =====================================================================

void oscEvent(OscMessage incoming) {                              // oscP5 runs this by itself when a message arrives from Wekinator.
  if (incoming.checkAddrPattern("/wek/outputs") == false) return; // Only Wekinator's answers are used; anything else is ignored.
  int newClass = round(incoming.get(0).floatValue());             // The answer arrives as a decimal number (like 2.0) and is rounded to 1, 2 or 3.
  if (newClass < 1 || newClass > 3) return;                       // Anything else is ignored.
  if (newClass != currentClass) addLog("WEKINATOR  class " + newClass);   // A new answer is shown in the window.
  currentClass = newClass;                                        // The answer is kept...
  lastAnswerTime = millis();                                      // ...together with the time it arrived.
}                                                                 // End of oscEvent.

void sendClassToArduino() {                                       // This function sends Wekinator's answer to the Arduino.
  if (arduino == null) return;                                    // Without an Arduino there is nobody to send to.
  if (millis() - lastClassSent < SEND_CLASS_EVERY_MS) return;     // It is only sent every 500 ms.
  lastClassSent = millis();                                       // The time of this send is remembered.
  if (millis() - lastAnswerTime > CLASS_FRESH_MS) return;         // An answer older than 2 s is not sent (Wekinator only answers while it gets data).
  arduino.write(currentClass + "\n");                             // The answer goes to the Arduino as one digit and a new line, for example "2".
}                                                                 // End of sendClassToArduino.


// =====================================================================
// TYPING IN THE WINDOW  ->  ARDUINO   (like the Serial Monitor)
// =====================================================================

void keyPressed() {                                               // Processing runs this by itself when a key is pressed in this window.
  if (key == ENTER || key == RETURN) {                            // Enter sends what was typed...
    if (typed.length() == 0) return;                              // ...unless nothing was typed.
    if (arduino != null) arduino.write(typed + "\n");             // The text goes to the Arduino, with a new line at the end, like the Serial Monitor.
    addLog("YOU      " + typed);                                  // It is shown in the window...
    typed = "";                                                   // ...and the typing line is cleared.
  } else if (key == BACKSPACE) {                                  // Backspace removes the last character.
    if (typed.length() > 0) typed = typed.substring(0, typed.length() - 1);   // Only if there is one.
  } else if (key != CODED && key >= ' ') {                        // Any normal character (not an arrow or shift key)...
    typed = typed + key;                                          // ...is added to the typing line.
  }                                                               // End of the keys.
}                                                                 // End of keyPressed.


// =====================================================================
// THE WINDOW
// =====================================================================

void addLog(String text) {                                        // This function adds one line to the messages in the window.
  for (int i = 0; i < LOG_LINES - 1; i++) log[i] = log[i + 1];    // Every line moves up by one (the oldest disappears)...
  log[LOG_LINES - 1] = text;                                      // ...and the new line goes at the bottom.
}                                                                 // End of addLog.

void drawScreen() {                                               // This function draws everything in the window.
  background(20);                                                 // An almost black background.
  fill(230);                                                      // Light grey text.

  // ----- top left: the Arduino and the 7 inputs -----
  text(arduino == null ? "ARDUINO: not connected" : "ARDUINO: " + PORT_NAME, 20, 30);   // Whether the Arduino is connected.
  text("to Wekinator: " + linesSent + "   ignored: " + linesIgnored, 20, 55);           // How many lines went to Wekinator, and how many were ignored.
  for (int i = 0; i < values.length; i++) text((i + 1) + "  " + values[i], 20, 85 + i * 22);   // The 7 inputs of the last line, numbered like in Wekinator.

  // ----- top right: Wekinator's answer -----
  text("WEKINATOR SAYS", 380, 30);                                // A title for the answer.
  textSize(28);                                                   // Big text...
  String[] names = { "no answer yet", "1  TOO FAST", "2  TOO SLOW", "3  ERRATIC" };   // ...for the name of the answer.
  text(names[currentClass], 380, 70);                             // The name of the current answer.
  textSize(16);                                                   // Back to normal text.
  if (currentClass > 0) text("last answer " + nf((millis() - lastAnswerTime) / 1000.0, 0, 1) + " s ago", 380, 100);   // How old it is. If it keeps growing, Wekinator is not running.

  // ----- bottom: the small Serial Monitor -----
  stroke(80);                                                     // A grey line...
  line(20, 255, width - 20, 255);                                 // ...separates the Serial Monitor part.
  for (int i = 0; i < LOG_LINES; i++) text(log[i], 20, 280 + i * 24);   // The last messages, oldest at the top.
  fill(255, 210, 80);                                             // Yellow for the typing line.
  text("> " + typed + (frameCount / 30 % 2 == 0 ? "_" : ""), 20, height - 20);   // What is being typed, with a blinking cursor.
  fill(140);                                                      // Grey for the hint.
  text("type  +  -  n  c  (or +++)  and press Enter", 380, height - 20);   // How to use the typing line.
}                                                                 // End of drawScreen.
