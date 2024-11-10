#include "../headers/core.h"

core::core(int speedMeasInterruptInterval, int engineSpeedPin, int primaryVehicleSpeedPin, int secondaryVehicleSpeedPin, int n2SpeedPin, int n3SpeedPin):
    engineSpeedMeas_(speedMeasurement(speedMeasInterruptInterval, 100, 9999, 10, true)),
    primaryVehicleSpeedMeas_(speedMeasurement(speedMeasInterruptInterval, 5, 2999, 1)), // 2900 rpm ~ 270 km/h
    secondaryVehicleSpeedMeas_(speedMeasurement(speedMeasInterruptInterval, 5, 2999, 1)), // 2900 rpm ~ 270 km/h
    n2SpeedMeas_(speedMeasurement(speedMeasInterruptInterval, 10, 9999, 100)),
    n3SpeedMeas_(speedMeasurement(speedMeasInterruptInterval, 10, 9999, 100)),
    oilTemp_PN_Meas_(analogMeasurement(7, 100)), // 7 = pin A0    
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
    oilTempStatus_(configHandler::oilTemp::cold)
{}

core::~core()
{}

// run once at startup
void core::initController()
{  
    pinMode(brakePedalSwitchPin, INPUT_PULLUP);
    pinMode(PswitchPin, INPUT_PULLUP);
    pinMode(RswitchPin, INPUT_PULLUP);

    config_.initMaps();
    parametersPtr_ = config_.givePtrToConfigurationSet()->parameters; // receive pointer to parameters. otherwise than maps container pointer, this is used also by core
    shiftControl_.initShiftControl(config_, MPC_, SPC_, driveType_, oilTemp_, load_, currentGear_, targetGear_, shifting_,
    lastShiftDuration_, transmissionRatio_.ratio, shiftPermission_, dOrRengaged_, engineSpeed_, vehicleSpeed_,
    overridePressureValues_, overridedMPCValue_, overridedSPCValue_);
    TCCcontrol_.setOutputLimits(0, 100);
    TCCcontrol_.setMeasurementPointers(tcSlip_);
    TCCcontrol_.setTCCmode(tccMode_); // WHERE TO GET THIS SETTING?

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

    overridedMPCValue_ = 100;
    overridedSPCValue_ = 100;
}

void core::coreloop() // this is called in 1ms intervals, see main.cpp
{     
    updateSpeedMeasurements(); //first one in loop, use these values during the loop
    detectDriveType();
    updateAnalogMeasurements();
    updateOilTempStatus();
    updateLeverPosition();
    updateGearByN3N2Ratio();
    shiftTo1stInP();
    checkIfCurrentGearEqualsMeasuredGear();
    updateCurrentGearByMeasuredGear();
    doAutoShifts();
    makeUpShiftCommand();
    makeDownShiftCommand();
    TCCcontrol_.calculatePIOutput();
    shiftControl_.runShifts();
    updateLog();
    brakePedalSwitchState_ = brakePedal_.giveState();
    measuredGear_ = shiftControl_.checkIfTransmissionRatioMatchesAnyGear();
    if (!configOK_) {shiftingMode_ = MAN;}

    //analogWrite(2, 0); // WHAT DOES THIS CAUSE??

    if (notificationTimerOn_)
    {
        notificationTimerCounter_++;
    }
    testcounter_++; 

/*
    Serial.print("|n2:");
    Serial.print(n2Speed_);
    Serial.print("|n3:");
    Serial.print(n3Speed_);
    Serial.print("|v:");
    Serial.print(vehicleSpeed_);
    Serial.print("|r:");
    Serial.print(transmissionRatio_.ratio);
    Serial.print("|mg:");
    Serial.print(measuredGear_);
    Serial.print("|sh:");
    Serial.println(shifting_); */

        Serial.print("lever: ");
        Serial.print(lever_);
        Serial.print("  speed: ");
        Serial.println(vehicleSpeed_);
}

void core::startupProcedure()
{
    applyParameters();
    shifting_ = false;

    configOK_ = config_.checkConfigStatus();

    while (startupCounter_ < -1)
    {
        //updateSpeedMeasurements();
        //detectDriveType();
        //updateAnalogMeasurements();
        //updateOilTempStatus();
        //updateLeverPosition();
        //measuredGear_ = shiftControl_.checkIfTransmissionRatioMatchesAnyGear();

        startupCounter_++;

    }

    if (enableAutoModeAtStartup_ )
    {
        shiftingMode_ = AUT;
    }
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
    if (p->ID == "enable_autoMode_at_startup")
    {
        enableAutoModeAtStartup_ = p->data;
    }    
    if (p->ID == "start_with_1St_gear")
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
    else if (p->ID == "accept_measuredGear_as_currentGear_after_delay")
    {
        acceptMeasuredGearAsCurrentGearAfterDelay_ = p->data;
    }
    else if (p->ID == "delay_to_accept_measuredGear_as_currentGear")
    {
        delayToAcceptMeasuredGearAsCurrentGear_ = p->data;
    }
    else if (p->ID == "delay_to_currentGear_eq_measuredGear_to_acpt_autoShift")
    {
        delayToCurrentGearEqualsMeasuredGearToAcceptAutoShift_ = p->data;
    }
    else if (p->ID == "threshold_for_warm_oil")
    {
        thresholdForWarmOil_ = p->data;
    }
}

String core::readConfFile()
{
    String msg = config_.readSDCardToDataStructs("CONF.CFG");
    configOK_ = config_.checkConfigStatus();
    if (!configOK_) {shiftingMode_ = MAN;}
    return msg;
}

String core::readDefaultsFile()
{
    String msg = config_.readSDCardToDataStructs("ORICONF.CFG");
    configOK_ = config_.checkConfigStatus();
    if (!configOK_) {shiftingMode_ = MAN;}
    return msg;
}

String core::writeConfFile()
{
    return config_.writeDataStructsToSDCard("CONF.CFG"); 
}

void core::increaseSpeedMeasCounters() // this is called in 10 us intervals
{
    engineSpeedMeas_.increaseCounter();
    primaryVehicleSpeedMeas_.increaseCounter();
    secondaryVehicleSpeedMeas_.increaseCounter();
    n3SpeedMeas_.increaseCounter();
    n2SpeedMeas_.increaseCounter();
    clock_++; // for core loop length calculation
}

void core::engineSpeedMeas() // run on rising edge of signal, update the variable
{
    engineSpeedMeas_.updatePeriodLength();    
}

void core::primaryVehicleSpeedMeas() // run on rising edge of signal, update the variable
{    
    primaryVehicleSpeedMeas_.updatePeriodLength();
}

void core::secondaryVehicleSpeedMeas() // run on rising edge of signal, update the variable
{    
    secondaryVehicleSpeedMeas_.updatePeriodLength();
}

void core::n2SpeedMeas() // run on rising edge of signal, update the variable
{
    n2SpeedMeas_.updatePeriodLength();
}

void core::n3SpeedMeas() // run on rising edge of signal, update the variable
{
    n3SpeedMeas_.updatePeriodLength();
}

void core::updateSpeedMeasurements()
{
    measMode_ = primary; // for testing

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

    cardanShaftSpeed_ = int(float(60 / float(driveShaftPulsesPerRev_)) * float(1000000) / float(usedVehicleSpeedPeriodLength) * float(finalDriveRatioX100_) / 100) ; // in case used speed measurement (pri or sec) is from driveshaft

    //cardanShaftSpeed_ = secondaryVehicleSpeedMeas_.giveRPM(); // in case secondary sensor measures directly cardan shaft

    //cardanShaftSpeed_ = primaryVehicleSpeedMeas_.giveRPM(); // in case primary sensor measures cardan shaft

    primaryVehicleSpeed_ = primaryVehicleSpeedMeas_.giveRPM();
    secondaryVehicleSpeed_ = secondaryVehicleSpeedMeas_.giveRPM(); 

    engineSpeed_ = engineSpeedMeas_.giveRPM();

    n2Speed_ = n2SpeedMeas_.giveRPM();
    n3Speed_ = n3SpeedMeas_.giveRPM();
    n3n2Ratio_ = float(n3Speed_) / float(n2Speed_);
    

    //calculating input shaft speed 
    if (n3n2Ratio_ <= 0.9) // actually, when n3Speed_ = 0. Presumably the shaft takes some time to slow down to 0, so this is just to speed up the measurement 
    {
        inputShaftSpeed_ = int(float(n2Speed_) * 1.64); // 1.64 is the ratio from n2 sensor's shaft to incoming shaft speed (=turbine speed), when gear is 1 or 5 (=n3 speed is zero)
    }
    else 
    {
        inputShaftSpeed_ = (n2Speed_ + n3Speed_) / 2; //when gear is 2, 3 or 4, n3 speed is not zero, and then incoming shaft speed (=turbine speed) equals to n2 speed
    }

    tcSlip_ = abs(engineSpeed_ - inputShaftSpeed_);
    //useLowPassForTCSlip(abs(engineSpeed_ - inputShaftSpeed_));

    useLowPassForTransmRatio(float(inputShaftSpeed_) / float(cardanShaftSpeed_));
    transmissionRatio_.isValid = true;   
}

void core::useLowPassForTCSlip(int slip)
{
    static int upCounter = 0;
    static int downCounter = 0;
    int delaySpeed = 1; // for every delaySpeed (ms), increase/decrease tcSlip_ by 1

    if (tcSlip_ == slip)
    {
        upCounter = 0;
        downCounter = 0;
        return;
    }
    else if (slip > tcSlip_)
    {
        upCounter++;
        downCounter = 0;
    }
    else if (slip < tcSlip_)
    {
        upCounter = 0;
        downCounter++;
    }

    if (upCounter >= delaySpeed)
    {
        tcSlip_++;   
        upCounter = 0;     
    }
    else if (downCounter >= delaySpeed)
    {
        tcSlip_--;
        downCounter = 0;
    }
}

void core::useLowPassForTransmRatio(float ratio)
{
    static int upCounter = 0;
    static int downCounter = 0;
    int delaySpeed = 5; // for every delaySpeed (ms)
    float step = 0.01; // increase/decrease ratio by one step

    if (transmissionRatio_.ratio == ratio)
    {
        upCounter = 0;
        downCounter = 0;
        return;
    }
    else if (ratio > transmissionRatio_.ratio)
    {
        upCounter++;
        downCounter = 0;
    }
    else if (ratio < transmissionRatio_.ratio)
    {
        upCounter = 0;
        downCounter++;
    }

    if (upCounter >= delaySpeed)
    {
        transmissionRatio_.ratio = transmissionRatio_.ratio + step;   
        upCounter = 0;     
    }
    else if (downCounter >= delaySpeed)
    {
        transmissionRatio_.ratio = transmissionRatio_.ratio - step;
        downCounter = 0;
    }
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
    static bool transitionToNorP;
    static bool transitionToRorD;
    static int lastRealOilTemp_ = 0;
    if ((lever_ == P || lever_ == N) && transitionToNorP)
    {
        lastRealOilTemp_ = oilTemp_;
        //oilTemp_ = -20;
        transitionToNorP = false;
        transitionToRorD = true;
    }
    else if (lever_ == D || lever_ == R)
    {
        if (transitionToRorD)
        {
            oilTemp_ = lastRealOilTemp_;
            transitionToRorD = false;
        }
        useLowPassForOilTemp(config_.giveOilTempValue(oilTemp_PN_sens_resistance_)); 
        transitionToNorP = true;
    }
    TPSVoltage_ = TPS_Meas_.giveVoltage() * 2.5; //factor 2.5 because voltage-divider on PCB //ADC->ADC_CDR[6] / 1023.0 * 3300; 
    TPS_ = config_.giveTPSValue(TPSVoltage_);
    calculateTPSdelayed();
    MAPVoltage_ = MAP_Meas_.giveVoltage() * 2.5; //factor 2.5 because voltage-divider on PCB //ADC->ADC_CDR[5] / 1023.0 * 3300; 
    MAP_ = config_.giveMAPValue(MAPVoltage_);
    load_ = int(0.5 * TPS_ + 0.5 * MAP_);

    if (load_ > 100)
    {
        load_ = 100;
    }
}

void core::useLowPassForOilTemp(int temp)
{
    static int upCounter = 0;
    static int downCounter = 0;
    int delaySpeed = 100; // for every delaySpeed (ms), increase/decrease oilTemp_ by 1

    if (oilTemp_ == temp)
    {
        upCounter = 0;
        downCounter = 0;
        return;
    }
    else if (temp > oilTemp_)
    {
        upCounter++;
        downCounter = 0;
    }
    else if (temp < oilTemp_)
    {
        upCounter = 0;
        downCounter++;
    }

    if (upCounter >= delaySpeed)
    {
        oilTemp_++;   
        upCounter = 0;     
    }
    else if (downCounter >= delaySpeed)
    {
        oilTemp_--;
        downCounter = 0;
    }
}

void core::updateOilTempStatus()
{
    if (oilTempStatus_ == configHandler::oilTemp::cold && oilTemp_ >= thresholdForWarmOil_)
    {
        oilTempStatus_ = configHandler::oilTemp::warm;
    }
    if (oilTempStatus_ == configHandler::oilTemp::warm && oilTemp_ <= (thresholdForWarmOil_ - 5))
    {
        oilTempStatus_ = configHandler::oilTemp::cold;
    }
}

void core::calculateTPSdelayed()
{
    int delaySpeed = 100 - TPSdelayed_; // unit = ms / % -> how many ms it needs to decrease TPSdelayed -1 % unit
                                        // this is to make TPSdelayed_ decrease rate logarithmic
    static int counter;
    
    if (TPS_ < TPSdelayed_)
    {
        counter++;
    }

    if (counter >= delaySpeed && TPSdelayed_ > 0)
    {
        TPSdelayed_--;
        counter = 0;
    }

    if (TPS_ >= TPSdelayed_)
    {
        TPSdelayed_ = TPS_;
        counter = 0;
    }
}

void core::updateLeverPosition()
{
    bool PNbyTempSens = (oilTemp_PN_sens_resistance_ > 5000) ? true: false; // true, if detected P/N - false, if detected R/D

    parkSwitchState_ = Pswitch_.giveState();
    reverseSwitchState_ = Rswitch_.giveState();

    static leverPos leverPosCandidate;
    static leverPos leverPosCandidatePrev;
    static int leverPosCounter; // delay for low-pass filter
    
    if (parkSwitchState_ && !reverseSwitchState_ && PNbyTempSens)
    {
        leverPosCandidate = P;
        dOrRengaged_ = false;
    }
    else if (!parkSwitchState_ && reverseSwitchState_ && !PNbyTempSens)
    {
        leverPosCandidate = R;
        dOrRengaged_ = true;
    }
    else if (!parkSwitchState_ && !reverseSwitchState_ && PNbyTempSens)
    {
        leverPosCandidate = N;
        dOrRengaged_ = false;
    }
    else if(!parkSwitchState_ && !reverseSwitchState_ && !PNbyTempSens)
    {
        leverPosCandidate = D;
        dOrRengaged_ = true;
    }

    // following if-elses are for low-pass filtering
    if (leverPosCandidate == leverPosCandidatePrev)
    {
        leverPosCounter++;
    }
    else
    {
        leverPosCounter = 0;
    }
    if (leverPosCounter >= 500)
    {
        leverPosCounter = 500; // to prevent overflow
        lever_ = leverPosCandidate;
    } 
    leverPosCandidatePrev = leverPosCandidate;
}

void core::updateGearByN3N2Ratio()
{
    static int counterFor1;
    static int counterFor2;
    if (lever_ == P && engineSpeed_ > 700 && !shifting_)
    {
        if (n3n2Ratio_ <= 0.50 && currentGear_ != 1)
        {
            counterFor1++;
            counterFor2 = 0;
        }
        else if (n3n2Ratio_ >= 0.50 && currentGear_ != 2)
        {
            counterFor1 = 0;
            counterFor2++;
        }
    }
    else
    {
        counterFor1 = 0;
        counterFor2 = 0;
    }

    if (counterFor1 >= 1000)
    {
        shiftControl_.forceGearVariables(1);
        counterFor1 = 1000; // to prevent overflow
    }
    if (counterFor2 >= 1000)
    {
        shiftControl_.forceGearVariables(2);
        counterFor2 = 1000; // to prevent overflow
    }
}

void core::checkIfCurrentGearEqualsMeasuredGear()
{
    static int counter;
    if (currentGear_ == measuredGear_ && counter < delayToCurrentGearEqualsMeasuredGearToAcceptAutoShift_)
    {
        counter++;
    }
    else
    {
        counter = 0;
    }
    if (counter == delayToCurrentGearEqualsMeasuredGearToAcceptAutoShift_) // set delay here
    {
        currentGearMatchesMeasuredGear_ = true;
        return;
    }
    currentGearMatchesMeasuredGear_ = false;
}

void core::shiftTo1stInP()
{
    static int counter;
    if (startWith1StGear_ && lever_ == P && engineSpeed_ > 700 && currentGear_ == 2 && vehicleSpeed_ == 0 && !shifting_)
    {
        counter++;
    }
    else
    {
        counter = 0;
    }
    if (counter == 1000) //delay, ms
    {
        targetGear_ = 1;
    }
}

void core::updateCurrentGearByMeasuredGear()
{
    static uint8_t currentGearCandidate;
    static uint8_t currentGearCandidatePrev;
    static int currentGearCounter;
    static bool currentGearSet;

    if (acceptMeasuredGearAsCurrentGearAfterDelay_)
    {
        currentGearCandidate = measuredGear_;

        if (currentGearCandidate == currentGearCandidatePrev && currentGearCounter < delayToAcceptMeasuredGearAsCurrentGear_) //last condition is to prevent overflow
        {
            currentGearCounter++;
        }
        else //reset variables
        {
            currentGearCounter = 0;
            currentGearSet = false;
        }
        if (!currentGearSet && currentGear_ != measuredGear_ && //this is to make sure that the currentGear_ = measuredGear is done only once
                currentGearCounter >= delayToAcceptMeasuredGearAsCurrentGear_ &&
                currentGearCandidate != 0 &&
                engineSpeed_ >= 800 &&
                vehicleSpeed_ >= 10 &&
                lever_ == D)
        {
            shiftControl_.forceGearVariables(currentGearCandidate);
            currentGearSet = true; //this is to make sure that the currentGear_ = measuredGear is done only once
        }
        currentGearCandidatePrev = currentGearCandidate;
    }
    else
    {   
        currentGearCandidate = 0;
        currentGearCandidatePrev = 0;
        currentGearCounter = 0;
        currentGearSet = false;
    }
}

void core::forceGearVariables()
{
    if (measuredGear_ != 0)
    {
        shiftControl_.forceGearVariables(measuredGear_);
    }
}

void core::doAutoShifts()
{  
    if (shiftingMode_ == AUT && !shifting_ && lever_ == D)
    {
        autoModeTargetGear_ = config_.giveAutoModeTargetGear(vehicleSpeed_, currentGear_, TPSdelayed_, oilTempStatus_);
        if (autoModeTargetGear_ == 0)
        {
            return; // config_ returns 0 -> no need to shift atm, exit function
        }
        // if speed is above gear ratio detection threshold, make sure currentGear matches measuredGear
        else if ((vehicleSpeed_ <= minimumVehicleSpeedForGearRatioDetection_ || currentGearMatchesMeasuredGear_))
        {
            targetGear_ = autoModeTargetGear_;
        }
    }
}

bool core::toggleAutoMan()
{
    if (shiftingMode_ == MAN && configOK_)
    {
        shiftingMode_ = AUT;
        return true;
    }
    else if (shiftingMode_ == MAN && !configOK_)
    {
        return false;
    }
    else if (shiftingMode_ == AUT)
    {
        shiftingMode_ = MAN;
        return true;
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
                var2 = tcSlip_;
            break;

            case oilTemp:
                var1 = oilTemp_;
            break;

            case TPSes:
                var1 = TPS_;
                var2 = TPSdelayed_;
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
    if (shiftingMode_ == MAN && targetGear_ < 5 && !shifting_ && shiftPermission_)
    {
        targetGear_ ++;
    }
}

void core::gearDownRequest() // for manual downshift, call this
{   
    if (shiftingMode_ == MAN && targetGear_ > 1 && !shifting_ && shiftPermission_)
    {
        targetGear_ --;
    }
}

void core::makeUpShiftCommand()
{/*
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
    }*/
}

void core::makeDownShiftCommand()
{ /*
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
    } */
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
    data_.measuredGear = &measuredGear_;
    data_.autoModeTargetGear = &autoModeTargetGear_;
    data_.engineSpeed = &engineSpeed_;
    data_.vehicleSpeed = &vehicleSpeed_;
    data_.primaryVehicleSpeed = &primaryVehicleSpeed_;
    data_.secondaryVehicleSpeed = &secondaryVehicleSpeed_;     
    data_.brakePedalSwitch = &brakePedalSwitchState_; 
    data_.parkSwitch = &parkSwitchState_;
    data_.reverseSwitch = &reverseSwitchState_;
    data_.overridePressureValues = &overridePressureValues_;
    data_.overridedMPCValue = &overridedMPCValue_;
    data_.overridedSPCValue = &overridedSPCValue_;
    data_.leverPosition = &lever_;
    data_.shifting = &shifting_;
    data_.shiftingMod = &shiftingMode_;
    data_.tccMod = &tccMode_;
    data_.dType = &driveType_;
    data_.oilTempStatus = &oilTempStatus_;
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
    data_.tccSlipSetpoint = TCCcontrol_.giveSetpointPointer();
    data_.malfuncs = &malfunctions_;
    return data_;
}

void core::printData()
{ 

}

void core::activateMalfunction(uint8_t code)
{
    malfunctions_.codes[code] = true;
    malfunctions_.activeMalfunctions = true;
}



