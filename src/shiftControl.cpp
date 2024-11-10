#include "../headers/shiftControl.h"  

shiftControl::shiftControl()
{

}

shiftControl::~shiftControl()
{

}

void shiftControl::initShiftControl(configHandler &configHandler, uint8_t &MPC, uint8_t &SPC, configHandler::driveType &driveType,
 int &oilTemp, uint8_t &load, uint8_t &currentGear, uint8_t &targetGear, bool &shifting,
 int &lastShiftDuration, float &transmissionRatio, bool &shiftPermission, bool &dOrRengaged, int &engineSpeed,
 int &vehicleSpeed, bool &overridePressureValues, uint8_t &overridedMPCValue, uint8_t &overridedSPCValue)
{
    config_ = &configHandler;
    MPC_ = &MPC;
    SPC_ = &SPC;
    driveType_ = &driveType;
    oilTemp_ = &oilTemp;
    load_ = &load;
    currentGear_ = &currentGear;
    nextGear_ = *currentGear_; // get initial status
	targetGear_ = &targetGear;
    shifting_ = &shifting;
    lastShiftDuration_ = &lastShiftDuration;
    transmissionRatio_ = &transmissionRatio;
    shiftPermission_ = &shiftPermission;
    dOrRengaged_ = &dOrRengaged;
    engineSpeed_ = &engineSpeed;
    vehicleSpeed_ = &vehicleSpeed;
    overridePressureValues_ = &overridePressureValues;
    overridedMPCValue_ = &overridedMPCValue;
    overridedSPCValue_ = &overridedSPCValue;
    pinMode(SOL_12_45, OUTPUT);
    pinMode(SOL_23, OUTPUT);
    pinMode(SOL_34, OUTPUT);
}

void shiftControl::runShifts()
{
    //checkIfPreshiftDelayIsNeeded();
   //uint8_t targetGearForShift = *targetGear_;
    if (!*shifting_ && *shiftPermission_) // shift starts here
    {
        if (*targetGear_ > *currentGear_ && *currentGear_ < 5) // upshifts
        {
            nextGear_ = *currentGear_ + 1;
            // read parameters into temp variables and apply them for this one particular shift:
            currentGearForShift_ = *currentGear_;                    
        }
        else if (*targetGear_ < *currentGear_ && *currentGear_ > 1) // downshifts
        {
            nextGear_ = *currentGear_ - 1;
            // read parameters into temp variables and apply them for this one particular shift:
            currentGearForShift_ = *currentGear_;                     
        }
        shiftTimer_ = 0;
    }

    if (*shifting_) // what is executed during shift
    { 
        if (shiftTimer_ >= shiftSolenoidTimeForShift_) //
        {
            digitalWrite(SOL_12_45, LOW);
            digitalWrite(SOL_23, LOW);
            digitalWrite(SOL_34, LOW);
            *currentGear_= currentGearForShift_ = nextGear_;
            *shifting_ = false;
        }
        *lastShiftDuration_ = shiftTimer_;
        shiftTimer_++;
    }

    if (!*shifting_) {activateSolenoids();} // no need to call this when shift is ongoing, because solenoids are activated when the shift starts
    controlPressureSolenoids(); // see function below
}

void shiftControl::forceGearVariables(uint8_t gear)
{
    if (!*shifting_)
    {
        *currentGear_ = currentGearForShift_ = *targetGear_ = nextGear_ = gear;
    }
}

// control MPC & SPC solenoids
void shiftControl::controlPressureSolenoids()
{
    if (!*shifting_ && *dOrRengaged_) // outside of shifts, R or D engaged = regular drive.
    {
        *MPC_ = uint8_t(config_->giveRegularMPCMapValue(*oilTemp_, int(*load_))); // during shifts, MPC value will be set in activateSolenoids(), except when vehicleSpeed_ = 0
        *SPC_ = 100; // outside shifts, 100, because inverse control -> 100 = zero current = full pressure
    }
    else if (!*shifting_ && !*dOrRengaged_) // outside of shifts, P or N engaged = "garage shifts"
    {
        *MPC_ = 30;
        *SPC_ = 100;
    }   

    // override values if set in UI
    if (*overridePressureValues_ && *shifting_)
    {
        *MPC_ = *overridedMPCValue_;
        *SPC_ = *overridedSPCValue_;
    }

    // if vehicle is stationary, it is ok to use full pressures during shifting
    if (*vehicleSpeed_ == 0 && *shifting_)
    {
        *MPC_ = 100;
        *SPC_ = 100;
        shiftSolenoidTimeForShift_ = 600;
    }
    
    if (*SPC_ > 100){*SPC_ = 100;}
    if (*MPC_ > 100){*MPC_ = 100;}
    REG_PWM_CDTYUPD0 = *MPC_; //pin 34/35, control the MPC solenoid
    REG_PWM_CDTYUPD1 = *SPC_; //pin 36/37, control the SPC solenoid   
}

// activate both shift control & pressure control solenoids, when shift starts
void shiftControl::activateSolenoids()
{
    if (currentGearForShift_ == 1 && nextGear_ == 2) // shift from 1 to 2
    {
        *MPC_ = uint8_t(config_->giveShiftMapValue(configHandler::MPC_1to2, *driveType_, *oilTemp_, *load_));
        *SPC_ = uint8_t(config_->giveShiftMapValue(configHandler::SPC_1to2, *driveType_, *oilTemp_, *load_) + config_->giveEngSpdOilPressCorrectionValue(*engineSpeed_));
        shiftSolenoidTimeForShift_ = config_->giveShiftSolenoidTimeValue(*oilTemp_, *MPC_);       
        digitalWrite(SOL_12_45, HIGH);
        *shifting_ = true;
    }
    else if (currentGearForShift_ == 2 && nextGear_ == 3) // etc
    {
        *MPC_ = uint8_t(config_->giveShiftMapValue(configHandler::MPC_2to3, *driveType_, *oilTemp_, *load_));
        *SPC_ = uint8_t(config_->giveShiftMapValue(configHandler::SPC_2to3, *driveType_, *oilTemp_, *load_) + config_->giveEngSpdOilPressCorrectionValue(*engineSpeed_));
        shiftSolenoidTimeForShift_ = config_->giveShiftSolenoidTimeValue(*oilTemp_, *MPC_);  
        digitalWrite(SOL_23, HIGH);
        *shifting_ = true;
    }
    else if (currentGearForShift_ == 3 && nextGear_ == 4)
    {
        *MPC_ = uint8_t(config_->giveShiftMapValue(configHandler::MPC_3to4, *driveType_, *oilTemp_, *load_));
        *SPC_ = uint8_t(config_->giveShiftMapValue(configHandler::SPC_3to4, *driveType_, *oilTemp_, *load_) + config_->giveEngSpdOilPressCorrectionValue(*engineSpeed_)); 
        shiftSolenoidTimeForShift_ = config_->giveShiftSolenoidTimeValue(*oilTemp_, *MPC_); 
        digitalWrite(SOL_34, HIGH);
        *shifting_ = true;
    }
    else if (currentGearForShift_ == 4 && nextGear_ == 5)
    {
        *MPC_ = uint8_t(config_->giveShiftMapValue(configHandler::MPC_4to5, *driveType_, *oilTemp_, *load_));
        *SPC_ = uint8_t(config_->giveShiftMapValue(configHandler::SPC_4to5, *driveType_, *oilTemp_, *load_) + config_->giveEngSpdOilPressCorrectionValue(*engineSpeed_));
        shiftSolenoidTimeForShift_ = config_->giveShiftSolenoidTimeValue(*oilTemp_, *MPC_); 
        digitalWrite(SOL_12_45, HIGH);
        *shifting_ = true;
    }
    else if (currentGearForShift_ == 5 && nextGear_ == 4)
    {
        *MPC_ = uint8_t(config_->giveShiftMapValue(configHandler::MPC_5to4, *driveType_, *oilTemp_, *load_));
        *SPC_ = uint8_t(config_->giveShiftMapValue(configHandler::SPC_5to4, *driveType_, *oilTemp_, *load_) + config_->giveEngSpdOilPressCorrectionValue(*engineSpeed_)); 
        shiftSolenoidTimeForShift_ = config_->giveShiftSolenoidTimeValue(*oilTemp_, *MPC_);  
        digitalWrite(SOL_12_45, HIGH);
        *shifting_ = true;
    }
    else if (currentGearForShift_ == 4 && nextGear_ == 3)
    {
        *MPC_ = uint8_t(config_->giveShiftMapValue(configHandler::MPC_4to3, *driveType_, *oilTemp_, *load_));
        *SPC_ = uint8_t(config_->giveShiftMapValue(configHandler::SPC_4to3, *driveType_, *oilTemp_, *load_) + config_->giveEngSpdOilPressCorrectionValue(*engineSpeed_));
        shiftSolenoidTimeForShift_ = config_->giveShiftSolenoidTimeValue(*oilTemp_, *MPC_);  
        digitalWrite(SOL_34, HIGH);
        *shifting_ = true;
    } 
    else if (currentGearForShift_ == 3 && nextGear_ == 2)
    {
        *MPC_ = uint8_t(config_->giveShiftMapValue(configHandler::MPC_3to2, *driveType_, *oilTemp_, *load_));
        *SPC_ = uint8_t(config_->giveShiftMapValue(configHandler::SPC_3to2, *driveType_, *oilTemp_, *load_) + config_->giveEngSpdOilPressCorrectionValue(*engineSpeed_));
        shiftSolenoidTimeForShift_ = config_->giveShiftSolenoidTimeValue(*oilTemp_, *MPC_);  
        digitalWrite(SOL_23, HIGH);
        *shifting_ = true;
    } 
    else if (currentGearForShift_ == 2 && nextGear_ == 1)
    {
        *MPC_ = uint8_t(config_->giveShiftMapValue(configHandler::MPC_2to1, *driveType_, *oilTemp_, *load_));
        *SPC_ = uint8_t(config_->giveShiftMapValue(configHandler::SPC_2to1, *driveType_, *oilTemp_, *load_) + config_->giveEngSpdOilPressCorrectionValue(*engineSpeed_));  
        shiftSolenoidTimeForShift_ = config_->giveShiftSolenoidTimeValue(*oilTemp_, *MPC_); 
        digitalWrite(SOL_12_45, HIGH);
        *shifting_ = true;
    } 
}

// loop through all the gears and return the one that matches the ratio. if no gear matches, return 0
uint8_t shiftControl::checkIfTransmissionRatioMatchesAnyGear()
{
    for (uint8_t gear = 1; gear <= 5; gear++)
    {
        if (checkIfTransmissionRatioMatchesForGear(gear))
        {
            return gear;
        }
    }
    return 0; // if ratio does not match any gear, return 0
}

// return true if transmission ratio matches given gear
bool shiftControl::checkIfTransmissionRatioMatchesForGear(uint8_t gear)
{
    float gap = 0.12;
    switch(gear)
    {
    case 1:
        //if (*transmissionRatio_ <= 3.59 + gap && *transmissionRatio_ >= 3.59 - gap)
        if (3.10 < *transmissionRatio_ && *transmissionRatio_ < 4.10)
        {
            return true;
        }
        break;
    case 2:
        //if (*transmissionRatio_ <= 2.19 + gap && *transmissionRatio_ >= 2.19 - gap)
        if (1.80 < *transmissionRatio_ && *transmissionRatio_ < 3.10)
        {
            return true;
        }
        break;
    case 3:
        //if (*transmissionRatio_ <= 1.41 + gap && *transmissionRatio_ >= 1.41 - gap)
        if (1.10 < *transmissionRatio_ && *transmissionRatio_ < 1.80)
        {
            return true;
        }
        break;
    case 4:
        //if (*transmissionRatio_ <= 1.00 + gap && *transmissionRatio_ >= 1.00 - gap)
        if (0.95 < *transmissionRatio_ && *transmissionRatio_ < 1.10)
        {
            return true;
        }
        break;
    case 5:
        //if (*transmissionRatio_ <= 0.83 + gap && *transmissionRatio_ >= 0.83 - gap)
        if (0.70 < *transmissionRatio_ && *transmissionRatio_ < 0.95)
        {
            return true;
        }
        break;
    }
}

