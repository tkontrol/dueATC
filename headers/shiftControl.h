
#ifndef ARDUINO_SHIFTCONTROL_H
#define ARDUINO_SHIFTCONTROL_H
#include <Arduino.h>
#include <../headers/configHandler.h>

#define SOL_12_45 40 //pin for 1-2/4-5 shift solenoid
#define SOL_23 42 //pin for 2-3 shift solenoid
#define SOL_34 44 //pin for 3-4 shift solenoid

class shiftControl  
{
	private:
		configHandler* config_;
		configHandler::driveType* driveType_;
		uint8_t* MPC_;
		uint8_t* SPC_;
		int* oilTemp_;
		uint8_t* load_;
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
		uint8_t checkIfTransmissionRatioMatchesAnyGear();
		bool checkIfTransmissionRatioMatchesForGear(uint8_t gear);
		void controlPressureSolenoids();

	public:

		shiftControl();
		~shiftControl();

		void initShiftControl(configHandler &configHandler, uint8_t &MPC, uint8_t &SPC, configHandler::driveType &driveType, int &oilTemp, uint8_t &load,
 		uint8_t &currentGear, uint8_t &targetGear, bool &shifting, int &lastShiftDuration, float &transmissionRatio, bool &useGearRatioDetection, bool &shiftPermission);
		void runShifts();

};
#endif
