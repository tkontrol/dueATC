#ifndef ARDUINO_MAPHANDLER_H
#define ARDUINO_MAPHANDLER_H
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

#define DUAL_AXIS_MAP_MAXIMUM_ROWS 13
#define DUAL_AXIS_MAP_MAXIMUM_COLS 11

#define SINGLE_AXIS_MAP_MAXIMUM_COLS 22

class configHandler  
{
	public:

		configHandler();
		~configHandler();

		enum shiftType {
		MPC_1to2, MPC_2to3, MPC_3to4, MPC_4to5,
		MPC_5to4, MPC_4to3, MPC_3to2, MPC_2to1,

		SPC_1to2, SPC_2to3, SPC_3to4, SPC_4to5,
		SPC_5to4, SPC_4to3, SPC_3to2, SPC_2to1,
		};		 

		enum driveType {load, coast};

		struct dualAxisMap
		{
			String ID;
			bool readFromSD;
			uint8_t rows;
			uint8_t columns;
			int rowTitles[DUAL_AXIS_MAP_MAXIMUM_ROWS];
			int columnTitles[DUAL_AXIS_MAP_MAXIMUM_COLS];
			int data[DUAL_AXIS_MAP_MAXIMUM_ROWS][DUAL_AXIS_MAP_MAXIMUM_COLS];
			int minData;
			int maxData;
		};

		struct dualAxisMapContainer
		{
			uint8_t size;
			dualAxisMap* maps;
		};

		struct singleAxisMap
		{
			String ID;
			bool readFromSD;
			uint8_t columns;
			int columnTitles[SINGLE_AXIS_MAP_MAXIMUM_COLS];
			int data[SINGLE_AXIS_MAP_MAXIMUM_COLS];
			int minData;
			int maxData;
		};

		struct singleAxisMapContainer
		{
			uint8_t size;
			singleAxisMap* maps;
		};

		struct parameter
		{
			String ID;
			bool readFromSD;
			int data;
			int minData;
			int maxData;
		};

		struct parameterContainer
		{
			uint8_t size;
			parameter* parameters;
		};

		struct configurationSet
		{
			struct dualAxisMapContainer* dualAxisMaps;
			struct singleAxisMapContainer* singleAxisMaps;
			struct parameterContainer* parameters;
		};

		void initMaps();
		void printObjectsToSerial();
		void setObjectIDs();
		int giveShiftMapValue(shiftType stype, driveType dtype, int oil, int load);
		void changeShiftMapValue(dualAxisMap* map, int rowVal, int colVal, int newVal);
		void modifyLastShiftMaps(int MPCchange, int SPCchange);
		uint8_t giveAutoModeTargetGear(int vehicleSpeed, uint8_t currentGear, int rowVal);
		int giveRegularMPCMapValue(int rowVal, int colVal);
		int giveOilTempValue(int resistance);
		int giveTPSValue(int voltage);
		int giveMAPValue(int voltage);
		int giveEngSpdLoadFactorValue(int engineSpeed);
		int giveShiftTimeTargetValue(int load);
		struct dualAxisMap* identifyShiftMap(shiftType stype, driveType dtype);
		int readDualAxisMap(dualAxisMap* map, int rowVal, int colVal);
		int readSingleAxisMap(singleAxisMap* map, int colVal);
		int readInverseSingleAxisMap(singleAxisMap* map, int colVal);
		int linearizeBetweenTwoValues(int x1, int x2, int xVal, int y1, int y2);
		int linearlyInterpolate(float x1, float x2, float xVal, float y1, float y2);
		void printDualAxisMapToSerial(shiftType stype, driveType dtype);
		String readSDCardToDataStructs(char filename[]);
		String writeDataStructsToSDCard(char filename[]);
		void clearConfigReadFlags();
		bool checkConfigStatus();
		struct configurationSet* givePtrToConfigurationSet();	


		dualAxisMap* lastMPCmap_;
		dualAxisMap* lastSPCmap_;
		int lastShiftLoad_;
		int lastShiftOilTemp_;
		driveType lastShiftDriveType_;
		int lastMPCval_;
		int lastSPCval_;

		shiftType lastShiftMPCType_;
		shiftType lastShiftSPCType_;

	private:
		File myFile_;

		const int chipSelectPin_ = 46;

		// Maps
		dualAxisMap Automode_gearMap_;
		dualAxisMap Shift_solenoid_time_map_;

		dualAxisMap MPC_regularDrive_;
		
		dualAxisMap MPC_1to2_load_;
		dualAxisMap MPC_2to3_load_;
		dualAxisMap MPC_3to4_load_;
		dualAxisMap MPC_4to5_load_;
		dualAxisMap MPC_5to4_load_;
		dualAxisMap MPC_4to3_load_;
		dualAxisMap MPC_3to2_load_;
		dualAxisMap MPC_2to1_load_;
        dualAxisMap MPC_1to2_coast_;
		dualAxisMap MPC_2to3_coast_;
		dualAxisMap MPC_3to4_coast_;
		dualAxisMap MPC_4to5_coast_;
		dualAxisMap MPC_5to4_coast_;
		dualAxisMap MPC_4to3_coast_;
		dualAxisMap MPC_3to2_coast_;
		dualAxisMap MPC_2to1_coast_;

		dualAxisMap SPC_1to2_load_;
		dualAxisMap SPC_2to3_load_;
		dualAxisMap SPC_3to4_load_;
		dualAxisMap SPC_4to5_load_;
		dualAxisMap SPC_5to4_load_;
		dualAxisMap SPC_4to3_load_;
		dualAxisMap SPC_3to2_load_;
		dualAxisMap SPC_2to1_load_;
        dualAxisMap SPC_1to2_coast_;
		dualAxisMap SPC_2to3_coast_;
		dualAxisMap SPC_3to4_coast_;
		dualAxisMap SPC_4to5_coast_;
		dualAxisMap SPC_5to4_coast_;
		dualAxisMap SPC_4to3_coast_;
		dualAxisMap SPC_3to2_coast_;
		dualAxisMap SPC_2to1_coast_;

		const uint8_t dualAxisMapsAmount_ = 35; // remember to update after adding maps

		dualAxisMap* dualAxisMaps_[35] = {
		 &Automode_gearMap_, &Shift_solenoid_time_map_, &MPC_regularDrive_,
		 &MPC_1to2_load_, &MPC_2to3_load_, &MPC_3to4_load_, &MPC_4to5_load_, &MPC_5to4_load_, &MPC_4to3_load_, &MPC_3to2_load_, &MPC_2to1_load_,
		 &MPC_1to2_coast_, &MPC_2to3_coast_, &MPC_3to4_coast_, &MPC_4to5_coast_, &MPC_5to4_coast_, &MPC_4to3_coast_, &MPC_3to2_coast_, &MPC_2to1_coast_,
		 &SPC_1to2_load_, &SPC_2to3_load_, &SPC_3to4_load_, &SPC_4to5_load_, &SPC_5to4_load_, &SPC_4to3_load_, &SPC_3to2_load_, &SPC_2to1_load_,
		 &SPC_1to2_coast_, &SPC_2to3_coast_, &SPC_3to4_coast_, &SPC_4to5_coast_, &SPC_5to4_coast_, &SPC_4to3_coast_, &SPC_3to2_coast_, &SPC_2to1_coast_
		};		

		singleAxisMap shiftTimeTargetMap_;
		singleAxisMap engSpdLoadFactorMap_;
		singleAxisMap TPSLinearizationMap_;
		singleAxisMap MAPLinearizationMap_;
		singleAxisMap oilTempMap_;
		

		const uint8_t singleAxisMapsAmount_ = 5; // remember to update after adding maps

		singleAxisMap* singleAxisMaps_[5] = {&shiftTimeTargetMap_, &engSpdLoadFactorMap_, &TPSLinearizationMap_, &MAPLinearizationMap_, &oilTempMap_};

		parameter tccControlPfactor_;
		parameter tccControlIfactor_;	
		parameter startWith1StGear_;
		parameter minimumVehicleSpeedForGearRatioDetection_;
		parameter wheelCircum_;
		parameter brightness_;
		parameter engineSpeedPulsesPerRev_;
		parameter driveShaftPulsesPerRev_;
	

		const uint8_t parametersAmount_ = 8; // remember to update after adding params

		parameter* parameters_[8] = {&tccControlPfactor_, &tccControlIfactor_, &startWith1StGear_ ,&minimumVehicleSpeedForGearRatioDetection_, &wheelCircum_, 
		&brightness_, &engineSpeedPulsesPerRev_, &driveShaftPulsesPerRev_};


		// containers
		dualAxisMapContainer dualAxisMapContainer_ = {dualAxisMapsAmount_, *dualAxisMaps_};
		singleAxisMapContainer singleAxisMapContainer_ = {singleAxisMapsAmount_, *singleAxisMaps_};
		parameterContainer parameterContainer_ = {parametersAmount_, *parameters_};
		configurationSet configSet_ = {&dualAxisMapContainer_, &singleAxisMapContainer_, &parameterContainer_};
};
#endif
