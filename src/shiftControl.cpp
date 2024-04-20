#include "../headers/shiftControl.h"  

shiftControl::shiftControl()
{

}

shiftControl::~shiftControl()
{

}

void shiftControl::initShiftControl(configHandler &configHandler, uint8_t &MPC, uint8_t &SPC, configHandler::driveType &driveType, int &oilTemp, uint8_t &load,
 uint8_t &currentGear, uint8_t &targetGear, bool &usePreShiftDelay, bool &shifting, int &lastShiftDuration, float &transmissionRatio, bool &useGearRatioDetection, bool &shiftPermission, bool &dOrRengaged)
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
    targetGearDelayed_ = *targetGear_; // get initial status
    usePreShiftDelay_ = &usePreShiftDelay;
    shifting_ = &shifting;
    lastShiftDuration_ = &lastShiftDuration;
    transmissionRatio_ = &transmissionRatio;
    useGearRatioDetection_ = &useGearRatioDetection;
    shiftPermission_ = &shiftPermission;
    dOrRengaged_ = &dOrRengaged;
    pinMode(SOL_12_45, OUTPUT);
    pinMode(SOL_23, OUTPUT);
    pinMode(SOL_34, OUTPUT);
}

void shiftControl::runShifts()
{
    checkIfPreshiftDelayIsNeeded();
    if (!*shifting_ && *shiftPermission_) // shift starts here
    {
        if (targetGearDelayed_ > *currentGear_ && *currentGear_ < 5) // upshifts
        {
            nextGear_ = *currentGear_ + 1;
            // read parameters into temp variables and apply them for this one particular shift:
            currentGearForShift_ = *currentGear_;            
            useGearRatioDetectionForShift_ = *useGearRatioDetection_;             
        }
        else if (targetGearDelayed_ < *currentGear_ && *currentGear_ > 1) // downshifts
        {
            nextGear_ = *currentGear_ - 1;
            // read parameters into temp variables and apply them for this one particular shift:
            currentGearForShift_ = *currentGear_;            
            useGearRatioDetectionForShift_ = *useGearRatioDetection_;            
        }
        shiftTimer_ = 0;
    }

    if (*shifting_) // what is executed during shift
    {        
        if (useGearRatioDetectionForShift_)
        {
            if (checkIfTransmissionRatioMatchesForGear(nextGear_) && shiftTimer_ > 100) // if ratio is reached in plausible time,
            { // end shift immediately
                digitalWrite(SOL_12_45, LOW);
                digitalWrite(SOL_23, LOW);
                digitalWrite(SOL_34, LOW);
                *currentGear_ = currentGearForShift_ = nextGear_;
                *shifting_ = false;

            }
            else if (shiftTimer_ == 3000 || shiftTimer_ == 6000) // if ratio is not reached in 3000ms or 6000ms, try to see if ratio matches any gear at all
            {
                uint8_t g = checkIfTransmissionRatioMatchesAnyGear();
                if (g != 0)
                {
                    digitalWrite(SOL_12_45, LOW);
                    digitalWrite(SOL_23, LOW);
                    digitalWrite(SOL_34, LOW);
                   *currentGear_= currentGearForShift_ = nextGear_ = *targetGear_ = g; // force the gear to be the one that is measured
                   *shifting_ = false; // and then end shift
                }                
            }
            else if (!*useGearRatioDetection_ && shiftTimer_ > 1500) // if decided not to use gear ratio detection during the shift,            
             // albeit it was used in the beginning of the shift, end shift in 1500ms, not so unusual situation and nothing to worry
            {
                digitalWrite(SOL_12_45, LOW);
                digitalWrite(SOL_23, LOW);
                digitalWrite(SOL_34, LOW); 
                *currentGear_= currentGearForShift_ = nextGear_;
                *shifting_ = false;
            }
            else if(shiftTimer_ > 6000) // if no gear ratio was reached after 6000ms, transmission is slipping! at least disable automatic mode here
            { // maybe show a notification and activate fault code?   
                digitalWrite(SOL_12_45, LOW);
                digitalWrite(SOL_23, LOW);
                digitalWrite(SOL_34, LOW); 
                *currentGear_= currentGearForShift_ = *targetGear_ = nextGear_;
                *shifting_ = false;
                Serial.println("activate manual mode!");
            }
        }
        else if (shiftTimer_ == shiftSolenoidTimeForShift_) //
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
    *currentGear_ = currentGearForShift_ = *targetGear_ = targetGearDelayed_ = nextGear_ = gear;
}

// control MPC & SPC solenoids
void shiftControl::controlPressureSolenoids()
{
    if (!*shifting_ && *dOrRengaged_)
    {
        *MPC_ = uint8_t(config_->giveRegularMPCMapValue(*oilTemp_, int(*load_))); // outside of shifts = regular drive. during shifts, MPC value will be set in activateSolenoids()
        *SPC_ = 100; // outside shifts, 100, because inverse control -> 100 = zero pressure
    }
    else if (!*shifting_ && !*dOrRengaged_)
    {
        *MPC_ = 30;
        *SPC_ = 100;
    }   

    REG_PWM_CDTYUPD0 = *MPC_; //pin 34/35, control the MPC solenoid
    REG_PWM_CDTYUPD1 = *SPC_; //pin 36/37, control the SPC solenoid   
    //Serial.print("MPC: ");
    //Serial.println(*MPC_);
    //Serial.print("  SPC: ");
    //Serial.println(*SPC_);
}

// activate both shift control & pressure control solenoids, when shift starts
void shiftControl::activateSolenoids()
{
    if (currentGearForShift_ == 1 && nextGear_ == 2) // shift from 1 to 2
    {
        *MPC_ = uint8_t(config_->giveShiftMapValue(configHandler::MPC_1to2, *driveType_, *oilTemp_, *load_));
        *SPC_ = uint8_t(config_->giveShiftMapValue(configHandler::SPC_1to2, *driveType_, *oilTemp_, *load_));
        shiftSolenoidTimeForShift_ = config_->giveShiftSolenoidTimeValue(*oilTemp_, *MPC_);       
        digitalWrite(SOL_12_45, HIGH);
        *shifting_ = true;
    }
    else if (currentGearForShift_ == 2 && nextGear_ == 3) // etc
    {
        *MPC_ = uint8_t(config_->giveShiftMapValue(configHandler::MPC_2to3, *driveType_, *oilTemp_, *load_));
        *SPC_ = uint8_t(config_->giveShiftMapValue(configHandler::SPC_2to3, *driveType_, *oilTemp_, *load_));  
        shiftSolenoidTimeForShift_ = config_->giveShiftSolenoidTimeValue(*oilTemp_, *MPC_);  
        digitalWrite(SOL_23, HIGH);
        *shifting_ = true;
    }
    else if (currentGearForShift_ == 3 && nextGear_ == 4)
    {
        *MPC_ = uint8_t(config_->giveShiftMapValue(configHandler::MPC_3to4, *driveType_, *oilTemp_, *load_));
        *SPC_ = uint8_t(config_->giveShiftMapValue(configHandler::SPC_3to4, *driveType_, *oilTemp_, *load_));  
        shiftSolenoidTimeForShift_ = config_->giveShiftSolenoidTimeValue(*oilTemp_, *MPC_); 
        digitalWrite(SOL_34, HIGH);
        *shifting_ = true;
    }
    else if (currentGearForShift_ == 4 && nextGear_ == 5)
    {
        *MPC_ = uint8_t(config_->giveShiftMapValue(configHandler::MPC_4to5, *driveType_, *oilTemp_, *load_));
        *SPC_ = uint8_t(config_->giveShiftMapValue(configHandler::SPC_4to5, *driveType_, *oilTemp_, *load_));  
        shiftSolenoidTimeForShift_ = config_->giveShiftSolenoidTimeValue(*oilTemp_, *MPC_); 
        digitalWrite(SOL_12_45, HIGH);
        *shifting_ = true;
    }
    else if (currentGearForShift_ == 5 && nextGear_ == 4)
    {
        *MPC_ = uint8_t(config_->giveShiftMapValue(configHandler::MPC_5to4, *driveType_, *oilTemp_, *load_));
        *SPC_ = uint8_t(config_->giveShiftMapValue(configHandler::SPC_5to4, *driveType_, *oilTemp_, *load_)); 
        shiftSolenoidTimeForShift_ = config_->giveShiftSolenoidTimeValue(*oilTemp_, *MPC_);  
        digitalWrite(SOL_12_45, HIGH);
        *shifting_ = true;
    }
    else if (currentGearForShift_ == 4 && nextGear_ == 3)
    {
        *MPC_ = uint8_t(config_->giveShiftMapValue(configHandler::MPC_4to3, *driveType_, *oilTemp_, *load_));
        *SPC_ = uint8_t(config_->giveShiftMapValue(configHandler::SPC_4to3, *driveType_, *oilTemp_, *load_)); 
        shiftSolenoidTimeForShift_ = config_->giveShiftSolenoidTimeValue(*oilTemp_, *MPC_);  
        digitalWrite(SOL_34, HIGH);
        *shifting_ = true;
    } 
    else if (currentGearForShift_ == 3 && nextGear_ == 2)
    {
        *MPC_ = uint8_t(config_->giveShiftMapValue(configHandler::MPC_3to2, *driveType_, *oilTemp_, *load_));
        *SPC_ = uint8_t(config_->giveShiftMapValue(configHandler::SPC_3to2, *driveType_, *oilTemp_, *load_)); 
        shiftSolenoidTimeForShift_ = config_->giveShiftSolenoidTimeValue(*oilTemp_, *MPC_);  
        digitalWrite(SOL_23, HIGH);
        *shifting_ = true;
    } 
    else if (currentGearForShift_ == 2 && nextGear_ == 1)
    {
        *MPC_ = uint8_t(config_->giveShiftMapValue(configHandler::MPC_2to1, *driveType_, *oilTemp_, *load_));
        *SPC_ = uint8_t(config_->giveShiftMapValue(configHandler::SPC_2to1, *driveType_, *oilTemp_, *load_));  
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
    float gap = 0.1;
    switch(gear)
    {
    case 1:
        if (*transmissionRatio_ <= 3.59 + gap && *transmissionRatio_ >= 3.59 - gap)
        {
            return true;
        }
        break;
    case 2:
        if (*transmissionRatio_ <= 2.19 + gap && *transmissionRatio_ >= 2.19 - gap)
        {
            return true;
        }
        break;
    case 3:
        if (*transmissionRatio_ <= 1.41 + gap && *transmissionRatio_ >= 1.41 - gap)
        {
            return true;
        }
        break;
    case 4:
        if (*transmissionRatio_ <= 1.00 + gap && *transmissionRatio_ >= 1.00 - gap)
        {
            return true;
        }
        break;
    case 5:
        if (*transmissionRatio_ <= 0.83 + gap && *transmissionRatio_ >= 0.83 - gap)
        {
            return true;
        }
        break;
    }
}

void shiftControl::checkIfPreshiftDelayIsNeeded()
{
    static int delayCounter = 0;
    static bool counting;

    if (!*usePreShiftDelay_)
    {
        targetGearDelayed_ = *targetGear_;
    }
    else
    {
        if (delayCounter == 0 && *targetGear_ != *currentGear_ && !*shifting_)
        {
            counting = true;
        }
        if (counting)
        {
            delayCounter++;
        }
        if (delayCounter == 10) 
        {
            counting = false;
            delayCounter = 0;
             targetGearDelayed_ = *targetGear_;
        }
    }
}

