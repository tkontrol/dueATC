#include "../headers/core.h"

core::core(int speedMeasInterruptInterval, int engineSpeedPin, int primaryVehicleSpeedPin, int secondaryVehicleSpeedPin, int n2SpeedPin, int n3SpeedPin):
    engineSpeedMeas_(speedMeasurement(speedMeasInterruptInterval, 100, 9999, 5)),
    primaryVehicleSpeedMeas_(speedMeasurement(speedMeasInterruptInterval, 50, 2999, 5)), // 2900 rpm ~ 270 km/h
    secondaryVehicleSpeedMeas_(speedMeasurement(speedMeasInterruptInterval, 50, 2999, 5)), // 2900 rpm ~ 270 km/h
    n2SpeedMeas_(speedMeasurement(speedMeasInterruptInterval, 10, 9999, 5)),
    n3SpeedMeas_(speedMeasurement(speedMeasInterruptInterval, 10, 9999, 5)),
    oilTemp_PN_Meas_(analogMeasurement(7, 10)), // 7 = pin A0    
    MAP_Meas_(analogMeasurement(6, 5)), // 6 = pin A1
    TPS_Meas_(analogMeasurement(5, 5)), // 5 = pin A2
    shiftControl_(shiftControl()),
    config_(configHandler()),
    TCCcontrol_(TCCcontrol()),
    startupCounter_(0),
    currentGear_(2),
    targetGear_(2),
    brakePedal_(brakePedalSwitchPin),
    Pswitch_(PswitchPin),
    Rswitch_(RswitchPin),
    gearPlus_(gearPlusPin),
    gearMinus_(gearMinusPin)
{}

core::~core()
{}

// run once at startup
void core::initController()
{  
    pinMode(brakePedalSwitchPin, INPUT_PULLUP);
    pinMode(PswitchPin, INPUT_PULLUP);
    pinMode(RswitchPin, INPUT_PULLUP);
    pinMode(gearPlusPin, INPUT_PULLUP);
    pinMode(gearMinusPin, INPUT_PULLUP);

    config_.initMaps();
    parametersPtr_ = config_.givePtrToConfigurationSet()->parameters; // receive pointer to parameters. otherwise than maps container pointer, this is used also by core
    shiftControl_.initShiftControl(config_, MPC_, SPC_, driveType_, oilTemp_, load_, currentGear_, targetGear_, usePreShiftDelay_, shifting_, lastShiftDuration_, transmissionRatio_.ratio, useGearRatioDetection_, shiftPermission_, dOrRengaged_);
    TCCcontrol_.setOutputLimits(0, 100);
    TCCcontrol_.setMeasurementPointers(engineSpeed_, inputShaftSpeed_);
    TCCcontrol_.setTCCmode(tccMode_); // WHERE TO GET THIS SETTING?

    useGearRatioDetection_ = false; // REMOVE AFTER TESTING
    shiftPermission_ = true; // REMOVE AFTER TESTING

    notification_.pending = false;
    notification_.isShown = true;
    log_.amount = 0;

    malfunctions_.descriptions[0] = "Failed to shift to gear 1";
    malfunctions_.descriptions[1] = "Failed to shift to gear 2";
    malfunctions_.descriptions[2] = "Failed to shift to gear 3";
    malfunctions_.descriptions[3] = "Failed to shift to gear 4";
    malfunctions_.descriptions[4] = "Failed to shift to gear 5";
    malfunctions_.descriptions[5] = "Transm ratio does not match any gear after shift"; // activated if transmission ratio after shift does not match any of the gears -> slipping?

    malfunctions_.descriptions[6] = "Too big diff with prim and sec veh spd sensors";
    malfunctions_.descriptions[7] = "Some kind of error";
    malfunctions_.descriptions[8] = "Some another kind of error";
    malfunctions_.descriptions[9] = "Some third kind of error";

 
    // REMOVE AFTER TESTING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //activateMalfunction(1);
    //activateMalfunction(4);
    //activateMalfunction(5);
    //activateMalfunction(6);
    //activateMalfunction(8);
    //activateMalfunction(9);
    //activateMalfunction(0);
    //activateMalfunction(3);
    //activateMalfunction(2);
    //activateMalfunction(7); 
}

void core::coreloop() // this is called in 1ms intervals, see main.cpp
{     
    gearPlus_.releaseBlock();
    gearMinus_.releaseBlock();

    updateSpeedMeasurements(); //first one in loop, use these values during the loop
    detectDriveType();
    updateAnalogMeasurements();
    updateLeverPosition();
    updateGearByN3N2Ratio();
    readShiftSwitches();
    doAutoShifts();
    makeUpShiftCommand();
    makeDownShiftCommand();
    shiftControl_.runShifts();
    updateLog();
    brakePedalSwitchState_ = brakePedal_.giveState();

    //analogWrite(2, 0); // WHAT DOES THIS CAUSE??

    if (notificationTimerOn_)
    {
        notificationTimerCounter_++;
    }
    testcounter_++; 
}

void core::startupProcedure()
{
    applyParameters();
    shifting_ = false;

    usePreShiftDelay_ = true; //// //// REMOVE AFTER TESTING!!!!!!!!!!!!!!!!!!!!!!

    while (startupCounter_ < 100)
    {
        updateSpeedMeasurements();
        detectDriveType();
        updateAnalogMeasurements();
        updateLeverPosition();
        startupCounter_++;
    }
    detectGear();
}

void core::applyParameters()
{ 
    n2SpeedMeas_.setPulsesPerRev(6); // 6, not 60 because PCB has divider by 10 ICs
    n3SpeedMeas_.setPulsesPerRev(6); // 6, not 60 because PCB has divider by 10 ICs

    for (int i = 0; i < parametersPtr_->size; i++)
    {
        updateParameter(&parametersPtr_->parameters[i]);
    } 
}

// applies given parameter
void core::updateParameter(configHandler::parameter* p)
{
    if (p->ID == "Start_with_1St_gear")
    {
        startWith1StGear_ = p->data;
    }   
    else if (p->ID == "Wheel_circum")
    {
        wheelCircum_ = p->data;
    }
    else if (p->ID == "driveShaft_Pulses_Per_Revolution")
    {       
        primaryVehicleSpeedMeas_.setPulsesPerRev(p->data);
        secondaryVehicleSpeedMeas_.setPulsesPerRev(p->data); // REMEMBER TO CHANGE THIS IF SECONDARY VEH SPD SENSOR DOES NOT HAVE SAME PULSE AMOUNT THAN PRIMARY
        driveShaftPulsesPerRev_ = p->data;
    }
    else if (p->ID == "engineSpeed_Pulses_Per_Revolution")
    {
        engineSpeedMeas_.setPulsesPerRev(p->data);
    }
    else if (p->ID == "Min_vehicle_spd_for_gear_ratio_detection")
    {
        minimumVehicleSpeedForGearRatioDetection_ = p->data;
    }
    else if (p->ID == "TCC_PI_control_P_factor")
    {
        TCCcontrol_.setKP(p->data);
    }
    else if (p->ID == "TCC_PI_control_I_factor")
    {
        TCCcontrol_.setKI(p->data);
    }
    else if (p->ID == "Final_drive_ratio_x100")
    {
        finalDriveRatioX100_ = p->data;
    }

}

String core::readConfFile()
{
    return config_.readSDCardToDataStructs("CONF.CFG");
}

String core::readDefaultsFile()
{
    return config_.readSDCardToDataStructs("ORICONF.CFG");
}

String core::writeConfFile()
{
    return config_.writeDataStructsToSDCard("CONF.CFG"); 
}

void core::increaseSpeedMeasCounters()
{
    engineSpeedMeas_.increaseCounter();
    primaryVehicleSpeedMeas_.increaseCounter();
    secondaryVehicleSpeedMeas_.increaseCounter();
    n3SpeedMeas_.increaseCounter();
    n2SpeedMeas_.increaseCounter();
    clock_++; //POISTA
}

void core::engineSpeedMeas() // run on rising edge of signal, update the variable
{
    engineSpeedMeas_.calcPeriodLength();    
}

void core::primaryVehicleSpeedMeas() // run on rising edge of signal, update the variable
{    
    primaryVehicleSpeedMeas_.calcPeriodLength();
}

void core::secondaryVehicleSpeedMeas() // run on rising edge of signal, update the variable
{    
    secondaryVehicleSpeedMeas_.calcPeriodLength();
}

void core::n2SpeedMeas() // run on rising edge of signal, update the variable
{
    n2SpeedMeas_.calcPeriodLength();
}

void core::n3SpeedMeas() // run on rising edge of signal, update the variable
{
    n3SpeedMeas_.calcPeriodLength();
}

void core::updateSpeedMeasurements()
{
    measMode_ = primary;

    int usedVehicleSpeedPeriodLength;

    switch(measMode_)
    {
    case vehicleSpeedMeasureMode::primary:
        usedVehicleSpeedPeriodLength = primaryVehicleSpeedMeas_.givePeriodLength(); // if only primary in use
    break;
    case vehicleSpeedMeasureMode::secondary:
        usedVehicleSpeedPeriodLength = secondaryVehicleSpeedMeas_.givePeriodLength(); // if only secondary in use
    break;
        case vehicleSpeedMeasureMode::both:
        usedVehicleSpeedPeriodLength = (primaryVehicleSpeedMeas_.givePeriodLength() + secondaryVehicleSpeedMeas_.givePeriodLength()) / 2; // if pri and sec both in use, use average
    break;
    }
    
    vehicleSpeed_  = (((wheelCircum_ * 10000) / (driveShaftPulsesPerRev_ * usedVehicleSpeedPeriodLength))) / 3.6; // in case used speed measurement is from driveshaft!

    cardanShaftSpeed_ = (60 / driveShaftPulsesPerRev_) * 1000000 / (usedVehicleSpeedPeriodLength) * float(finalDriveRatioX100_ / 100) ; // in case used speed measurement (pri or sec) is from driveshaft

    //cardanShaftSpeed_ = secondaryVehicleSpeedMeas_.giveRPM(); // in case secondary sensor measures directly cardan shaft

    //cardanShaftSpeed_ = primaryVehicleSpeedMeas_.giveRPM(); // in case primary sensor measures cardan shaft

    primaryVehicleSpeed_ = primaryVehicleSpeedMeas_.giveRPM();
    secondaryVehicleSpeed_ = secondaryVehicleSpeedMeas_.giveRPM(); 

    engineSpeed_ = engineSpeedMeas_.giveRPM();
    n2Speed_ = n2SpeedMeas_.giveRPM();
    n3Speed_ = n3SpeedMeas_.giveRPM();
    n3n2Ratio_ = float(n3Speed_) / float(n2Speed_);
    

    //calculating input shaft speed 
    if (n3n2Ratio_ <= 0.5) // actually, when n3Speed_ = 0. Presumably the shaft takes some time to slow down to 0, so this is just to speed up the measurement 
    {
        inputShaftSpeed_ = int(float(n2Speed_) * 1.64); // 1.64 is the ratio from n2 sensor's shaft to incoming shaft speed (=turbine speed), when gear is 1 or 5 (=n3 speed is zero)
    }
    else 
    {
        inputShaftSpeed_ = (n2Speed_ + n3Speed_) / 2; //when gear is 2, 3 or 4, n3 speed is not zero, and then incoming shaft speed (=turbine speed) equals to n2 speed
    }

    tcSlip_ = abs(engineSpeed_ - inputShaftSpeed_);

    transmissionRatio_.ratio = float(inputShaftSpeed_) / float(cardanShaftSpeed_); 
    transmissionRatio_.isValid = true;
}

void core::detectDriveType()
{
    static int value = 0;
    static int prevVehicleSpeed = vehicleSpeed_;

    if (vehicleSpeed_ > prevVehicleSpeed && value < 2)
    {
        value++;
    }
    else if (vehicleSpeed_ < prevVehicleSpeed && value > -2)
    {
        value--;
    }

    if (value >= 0)
    {
        driveType_ = configHandler::driveType::load;
    }
    else if (value < 0)
    {
        driveType_ = configHandler::driveType::load; // TEMPORARY; USE ONLY LOAD MAPS
    }
    prevVehicleSpeed = vehicleSpeed_;
}

void core::updateAnalogMeasurements()
{
    const int Rin = 1000; // internal pull-up resistor in a PCB, ohms
    const int Vcc = 3300; // pull-up voltage

    int voltage = oilTemp_PN_Meas_.giveVoltage();
    oilTemp_PN_sens_resistance_ = int(float(voltage) / (float(Vcc - voltage) / float(Rin)));
    if (lever_ == P || lever_ == N)
    {
        oilTemp_ = -20;
    }
    else
    {
        oilTemp_ = config_.giveOilTempValue(oilTemp_PN_sens_resistance_); 
    }
    TPSVoltage_ = TPS_Meas_.giveVoltage() * 2.5; //factor 2.5 because voltage-divider on PCB //ADC->ADC_CDR[6] / 1023.0 * 3300; 
    TPS_ = config_.giveTPSValue(TPSVoltage_);
    MAPVoltage_ = MAP_Meas_.giveVoltage() * 2.5; //factor 2.5 because voltage-divider on PCB //ADC->ADC_CDR[5] / 1023.0 * 3300; 
    MAP_ = config_.giveMAPValue(MAPVoltage_);
    load_ = int(0.5 * TPS_ + 0.5 * MAP_);

    if (load_ > 100)
    {
        load_ = 100;
    }
}

void core::updateLeverPosition()
{
    bool PNbyTempSens = (oilTemp_PN_sens_resistance_ > 5000) ? true: false; // true, if detected P/N - false, if detected R/D

    parkSwitchState_ = Pswitch_.giveState();
    reverseSwitchState_ = Rswitch_.giveState();
    
    if (parkSwitchState_ && !reverseSwitchState_ && PNbyTempSens)
    {
        lever_ = P;
        dOrRengaged_ = false;
    }
    else if (!parkSwitchState_ && reverseSwitchState_ && !PNbyTempSens)
    {
        lever_ = R;
        dOrRengaged_ = true;
    }
    else if (!parkSwitchState_ && !reverseSwitchState_ && PNbyTempSens)
    {
        lever_ = N;
        dOrRengaged_ = false;
    }
    else if(!parkSwitchState_ && !reverseSwitchState_ && !PNbyTempSens)
    {
        lever_ = D;
        dOrRengaged_ = true;
    }
}

void core::updateGearByN3N2Ratio()
{
    if (lever_ == P && engineSpeed_ > 500 && !shifting_)
    {
        if (n3n2Ratio_ <= 0.50)
        {
            shiftControl_.forceGearVariables(1);
        }
        else if (n3n2Ratio_ >= 0.50)
        {
            shiftControl_.forceGearVariables(2);
        }
    }
}

void core::readShiftSwitches()
{
    gearPlusSwitchState_ = gearPlus_.giveState();
    gearMinusSwitchState_ = gearMinus_.giveState();

    if (targetGear_ == currentGear_)
    {
        if (gearPlusSwitchState_) {gearUpRequest();}
        if (gearMinusSwitchState_) {gearDownRequest();}
    }

}

void core::doAutoShifts()
{  
    if (shiftingMode_ == AUT && !shifting_)
    {
        autoModeTargetGear_ = config_.giveAutoModeTargetGear(vehicleSpeed_, currentGear_, load_); // VAIHDA TÄHÄN KAASUN ASENTO TAI KUORMA
        if (autoModeTargetGear_ == 0)
        {
            return; // config_ returns 0 -> no need to shift atm, exit function
        }
        else
        {
            targetGear_ = autoModeTargetGear_;
        }
    }
}

void core::toggleAutoMan()
{
    if (shiftingMode_ == MAN)
    {
        shiftingMode_ = AUT;
    }
    else if (shiftingMode_ == AUT)
    {
        shiftingMode_ = MAN;
    }
}

void core::toggleRatioDetection()
{
    if (useGearRatioDetection_)
    {
        useGearRatioDetection_ = false;
        Serial.println("ratio detection off");
    }
    else
    {
        useGearRatioDetection_ = true;
        Serial.println("ratio detection on");
    }
}

void core::updateLog()
{
    if (logging_) // if logging is active
    {
        if (logTimerCounter_ == 100) // core-loop is called at 1ms intervals, so 100 loops equals 0,1 second
        {
            logTimerCounter_ = 0;
            int var1 = 0;
            float var2 = 0;

            switch(variableToBeLogged_) // set loggable variable
            {
            case engineSpeed:
                var1 = engineSpeed_;   
            break;

            case vehicleSpeed:
                var1 = vehicleSpeed_;
            break;

            case primAndSecVehSpds:
                var1 = primaryVehicleSpeed_;
                var2 = secondaryVehicleSpeed_;
            break;

            case n2Andn3Speed:
                var1 = n2Speed_;
                var2 = n3Speed_;
            break;

            case n3n2Ratio:
                var2 = n3n2Ratio_;
            break;

            case transmissionRatio:
                var2 = transmissionRatio_.ratio;
            break;

            case TCSlipAndTCControl:
                var1 = *TCCcontrol_.giveOutputPointer();
                var2 = tcSlip_; // engineSpeed_ - inputShaftSpeed_;
            break;

            case oilTemp:
                var1 = oilTemp_;
            break;

            default:
                var1 = 0;
                var2 = 0;
            break;
            }

            if (log_.amount == 0) // first insert on first run...
            {
                log_.y1Data[0] = var1; // insert first measurement
                log_.y2Data[0] = var2;
                int counter = 0;
                for (int i = 0; i < LOGGER_MAX_SIZE; i++) // insert the x axis
                {
                    log_.xData[i] = counter;
                    counter++;
                }
                log_.amount++;
            }
            else // next inserts
            {
                log_.amount++;
                for(int i = log_.amount-1; i > 0; i--)
                {
                    log_.y1Data[i] = log_.y1Data[i-1]; // move all cells to one forward
                    log_.y2Data[i] = log_.y2Data[i-1];
                }
                log_.y1Data[0] = var1; // insert current measurement to the newest measurement, first of array 
                log_.y2Data[0] = var2;
            }
            if (log_.amount == LOGGER_MAX_SIZE) // if the array is full
            {
                log_.amount = LOGGER_MAX_SIZE-1;
            }        
        }
        logTimerCounter_++; // increase the counter if logging is active
    }
}

void core::modifyLastShiftMaps(int MPCchange, int SPCchange)
{
    config_.modifyLastShiftMaps(MPCchange, SPCchange);
}

void core::gearUpRequest() // for manual upshift, call this
{
    if (targetGear_ < 5)
    {
        targetGear_ ++;
    }
}

void core::gearDownRequest() // for manual downshift, call this
{   
    if (targetGear_ > 1)
    {
        targetGear_ --;
    }
}

void core::makeUpShiftCommand()
{
    static int delayCounter = 0;
    static bool counting;

    if (gearUpReq_ && delayCounter == 0 && usePreShiftDelay_ ) //start shift after delay
    {
        counting = true;
        //gearUpReq_ = false;
    }
    else if (gearUpReq_ && !usePreShiftDelay_) //direct shift w/o delay
    {
        gearUpReq_ = false;
        gearUpComm_ = true;
    }
    if (counting)
    {
        delayCounter++;
    }
    if (delayCounter == 1000)
    {
        gearUpReq_ = false;
        counting = false;
        delayCounter = 0;
        gearUpComm_ = true;
    }
}

void core::makeDownShiftCommand()
{
    static int delayCounter = 0;
    static bool counting;


    if (gearDownReq_ && delayCounter == 0 && usePreShiftDelay_ ) //start shift after delay
    {
        counting = true;
        //gearDownReq_ = false;
    }
    else if (gearDownReq_ && !usePreShiftDelay_) //direct shift w/o delay
    {
        gearDownReq_ = false;
        gearDownComm_ = true;
    }
    if (counting)
    {
        delayCounter++;
    }
    if (delayCounter == 1000)
    {
        gearDownReq_ = false;
        counting = false;
        delayCounter = 0;
        gearDownComm_ = true;
    }
}
 
bool core::confirmGear(uint8_t gear)
{ 

}
 
bool core::detectGear()
{ /*
    uint8_t gear = 0;
    switch (lever_)
    {
    case P:
        if (n3n2Ratio_ < 0.5) // we can presume the vehicle is stationary here, since P
        {
            currentGear_ = targetGear_ = 1;
        }
        else
        {
            currentGear_ = targetGear_ = 2;
            if (startWith1StGear_ && currentGear_ == 2 && !malfunctions_.activeMalfunctions)
            {
                gearDownComm_ = true; //here command, no need to create request
            }            
        }
        break;

    case R:
        if (n3n2Ratio_ < 0.5) // this is the best guess, since speed might be too low to detect right gear accurately
        {
            currentGear_ = targetGear_ = 1;
        }
        else
        {
            currentGear_ = targetGear_ = 2;
        }
        break;

    case N:
        // TO BE IMPLEMENTED:
        // BLOCK ALL ACTIONS HERE AND MAKE GEAR RATIO CHECK AFTER MOVING AWAY FROM N !

        break;
    
    case D:
        if (vehicleSpeed_ != 0) // ok, vehicle is moving and we get to measure the right gear
        {      /*      
            for (gear = 1; gear <= 5; gear++)
            {
                if (gear == 1 && confirmGear(gear))
                {
                    currentGear_ = targetGear_ = gear;
                }
                else if (gear == 2 && confirmGear(gear))
                {
                    currentGear_ = targetGear_ = gear;
                }
                else if (gear == 3 && confirmGear(gear))
                {
                    currentGear_ = targetGear_ = gear;
                }
                else if (gear == 4 && confirmGear(gear))
                {
                    currentGear_ = targetGear_ = gear;
                }
                else if (gear == 5 && confirmGear(gear))
                {
                    currentGear_ = targetGear_ = gear;
                }
                else // measured ratio does not match any gear, return false
                {
                    return false;
                }
            }           
        }
        else if (vehicleSpeed_ == 0) // vehicle is stationary
        {
            if (n3n2Ratio_ < 0.5) // this is the best guess...
            {
                currentGear_ = targetGear_ = 1;
            }
            else
            {
                currentGear_ = targetGear_ = 2;
            }
        }
        break;
    }  
    return true; // ratio matches certain gear, all ok -> return true 
    */
}

void core::setLoggableVariable(core::loggableVariable var)
{
    variableToBeLogged_ = var;
}

void core::setLoggingStatus(bool state)
{
    logging_ = state;
    if (!state) // clear log when logging stops
    {
        for(int i = 0; i < LOGGER_MAX_SIZE; i++)
        {
            log_.xData[i] = 0;
            log_.y1Data[i] = 0;
            log_.y2Data[i] = 0;
            log_.amount = 0;
        }
    }
}

void core::startNotificationTimer(int time)
{
    notificationTimerTargetTime_ = time;
    notificationTimerOn_ = true;
}

bool core::hasNotificationTimePassed()
{
    if (notificationTimerTargetTime_ == -1) // if -1, notification never passes
    {
        return false;
    }  
    else if (notificationTimerCounter_ >= notificationTimerTargetTime_)
    {
        notificationTimerOn_ = false;
        notificationTimerCounter_ = 0;
        return true;
    }
    return false;
}

struct core::notificationStruct core::checkNotification()
{
    return notification_;
}

void core::markNotificationAsReceived()
{
    notification_.pending = false;
}

void core::markNotificationAsShown()
{
    notification_.isShown = true;
}

void core::showNotification(int time, String msg)
{
    if (notification_.isShown && !notification_.pending)
    {
        notification_.pending = true;
        notification_.isShown = false;
        notification_.time = time;
        notification_.message = msg;
    }    
}

struct configHandler::configurationSet* core::givePtrToConfigurationSet()
{ 
    return config_.givePtrToConfigurationSet(); // pass to ui
}

struct core::logStruct* core::giveLogPtr()
{
    return &log_;
}

bool core::giveMalfunctionStatus()
{
    return malfunctions_.activeMalfunctions;
}

void core::clearFaultCodes()
{
    for (uint8_t i = 0; i < sizeof(malfunctions_.codes)/sizeof(malfunctions_.codes[0]); i++)
    {
        malfunctions_.codes[i] = false;
    }
    malfunctions_.activeMalfunctions = false;
}

struct core::dataStruct core::giveDataPointers()
{ 
    data_.lastShiftDuration = &lastShiftDuration_;
    data_.currentGear = &currentGear_;
    data_.targetGear = &targetGear_;
    data_.autoModeTargetGear = &autoModeTargetGear_;
    data_.engineSpeed = &engineSpeed_;
    data_.vehicleSpeed = &vehicleSpeed_;
    data_.primaryVehicleSpeed = &primaryVehicleSpeed_;
    data_.secondaryVehicleSpeed = &secondaryVehicleSpeed_;     
    data_.brakePedalSwitch = &brakePedalSwitchState_; 
    data_.parkSwitch = &parkSwitchState_;
    data_.reverseSwitch = &reverseSwitchState_;
    data_.gearPlusSwitch = &gearPlusSwitchState_;
    data_.gearMinusSwitch = &gearMinusSwitchState_;     
    data_.leverPosition = &lever_;
    data_.shifting = &shifting_;
    data_.shiftingMod = &shiftingMode_;
    data_.tccMod = &tccMode_;
    data_.dType = &driveType_;
    data_.oilTemp = &oilTemp_;
    data_.oilTemp_PN_sens_res = &oilTemp_PN_sens_resistance_;
    data_.MAP = &MAP_;
    data_.MAPVoltage = &MAPVoltage_;
    data_.TPS = &TPS_;
    data_.TPSVoltage = &TPSVoltage_;
    data_.load = &load_;
    data_.MPC = &MPC_;
    data_.SPC = &SPC_;
    data_.n2Speed = &n2Speed_;
    data_.n3Speed = &n3Speed_;
    data_.tcSlip = &tcSlip_;
    data_.n3n2Ratio = &n3n2Ratio_;
    data_.transmissionRatio = &transmissionRatio_.ratio;
    data_.cardanSpeed = &cardanShaftSpeed_;
    data_.MPCchange = &lastMPCchange_;
    data_.SPCchange = &lastSPCchange_;
    data_.tccControlOutput = TCCcontrol_.giveOutputPointer();
    data_.malfuncs = &malfunctions_;
    return data_;
}

void core::printData()
{ 
    /*
    Serial.print("engineSpeed ");
    Serial.print(engineSpeed_);
    Serial.print("     "); 
    Serial.print("vehicleSpeed: ");
    Serial.print(vehicleSpeed_);
    Serial.print("     ");
    Serial.print("n2Speed: ");
    Serial.print(n2Speed_);
    Serial.print("     ");
    Serial.print("n3n2Ratio: ");
    Serial.print(n3n2Ratio_);
 
    Serial.print("up ");
    Serial.print(upButton_.giveState());
    Serial.print("     "); 
    Serial.print("down ");
    Serial.print(downButton_.giveState());
    Serial.print("     "); 
    Serial.print("left ");
    Serial.print(leftButton_.giveState());
    Serial.print("     "); 
    Serial.print("right ");
    Serial.print(rightButton_.giveState());
    Serial.print("     "); 
    Serial.print("ok ");
    Serial.print(okButton_.giveState());
    Serial.print("     "); 
    Serial.print("cancel ");
    Serial.print(cancelButton_.giveState());
    Serial.print("     "); 

    Serial.print("\n");    */
}

void core::activateMalfunction(uint8_t code)
{
    malfunctions_.codes[code] = true;
    malfunctions_.activeMalfunctions = true;
}



