
#ifndef ARDUINO_UI_H
#define ARDUINO_UI_H
#include <Arduino.h>
#include "../headers/controlButton.h"
#include "../headers/screenControl.h"
#include "../headers/core.h"
#include "../headers/configHandler.h"

#define autoModeSwitchPin 28
#define upButtonPin 17
#define downButtonPin 16
#define leftButtonPin 15
#define rightButtonPin 14
#define okButtonPin 19
#define cancelButtonPin 18


class ui
{
	private:

		core::dataStruct dataPtrs_;
		configHandler::dualAxisMapContainer* dualAxisMapsPtr_;
		configHandler::singleAxisMapContainer* singleAxisMapsPtr_;
		configHandler::parameterContainer* parametersPtr_;
		

		enum variableTypes {oilTempAndLoad};

		void setScreenDataPointers();
		void setDataPointers(core::dataStruct data);
		void showNotification(int time, String msg);
		void checkForMalfunction();
		void checkPendingCoreNotifications();
		void printNotification();
		void updateParameter(configHandler::parameter* p);
		struct configHandler::dualAxisMap* givePtrToDualAxisMap(String name);
		struct configHandler::singleAxisMap* givePtrToSingleAxisMap(String name);

		core core_;
		screenControl screen_;
		controlButton upButton_;
		controlButton downButton_;
		controlButton leftButton_;
		controlButton rightButton_;
		controlButton okButton_;
		controlButton cancelButton_;
		controlButton autoModeSwitch_;

		int loopTimeCounter_;
		int loopsPerSecond_;
		int plotTime_ = 10; // default time for plot, seconds
		float yLeftMax_ = 0;
		float yRightMax_ = 0;	

		unsigned long int clock_;
		unsigned long int loopStartTime_;
		unsigned long int loopEndTime_;

		int menuItemIndex_; // for menu
		bool menuFuncExecuting_; // for menu
		core::logStruct* log_; // for logger
		uint8_t editorRow_ = 0; // for map editor
		uint8_t editorColumn_ = 0; // for map editor
		bool editingMode_; // for map editor

	
		bool notificationActive_;
		String notificationMessage_;
		
		// Menu structure
		typedef bool (ui::*itemf)();

 		struct menuObj
		{
			char title[30];
			itemf function;
		};
 
		struct menuCollection
		{
			uint8_t size;
			uint8_t selection;
			struct menuObj* menuObj;
		}; 

		menuCollection* currentMenu_;

		// to save menu and selection
		menuCollection* fallBackMenu_;
    	uint8_t fallBackSelection_;

		// Main menu
		menuObj mainM_[5] = {
			{"Main Screen", &ui::showMainScreen},
			{"Settings", &ui::goToSettingsMenu},
			{"Live Data", &ui::goToLiveDataMenu},
			{"TCC PI ctrl tune", &ui::showTCCTuneView},
			{"Fault codes", &ui::showMalfunctionCodes}
		}; 
		menuCollection mainMenu_ = {sizeof(mainM_)/sizeof(mainM_[0]), 0, &*mainM_};

		// Settings menu
		menuObj settingsM_[6] = {
			{"Dual-axis maps", &ui::goToDualAxisMapEditorMenu},
			{"Single-axis maps", &ui::goToSingleAxisMapEditorMenu},
			{"Parameters", &ui::showParamEditor},
			{"Button tests", &ui::buttonTests},
			{"SD card actions", &ui::goToSDMenu},
			{"Back to MainMenu", &ui::goToMainMenu}
		}; 
		menuCollection settingsMenu_ = {sizeof(settingsM_)/sizeof(settingsM_[0]), 0, &*settingsM_};	

		// Live data menu
		menuObj liveDatamM_[6] = {
			{"RPM measurements", &ui::showLiveData1},
			{"RPMs & Ratios", &ui::showLiveData2},
			{"Analog measurements", &ui::showLiveData3},
			{"Binary measurements", &ui::showLiveData4},
			{"Data Plotter", &ui::goToDataPlotterMenu},
			{"Back to MainMenu", &ui::goToMainMenu}
		}; 
		menuCollection liveDataMenu_ = {sizeof(liveDatamM_)/sizeof(liveDatamM_[0]), 0, &*liveDatamM_};

		// Data plotter menu
		menuObj dataPlottermM_[9] = {
			{"Engine speed", &ui::plotEngineSpeed},
			{"Vehicle speed", &ui::plotVehicleSpeed},
			{"Prim&Sec veh.spd", &ui::plotPrimAndSecSpds},
			{"n2, n3 speeds", &ui::plotn2n3Speeds},			
			{"n3/n2 ratio", &ui::plotn2n3Ratio},
			{"Transmission ratio", &ui::plotTransmissionRatio},
			{"TC slip & control", &ui::plotTCSlipAndCtrl},
			{"Oil temp", &ui::plotOilTemp},
			{"Back to LiveData", &ui::goToLiveDataMenu}
		}; 
		menuCollection dataPlotterMenu_ = {sizeof(dataPlottermM_)/sizeof(dataPlottermM_[0]), 0, &*dataPlottermM_};

		// Map editor menu
		menuObj mapEditorM_[4] = {
			{"AutoMode gear", &ui::editAutoModeWantedGearMap},
			{"MPC maps", &ui::goToMPCMapMenu},
			{"SPC maps", &ui::goToSPCMapMenu},
			{"Back to settings", &ui::goToSettingsMenu}
		}; 
		menuCollection mapEditorMenu_ = {sizeof(mapEditorM_)/sizeof(mapEditorM_[0]), 0, &*mapEditorM_};

		// SD Card menu
		menuObj SDM_[4] = {
			{"Re-read config file", &ui::readConfigFile},
			{"Write RAM to file", &ui::overWriteConfigFile},
			{"Restore def. conf", &ui::restoreDefaultConfigFile},
			{"Back to settings", &ui::goToSettingsMenu}
		}; 
		menuCollection SDMenu_ = {sizeof(SDM_)/sizeof(SDM_[0]), 0, &*SDM_};

		// MPC maps editor menu
		menuObj MPCM_[18] = {
			{"MPC normal", &ui::editMPCNormalDriveMap},
			{"load, 1->2", &ui::editMPC1to2loadMap},
			{"load, 2->3", &ui::editMPC2to3loadMap},
			{"load, 3->4", &ui::editMPC3to4loadMap},
			{"load, 4->5", &ui::editMPC4to5loadMap},
			{"load, 5->4", &ui::editMPC5to4loadMap},
			{"load, 4->3", &ui::editMPC4to3loadMap},
			{"load, 3->2", &ui::editMPC3to2loadMap},
			{"load, 2->1", &ui::editMPC2to1loadMap},
			{"coast, 1->2", &ui::editMPC1to2coastMap},
			{"coast, 2->3", &ui::editMPC2to3coastMap},
			{"coast, 3->4", &ui::editMPC3to4coastMap},
			{"coast, 4->5", &ui::editMPC4to5coastMap},
			{"coast, 5->4", &ui::editMPC5to4coastMap},
			{"coast, 4->3", &ui::editMPC4to3coastMap},
			{"coast, 3->2", &ui::editMPC3to2coastMap},
			{"coast, 2->1", &ui::editMPC2to1coastMap},
			{"Back to editor menu", &ui::goToDualAxisMapEditorMenu}
		}; 
		menuCollection MPCMenu_ = {sizeof(MPCM_)/sizeof(MPCM_[0]), 0, &*MPCM_};

		// SPC maps editor menu
		menuObj SPCM_[17] = {
			{"load, 1->2", &ui::editSPC1to2loadMap},
			{"load, 2->3", &ui::editSPC2to3loadMap},
			{"load, 3->4", &ui::editSPC3to4loadMap},
			{"load, 4->5", &ui::editSPC4to5loadMap},
			{"load, 5->4", &ui::editSPC5to4loadMap},
			{"load, 4->3", &ui::editSPC4to3loadMap},
			{"load, 3->2", &ui::editSPC3to2loadMap},
			{"load, 2->1", &ui::editSPC2to1loadMap},
			{"coast, 1->2", &ui::editSPC1to2coastMap},
			{"coast, 2->3", &ui::editSPC2to3coastMap},
			{"coast, 3->4", &ui::editSPC3to4coastMap},
			{"coast, 4->5", &ui::editSPC4to5coastMap},
			{"coast, 5->4", &ui::editSPC5to4coastMap},
			{"coast, 4->3", &ui::editSPC4to3coastMap},
			{"coast, 3->2", &ui::editSPC3to2coastMap},
			{"coast, 2->1", &ui::editSPC2to1coastMap},
			{"Back to editor menu", &ui::goToDualAxisMapEditorMenu}
		}; 
		menuCollection SPCMenu_  = {sizeof(SPCM_)/sizeof(SPCM_[0]), 0, &*SPCM_};

		/*

		// Malfunctions menu, hidden!
		menuObj malfM_[1] = {
			{"1", &ui::showMalfunctionCode1},
		}; 
		menuCollection malfunctionMenu_ = {sizeof(malfM_)/sizeof(malfM_[0]), 0, &*malfM_};*/

	public:
		ui();
		~ui();

		void increaseSpeedMeasCounters();
		void engineSpeedMeas();
		void primaryVehicleSpeedMeas();
		void secondaryVehicleSpeedMeas();
		void n2SpeedMeas();
		void n3SpeedMeas();

		void runCoreLoop();
		void runUiLoop();
		void printLoopDuration();
		void initUI();

		void updateMenu();		
		void firstPage(); // for picture loop
		bool nextPage(); // for picture loop
		bool showMainScreen();
		void drawMainScreen();
		bool showLiveData1();
		bool showLiveData2();
		bool showLiveData3();
		bool showLiveData4();
		void drawLiveData1();
		void drawLiveData2();
		void drawLiveData3();
		void drawLiveData4();
		void adjustPlotTime();
		bool plotEngineSpeed();
		bool plotVehicleSpeed();
		bool plotPrimAndSecSpds();
		bool plotn2n3Speeds();
		bool plotn2n3Ratio();
		bool plotTransmissionRatio();
		bool plotTCSlipAndCtrl();
		bool plotOilTemp();
		void plotDataForSingleYAxis(bool leftAxisFloat = false, bool displayData2 = false);
		//void plotDataForDualYAxis(bool leftAxisFloat = false);
		void plotDataForDualYAxis(bool leftAxisFloat = false, bool useSmallView = false);
		bool goToSettingsMenu();
		bool goToDualAxisMapEditorMenu();
		bool goToSingleAxisMapEditorMenu();
		bool showParamEditor();
		bool showTCCTuneView();
		bool showMalfunctionCodes();
		bool goToMPCMapMenu();
		bool goToSPCMapMenu();
		bool editMPCNormalDriveMap();
		bool editAutoModeWantedGearMap();

		bool editMPC1to2loadMap();
		bool editMPC2to3loadMap(); 
		bool editMPC3to4loadMap();
		bool editMPC4to5loadMap();
		bool editMPC5to4loadMap();
		bool editMPC4to3loadMap();
		bool editMPC3to2loadMap();
		bool editMPC2to1loadMap();

		bool editMPC1to2coastMap();
		bool editMPC2to3coastMap();
		bool editMPC3to4coastMap();
		bool editMPC4to5coastMap();
		bool editMPC5to4coastMap();
		bool editMPC4to3coastMap();
		bool editMPC3to2coastMap();
		bool editMPC2to1coastMap(); 

		bool editSPC1to2loadMap();
		bool editSPC2to3loadMap(); 
		bool editSPC3to4loadMap();
		bool editSPC4to5loadMap();
		bool editSPC5to4loadMap();
		bool editSPC4to3loadMap();
		bool editSPC3to2loadMap();
		bool editSPC2to1loadMap();

		bool editSPC1to2coastMap();
		bool editSPC2to3coastMap();
		bool editSPC3to4coastMap();
		bool editSPC4to5coastMap();
		bool editSPC5to4coastMap();
		bool editSPC4to3coastMap();
		bool editSPC3to2coastMap();
		bool editSPC2to1coastMap(); 

		void editDualAxisMap(configHandler::dualAxisMap* map, bool tracing = false, variableTypes vars = oilTempAndLoad);
		bool editSingleAxisMap(configHandler::singleAxisMap* map);
		int getClosestCell(int values[], uint8_t size, int val);
		bool exitMapEditor();
		bool buttonTests();
		bool readConfigFile();
		bool overWriteConfigFile();
		bool restoreDefaultConfigFile();
		bool showMalfunctionCode1();
		bool showOther();
		bool goToMainMenu();
		bool goToLiveDataMenu();
		bool goToDataPlotterMenu();
		bool goToSDMenu();
		void drawMenu(menuCollection* menu);

};
#endif
