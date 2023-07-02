
#ifndef ARDUINO_SCREENCONTROL_H
#define ARDUINO_SCREENCONTROL_H
#include <Arduino.h>
#include "../lib/U8g2/src/U8g2lib.h"


class screenControl : public U8G2_ST7920_128X64_1_SW_SPI
{

	public:
		screenControl();
		~screenControl();
		void drawLiveData2();
		void setBrightness(int brightness);
		int giveBrightness();
		void drawOther();	
//		void setDataPointers(core::dataStruct data);	

	private: /*
		int* engineSpeed_;
		int* vehicleSpeed_;
		int* n2Speed_;
		int* n3Speed_;
		int* cardanSpeed_;
		float* n3n2Ratio_;	 */

		int brightness_;

	

};
#endif

