#include "../headers/core.h"

core::core(int speedMeasInterruptInterval, int engineSpeedPin, int primaryVehicleSpeedPin, int secondaryVehicleSpeedPin, int n2SpeedPin, int n3SpeedPin):
    engineSpeedMeas_(speedMeasurement(speedMeasInterruptInterval, 100, 9999)),
    primaryVehicleSpeedMeas_(speedMeasurement(speedMeasInterruptInterval, 50, 2999)), //~ 270 km/h
    secondaryVehicleSpeedMeas_(speedMeasurement(speedMeasInterruptInterval, 50, 2999)), //~ 270 km/h
    n2SpeedMeas_(speedMeasurement(speedMeasInterruptInterval, 200, 9999)),
    n3SpeedMeas_(speedMeasurement(speedMeasInterruptInterval, 200, 9999)),
    oilTemp_PN_Meas_(analogMeasurement(7, 1)),
    TPS_Meas_(analogMeasurement(6, 10)),
    MAP_Meas_(analogMeasurement(5, 10)),
    config_(configHandler()),
    TCCcontrol_(TCCcontrol()),
    startupCounter_(0),
    currentGear_(2),
    targetGear_(2)
{
   
}

core::~core()
{

}

// run once at startup
void core::initController()
{  
    config_.initMaps();
    parametersPtr_ = config_.givePtrToConfigurationSet()->parameters; //config_.givePtrToParameterContainer(); // receive pointer to parameters. otherwise than maps container pointer, this is used also by core
    TCCcontrol_.setOutputLimits(0, 100);
    TCCcontrol_.setMeasurementPointers(n2Speed_, n3Speed_); // VAIHDA TÄHÄN engineSpeed ja incomingShaftSpeed
    TCCcontrol_.setTCCmode(tccMode_); // SIIRRÄ MUUALLE?

    notification_.pending = false;
    notification_.isShown = true;
    log_.amount = 0;
}


void core::startupProcedure()
{
    applyParameters();
    lever_ = P;  //// REMOVE AFTER TESTING!!!!!!!!!!!!!!!!!!!!!!
    shifting_ = false;

    while (startupCounter_ < 100)
    {
        updateSpeedMeasurements();
        detectDriveType();
        updateAnalogMeasurements();
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
        secondaryVehicleSpeedMeas_.setPulsesPerRev(p->data); // MUISTA VAIHTAA TÄMÄ JOS TOISSIJAISELLA EI OOKKAAN SAMA PULSSIMÄÄRÄ
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

}

void core::coreloop() // this is called in 1ms intervals, see main.cpp
{     
    updateSpeedMeasurements(); //first one in loop, use these values during the loop
    detectDriveType();
    updateAnalogMeasurements();
    makeUpShiftCommand();
    makeDownShiftCommand();
    doShiftLogic();
    controlPWMSolenoids();
    updateLog();

    lever_ = D; // POISTA !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!11
    //analogWrite(2, 0); // MITÄ TÄMÄ OIKEIN AIHEUTTAA????
    //analogWrite(3, 0);
    //analogWrite(4, 0);

    if (notificationTimerOn_)
    {
        notificationTimerCounter_++;
    }
    testcounter_++; 


    int luku = TCCcontrol_.givePIOutput();
    
 /*
    if (testcounter_ == 700)
    {
        lever_ = D;
        targetGear_ = 1;
    }

    if (testcounter_ == 1000)
    {
        
        currentGear_ = 1;
        shiftingMode_ = AUT;
    }

    if (testcounter_ == 1700)
    {
        targetGear_ = 2;
    }

    if (testcounter_ == 2000)
    {
        currentGear_ = 2;
        tccMode_ = open;
    }

    if (testcounter_ == 2700)
    {
        targetGear_ = 3;
    }

    if (testcounter_ == 3000)
    {
        currentGear_ = 3;
        shiftingMode_ = MAN;
    }

    if (testcounter_ == 3700)
    {
        targetGear_ = 4;
    }

    if (testcounter_ == 4000)
    {
        currentGear_ = 4;
        tccMode_ = slipping;
    }

    if (testcounter_ == 4700)
    {
        targetGear_ = 5;
    }
    
    if (testcounter_ == 5000)
    {
        shiftingMode_ = AUT;
        currentGear_ = 5;        
        testcounter_ = 0;
    } */

    float diff = abs(prevRatio_ - n3n2Ratio_);
    if (0)
    {        
        Serial.print(n3n2Ratio_);
        Serial.print("  ");
        Serial.print(n3Speed_);
        Serial.print("  ");
        Serial.print(n2Speed_);
        Serial.print("  diff:");
        Serial.println(diff);
    } 
    prevRatio_ = n3n2Ratio_;
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

    cardanShaftSpeed_ = (60 / driveShaftPulsesPerRev_) * 1000000 / (usedVehicleSpeedPeriodLength) * 3.15; // in case used speed measurement (pri or sec) is from driveshaft

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
        incomingShaftSpeed_ = n2Speed_ * 1.64; // 1.64 is the ratio from n2 sensor's shaft to incoming shaft speed (=turbine speed), when gear is 1 or 5 (=n3 speed is zero)
    }
    else 
    {
        incomingShaftSpeed_ = n2Speed_; //when gear is 2, 3 or 4, n3 speed is not zero, and then incoming shaft speed (=turbine speed) equals to n2 speed
    }

    tcSlip_ = abs(n2Speed_ - n3Speed_);

    transmissionRatio_ = float(incomingShaftSpeed_) / float(cardanShaftSpeed_); 
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
        driveType_ = configHandler::driveType::coast;
    }
    prevVehicleSpeed = vehicleSpeed_;
}

void core::updateAnalogMeasurements()
{
    oilTemp_PN_sens_resistance_ = oilTemp_PN_Meas_.giveVoltage(); // ADC->ADC_CDR[7] * 2; // laske jännitteestä resistanssi? 7 = pin A0
    oilTemp_ = config_.giveOilTempValue(oilTemp_PN_sens_resistance_ ); 
    TPSVoltage_ = TPS_Meas_.giveVoltage(); //ADC->ADC_CDR[6] / 1023.0 * 3300; // 6 = pin A1
    TPS_ = config_.giveTPSValue(TPSVoltage_);
    MAPVoltage_ = MAP_Meas_.giveVoltage(); //ADC->ADC_CDR[5] / 1023.0 * 3300; // 5 = pin A2
    MAP_ = config_.giveMAPValue(MAPVoltage_);
    load_ = int(0.25 * TPS_ + 0.25 * MAP_ + 0.5 * config_.giveEngSpdLoadFactorValue(engineSpeed_));
}

void core::doShiftLogic()
{  
    if (shiftingMode_ == AUT && lever_ == D) // lever == D is included only to this and the if-condition below and not the entire function,
    {                                       // to make sure the shift is finalized properly, no matter if driver sets the lever off the D (or R, P) during the shift
            doAutoShifts(); // gearUpReq or gearDownReq is set in this function if necessary, and those are requests handled below, same way as if they were given in manual mode
    } 
    if (!shifting_ && (lever_ != N)) // what is executed outside shifts = regular drive in D, R, or in P ("garage shifts"). also do not accept shift commands in N
    {
        MPC_ = config_.giveRegularMPCMapValue(oilTemp_, load_); // outside of shifts = regular drive. MPC value will be set below during shift
        SPC_ = 100; // -> no power to SPC solenoid during regular drive, as the 722.6 manual states (inverse control, 100% = 0 volts!)
        shiftTimer_ = 0;

        if (gearUpComm_ && targetGear_ < 5) // basically shift procedure starts here. upshift...
        {
            gearUpComm_ = false;
            gearDownComm_ = false;
            if (lever_ == D || (lever_ == R && currentGear_ == 1))
            {
                targetGear_ ++;
                
                if (lever_ == D && vehicleSpeed_ >= minimumVehicleSpeedForGearRatioDetection_) // setting to be used for one particular shift
                {
                    useGearRatioDetection_ = true;
                }
                else
                {
                    useGearRatioDetection_ = false;
                }
            }
        }

        if (gearDownComm_ && targetGear_ > 1) //... or here, downshift
        {
            gearUpComm_ = false;
            gearDownComm_ = false;
            targetGear_--;

            if (lever_ == D && vehicleSpeed_ >= minimumVehicleSpeedForGearRatioDetection_)
            {
                useGearRatioDetection_ = true;
            }
            else
            {
                useGearRatioDetection_ = false;
            }
        }

        if (currentGear_ == 1 && targetGear_ == 2 ) // shift from 1 to 2
        {
            MPC_ = config_.giveShiftMapValue(configHandler::MPC_1to2, driveType_, oilTemp_, load_);
            SPC_ = config_.giveShiftMapValue(configHandler::SPC_1to2, driveType_, oilTemp_, load_);
            digitalWrite(SOL_12_45, HIGH);
            shifting_ = true;
        }
        else if (currentGear_ == 2 && targetGear_ == 3) // etc
        {
            MPC_ = config_.giveShiftMapValue(configHandler::MPC_2to3, driveType_, oilTemp_, load_);
            SPC_ = config_.giveShiftMapValue(configHandler::SPC_2to3, driveType_, oilTemp_, load_);
            digitalWrite(SOL_23, HIGH);
            shifting_ = true;
        }
        else if (currentGear_ == 3 && targetGear_ == 4)
        {
            MPC_ = config_.giveShiftMapValue(configHandler::MPC_3to4, driveType_, oilTemp_, load_);
            SPC_ = config_.giveShiftMapValue(configHandler::SPC_3to4, driveType_, oilTemp_, load_);
            digitalWrite(SOL_34, HIGH);
            shifting_ = true;
        }
        else if (currentGear_ == 4 && targetGear_ == 5)
        {
            MPC_ = config_.giveShiftMapValue(configHandler::MPC_4to5, driveType_, oilTemp_, load_);
            SPC_ = config_.giveShiftMapValue(configHandler::SPC_4to5, driveType_, oilTemp_, load_);
            digitalWrite(SOL_12_45, HIGH);
            shifting_ = true;
        }
        else if (currentGear_ == 5 && targetGear_ == 4)
        {
            MPC_ = config_.giveShiftMapValue(configHandler::MPC_5to4, driveType_, oilTemp_, load_);
            SPC_ = config_.giveShiftMapValue(configHandler::SPC_5to4, driveType_, oilTemp_, load_);
            digitalWrite(SOL_12_45, HIGH);
            shifting_ = true;
        }
        else if (currentGear_ == 4 && targetGear_ == 3)
        {
            MPC_ = config_.giveShiftMapValue(configHandler::MPC_4to3, driveType_, oilTemp_, load_);
            SPC_ = config_.giveShiftMapValue(configHandler::SPC_4to3, driveType_, oilTemp_, load_);
            digitalWrite(SOL_34, HIGH);
            shifting_ = true;
        } 
        else if (currentGear_ == 3 && targetGear_ == 2)
        {
            MPC_ = config_.giveShiftMapValue(configHandler::MPC_3to2, driveType_, oilTemp_, load_);
            SPC_ = config_.giveShiftMapValue(configHandler::SPC_3to2, driveType_, oilTemp_, load_);
            digitalWrite(SOL_23, HIGH);
            shifting_ = true;
        } 
        else if (currentGear_ == 2 && targetGear_ == 1)
        {
            MPC_ = config_.giveShiftMapValue(configHandler::MPC_2to1, driveType_, oilTemp_, load_);
            SPC_ = config_.giveShiftMapValue(configHandler::SPC_2to1, driveType_, oilTemp_, load_);
            digitalWrite(SOL_12_45, HIGH);
            shifting_ = true;
        } 
    }
    else // what is executed during shift -> shifting_ = true
    {
        shiftTimer_++;
        lastShiftDuration_ = shiftTimer_;
        // shift is now ongoing, timer is running.

        // end shift if:
        // we use gear ratio detection and the correct ratio of targetGear_ is reached, or;
        // we dont use gear ratio detection and shift has lasted > 1500ms, so presuming the gear has coupled
        if ((useGearRatioDetection_ && confirmGear(targetGear_)) || (!useGearRatioDetection_ && shiftTimer_ == 1500)) // when the gear is detected to be properly coupled, change shifting_ = false
        {
            digitalWrite(SOL_12_45, LOW);
            digitalWrite(SOL_23, LOW);
            digitalWrite(SOL_34, LOW);
            currentGear_ = targetGear_; // target gear is reached
            shifting_ = false;
            lastMPCchange_ = 32; // 32 = space char -> to indicate empty in MainScreen
            lastSPCchange_ = 32; // 32 = space char -> to indicate empty in MainScreen
        }
        // end shift if:
        // we use gear ratio detection, and the correct ratio of targetGear_ is not reached in 3000ms
        if (shiftTimer_ >= 3000) 
        {   
            digitalWrite(SOL_12_45, LOW);
            digitalWrite(SOL_23, LOW);
            digitalWrite(SOL_34, LOW);

            if (!detectGear()) // if gear ratio does not match any of the gears, sth is wrong. if detectGear returns true, everything is ok and shift can be terminated.
            {
                currentGear_ = targetGear_; // pakko olla tässä, että vaihto loppuu joskus... VAI ONKO PAKKO OLLA TÄSSÄ?
            }
            shifting_ = false;

            if (lastShiftDuration_ < config_.giveShiftTimeTargetValue(load_) - 50)
            {
                config_.modifyLastShiftMaps(-5,-5);                            
                lastMPCchange_ = '-';
                lastSPCchange_ = '-';
            }
            else if (lastShiftDuration_ > config_.giveShiftTimeTargetValue(load_) + 50)
            {
                config_.modifyLastShiftMaps(+5,+5);                            
                lastMPCchange_ = '+';
                lastSPCchange_ = '+';  
            }
        }
    }
}

void core::doAutoShifts()
{
    autoModeTargetGear_ = config_.giveAutoModeTargetGear(vehicleSpeed_, currentGear_, 54); // VAIHDA TÄHÄN KAASUN ASENTO TAI KUORMA
    if (autoModeTargetGear_ == 0) {return;} // config_ returns 0 -> no need to shift atm, exit function

    if (!shifting_ && autoModeTargetGear_ > currentGear_)
    {
        gearUpReq_ = true;
    }
    
    if (!shifting_ && autoModeTargetGear_ < currentGear_)
    {
        gearDownReq_ = true;
    }
}

void core::controlPWMSolenoids()
{
    REG_PWM_CDTYUPD0 = MPC_; //pin 34/35, control the MPC solenoid
    REG_PWM_CDTYUPD1 = SPC_; //pin 36/37, control the SPC solenoid    
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
                var2 = transmissionRatio_;
            break;

            case TCSlipAndTCControl:
                var1 = *TCCcontrol_.giveOutputPointer();
                var2 = tcSlip_; // engineSpeed_ - incomingShaftSpeed_;
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

void core::gearUpRequest()
{
    if (shiftingMode_ == MAN && (lever_ == D || lever_ == R))
    {
        gearUpReq_ = true;
    }
}

void core::gearDownRequest()
{
    if (shiftingMode_ == MAN && (lever_ == D || lever_ == R))
    {  
        gearDownReq_ = true;
    }
}

void core::makeUpShiftCommand()
{
    static int delayCounter = 0;
    static bool counting;

    if (gearUpReq_ && delayCounter == 0 && !shifting_ && usePreShiftDelay_)
    {
        counting = true;
        gearUpReq_ = false;
    }
    else if (gearUpReq_ && !usePreShiftDelay_)
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
        counting = false;
        delayCounter = 0;
        gearUpComm_ = true;
    }
}

void core::makeDownShiftCommand()
{
    static int delayCounter = 0;
    static bool counting;

    if (gearDownReq_ && delayCounter == 0 && !shifting_ && usePreShiftDelay_)
    {
        counting = true;
        gearDownReq_ = false;
    }
    if (counting)
    {
        delayCounter++;
    }
    if (delayCounter == 1000)
    {
        counting = false;
        delayCounter = 0;
        gearDownComm_ = true;
    }
}



bool core::confirmGear(uint8_t gear)
{
    float gap = 0.1;
    switch(gear)
    {
        case 1:
            if (transmissionRatio_ <= 3.59 + gap && transmissionRatio_ >= 3.59 - gap)
            {
                return true;
            }
            break;
        case 2:
            if (transmissionRatio_ <= 2.19 + gap && transmissionRatio_ >= 2.19 - gap)
            {
                return true;
            }
            break;
        case 3:
            if (transmissionRatio_ <= 1.41 + gap && transmissionRatio_ >= 1.41 - gap)
            {
                return true;
            }
            break;
        case 4:
            if (transmissionRatio_ <= 1.00 + gap && transmissionRatio_ >= 1.00 - gap)
            {
                return true;
            }
            break;
        case 5:
            if (transmissionRatio_ <= 0.83 + gap && transmissionRatio_ >= 0.83 - gap)
            {
                return true;
            }
            break;
    }
    return false;
}

bool core::detectGear()
{
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
            if (startWith1StGear_ && currentGear_ == 2)
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
        {            
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
    return true; // ratio matches a certain gear, all ok -> return true
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
    data_.leverPosition = &lever_;
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
    data_.transmissionRatio = &transmissionRatio_;
    data_.cardanSpeed = &cardanShaftSpeed_;
    data_.MPCchange = &lastMPCchange_;
    data_.SPCchange = &lastSPCchange_;
    data_.tccControlOutput = TCCcontrol_.giveOutputPointer();

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



