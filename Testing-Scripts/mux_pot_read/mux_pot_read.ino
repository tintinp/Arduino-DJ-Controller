// Thank you Sparkfun for guides on Mux

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

// Pin Definitions
const int selectPins[3] = {2, 3, 4}; // {S0, S1, S2}
const int zInput = A0; // Connect common (z) to pin analog0

const int DELAY_TIME = 1000;

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
  for (byte pin = 0; pin <= 7; pin++) {
    selectMuxPin(pin); //Select one pot at a time
    int potValue = analogRead(zInput);
    Serial.print(String(potValue) + "\t");
  }
  Serial.println();
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
