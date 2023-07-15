
#ifndef ARDUINO_SCREENCONTROL_H
#define ARDUINO_SCREENCONTROL_H
#include <Arduino.h>
#include "../lib/U8g2/src/U8g2lib.h"


class screenControl : public U8G2_ST7920_128X64_1_SW_SPI
{

	public:
		screenControl();
		~screenControl();
		void advanceTextScrolls();
		void setBrightness(int brightness);
		int giveBrightness();
		void drawOther();	
		void scrollText(String *text, uint8_t maxChars, int uiLoopsBetweenCharSrolls);

	private:
		int brightness_;	
		int scrollCounter_;

};
#endif

