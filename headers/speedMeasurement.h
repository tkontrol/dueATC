#ifndef ARDUINO_SPEEDMEASUREMENT_H
#define ARDUINO_SPEEDMEASUREMENT_H
#include <Arduino.h>

class speedMeasurement  
{
	private:
		int interruptInterval_;  // us
		int minRPM_;
		int maxRPM_;
		int longestPeriodToAccept_;
		int shortestPeriodToAccept_;
		float lowerFactorForLowPass_;
		float upperFactorForLowPass_;
		int counter_;
		int periodLength_;  // us
		int calcConst_; // this equals to "60 / pulsesPerRev * 1000000" -> unit = (min*us)/1
		int mem_;
		void useLowPass();


	public:
		speedMeasurement(int interruptInterval, int minRPM, int maxRPM, int maxAllowedPeriodLengthChangeBetweenMeasurements);
		~speedMeasurement();

		void setPulsesPerRev(int amount);
		void increaseCounter();
		void calcPeriodLength();
		int givePeriodLength();
		int giveCounter() const;
		int giveRPM();

};
#endif
