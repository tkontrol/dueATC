#include "../headers/controlButton.h"  

controlButton::controlButton(int buttonPin, bool enableRepeat):
buttonPin_(buttonPin),
repeatable_(enableRepeat)
{
    
}

controlButton::~controlButton()
{

}

//returns 'true's as long as button is pressed
bool controlButton::givePulse()
{
    bool buttonState = !digitalRead(buttonPin_); // inverse because pull-up    
    bool state;
                
    if (buttonState && buttonLastState_)
    {
        buttonCounter_++;
    }
    else if (buttonLastState_)
    {
        buttonCounter_ = 0;
        fastRepeatCounter_ = 0;
        countLimit_ = SINGLECLICK_DETECTION_COUNTS;
        state = false;
    }
    if (buttonCounter_ == countLimit_)
    {
        state = true;
        fastRepeatCounter_++;
        if (repeatable_)
        {
            if (fastRepeatCounter_ < FASTREPEAT_LIMIT)
            {
                countLimit_ = REPEAT_DETECTION_COUNTS;
            }
            else
            {
                countLimit_ = FASTREPEAT_DETECTION_COUNTS;
            }
        }
        else
        {
            countLimit_ = -1; // this disables the repeating, because -1 is never reached
        }        
        buttonCounter_ = 0;
    }
    buttonLastState_ = buttonState;

    if (state && !block_)
    {
        block_ = true;
        return true;
    }
    return state;
} 

//returns only ONE 'true' in the first loop, not in upcoming loops, even if the button is kept pressed 
/*
bool controlButton::giveSingleShot()
{
    bool buttonState = !digitalRead(buttonPin_); // inverse because pull-up   

    if (buttonState && !block_)
    {
        block_ = true;
        return true;
    }
    return false;
} */

//returns only ONE 'true' in the first loop, not in upcoming loops, even if the button is kept pressed 
bool controlButton::giveSingleShot()
{
    bool buttonState = !digitalRead(buttonPin_); // inverse because pull-up
    bool state;
    
    if (buttonState && buttonLastState1_)
    {
        blockCounter_++;
    }
    else if (buttonLastState1_)
    {
        blockCounter_ = 0;
        state = false;
    }
    if (blockCounter_ == SINGLECLICK_DETECTION_COUNTS)
    {
        state = true;       
        blockCounter_ = 0;
    }    
    buttonLastState1_ = buttonState;

    if (state && !block_)
    {
        block_ = true;
        return true;
    }
    return false;
}

//called in ui-loop before updateMenu()
void controlButton::releaseBlock()
{ 
    if (digitalRead(buttonPin_)) // inverse because pull-up -> if button is not pressed, then...
    {
        block_ = false;
    }  
    /*
    bool buttonState = digitalRead(buttonPin_); // inverse because pull-up -> if button is not pressed, then...
    bool state;
    
    if (buttonState && buttonLastState_)
    {
        blockCounter_++;
    }
    else if (buttonLastState_)
    {
        buttonCounter_ = 0;
        state = false;
    }
    if (blockCounter_ == BLOCK_COUNTS)
    {
        state = true;       
        blockCounter_ = 0;
    }
    
    buttonLastState_ = buttonState;
    block_ = state;  */
}

// return current "raw" state of pin
bool controlButton::isPressed()
{
    return !digitalRead(buttonPin_); // inverse because pull-up
}

