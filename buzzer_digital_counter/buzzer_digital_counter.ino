// www.elegoo.com
// 2016.12.12

#include "pitches.h"

int latch = 9;  // 74HC595 pin 9 STCP
int clock = 10; // 74HC595 pin 10 SHCP
int data = 8;   // 74HC595 pin 8 DS

unsigned char table[] = {
  0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c,
  0x39, 0x5e, 0x79, 0x71, 0x00
};

int notes[] = {
  NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5,
  NOTE_C5, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4, NOTE_C4
};

// Choose a specific note to play continuously
int soundNote = NOTE_C5; // Define your desired note
int duration = 100;      // 500 milliseconds
int tonePin = 12;

void setup() {
  pinMode(latch, OUTPUT);
  pinMode(clock, OUTPUT);
  pinMode(data, OUTPUT);
}

void Display(unsigned char num) {
  digitalWrite(latch, LOW);
  shiftOut(data, clock, MSBFIRST, table[num]);
  digitalWrite(latch, HIGH);
}

void playSound(int note) {
  tone(tonePin, note, duration);
}

void loop() {
  for (int i = 0; i < 16; i++) {
    Display(i);
    playSound(notes[i]); // Play the same sound for each number
    delay(500); // 500 milliseconds delay
  }
}
