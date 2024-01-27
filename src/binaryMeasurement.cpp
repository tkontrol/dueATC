#include "../headers/binaryMeasurement.h"  

binaryMeasurement::binaryMeasurement(uint8_t inputPin):
inputPin_(inputPin)
{

}

binaryMeasurement::~binaryMeasurement()
{

}

//return a low-pass-filtered state of the pin
bool binaryMeasurement::giveState()
{
    bool state = !digitalRead(inputPin_);
    
    if (state != returnState_)
    {
        counter_++;
    }
    else
    {
        counter_ = 0;
    }

    if (counter_ == STATE_TRANSITION_COUNTS && returnState_ == false)
    {
        returnState_ = true;
        counter_ = 0;
    }
    else if (counter_ == STATE_TRANSITION_COUNTS)
    {
        returnState_ = false;
        counter_ = 0;
    }
    return returnState_;
}

bool binaryMeasurement::giveSingleShot()
{
    bool inputState = !digitalRead(inputPin_); // inverse because pull-up
    bool state;
    
    if (inputState && lastState_)
    {
        counter_++;
    }
    else if (lastState_)
    {
        counter_ = 0;
        state = false;
    }
    if (counter_ == SINGLECLICK_DETECTION_COUNTS)
    {
        state = true;       
        counter_ = 0;
    }    
    lastState_ = inputState;

    if (state && !block_)
    {
        block_ = true;
        return true;
    }
    return false;
}

void binaryMeasurement::releaseBlock()
{ 
    if (digitalRead(inputPin_)) // inverse because pull-up -> if button is not pressed, then...
    {
        block_ = false;
    }  
}