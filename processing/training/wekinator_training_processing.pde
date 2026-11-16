// =====================================================================
// WEKINATOR TRAINING  :  PROCESSING SKETCH (CLASSIFIER)
// =====================================================================
// Reads the 7 breathing features from the Nano, forwards them to
// Wekinator, receives the class Wekinator recognises and sends it back
// to the Nano.
//
// Line from the Nano (20 times per second):
//   bpm:18.20,irregular:0.08,depth:9.50,ratio:0.85,chest:3.10,belly:4.20,bellyShare:0.58
//
// Wekinator setup: 7 inputs, 1 output, All classifiers, kNN.
//   Classes: 1 = too fast, 2 = too slow, 3 = erratic, 4 = calm (optional).
//
// Libraries needed: Serial (built into Processing) and oscP5
//   (Sketch > Import Library > Manage Libraries, search oscP5, Install).
// =====================================================================


// ---------------------------------------------------------------------
// PART 1 : LIBRARIES
// ---------------------------------------------------------------------

import processing.serial.*;     // reads the USB serial connection to the Nano
import oscP5.*;                 // sends and receives OSC messages, the format Wekinator uses
import netP5.*;                 // network addresses for oscP5


// ---------------------------------------------------------------------
// PART 2 : SETTINGS
// ---------------------------------------------------------------------

int SERIAL_PORT_INDEX = -1;              // -1 = find the Nano automatically; otherwise the port number shown in the console (for example 3)
int BAUD_RATE = 115200;                  // same speed as BAUD_RATE in the Nano sketch
String WEKINATOR_IP = "127.0.0.1";       // 127.0.0.1 = this same computer
int WEKINATOR_INPUT_PORT = 6448;         // port where Wekinator receives inputs (Wekinator default)
int LISTEN_PORT = 12000;                 // port where Wekinator's class arrives (Wekinator default)
int SEND_CLASS_EVERY_MS = 500;           // the class goes to the Nano every 500 milliseconds (twice per second)
int FORGET_CLASS_AFTER_MS = 20000;       // after 20 seconds with no class from Wekinator, 0 ("no answer") is sent, so an old visitor's class does not stay


// ---------------------------------------------------------------------
// PART 3 : CONNECTIONS
// ---------------------------------------------------------------------

Serial boardPort;                        // USB connection to the Nano (stays empty if no Nano is found)
OscP5 oscP5;                             // OSC listener on LISTEN_PORT
NetAddress wekinatorAddress;             // Wekinator's address and input port


// ---------------------------------------------------------------------
// PART 4 : VALUES
// ---------------------------------------------------------------------

// The Nano sends 7 features per line, always in the same order:
// bpm, irregular, depth, ratio, chest, belly, bellyShare.
// Wekinator's number of inputs has to be 7.
int MAX_INPUTS = 10;                     // maximum number of values accepted in one line
String[] inputNames = new String[MAX_INPUTS];   // feature names, for example "bpm"
float[] inputValues = new float[MAX_INPUTS];    // feature values
int inputCount = 0;                      // number of values in the last valid line (should be 7)
int linesReceived = 0;                   // count of valid lines from the Nano (shows the cable works)
int linesSentToWekinator = 0;            // count of messages sent to Wekinator

int currentClass = 0;                    // latest class from Wekinator: 0 = none yet, 1, 2, 3 (or 4 if calm was trained)
int lastAnswerTime = -1;                 // time of the latest class (-1 = never)
int lastClassSendTime = 0;               // time the class was last sent to the Nano


// =====================================================================
// setup()  runs once at start
// =====================================================================

void setup() {                                                    // start of setup
  size(700, 540);                                                 // window 700 pixels wide and 540 tall
  textSize(16);                                                   // text 16 pixels tall

  oscP5 = new OscP5(this, LISTEN_PORT);                           // starts OSC, listening on port 12000
  wekinatorAddress = new NetAddress(WEKINATOR_IP, WEKINATOR_INPUT_PORT);   // stores Wekinator's address

  String[] portNames = Serial.list();                             // list of all serial ports on the computer
  println("These are the serial ports on this Mac:");             // console title
  for (int i = 0; i < portNames.length; i = i + 1) {              // loops through every port
    println("  [" + i + "]  " + portNames[i]);                    // prints the port number and name
  }                                                               // end of the port list

  int chosenPort = -1;                                            // -1 = no port chosen yet
  if (SERIAL_PORT_INDEX >= 0) {                                   // port number set by hand in the settings?
    chosenPort = SERIAL_PORT_INDEX;                               // uses that port
  } else {                                                        // otherwise searches for the Nano by name
    for (int i = 0; i < portNames.length; i = i + 1) {            // loops through every port
      String name = portNames[i];                                 // name of this port
      boolean isCuPort = name.startsWith("/dev/cu.");             // on a Mac the right ports start with /dev/cu.
      boolean looksLikeNano = false;                              // assumes it is not the Nano
      if (name.indexOf("usbserial") >= 0) {                       // name contains "usbserial"?
        looksLikeNano = true;                                     // likely the Nano
      }                                                           // end of check
      if (name.indexOf("wchusbserial") >= 0) {                    // name contains "wchusbserial" (CH340 chip on most Nano copies)?
        looksLikeNano = true;                                     // likely the Nano
      }                                                           // end of check
      if (name.indexOf("usbmodem") >= 0) {                        // name contains "usbmodem"?
        looksLikeNano = true;                                     // likely the Nano
      }                                                           // end of check
      if (isCuPort == true && looksLikeNano == true) {            // both conditions true?
        chosenPort = i;                                           // chooses this port
      }                                                           // end of check
    }                                                             // end of the search
  }                                                               // end of the if / else

  if (chosenPort >= 0 && chosenPort < portNames.length) {         // valid port found?
    println("Using port: " + portNames[chosenPort]);              // prints the chosen port
    boardPort = new Serial(this, portNames[chosenPort], BAUD_RATE);   // opens the USB connection
    boardPort.bufferUntil('\n');                                  // serialEvent() runs each time a full line has arrived
  } else {                                                        // no port found
    println("No Nano found. Close the Serial Monitor, check the cable, or set SERIAL_PORT_INDEX.");   // troubleshooting message
  }                                                               // end of the if / else
}                                                                 // end of setup


// =====================================================================
// draw()  repeats 60 times per second
// =====================================================================

void draw() {                                                     // start of draw
  sendClassToNano();                                              // step 1: sends Wekinator's class to the Nano
  drawScreen();                                                   // step 2: draws the values on screen
}                                                                 // end of draw


// =====================================================================
// LINE FROM THE NANO
// =====================================================================
// serialEvent() runs automatically each time a full line arrives.

void serialEvent(Serial port) {                                   // start of serialEvent
  String line = port.readStringUntil('\n');                       // reads the full line
  if (line == null) {                                             // empty?
    return;                                                       // stops here
  }                                                               // end of check
  line = trim(line);                                              // removes invisible characters at the start and end
  String[] pairs = split(line, ',');                              // splits at every comma: "bpm:18.20" and so on
  if (pairs.length < 1 || pairs.length > MAX_INPUTS) {            // no pieces, or too many?
    return;                                                       // broken line, ignored
  }                                                               // end of check

  String[] newNames = new String[pairs.length];                   // new list for the names in this line
  float[] newValues = new float[pairs.length];                    // new list for the values in this line
  for (int i = 0; i < pairs.length; i = i + 1) {                  // loops through the pieces
    String[] parts = split(pairs[i], ':');                        // splits "bpm:18.20" into "bpm" and "18.20"
    if (parts.length != 2) {                                      // not a name and a value?
      return;                                                     // whole line ignored
    }                                                             // end of check
    float value = float(parts[1]);                                // converts the text to a number
    if (Float.isNaN(value)) {                                     // not a number?
      return;                                                     // whole line ignored
    }                                                             // end of check
    newNames[i] = parts[0];                                       // stores the name
    newValues[i] = value;                                         // stores the value
  }                                                               // end of the pieces

  for (int i = 0; i < pairs.length; i = i + 1) {                  // line is valid: copies it over
    inputNames[i] = newNames[i];                                  // copies the name
    inputValues[i] = newValues[i];                                // copies the value
  }                                                               // end of copying
  inputCount = pairs.length;                                      // stores how many values the line had
  linesReceived = linesReceived + 1;                              // one more valid line
  sendToWekinator();                                              // forwards the values to Wekinator immediately
}                                                                 // end of serialEvent


// =====================================================================
// WEKINATOR
// =====================================================================

void sendToWekinator() {                                          // start of sendToWekinator
  OscMessage message = new OscMessage("/wek/inputs");             // new message with Wekinator's input address
  for (int i = 0; i < inputCount; i = i + 1) {                    // loops through every value, in order
    message.add(inputValues[i]);                                  // adds input number i + 1
  }                                                               // end of the values
  oscP5.send(message, wekinatorAddress);                          // sends the message to Wekinator on port 6448
  linesSentToWekinator = linesSentToWekinator + 1;                // counts the message
}                                                                 // end of sendToWekinator


void oscEvent(OscMessage incoming) {                              // runs automatically when an OSC message arrives
  if (incoming.checkAddrPattern("/wek/outputs") == true) {        // classifier output from Wekinator?
    float classNumber = incoming.get(0).floatValue();             // reads the class number (for example 2.0)
    setClass(round(classNumber));                                 // rounds it to 1, 2, 3 or 4 and stores it
  }                                                               // end of check
}                                                                 // end of oscEvent


void setClass(int newClass) {                                     // stores a new class if it is 1, 2, 3 or 4
  if (newClass >= 1 && newClass <= 4) {                           // is it 1, 2, 3 or 4?
    currentClass = newClass;                                      // stores the class
    lastAnswerTime = millis();                                    // stores the time it arrived
  }                                                               // end of check
}                                                                 // end of setClass


// =====================================================================
// CLASS BACK TO THE NANO
// =====================================================================

void sendClassToNano() {                                          // start of sendClassToNano
  if (boardPort == null) {                                        // no Nano connected?
    return;                                                       // nothing to send to
  }                                                               // end of check
  int now = millis();                                             // current time
  if (now - lastClassSendTime < SEND_CLASS_EVERY_MS) {            // last send less than half a second ago?
    return;                                                       // waits
  }                                                               // end of check
  lastClassSendTime = now;                                        // stores the time of this send
  int classToSend = currentClass;                                 // by default sends Wekinator's latest class
  if (lastAnswerTime < 0 || now - lastAnswerTime > FORGET_CLASS_AFTER_MS) {   // no class ever, or none for 20 seconds?
    classToSend = 0;                                              // sends 0: "no answer"
  }                                                               // end of check
  if (classToSend < 0 || classToSend > 4) {                       // unexpected value?
    classToSend = 0;                                              // sends 0 instead
  }                                                               // end of check
  boardPort.write(str(classToSend) + "\n");                       // sends the digit and a new line to the Nano
}                                                                 // end of sendClassToNano


// =====================================================================
// SCREEN
// =====================================================================

String className(int classNumber) {                               // converts a class number into its name
  if (classNumber == 1) {                                         // 1?
    return "1  TOO FAST";                                         // name of class 1
  }                                                               // end of check
  if (classNumber == 2) {                                         // 2?
    return "2  TOO SLOW";                                         // name of class 2
  }                                                               // end of check
  if (classNumber == 3) {                                         // 3?
    return "3  ERRATIC";                                          // name of class 3
  }                                                               // end of check
  if (classNumber == 4) {                                         // 4?
    return "4  CALM";                                             // name of class 4
  }                                                               // end of check
  return "no answer yet";                                         // any other number
}                                                                 // end of className


void drawScreen() {                                               // start of drawScreen
  background(20);                                                 // almost black background
  fill(230);                                                      // light grey text
  text("Lines from the Nano: " + linesReceived, 30, 40);          // shows the cable works
  text("Sent to Wekinator: " + linesSentToWekinator, 30, 66);     // shows Wekinator is receiving

  text("Inputs per line: " + inputCount + "   (Wekinator's # inputs must be the same)", 30, 92);   // should be 7

  for (int i = 0; i < inputCount; i = i + 1) {                    // loops through every value
    float y = 130 + i * 24;                                       // each row 24 pixels below the previous one
    text((i + 1) + "  " + inputNames[i] + "   " + nf(inputValues[i], 0, 3), 30, y);   // input number, name and value
  }                                                               // end of the values

  text("WEKINATOR SAYS", 30, 400);                                // title
  textSize(30);                                                   // large text
  text(className(currentClass), 30, 440);                         // current class
  textSize(16);                                                   // back to normal size
  if (lastAnswerTime >= 0) {                                      // has a class ever arrived?
    float secondsAgo = (millis() - lastAnswerTime) / 1000.0;      // seconds since the latest class
    text("last answer " + nf(secondsAgo, 0, 1) + " s ago", 30, 470);   // if this keeps growing, Wekinator is not running
  }                                                               // end of check

  if (boardPort == null) {                                        // no Nano?
    fill(255, 90, 90);                                            // red text
    text("NO NANO CONNECTED: see the console", 30, 510);          // warning
  }                                                               // end of check
}                                                                 // end of drawScreen
