
#ifndef ARDUINO_ANALOGMEASUREMENT_H
#define ARDUINO_ANALOGMEASUREMENT_H
#include <Arduino.h>

class analogMeasurement  
{
	private:
		uint8_t ADCRegisterNum_;
		int maxChangeBetweenMeasurements_;
		int mem_;
		int voltage_;
		void useLowPass();

	public:

		analogMeasurement(uint8_t ADCRegisterNum, int maxChangeBetweenMeasurements);
		~analogMeasurement();

		int giveVoltage();

};
#endif
