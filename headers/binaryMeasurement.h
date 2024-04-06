
#ifndef ARDUINO_BINARYMEASUREMENT_H
#define ARDUINO_BINARYMEASUREMENT_H
#include <Arduino.h>

#define STATE_TRANSITION_COUNTS 100
#define SINGLECLICK_DETECTION_COUNTS 200

class binaryMeasurement  
{
	private:
		uint8_t inputPin_;
		int counter_;
		bool returnState_;
		bool lastState_;
		bool block_;

	public:
		binaryMeasurement(uint8_t inputPin);
		~binaryMeasurement();

		bool giveState();	
		//bool giveSingleShot();
		void releaseBlock();

};
#endif
