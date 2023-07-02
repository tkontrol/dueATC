#include "../headers/TCCcontrol.h"  

TCCcontrol::TCCcontrol()
{

}

TCCcontrol::~TCCcontrol()
{

}

void TCCcontrol::setMeasurementPointers(int &engineSpeed, int &incomingShaftSpeed)
{
    engineSpeed_ = &engineSpeed;
    incomingShaftSpeed_ = &incomingShaftSpeed;
}

int* TCCcontrol::giveOutputPointer()
{
    return &output_;
}

void TCCcontrol::setOutputLimits(int lowerLimit, int upperLimit)
{
    outputLowerLimit_ = lowerLimit;
    outputUpperLimit_ = upperLimit;
}

void TCCcontrol::setKP(int kp)
{
    kp_ = kp;
    Serial.println(kp_);
}

void TCCcontrol::setKI(int ki)
{
    ki_ = ki;
    Serial.println(ki_);
}

void TCCcontrol::setTCCmode(TCCcontrol::TCCMode &mode)
{
    TCCmode_ = &mode;
}

void TCCcontrol::updateTCC()
{     
    switch (*TCCmode_)
    {
        case open:
            REG_PWM_CDTYUPD2 = 100; //pin 38/39, no voltage to TCC solenoid
        break;

        case slipping:
            REG_PWM_CDTYUPD2 = givePIOutput(); //pin 38/39, PI control
        break;

        case closed:
            REG_PWM_CDTYUPD2 = 0; //pin 38/39, 12v to TCC solenoid
        break;
    }
}

int TCCcontrol::givePIOutput()
{   
    static int counterTimer = 0;
    counterTimer++;
    if (counterTimer == 100)
    {
        counterTimer = 0;
        static int iterm = 0;
        int slip = abs(*engineSpeed_ - *incomingShaftSpeed_);
        int error = setPoint_ - slip;  

        if (output_ < outputUpperLimit_ && output_ > outputLowerLimit_) //integer windup
        {
            iterm = error * ki_ + iterm;  
        }   

        output_ = kp_ * error + iterm; 

        if (output_ > outputUpperLimit_) //limit output
        {
            output_ = outputUpperLimit_;
        }
        else if (output_ < outputLowerLimit_) //limit output
        {
            output_ = outputLowerLimit_;
        }
       // Serial.println(slip);
    }
    return output_;    
}