#ifndef ARDUINO_CONTROLBUTTON_H
#define ARDUINO_CONTROLBUTTON_H
#include <Arduino.h>

#define SINGLECLICK_DETECTION_COUNTS 4 // how many program loops the button has to be pressed down to read it as single click (low-pass filter)
#define REPEAT_DETECTION_COUNTS 17 // how many program loops the button has to be pressed down to read it repeatedly again (= how fast the repeats are, smaller number -> faster)
#define FASTREPEAT_LIMIT 10 // how many repeats have to be read until entering the fast repeat mode
#define FASTREPEAT_DETECTION_COUNTS 4 // how many program loops the button has to be pressed down to read it in fast repeat mode (= how fast the fast repeats are)

class controlButton  
{
	public:
		controlButton(int buttonPin, bool enableRepeat = false);
		~controlButton();

		bool givePulse();
		bool giveSingleShot();
		void releaseBlock();
		bool isPressed();


	private:
		const int buttonPin_;
		const bool repeatable_;			
		bool buttonLastState_;	
		bool buttonLastState1_;	
		bool buttonState_;
		bool stateUsed_;
		int buttonCounter_;
		int blockCounter_;
		int countLimit_ = SINGLECLICK_DETECTION_COUNTS;
		int fastRepeatCounter_;
		bool block_;
		bool buttonReturnedTrue_;
};
#endif