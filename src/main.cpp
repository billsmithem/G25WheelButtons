#include <Arduino.h>
#include <Keypad.h>
#include <Encoder.h>
#include "Joystick.h"

//#define DEBUG
//#define TESTING

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_GAMEPAD, 12, 0,
  false, false, false, false, false, false,
  false, false, false, false, false);
//Joystick_ Joystick();

const byte ROWS = 3; //four rows
const byte COLS = 3; //three columns
//char keys[ROWS][COLS] = {
//  {0,1,2},
//  {3,4,5},
//  {6,7,8}
//};
// used map above to see what order buttons come up and created map below to set preferred order
char keys[ROWS][COLS] = {
  {4,7,1},
  {5,6,2},
  {3,8,0}
};
byte rowPins[ROWS] = {4, 3, 2}; //connect to the row pinouts of the kpd
byte colPins[COLS] = {7, 6, 5}; //connect to the column pinouts of the kpd

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

Encoder myEnc(0, 1);
// encoder button & directions
const int encoderButton = 9;
const int counterClockwise = 10;
const int clockwise = 11;

const byte encSwitch = 9;
int lastButtonState = 0;
int buttonState = 0;
int debounceCount = 0;
const int buttonDebounce = 4;

#ifdef DEBUG
String msg;
#endif

long oldPosition  = -999;

void setup() {
    Serial.begin(115200);
    Joystick.begin();
    pinMode(encSwitch, INPUT);
    #ifdef DEBUG
    msg = "";
    #endif
}

void loop() {
    void joystickToggle(int button);

    // Fills kpd.key[ ] array with up-to 10 active keys.
    // Returns true if there are ANY active keys.
    if (kpd.getKeys())
    {
        for (int i=0; i<LIST_MAX; i++)   // Scan the whole key list.
        {
            if ( kpd.key[i].stateChanged )   // Only find keys that have changed state.
            {
                if (kpd.key[i].kstate == PRESSED) {
                    Joystick.pressButton((int) kpd.key[i].kchar);
                    #ifdef DEBUG
                        msg = " PRESSED.";
                    #endif
                } else if (kpd.key[i].kstate == RELEASED) {
                    Joystick.releaseButton((int) kpd.key[i].kchar);
                    #ifdef DEBUG
                        msg = " RELEASED.";
                    #endif
                }
#ifdef DEBUG
                Serial.print("Key ");
                Serial.print((int) kpd.key[i].kchar);
                Serial.println(msg);
#endif
            }
        }
    }

    // read rotary encoder
    long newPosition = myEnc.read();
    const int countDiv = 3;
    if (newPosition != oldPosition) {
        if (newPosition > (oldPosition + countDiv)) {
            joystickToggle(counterClockwise);
            oldPosition = newPosition;
            #ifdef DEBUG
            Serial.println(newPosition);
            #endif
        } else if (newPosition < (oldPosition - countDiv)) {
            joystickToggle(clockwise);
            oldPosition = newPosition;
            #ifdef DEBUG
            Serial.println(newPosition);
            #endif
        }
    }

    // read rotary encoder switch
    buttonState = digitalRead(encSwitch);
    if (buttonState != lastButtonState) {
        if (++debounceCount > buttonDebounce) {
            if (buttonState == 0) {
                Joystick.pressButton(encoderButton);
                Serial.println(" PRESSED.");
            } else {
                Joystick.releaseButton(encoderButton);
                Serial.println(" RELEASED.");
            }
            lastButtonState = buttonState;
            debounceCount = 0;
        }
    }
}  // End loop

#ifdef TESTING
const int delayTime = 200;      // slow it down so I can see it onscreen
#else
const int delayTime = 12;
#endif
void joystickToggle(int button) {
    Joystick.pressButton(button);       // button pressed
    delay(delayTime);
    Joystick.releaseButton(button);       // button released
}