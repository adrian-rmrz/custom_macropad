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
 * ESP-BLE-Keyboard by T-vk; Adafruit SSD1306 by Adafruit
 * =======================================================
 * - Inspiration: arduino-switcheroonie by witnessmenow
 * (Brian Lough)
 * ==================================================== */

#include <BleKeyboard.h>
#include <Keypad.h>
BleKeyboard bleKeyboard;

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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
byte rowPins[ROWS] = {27, 26, 25, 33}; // Row pins
byte colPins[COLS] = {13, 12, 14}; // Column pins

// Definition of keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

/* ====================
 * Pins for OLED:
 * SCL - 22; SDA - 21
 * ==================== */
// Define screen dimensions (in pixels)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  bleKeyboard.begin();

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

    // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
  testdrawline();      // Draw many lines
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

void testdrawline() {
  int16_t i;

  display.clearDisplay(); // Clear display buffer

  for(i=0; i<display.width(); i+=4) {
    display.drawLine(0, 0, i, display.height()-1, SSD1306_WHITE);
    display.display(); // Update screen with each newly-drawn line
    delay(1);
  }
  for(i=0; i<display.height(); i+=4) {
    display.drawLine(0, 0, display.width()-1, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();

  for(i=0; i<display.width(); i+=4) {
    display.drawLine(0, display.height()-1, i, 0, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  for(i=display.height()-1; i>=0; i-=4) {
    display.drawLine(0, display.height()-1, display.width()-1, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();

  for(i=display.width()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, i, 0, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  for(i=display.height()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, 0, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();

  for(i=0; i<display.height(); i+=4) {
    display.drawLine(display.width()-1, 0, 0, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  for(i=0; i<display.width(); i+=4) {
    display.drawLine(display.width()-1, 0, i, display.height()-1, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(2000); // Pause for 2 seconds
}