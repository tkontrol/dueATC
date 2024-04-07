#include "../headers/shiftControl.h"  

shiftControl::shiftControl()
{

}

shiftControl::~shiftControl()
{

}

void shiftControl::initShiftControl(uint8_t &currentGear, uint8_t &targetGear, bool &shifting, int &lastShiftDuration, float &transmissionRatio, bool &useGearRatioDetection, bool &shiftPermission)
{
    currentGear_ = &currentGear;
    nextGear_ = *currentGear_; // initial status
	targetGear_ = &targetGear;
    shifting_ = &shifting;
    lastShiftDuration_ = &lastShiftDuration;
    transmissionRatio_ = &transmissionRatio;
    useGearRatioDetection_ = &useGearRatioDetection;
    shiftPermission_ = &shiftPermission;
    pinMode(SOL_12_45, OUTPUT);
    pinMode(SOL_23, OUTPUT);
    pinMode(SOL_34, OUTPUT);
}

void shiftControl::runShifts()
{
    //Serial.print("curr: ");
    //Serial.print(*currentGear_);
    //Serial.print("  target: ");
    //Serial.print(*targetGear_);
    //Serial.print("  next: ");
    //Serial.print(nextGear_);
    //Serial.print(" shifting:");
    //Serial.println(*shifting_);

    if (!*shifting_ && *shiftPermission_) // shift starts here
    {
        if (*targetGear_ > *currentGear_ && *currentGear_ < 5) // upshifts
        {
            nextGear_ = *currentGear_ + 1;
            // read parameters into temp variables and apply them for this one particular shift:
            currentGearForShift_ = *currentGear_;            
            useGearRatioDetectionForShift_ = *useGearRatioDetection_;             
        }
        else if (*targetGear_ < *currentGear_ && *currentGear_ > 1) // downshifts
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
            if (checkIfTransmissionRatioMatchesForGear(nextGear_)) // if ratio is reached, end shift
            {
                digitalWrite(SOL_12_45, LOW);
                digitalWrite(SOL_23, LOW);
                digitalWrite(SOL_34, LOW);
                *currentGear_ = currentGearForShift_ = nextGear_;
                *shifting_ = false;
            }
        }
        else if (shiftTimer_ >= 1500) //
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
}

void shiftControl::activateSolenoids()
{
    if (currentGearForShift_ == 1 && nextGear_ == 2 ) // shift from 1 to 2
    {
        digitalWrite(SOL_12_45, HIGH);
        *shifting_ = true;
    }
    else if (currentGearForShift_ == 2 && nextGear_ == 3) // etc
    {
        digitalWrite(SOL_23, HIGH);
        *shifting_ = true;
    }
    else if (currentGearForShift_ == 3 && nextGear_ == 4)
    {
        digitalWrite(SOL_34, HIGH);
        *shifting_ = true;
    }
    else if (currentGearForShift_ == 4 && nextGear_ == 5)
    {
        digitalWrite(SOL_12_45, HIGH);
        *shifting_ = true;
    }
    else if (currentGearForShift_ == 5 && nextGear_ == 4)
    {
        digitalWrite(SOL_12_45, HIGH);
        *shifting_ = true;
    }
    else if (currentGearForShift_ == 4 && nextGear_ == 3)
    {
        digitalWrite(SOL_34, HIGH);
        *shifting_ = true;
    } 
    else if (currentGearForShift_ == 3 && nextGear_ == 2)
    {
        digitalWrite(SOL_23, HIGH);
        *shifting_ = true;
    } 
    else if (currentGearForShift_ == 2 && nextGear_ == 1)
    {
        digitalWrite(SOL_12_45, HIGH);
        *shifting_ = true;
    } 
}

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

