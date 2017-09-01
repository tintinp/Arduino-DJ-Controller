/*
Hardware Hookup:
Mux Breakout ----------- Arduino
     S0 ------------------- 2
     S1 ------------------- 3
     S2 ------------------- 4
     Z -------------------- A0
    VCC ------------------- 5V
    GND ------------------- GND

*/

#include "MIDIUSB.h"

// Multiplexer Pin Definitions /////////////////
const int selectPins[3] = {2, 3, 4}; // {S0, S1, S2}
const int zInput = A0; // Connect common (z) to pin Analog 0


// Potentiometers //////////////////////////////
const byte nPots = 8; // number of potentiometers
int potCurrentValue[nPots] = {0}; // store current pot values
int potPrevValue[nPots] = {0}; // store previous pot values
int potValueDiff = 0; // potentiometer value differnce
int ccPrevValue[nPots] = {0}; // store previous cc values


// Potentiometer reading settings //////////////
int TIMEOUT = 50;
byte tresholdValue = 5;
boolean potMoving = true;
unsigned long pTime[nPots] = {0};
unsigned long timer[nPots] = {0};

// Midi ////////////////////////////////////////
byte cc = 1;


// Other constants /////////////////////////////
const int DELAY_TIME = 100;



void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 3; i++) {
    pinMode(selectPins[i], OUTPUT);
    digitalWrite(selectPins[i], HIGH);
  }
  pinMode(zInput, INPUT); // Set up Z as an input
  
  // Print the header:
  Serial.println("Y0\tY1\tY2\tY3\tY4\tY5\tY6\tY7");
  Serial.println("---\t---\t---\t---\t---\t---\t---\t---");
}

void loop() {
  readPot();
}

void readPot() {
  // gather pot value from each pots
  for (byte pin = 0; pin <= 7; pin++) {
    selectMuxPin(pin); //Select one pot at a time
    int potReading = analogRead(zInput);
    potCurrentValue[(int)pin] = potReading;
    Serial.print(String(potReading) + "\t");
  }
  Serial.println();

  // check for change in pot value
  for (int i = 0; i < nPots; i++) {
    potValueDiff = abs(potCurrentValue[i] - potPrevValue[i]);

    if (potValueDiff >= tresholdValue) {
      potMoving = true;
    } else {
      potMoving = false;
    }

    if (potMoving = true) {
      int ccValue = map(potCurrentValue[i], 0, 1023, 0, 127);
      if (ccPrevValue[i] != ccValue) {
        sendPotMidi(11, cc + i, ccValue);
        MidiUSB.flush();
        potPrevValue[i] = potCurrentValue[i];
        ccPrevValue[i] = ccValue;
      }
    }
  }
  delay(DELAY_TIME);
}

void selectMuxPin(byte pin) {
  for (int i = 0; i < 3; i++) {
    if (pin & (1 << i)) {
      digitalWrite(selectPins[i], HIGH);
    } else {
      digitalWrite(selectPins[i], LOW);
    }
  }
}

void sendPotMidi(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
  Serial.println("sent!");
}
