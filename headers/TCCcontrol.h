
#ifndef ARDUINO_TCCCONTROL_H
#define ARDUINO_TCCCONTROL_H
#include <Arduino.h>

class TCCcontrol  
{
	private:

		int* engineSpeed_;
		int* incomingShaftSpeed_;

		int setPoint_ = 50;
		int output_;

		int kp_;
		int ki_;
		
		int outputLowerLimit_;
		int outputUpperLimit_;

	public:

		TCCcontrol();
		~TCCcontrol();

		enum TCCMode {open, slipping, closed};
		TCCMode* TCCmode_;

		void setMeasurementPointers(int &engineSpeed, int &incomingShaftSpeed);
		int* giveOutputPointer();
		void setOutputLimits(int lowerLimit, int upperLimit);
		void setKP(int kp);
		void setKI(int ki);
		void setTCCmode(TCCcontrol::TCCMode &mode);

		void updateTCC();
		int givePIOutput();

};
#endif
