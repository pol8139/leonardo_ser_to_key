#include "HID-Project.h"

#define CIRCLE KEY_K
#define CROSS KEY_J
#define SQUARE KEY_F
#define TRIANGLE KEY_D

#define LL KEY_E
#define LR KEY_R
#define RL KEY_U
#define RR KEY_I

#define LU KEY_UP_ARROW
#define LD KEY_DOWN_ARROW

#define UP KEY_UP_ARROW
#define DOWN KEY_DOWN_ARROW
#define LEFT KEY_LEFT_ARROW
#define RIGHT KEY_RIGHT_ARROW

#define L1 KEY_3
#define L2 KEY_4
#define R1 KEY_8
#define R2 KEY_7

#define L3 KEY_5
#define R3 KEY_6

#define SHARE KEY_G
#define OPTION KEY_H

#define TOUCH KEY_T
#define PS KEY_Y

#define CIRCLE_PIN 2
#define CROSS_PIN 3
#define SQUARE_PIN 4
#define TRIANGLE_PIN 5

#define TIMING_CHECK_PIN 8
#define ENABLE_PIN 9

#define BUTTON_NUM 4

const KeyboardKeycode button_serial_table[8] = {
	LU, LD, L2, R2, LL, LR, RL, RR
};

const KeyboardKeycode button_direct_table[8] = {
	TRIANGLE, SQUARE, CROSS, CIRCLE, 0, 0, 0, 0
};

const int button_direct_pin_table[BUTTON_NUM] = {
	TRIANGLE_PIN, SQUARE_PIN, CROSS_PIN, CIRCLE_PIN
};

uint8_t read_data_byte = 0;
uint8_t button_data_byte = 0;

uint8_t readDirectlyConnectedButtons(void);
void addHIDreportFromTable(uint8_t data_byte, KeyboardKeycode *button_table, int contents_of_table_num);

void setup(void) {
	for(int i = 0; i < BUTTON_NUM; i++) {
		pinMode(button_direct_pin_table[i], INPUT_PULLUP);
	}
	pinMode(ENABLE_PIN, INPUT_PULLUP);
	pinMode(TIMING_CHECK_PIN, OUTPUT);
	Serial1.begin(115200);
	NKROKeyboard.begin();
}

void loop(void) {
	digitalWrite(TIMING_CHECK_PIN, 1);
	if(!digitalRead(ENABLE_PIN)) {
		read_data_byte = Serial1.read();
		button_data_byte = readDirectlyConnectedButtons();
		digitalWrite(TIMING_CHECK_PIN, 0);
		if(read_data_byte != 0xFF) {	// シリアルで何らかの信号を受け取ったとき...
			addHIDreportFromTable(read_data_byte, button_serial_table, 8);
		}
		addHIDreportFromTable(button_data_byte, button_direct_table, BUTTON_NUM);
	} else {
		NKROKeyboard.releaseAll();
	}
	NKROKeyboard.send();
}

uint8_t readDirectlyConnectedButtons(void) {
	uint8_t result = 0;
	for(int i = 0; i < BUTTON_NUM; i++) {
		result |= ((!digitalRead(button_direct_pin_table[i])) << (7 - i));
	}
	return result;
}

void addHIDreportFromTable(uint8_t data_byte, KeyboardKeycode *button_table, int contents_of_table_num) {
	for(int i = 0; i < contents_of_table_num; i++) {
		if((data_byte >> (7 - i)) & 0x01) {
			NKROKeyboard.add(button_table[i]);
		} else {
			NKROKeyboard.remove(button_table[i]);
		}
	}
}
