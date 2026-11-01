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
#define ANALOG_X_CORRECTION 128.0
#define ANALOG_Y_CORRECTION 128.0

double joyXerror;
double joyYerror;
double joyXmax = 8;

// Speed Buttons
#define SPEED_1 2
#define SPEED_2 3
#define SPEED_3 4

double lowSpeed = .25;
double midSpeed = 0.5;
double highSpeed = 1;
double currentSpeed = 0;
double speed = 0;

#define DEBOUNCE_DELAY 100

int buttons[3] = { SPEED_1, SPEED_2, SPEED_3 };
double speeds[3] = { lowSpeed, midSpeed, highSpeed };

int lastReading[3] = { HIGH, HIGH, HIGH };  // raw pin value last loop
int lastState[3] = { HIGH, HIGH, HIGH };    // debounced stable state
unsigned long lastDebounceTime[3] = { 0, 0, 0 };

struct analog {
	double x, y;
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
	for (int i = 0; i < 3; i++) {
		pinMode(buttons[i], INPUT_PULLUP);
	}

	Serial.begin(115200);

	joyXerror = readAnalogAxisLevel(JOY_X_Y) - ANALOG_Y_CORRECTION;
	joyYerror = readAnalogAxisLevel(JOY_Y_Y) - ANALOG_Y_CORRECTION;

	matrix.begin();
}

void loop() {
	checkSpeed();

	double inverse = 1 / currentSpeed;
	analog joyX;
	analog joyY;

	joyX.y = readAnalogAxisLevel(JOY_X_Y) - ANALOG_Y_CORRECTION - joyXerror;
	joyY.y = readAnalogAxisLevel(JOY_Y_Y) - ANALOG_Y_CORRECTION - joyYerror;

	speed = joyX.y / joyXmax * currentSpeed;

	Serial.print("Forward/Back:");
	Serial.print(joyX.y);
	Serial.print("\t Left/Right: ");
	Serial.println(joyY.y);
	Serial.println(speed);

	if (joyX.y > 0) {
		matrix.loadFrame(happy);
	} else if (joyX.y < 0) {
		matrix.loadFrame(heart);
	}
}

byte readAnalogAxisLevel(int pin) {
	return map(analogRead(pin), 0, 1023, 0, 255);
}

void checkSpeed() {
	for (int i = 0; i < 3; i++) {
    int reading = digitalRead(buttons[i]);

    // Reset timer ONLY when the raw reading changes (a transition)
    if (reading != lastReading[i]) {
      lastDebounceTime[i] = millis();
    }
    lastReading[i] = reading;

    // Once stable for DEBOUNCE_DELAY, accept the new state
    if ((millis() - lastDebounceTime[i]) > DEBOUNCE_DELAY) {
      if (reading != lastState[i]) {
        lastState[i] = reading;
        if (reading == LOW) {
          currentSpeed = speeds[i];
          Serial.println("New Speed: ");
          Serial.println(currentSpeed);
        }
      }
    }
  }
}