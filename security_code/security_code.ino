#include <LiquidCrystal.h>
#include "pitches.h"
#include <Keypad.h>
#define IR_USE_AVR_TIMER1 // Use timer 1 for IR decoding COMPATIBILITY AND RANDOM CODES ISSUE
#include <IRremote.hpp>

// IR Receiver Module
enum RemoteNumber {
    NUM_0 = 0xFF6897,
    NUM_1 = 0xFF30CF,
    NUM_2 = 0xFF18E7,
    NUM_3 = 0xFF7A85,
    NUM_4 = 0xFF10EF,
    NUM_5 = 0xFF38C7,
    NUM_6 = 0xFF5AA5,
    NUM_7 = 0xFF42BD,
    NUM_8 = 0xFF4AB5,
    NUM_9 = 0xFF52AD,
    OTHER = 0xFF6897 // Default value also 0
};

char translateIRValue(uint64_t rawData) {
    switch (rawData) {
        case 0xE916FF00: return '0'; // Example mapping for button '0'
        case 0xF30CFF00: return '1'; // Example mapping for button '1'
        case 0xE718FF00: return '2'; // Example mapping for button '2'
        case 0xA15EFF00: return '3'; // Example mapping for button '3'
        case 0xF708FF00: return '4'; // Example mapping for button '4'
        case 0xE31CFF00: return '5'; // Example mapping for button '5'
        case 0xA55AFF00: return '6'; // Example mapping for button '6'
        case 0xBD42FF00: return '7'; // Example mapping for button '7'
        case 0xAD52FF00: return '8'; // Example mapping for button '8'
        case 0xB54AFF00: return '9'; // Example mapping for button '9'
        default: return '\0'; // Return null character for unknown codes
    }
}
int irSensorPin = 29; // IR sensor pin

// LCD Module
int liquidRS = 2;  // LCD RS pin
int liquidE = 3;   // LCD Enable pin
int liquidD4 = 4;  // LCD D4 pin
int liquidD5 = 5;  // LCD D5 pin
int liquidD6 = 6;  // LCD D6 pin
int liquidD7 = 7;  // LCD D7 pin
int line1 = 0;     // LCD line 1
int line2 = 1;     // LCD line 2
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(liquidRS, liquidE, liquidD4, liquidD5, liquidD6, liquidD7);

// Keypad Module
const byte ROWS = 4; // four rows
const byte COLS = 4; // four columns
// Define the symbols on the buttons of the keypad
char hexaKeys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
byte codePad1 = 22; // Code pad 1
byte codePad2 = 26; // Code pad 2
byte codePad3 = 30; // Code pad 3
byte codePad4 = 34; // Code pad 4
byte codePad5 = 38; // Code pad 5
byte codePad6 = 42; // Code pad 6
byte codePad7 = 46; // Code pad 7
byte codePad8 = 50; // Code pad 8
byte rowPins[ROWS] = {codePad1, codePad2, codePad3, codePad4}; // Connect to the row pinouts of the keypad
byte colPins[COLS] = {codePad5, codePad6, codePad7, codePad8}; // Connect to the column pinouts of the keypad
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// LEDs
int redLED = 13;   // Red LED pin
int greenLED = 12; // Green LED pin

// Buzzer
int buzzerPin = 8; // Buzzer pin
int enterBeep = NOTE_C5;
int errorBeep = NOTE_A4;
int successBeep = NOTE_C6;
int defaultSoundDuration = 100; // 100ms

// Door Lock
int doorUnlockDuration = 6; // 6 seconds
bool doorLocked = true;

// Security Code
char currentCode[] = "----";
char correctCode[] = "1234";
int currentDigitPointer = 0;

// Language
char *langEnterCode = "Code eingeben:";
char *langCorrectCode = "Richtiger Code!";
char *langAccessGranted = "Zugang gewaehrt!";
char *langIncorrectCode = "Falscher Code!";
char *langAccessDenied = "Zugang verweigert!";

void setup()
{
    Serial.begin(9600); // Initialize serial communication at 9600 bits per second
    pinMode(redLED, OUTPUT);
    pinMode(greenLED, OUTPUT);
    IrReceiver.begin(irSensorPin, DISABLE_LED_FEEDBACK); // Start the receiver
    lockDoor();
    // set up the LCD's number of columns and rows:
    lcd.begin(18, 2); // 16 columns, 2 rows
    lcd.clear();
    lcd.print(langEnterCode);
    renderCurrentCode();
}

void loop() {
    // Check for IR input
    if (IrReceiver.decode()) {
        Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
        handleIRInput(IrReceiver.decodedIRData.decodedRawData);
        IrReceiver.resume(); // Enable receiving of the next value
    }

    // Check for keypad input
    char inputChar = customKeypad.getKey();
    if (inputChar) {
        handleKeypadInput(inputChar);
    }
}

void handleIRInput(uint64_t rawData) {
    char resultChar = translateIRValue(rawData);

    // Check if the result is a valid digit
    if (resultChar != '\0' && isDigit(resultChar)) {
        enterNewNumber(resultChar);
    }
}

void handleKeypadInput(char inputChar) {
    Serial.print("Received character: ");
    Serial.println(inputChar);

    // Ignore newline and carriage return characters
    if (inputChar == '\n' || inputChar == '\r') {
        return;
    }

    // Validate the input as a digit
    if (isDigit(inputChar)) {
        enterNewNumber(inputChar);
    } else {
        playBeep(errorBeep, defaultSoundDuration);
        Serial.println("Invalid keypad input. Please enter a digit.");
    }
}

void renderLCD()
{
    lcd.clear();
    lcd.print(langEnterCode);
    renderCurrentCode();
}

void renderCurrentCode()
{
    int col = 0;
    lcd.setCursor(col, line2);
    lcd.print(currentCode);
}

void clearCode()
{
    currentDigitPointer = 0; // Reset the digit pointer

    // Reset the code to "----"
    for (int i = 0; i < 4; i++)
    {
        currentCode[i] = '-';
    }
}

void playBeep(int note, int duration)
{
    tone(buzzerPin, note, duration); // beep for 100ms
}

void enterNewNumber(char number)
{
    if (currentDigitPointer > 4)
    {
        handleIncorrectCode();
        return;
    };
    playBeep(enterBeep, defaultSoundDuration);
    currentCode[currentDigitPointer] = number;
    currentDigitPointer++;
    Serial.println(currentCode);
    renderCurrentCode();

    if (currentDigitPointer < 4)
        return;

    if (strcmp(currentCode, correctCode) == 0)
    {
        handleCorrectCode();
    }
    else
    {
        handleIncorrectCode();
    }
}

void handleCorrectCode()
{
    currentDigitPointer = 0;
    clearCode();

    // Show success message on LCD
    int col = 0;
    lcd.clear();
    lcd.print(langCorrectCode);
    lcd.setCursor(col, line2);
    lcd.print(langAccessGranted);

    unlockDoor();
    delay(1000 * doorUnlockDuration); // Keep the door unlocked for 6 seconds
    lockDoor();
    renderLCD();
}

void handleIncorrectCode()
{

    Serial.println("Access denied with code:");
    Serial.println(currentCode);

    // Your code here
    currentDigitPointer = 0;

    // Play error sound
    int beepDuration = 1000 * 1; // 1 seconds
    playBeep(errorBeep, beepDuration);

    // Show error message on LCD
    int col = 0;
    lcd.clear();
    lcd.print(langIncorrectCode);
    lcd.setCursor(col, line2);
    lcd.print(langAccessDenied);

    int textDuration = 1000 * 4; // 4 seconds
    delay(textDuration);
    clearCode();
    renderLCD();
}

void unlockDoor()
{
    // Your code here
    doorLocked = false;
    int beepDuration = 1000 * 1; // 1 seconds
    playBeep(successBeep, beepDuration);
    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, HIGH);
}

void lockDoor()
{
    // Your code here
    doorLocked = true;
    int beepDuration = 1000 * 2; // 1 seconds
    playBeep(errorBeep, beepDuration);
    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, LOW);
}