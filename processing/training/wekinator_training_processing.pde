// =====================================================================
// WEKINATOR TRAINING  :  PROCESSING SKETCH
// =====================================================================
// What this sketch does, in plain words:
//   1. It reads the line the Nano sends 20 times every second. That is either
//      7 breathing features (for a CLASSIFIER):
//        bpm:18.20,irregular:0.08,depth:9.50,ratio:0.85,chest:3.10,belly:4.20,bellyShare:0.58
//      or 4 clean values (for DYNAMIC TIME WARPING):
//        nose:12.40,mouth:3.10,chest:4.00,belly:7.60
//   2. It sends those numbers straight to Wekinator, in the same order
//      (OSC, port 6448, message /wek/inputs). No maths here: the Nano did it.
//   3. It listens for Wekinator's answer (OSC, port 12000):
//        1 = too fast,  2 = too slow,  3 = erratic  (4 = calm, if you trained it)
//   4. It sends that answer back to the Nano as one digit and a new line,
//      twice every second.
//   5. It shows the numbers and the answer on screen.
//
// It understands both kinds of Wekinator project:
//   * a CLASSIFIER project (answer arrives as /wek/outputs with 1, 2, 3 or 4)
//   * a DYNAMIC TIME WARPING project (answer arrives as /output_1,
//     /output_2, /output_3 or /output_4 when Wekinator recognises a pattern)
//
// Libraries needed: Serial (already inside Processing) and oscP5
//   (Sketch > Import Library > Manage Libraries, search oscP5, Install).
// =====================================================================


// ---------------------------------------------------------------------
// PART 1 : LIBRARIES (extra tools we borrow)
// ---------------------------------------------------------------------

import processing.serial.*;     // the tool that lets Processing read the USB cable
import oscP5.*;                 // the tool that speaks OSC, the language Wekinator speaks
import netP5.*;                 // a helper that comes with oscP5, it knows network addresses


// ---------------------------------------------------------------------
// PART 2 : SETTINGS YOU ARE ALLOWED TO CHANGE
// ---------------------------------------------------------------------

int SERIAL_PORT_INDEX = -1;              // -1 = find the Nano by itself. If that fails, put the number from the console here (for example 3).
int BAUD_RATE = 115200;                  // must be the same as BAUD_RATE in the Nano sketch
String WEKINATOR_IP = "127.0.0.1";       // 127.0.0.1 means "this same Mac"
int WEKINATOR_INPUT_PORT = 6448;         // Wekinator listens for inputs on this port (its default)
int LISTEN_PORT = 12000;                 // we listen for Wekinator's answer on this port (its default)
int SEND_CLASS_EVERY_MS = 500;           // send the class to the Nano every 500 milliseconds (twice a second)
int FORGET_CLASS_AFTER_MS = 20000;       // if Wekinator recognised nothing for 20 seconds, send 0 ("no answer"), so an old visitor's class does not stay forever


// ---------------------------------------------------------------------
// PART 3 : CONNECTIONS
// ---------------------------------------------------------------------

Serial boardPort;                        // the USB connection to the Nano (stays empty if no Nano is found)
OscP5 oscP5;                             // the OSC tool, it listens on LISTEN_PORT
NetAddress wekinatorAddress;             // where Wekinator lives (address and port)


// ---------------------------------------------------------------------
// PART 4 : THE NUMBERS
// ---------------------------------------------------------------------

// The Nano can send 4 clean values (for dynamic time warping) or
// 7 features (for a classifier). This sketch takes whatever comes, in order,
// so Wekinator's number of inputs must match: 4 or 7.
int MAX_INPUTS = 10;                     // the most numbers one line may hold
String[] inputNames = new String[MAX_INPUTS];   // the names of the numbers, for example "bpm"
float[] inputValues = new float[MAX_INPUTS];    // the numbers themselves
int inputCount = 0;                      // how many numbers the last good line had
int linesReceived = 0;                   // how many good lines arrived (to check the cable works)
int linesSentToWekinator = 0;            // how many times we sent the numbers to Wekinator

int currentClass = 0;                    // Wekinator's newest answer: 0 = none yet, 1, 2, 3 (or 4 if you trained calm)
int lastAnswerTime = -1;                 // when the last answer arrived (-1 = never)
int lastClassSendTime = 0;               // when we last sent the class to the Nano


// =====================================================================
// setup()  runs ONE time when you press Run
// =====================================================================

void setup() {                                                    // start of setup
  size(700, 540);                                                 // open a window 700 pixels wide and 540 tall
  textSize(16);                                                   // letters 16 pixels tall

  oscP5 = new OscP5(this, LISTEN_PORT);                           // start OSC and listen on port 12000
  wekinatorAddress = new NetAddress(WEKINATOR_IP, WEKINATOR_INPUT_PORT);   // remember where Wekinator is

  String[] portNames = Serial.list();                             // ask the Mac for every USB serial port
  println("These are the serial ports on this Mac:");             // a title in the console
  for (int i = 0; i < portNames.length; i = i + 1) {              // go through every port
    println("  [" + i + "]  " + portNames[i]);                    // write its number and name
  }                                                               // end of the list

  int chosenPort = -1;                                            // -1 means "no port chosen yet"
  if (SERIAL_PORT_INDEX >= 0) {                                   // did you type a port number in the settings?
    chosenPort = SERIAL_PORT_INDEX;                               // then use it
  } else {                                                        // otherwise look for the Nano by its name
    for (int i = 0; i < portNames.length; i = i + 1) {            // go through every port
      String name = portNames[i];                                 // the name of this port
      boolean isCuPort = name.startsWith("/dev/cu.");             // on a Mac we want ports starting with /dev/cu.
      boolean looksLikeNano = false;                              // start by assuming it is not the Nano
      if (name.indexOf("usbserial") >= 0) {                       // does the name contain "usbserial"?
        looksLikeNano = true;                                     // very likely the Nano
      }                                                           // end of check
      if (name.indexOf("wchusbserial") >= 0) {                    // "wchusbserial" (the CH340 chip on most Nano copies)?
        looksLikeNano = true;                                     // very likely the Nano
      }                                                           // end of check
      if (name.indexOf("usbmodem") >= 0) {                        // "usbmodem"?
        looksLikeNano = true;                                     // very likely the Nano
      }                                                           // end of check
      if (isCuPort == true && looksLikeNano == true) {            // both yes?
        chosenPort = i;                                           // choose this port
      }                                                           // end of check
    }                                                             // end of the search
  }                                                               // end of the if / else

  if (chosenPort >= 0 && chosenPort < portNames.length) {         // did we find a real port?
    println("Using port: " + portNames[chosenPort]);              // say which one
    boardPort = new Serial(this, portNames[chosenPort], BAUD_RATE);   // open the connection
    boardPort.bufferUntil('\n');                                  // call serialEvent() every time a whole line has arrived
  } else {                                                        // no port found
    println("No Nano found. Close the Serial Monitor, check the cable, or set SERIAL_PORT_INDEX.");   // what to do
  }                                                               // end of the if / else
}                                                                 // end of setup


// =====================================================================
// draw()  runs AGAIN AND AGAIN, 60 times every second
// =====================================================================

void draw() {                                                     // start of draw
  sendClassToNano();                                              // step 1: send Wekinator's answer back to the Nano
  drawScreen();                                                   // step 2: show the numbers
}                                                                 // end of draw


// =====================================================================
// A LINE ARRIVED FROM THE NANO
// =====================================================================
// Processing runs serialEvent() by itself every time a whole line arrived.

void serialEvent(Serial port) {                                   // start of the recipe
  String line = port.readStringUntil('\n');                       // take the whole line
  if (line == null) {                                             // nothing there?
    return;                                                       // leave
  }                                                               // end of the check
  line = trim(line);                                              // remove invisible letters at the start and end
  String[] pairs = split(line, ',');                              // cut at every comma: "bpm:18.20" and so on
  if (pairs.length < 1 || pairs.length > MAX_INPUTS) {            // no pieces, or far too many?
    return;                                                       // then the line is broken: ignore it
  }                                                               // end of the check

  String[] newNames = new String[pairs.length];                   // a fresh list for the names of this line
  float[] newValues = new float[pairs.length];                    // a fresh list for the numbers of this line
  for (int i = 0; i < pairs.length; i = i + 1) {                  // go through the pieces
    String[] parts = split(pairs[i], ':');                        // cut "bpm:18.20" into "bpm" and "18.20"
    if (parts.length != 2) {                                      // we expect a name and a number
      return;                                                     // otherwise ignore the whole line
    }                                                             // end of the check
    float value = float(parts[1]);                                // the number
    if (Float.isNaN(value)) {                                     // was it not a number?
      return;                                                     // ignore the whole line
    }                                                             // end of the check
    newNames[i] = parts[0];                                       // keep the name
    newValues[i] = value;                                         // keep the number
  }                                                               // end of the pieces

  for (int i = 0; i < pairs.length; i = i + 1) {                  // the whole line was good: copy it over
    inputNames[i] = newNames[i];                                  // copy the name
    inputValues[i] = newValues[i];                                // copy the number
  }                                                               // end of copying
  inputCount = pairs.length;                                      // remember how many numbers there are
  linesReceived = linesReceived + 1;                              // one more good line
  sendToWekinator();                                              // pass the numbers on to Wekinator straight away
}                                                                 // end of the recipe


// =====================================================================
// TALKING TO WEKINATOR
// =====================================================================

void sendToWekinator() {                                          // start of the recipe
  OscMessage message = new OscMessage("/wek/inputs");             // a new message with the address Wekinator expects
  for (int i = 0; i < inputCount; i = i + 1) {                    // go through every number, in order
    message.add(inputValues[i]);                                  // input number i + 1
  }                                                               // end of the numbers
  oscP5.send(message, wekinatorAddress);                          // send it to Wekinator on port 6448
  linesSentToWekinator = linesSentToWekinator + 1;                // count it
}                                                                 // end of the recipe


void oscEvent(OscMessage incoming) {                              // Processing runs this by itself when an OSC message arrives
  if (incoming.checkAddrPattern("/wek/outputs") == true) {        // a CLASSIFIER answer?
    float classNumber = incoming.get(0).floatValue();             // read the number (like 2.0)
    setClass(round(classNumber));                                 // round it to 1, 2, 3 or 4 and keep it
  }                                                               // end of the check
  if (incoming.checkAddrPattern("/output_1") == true) {           // a DYNAMIC TIME WARPING answer for pattern 1?
    setClass(1);                                                  // too fast
  }                                                               // end of the check
  if (incoming.checkAddrPattern("/output_2") == true) {           // pattern 2?
    setClass(2);                                                  // too slow
  }                                                               // end of the check
  if (incoming.checkAddrPattern("/output_3") == true) {           // pattern 3?
    setClass(3);                                                  // erratic
  }                                                               // end of the check
  if (incoming.checkAddrPattern("/output_4") == true) {           // pattern 4 (only if you trained calm)?
    setClass(4);                                                  // calm
  }                                                               // end of the check
}                                                                 // end of the recipe


void setClass(int newClass) {                                     // keeps a new answer, if it is 1, 2, 3 or 4
  if (newClass >= 1 && newClass <= 4) {                           // is it 1, 2, 3 or 4?
    currentClass = newClass;                                      // keep it
    lastAnswerTime = millis();                                    // remember when it arrived
  }                                                               // end of the check
}                                                                 // end of the recipe


// =====================================================================
// SENDING THE CLASS BACK TO THE NANO
// =====================================================================

void sendClassToNano() {                                          // start of the recipe
  if (boardPort == null) {                                        // no Nano connected?
    return;                                                       // nobody to send to
  }                                                               // end of the check
  int now = millis();                                             // what time is it
  if (now - lastClassSendTime < SEND_CLASS_EVERY_MS) {            // sent less than half a second ago?
    return;                                                       // then wait
  }                                                               // end of the check
  lastClassSendTime = now;                                        // remember that we send now
  int classToSend = currentClass;                                 // normally we send Wekinator's newest answer
  if (lastAnswerTime < 0 || now - lastAnswerTime > FORGET_CLASS_AFTER_MS) {   // no answer ever, or none for 20 seconds?
    classToSend = 0;                                              // then send 0: "no answer"
  }                                                               // end of the check
  if (classToSend < 0 || classToSend > 4) {                       // anything strange?
    classToSend = 0;                                              // then also send 0
  }                                                               // end of the check
  boardPort.write(str(classToSend) + "\n");                       // send the digit and a new line to the Nano
}                                                                 // end of the recipe


// =====================================================================
// THE SCREEN
// =====================================================================

String className(int classNumber) {                               // turns a class number into words
  if (classNumber == 1) {                                         // 1?
    return "1  TOO FAST";                                         // its name
  }                                                               // end of the check
  if (classNumber == 2) {                                         // 2?
    return "2  TOO SLOW";                                         // its name
  }                                                               // end of the check
  if (classNumber == 3) {                                         // 3?
    return "3  ERRATIC";                                          // its name
  }                                                               // end of the check
  if (classNumber == 4) {                                         // 4?
    return "4  CALM";                                             // its name
  }                                                               // end of the check
  return "no answer yet";                                         // anything else
}                                                                 // end of the recipe


void drawScreen() {                                               // start of the recipe
  background(20);                                                 // almost black
  fill(230);                                                      // light grey text
  text("Lines from the Nano: " + linesReceived, 30, 40);          // shows the cable works
  text("Sent to Wekinator: " + linesSentToWekinator, 30, 66);     // shows Wekinator is being fed

  text("Inputs per line: " + inputCount + "   (Wekinator's # inputs must be the same)", 30, 92);   // 4 or 7

  for (int i = 0; i < inputCount; i = i + 1) {                    // go through every number
    float y = 130 + i * 24;                                       // each one 24 pixels lower than the one before
    text((i + 1) + "  " + inputNames[i] + "   " + nf(inputValues[i], 0, 3), 30, y);   // its number, name and value
  }                                                               // end of the numbers

  text("WEKINATOR SAYS", 30, 400);                                // title
  textSize(30);                                                   // big letters
  text(className(currentClass), 30, 440);                         // the answer
  textSize(16);                                                   // back to normal
  if (lastAnswerTime >= 0) {                                      // did an answer ever arrive?
    float secondsAgo = (millis() - lastAnswerTime) / 1000.0;      // how long ago
    text("last answer " + nf(secondsAgo, 0, 1) + " s ago", 30, 470);   // if this keeps growing, Wekinator is not running
  }                                                               // end of the check

  if (boardPort == null) {                                        // no Nano?
    fill(255, 90, 90);                                            // red
    text("NO NANO CONNECTED: see the console", 30, 510);          // warning
  }                                                               // end of the check
}                                                                 // end of the recipe
