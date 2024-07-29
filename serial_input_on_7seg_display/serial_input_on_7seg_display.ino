int latch = 9;  // 74HC595 pin 9 STCP
int clock = 10; // 74HC595 pin 10 SHCP
int data = 8;   // 74HC595 pin 8 DS
int buzzer = 12; // Buzzer pin

int digitPins[] = {2, 3, 4, 5}; // Pins controlling each digit
int numDigits = 4; // Number of digits

// Enum to represent the digits and dash
enum SegmentEncoding {
  ZERO = 0x3f,   // 0
  ONE = 0x06,    // 1
  TWO = 0x5b,    // 2
  THREE = 0x4f,  // 3
  FOUR = 0x66,   // 4
  FIVE = 0x6d,   // 5
  SIX = 0x7d,    // 6
  SEVEN = 0x07,  // 7
  EIGHT = 0x7f,  // 8
  NINE = 0x6f,   // 9
  DASH = 0x40    // Dash (-), middle segment only
};

SegmentEncoding table[] = {
  ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, DASH
};

char inputBuffer[5] = {'-', '-', '-', '-', '\0'}; // Buffer to store up to 4 digits
int currentDigitCount = 0;

void setup() {
  pinMode(latch, OUTPUT);
  pinMode(clock, OUTPUT);
  pinMode(data, OUTPUT);
  pinMode(buzzer, OUTPUT);

  for (int i = 0; i < numDigits; i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], LOW); // Ensure all digits are off at the start
  }

  Serial.begin(9600);
  clearDisplay();
}

void loop() {
  if (Serial.available()) {
    char inputChar = Serial.read();
    
    if (isDigit(inputChar)) {
      if (currentDigitCount < 4) {
        inputBuffer[currentDigitCount++] = inputChar;
        playBeep();
        printCurrentCombination();
      } else {
        playErrorSound();
        resetInput();
      }
    }
    displayInput(); // Continuously update the display
  }
}

void displayInput() {
  for (int i = 0; i < numDigits; i++) {
    if (inputBuffer[i] == '-') {
      Display(DASH, i);
    } else {
      Display(table[inputBuffer[i] - '0'], i);
    }
  }
}

void Display(SegmentEncoding num, int digit) {
  digitalWrite(latch, LOW);
  shiftOut(data, clock, MSBFIRST, num);
  digitalWrite(latch, HIGH);

  selectDigit(digit); // Activate the correct digit
  delay(5); // Delay to allow the digit to be visible
  selectDigit(-1); // Turn off all digits
}

void selectDigit(int digit) {
  for (int i = 0; i < numDigits; i++) {
    if (i == digit) {
      digitalWrite(digitPins[i], HIGH); // Turn on the selected digit
    } else {
      digitalWrite(digitPins[i], LOW); // Turn off all other digits
    }
  }
}

void playBeep() {
  tone(buzzer, 1000, 100); // 1kHz beep for 100ms
  delay(100);
}

void playErrorSound() {
  for (int i = 0; i < 3; i++) {
    tone(buzzer, 500, 200); // 500Hz error beep
    delay(300);
  }
}

void resetInput() {
  for (int i = 0; i < numDigits; i++) {
    inputBuffer[i] = '-';
  }
  currentDigitCount = 0;
  clearDisplay();
  printCurrentCombination(); // Print the reset state
}

void clearDisplay() {
  for (int i = 0; i < numDigits; i++) {
    Display(DASH, i); // Display dash for all positions
  }
}

void printCurrentCombination() {
  Serial.print("Current combination: ");
  Serial.println(inputBuffer);
}
