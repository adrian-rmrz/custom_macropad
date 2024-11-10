/* =======================================================
 * - Author: adrian-rmrz
 * - Description: Experimentation with different libraries
 * to make a macropad with an ESP32 board and keypad
 * =======================================================
 * - Parts: ESP32 Development Board, Generic 12-Key Keypad
 * =======================================================
 * - Features: 
 * =======================================================
 * - Libraries: Keypad by Mark Stanley, Alexander Brevig;
 * ESP-BLE-Keyboard by T-vk
 * =======================================================
 * - Inspiration: arduino-switcheroonie by witnessmenow
 * (Brian Lough)
 * ==================================================== */

#include <BleKeyboard.h>
#include <Keypad.h>
BleKeyboard bleKeyboard;

/* =================================
 * Pins on keypad (left to right)
 * =================================
 * NC, R4, R3, R2, R1, C3, C2, C1
 * ============================== */
// Define number of rows and columns
const byte ROWS = 4; // 4 rows
const byte COLS = 3; // 3 columns
const byte NUM_KEYS = ROWS * COLS;

// Definition of key matrix
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'},
};

// Definition of pins on board
byte rowPins[ROWS] = {26, 25, 33, 32}; // Row pins
byte colPins[COLS] = {12, 14, 27}; // Column pins

// Definition of keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

/* =======================
 * Constants and variables
 * for layers
 * ==================== */
// Constant for max amount of layers
const int MAX_LAYERS = 3;

// Initial layer
int layer = 0;

/* For each key, assign the commands you want on each layer
 * Each key index is equivalent to its value - 1
 * Ex: Key 1's index is 0 (1 - 1)
*/ 
const uint8_t REWIND_YT = static_cast<uint8_t>('j');
const uint8_t PAUSE_YT = static_cast<uint8_t>('k');
const uint8_t FORWARD_YT = static_cast<uint8_t>('l');

const uint8_t* KEY_COMMANDS[NUM_KEYS][MAX_LAYERS] = {
  {KEY_MEDIA_MUTE, KEY_MEDIA_PREVIOUS_TRACK, &REWIND_YT},
  {KEY_MEDIA_VOLUME_DOWN, KEY_MEDIA_PLAY_PAUSE, &PAUSE_YT},
  {KEY_MEDIA_VOLUME_UP, KEY_MEDIA_NEXT_TRACK, &FORWARD_YT}
};

void setup() {
  Serial.begin(9600);
  Serial.println("Starting BLE work!");
  bleKeyboard.begin();
}

void loop() {
  char key = keypad.getKey();

  // If bluetooth is connected and a key is pressed, send a command
  if(bleKeyboard.isConnected() && key) {
    switch (key) {
      case '*':
        // Decrement layer
        layer -= 1;
        
        // Wraparound to max layer if lower than lowest layer
        if (layer < 0) {
          layer = MAX_LAYERS - 1;
        }

        Serial.print("In layer: ");
        Serial.print(layer);
        Serial.println();
        break;
      case '#':
        // Increment layer
        layer += 1;

        // Wraparound to layer 0 if past max layer
        if (layer >= MAX_LAYERS) {
          layer = 0;
        }

        Serial.print("In layer: ");
        Serial.print(layer);
        Serial.println();
        break;
      default:
        Serial.print("Pressing key ");
        Serial.print(key);
        Serial.println();
        
        if (layer == 2) {
          char tempChar = static_cast<char>(*KEY_COMMANDS[key - '1'][layer]);
          bleKeyboard.write(tempChar);

          Serial.print("Command: ");
          Serial.print(tempChar);
          Serial.println();
        } else {
          bleKeyboard.write(KEY_COMMANDS[key - '1'][layer]);

          Serial.print("Command: ");
          Serial.print(*KEY_COMMANDS[key - '1'][layer]);
          Serial.println();
        }
    }
  }

  delay(100);
}