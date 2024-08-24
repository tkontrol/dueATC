#ifndef ARDUINO_CORE_H
#define ARDUINO_CORE_H
#include <Arduino.h>
#include <DueTimer.h>
#include <../headers/speedMeasurement.h>
#include <../headers/configHandler.h>
#include <../headers/analogMeasurement.h>
#include <../headers/TCCcontrol.h>
#include <../headers/binaryMeasurement.h>
#include <../headers/shiftControl.h>

#define brakePedalSwitchPin 27
#define PswitchPin 25
#define RswitchPin 23
//#define gearPlusPin 50
//#define gearMinusPin 52

#define MALFUNCTIONMEMORY_MAX_SIZE 10 // how many malfunctions can be stored
#define LOGGER_MAX_SIZE 500 // max cells in log struct
#define SOL_12_45 40 //pin for 1-2/4-5 shift solenoid
#define SOL_23 42 //pin for 2-3 shift solenoid
#define SOL_34 44 //pin for 3-4 shift solenoid


class core  
{
	private:
		enum vehicleSpeedMeasureMode {primary, secondary, both};
		vehicleSpeedMeasureMode measMode_;

		speedMeasurement engineSpeedMeas_;
		speedMeasurement primaryVehicleSpeedMeas_;
		speedMeasurement secondaryVehicleSpeedMeas_;
		speedMeasurement n2SpeedMeas_;
		speedMeasurement n3SpeedMeas_;

		analogMeasurement oilTemp_PN_Meas_;
		analogMeasurement TPS_Meas_;
		analogMeasurement MAP_Meas_;

		binaryMeasurement brakePedal_;
		binaryMeasurement Pswitch_;
		binaryMeasurement Rswitch_;

		shiftControl shiftControl_;
    	configHandler config_;
		TCCcontrol TCCcontrol_;

		bool configOK_ = false;
		bool overridePressureValues_ = false;
		uint8_t overridedMPCValue_;
		uint8_t overridedSPCValue_;

		int wheelCircum_;
		int driveShaftPulsesPerRev_;
		int finalDriveRatioX100_;
		int startupCounter_;
		uint8_t currentGear_;
		uint8_t targetGear_;
		uint8_t autoModeTargetGear_;
		uint8_t measuredGear_;

		bool brakePedalSwitchState_;
		bool parkSwitchState_;
		bool reverseSwitchState_;
		int engineSpeed_;
		int vehicleSpeed_;
		int primaryVehicleSpeed_;
		int secondaryVehicleSpeed_;
		int cardanShaftSpeed_;
		int inputShaftSpeed_;
		int n2Speed_;
		int n3Speed_;
		int tcSlip_;
		int oilTemp_;
		int oilTemp_PN_sens_resistance_;
		int TPS_;
		int TPSVoltage_;
		int TPSdelayed_;
		int MAP_;
		int MAPVoltage_;
		uint8_t load_;
		uint8_t MPC_;
		uint8_t SPC_;
		float n3n2Ratio_;
		//float transmissionRatio_;

		struct transmissionRatio
		{
			float ratio;
			bool isValid;
		};

		transmissionRatio transmissionRatio_;

		int logTimerCounter_;
		bool logging_;
		int shiftTimer_;
		bool useGearRatioDetection_;
		int minimumVehicleSpeedForGearRatioDetection_;
		bool shifting_;
		bool gearUpReq_;
		bool gearDownReq_;

		bool gearUpComm_;
		bool gearDownComm_;

		bool shiftPermission_;
		bool usePreShiftDelay_ = false;
		int preShiftDelay_ = 1500;
		bool startWith1StGear_;
		int lastShiftDuration_;	
		bool acceptMeasuredGearAsCurrentGearAfterDelay_;
		int delayToAcceptMeasuredGearAsCurrentGear_;
		bool currentGearMatchesMeasuredGear_;
		int delayToCurrentGearEqualsMeasuredGearToAcceptAutoShift_;

		bool notificationTimerOn_;
		int notificationTimerCounter_;
		int notificationTimerTargetTime_;

		struct notificationStruct
		{
			int time;
			bool pending;
			bool isShown;
			String message;
		};

		struct notificationStruct notification_;
		configHandler::parameterContainer* parametersPtr_;

		struct malfunctionMemory
		{
			bool codes[MALFUNCTIONMEMORY_MAX_SIZE]; // position in array equals code
			String descriptions[MALFUNCTIONMEMORY_MAX_SIZE];
			bool activeMalfunctions;		
		};

		struct malfunctionMemory malfunctions_;
		void activateMalfunction(uint8_t code);

		// for test purposes
		int testcounter_;
		float prevRatio_;
		unsigned long int clock_;

	public:
		core(int speedMeasInterruptInterval, int engineSpeedPin, int primaryVehicleSpeedPin, int secondaryVehicleSpeedPin, int n2SpeedPin, int n3SpeedPin);
		~core();

		enum loggableVariable {engineSpeed, vehicleSpeed, primAndSecVehSpds, n2Andn3Speed, n3n2Ratio, transmissionRatio, TCSlipAndTCControl, oilTemp, TPSes};
		loggableVariable variableToBeLogged_;
		enum leverPos {P, R, N, D, undef};
		leverPos lever_;
		bool dOrRengaged_;
		enum shiftingMode {MAN, AUT};
		shiftingMode shiftingMode_;

		TCCcontrol::TCCMode tccMode_;
		configHandler::driveType driveType_;
		
		char lastMPCchange_;
		char lastSPCchange_;

		struct dataStruct
		{
			uint8_t* currentGear;
			uint8_t* targetGear;
			uint8_t* autoModeTargetGear;
			uint8_t* measuredGear;
			int* engineSpeed;
			int* vehicleSpeed;
			int* primaryVehicleSpeed;
			int* secondaryVehicleSpeed;
			bool* brakePedalSwitch;
			bool* parkSwitch;
			bool* reverseSwitch;
			uint8_t* overridedSPCValue;
			uint8_t* overridedMPCValue;
			bool* overridePressureValues;
			leverPos* leverPosition;
			shiftingMode* shiftingMod;
			TCCcontrol::TCCMode* tccMod;
			configHandler::driveType* dType;
			int* n2Speed;
			int* n3Speed;
			int *tcSlip;
			int* cardanSpeed;
			int* oilTemp;
			int* oilTemp_PN_sens_res;
			int* TPS;
			int* TPSVoltage;
			int* MAP;
			int* MAPVoltage;
			uint8_t* MPC;
			uint8_t* SPC;
			uint8_t* load;
			float* n3n2Ratio;
			float* transmissionRatio;		
			int* lastShiftDuration;	
			bool* shifting;
			char* MPCchange;
			char* SPCchange;
			int* tccControlOutput;
			malfunctionMemory* malfuncs;
		};	

		struct dataStruct data_ = {};

		struct logStruct
		{
			int amount;
			int xData[LOGGER_MAX_SIZE];
			int y1Data[LOGGER_MAX_SIZE];
			float y2Data[LOGGER_MAX_SIZE];
		};

		struct logStruct log_;

		
		void initController();
		void coreloop();
		void startupProcedure();
		void applyParameters();
		void updateParameter(configHandler::parameter* p);
		String readConfFile();
		String readDefaultsFile();
		String writeConfFile();
		void printData();
		void increaseSpeedMeasCounters();
		void engineSpeedMeas();
		void primaryVehicleSpeedMeas();
		void secondaryVehicleSpeedMeas();
		void n2SpeedMeas();
		void n3SpeedMeas();
		void updateSpeedMeasurements();		
		void detectDriveType();
		void updateAnalogMeasurements();
		void calculateTPSdelayed();
		void updateLeverPosition();
		void updateGearByN3N2Ratio();
		void checkIfCurrentGearEqualsMeasuredGear();
		void shiftTo1stInP();
		void updateCurrentGearByMeasuredGear();
		void forceGearVariables();
		void doAutoShifts();
		void updateLog();
		void modifyLastShiftMaps(int MPCchange, int SPCchange);
		void setLoggableVariable(core::loggableVariable var);
		void setLoggingStatus(bool state);
		void startNotificationTimer(int time); // time in ms
		bool hasNotificationTimePassed();
		struct notificationStruct checkNotification();
		void markNotificationAsReceived();
		void markNotificationAsShown();
		void showNotification(int time, String msg);
		struct configHandler::configurationSet* givePtrToConfigurationSet();

		struct logStruct* giveLogPtr();
		struct dataStruct giveDataPointers();

		void gearUpRequest();
		void gearDownRequest();

		void makeUpShiftCommand();
		void makeDownShiftCommand();
		bool toggleAutoMan();
		void toggleRatioDetection();
		bool giveMalfunctionStatus();
		void clearFaultCodes();

};
#endif
