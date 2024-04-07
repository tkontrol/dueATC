
#ifndef ARDUINO_SHIFTCONTROL_H
#define ARDUINO_SHIFTCONTROL_H
#include <Arduino.h>

#define SOL_12_45 40 //pin for 1-2/4-5 shift solenoid
#define SOL_23 42 //pin for 2-3 shift solenoid
#define SOL_34 44 //pin for 3-4 shift solenoid

class shiftControl  
{
	private:
		uint8_t* currentGear_;
		uint8_t currentGearForShift_;
		uint8_t* targetGear_;
		uint8_t nextGear_;
		bool* shifting_;
		int shiftTimer_;

		int* lastShiftDuration_;
		float* transmissionRatio_;
		bool* useGearRatioDetection_;
		bool useGearRatioDetectionForShift_;
		
		bool* shiftPermission_;

		void activateSolenoids();
		bool checkIfTransmissionRatioMatchesForGear(uint8_t gear);

	public:

		shiftControl();
		~shiftControl();

		void initShiftControl(uint8_t &currentGear, uint8_t &targetGear, bool &shifting, int &lastShiftDuration, float &transmissionRatio, bool &useGearRatioDetection, bool &shiftPermission);
		void runShifts();

};
#endif
