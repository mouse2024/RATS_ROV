// Code from: https://projecthub.arduino.cc/hibit/using-joystick-module-with-arduino-0ffdd4

#include "Arduino_LED_Matrix.h"
ArduinoLEDMatrix matrix;

// Joy Stick X-axis
#define JOY_X_X A5
#define JOY_X_Y A4
#define JOY_X_BUTTON 2
// Joy Stick Y-axis
#define JOY_Y_X A3
#define JOY_Y_Y A2
#define JOY_Y_BUTTON 3

//Default values when axis not actioned
#define ANALOG_X_CORRECTION 128
#define ANALOG_Y_CORRECTION 128

int joyXerror;
int joyYerror;

struct button {
	byte pressed = 0;
};

struct analog {
	short x, y;

	button buttonA;
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
	pinMode(JOY_X_BUTTON, INPUT_PULLUP);
	pinMode(JOY_Y_BUTTON, INPUT_PULLUP);

	Serial.begin(115200);

	joyXerror = readAnalogAxisLevel(JOY_X_Y) - ANALOG_Y_CORRECTION;
	joyYerror = readAnalogAxisLevel(JOY_Y_Y) - ANALOG_Y_CORRECTION;

	matrix.begin();
}

void loop() {
	analog joyX;
	analog joyY;

	joyX.y = readAnalogAxisLevel(JOY_X_Y) - ANALOG_Y_CORRECTION - joyXerror;
	//joyX.buttonA.pressed = isAnalogButtonPressed(JOY_X_BUTTON);
	joyY.y = readAnalogAxisLevel(JOY_Y_Y) - ANALOG_Y_CORRECTION - joyYerror;


	Serial.print("Forward/Back:");
	Serial.print(joyX.y);
	Serial.print("\t Left/Right: ");
	Serial.print(joyY.y);

	if (joyX.y > 0) {
		matrix.loadFrame(happy);
	} else if (joyX.y < 0) {
		matrix.loadFrame(heart);
	}

	//  if (joyX.buttonA.pressed) {
	//    Serial.println("X-Axis pressed");
	//  } else {
	//    Serial.println("Button not pressed");
	//  }
}

byte readAnalogAxisLevel(int pin) {
	return map(analogRead(pin), 0, 1023, 0, 255);
}

// bool isAnalogButtonPressed(int pin)
// {
// 	 return digitalRead(pin) == 0;
// }
