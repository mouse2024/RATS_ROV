// Code from: https://projecthub.arduino.cc/hibit/using-joystick-module-with-arduino-0ffdd4

#include "Arduino_LED_Matrix.h"
ArduinoLEDMatrix matrix;

// Joy Stick X-axis
#define JOY_X_X A5
#define JOY_X_Y A4

// Joy Stick Y-axis
#define JOY_Y_X A3
#define JOY_Y_Y A2

//Default values when axis not actioned
#define ANALOG_X_CORRECTION 128
#define ANALOG_Y_CORRECTION 128

int joyXerror;
int joyYerror;

// Speed Buttons
#define SPEED_1 1
#define SPEED_2 2
#define SPEED_3 3

int lowSpeed = .025;
int midSpeed = 0.5;
int highSpeed = 1;
int currentSpeed = 0;

#define DEBOUNCE_DELAY 100
unsigned long lastDebounceTime = 0;

struct button {
	byte pressed = 0;
};

struct analog {
	short x, y;
};

const uint32_t happy[] = {
	0x19819,
	0x80000001,
	0x81f8000
};
const uint32_t heart[] = {
	0x3184a444,
	0x44042081,
	0x100a0040
};


void setup() {
	pinMode(SPEED_1, INPUT_PULLUP);
	pinMode(SPEED_2, INPUT_PULLUP);
	pinMode(SPEED_3, INPUT_PULLUP);


  attachInterrupt(digitalPinToInterrupt(SPEED_1), buttonInterrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(SPEED_2), buttonInterrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(SPEED_3), buttonInterrupt, RISING);

	Serial.begin(115200);

	joyXerror = readAnalogAxisLevel(JOY_X_Y) - ANALOG_Y_CORRECTION;
	joyYerror = readAnalogAxisLevel(JOY_Y_Y) - ANALOG_Y_CORRECTION;

	matrix.begin();
}

void loop() {
	analog joyX;
	analog joyY;

	joyX.y = readAnalogAxisLevel(JOY_X_Y) - ANALOG_Y_CORRECTION - joyXerror;
	joyY.y = readAnalogAxisLevel(JOY_Y_Y) - ANALOG_Y_CORRECTION - joyYerror;


	Serial.print("Forward/Back:");
	Serial.print(joyX.y);
	Serial.print("\t Left/Right: ");
	Serial.println(joyY.y);

	if (joyX.y > 0) {
		matrix.loadFrame(happy);
	} else if (joyX.y < 0) {
		matrix.loadFrame(heart);
	}
}

byte readAnalogAxisLevel(int pin) {
	return map(analogRead(pin), 0, 1023, 0, 255);
}


void buttonInterrupt() {
  if (millis() - lastDebounceTime > DEBOUNCE_DELAY) {
    if (digitalRead(SPEED_1) == 1 && digitalRead(SPEED_2) == 0 && digitalRead(SPEED_3) == 0) {
      currentSpeed = lowSpeed;
    }
    if (digitalRead(SPEED_2) == 1 && digitalRead(SPEED_1) == 0 && digitalRead(SPEED_3) == 0) {
      currentSpeed = midSpeed;
    }
		if (digitalRead(SPEED_3) == 1 && digitalRead(SPEED_1) == 0 && digitalRead(SPEED_2) == 0) {
			currentSpeed = highSpeed;
		}
    lastDebounceTime = millis();
  }
}