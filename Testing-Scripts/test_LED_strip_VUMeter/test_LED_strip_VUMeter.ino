

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


#include <Adafruit_NeoPixel.h>
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

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

#define VU_1_PIN 9

Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, VU_1_PIN, NEO_GRB + NEO_KHZ800);

uint32_t green = strip.Color(0, 63, 0);
uint32_t yellow = strip.Color(63, 63, 0);
uint32_t red = strip.Color(63, 0, 0);
uint32_t black = strip.Color(0, 0, 0);

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 3; i++) {
    pinMode(selectPins[i], OUTPUT);
    digitalWrite(selectPins[i], HIGH);
  }
  pinMode(zInput, INPUT); // Set up Z as an input
  
  strip.begin();
  strip.show();
  
}

void loop() {
  readPot();
  midiEventPacket_t VuMeterData = MidiUSB.read();
  if (VuMeterData.header != 0) {
    activateLEDStrip(VuMeterData);
  } 
}

void readPot() {
  // gather pot value from each pots
  for (byte pin = 0; pin <= 7; pin++) {
    selectMuxPin(pin); //Select one pot at a time
    int potReading = analogRead(zInput);
    potCurrentValue[(int)pin] = potReading;
    //Serial.print(String(potReading) + "\t");
  }
  //Serial.println();

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
  //Serial.println("sent!");
}

void activateLEDStrip(midiEventPacket_t VuMeterData) {
  int header = VuMeterData.header;
  int deckPosition = VuMeterData.byte1;
  int colorAndPosition = VuMeterData.byte2;
  int onOff = VuMeterData.byte3;

  switch(colorAndPosition) {
    case 83:
      turnOnOff(0, onOff);
      break;
    case 84:
      turnOnOff(1, onOff);
      break;
    case 85:
      turnOnOff(2, onOff);
      break;
    case 86:
      turnOnOff(3, onOff);
      break;
    case 87:
      turnOnOff(4, onOff);
      break;
    case 88:
      turnOnOff(5, onOff);
      break;
    case 89:
      turnOnOff(6, onOff);
      turnOnOff(7, onOff);
      break;
    default:
      break;
  }
}

void turnOnOff(int ledPosition, int on) {
  if (on) {
    for (int i = 0; i <= ledPosition; i++) {
      if (i < 3) {
        strip.setPixelColor(i, green);
      } else if (i >= 3 && i < 6) {
        strip.setPixelColor(i, yellow);
      } else {
        strip.setPixelColor(i, red);
      }
      strip.show();
    }
  } else {
    strip.setPixelColor(ledPosition, black);
    strip.show();
  }
}
