#include "../headers/TCCcontrol.h"  

TCCcontrol::TCCcontrol()
{

}

TCCcontrol::~TCCcontrol()
{

}

void TCCcontrol::setMeasurementPointers(int &slip)
{
    tcSlip_ = &slip;
}

int* TCCcontrol::giveOutputPointer()
{
    return &output_;
}

int* TCCcontrol::giveSetpointPointer()
{
    return &setPoint_;
}

void TCCcontrol::setOutputLimits(int lowerLimit, int upperLimit)
{
    outputLowerLimit_ = lowerLimit;
    outputUpperLimit_ = upperLimit;
}

void TCCcontrol::setKP(int kp)
{
    kp_ = kp;
}

void TCCcontrol::setKI(int ki)
{
    ki_ = ki;
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
            //REG_PWM_CDTYUPD2 = 100; //pin 38/39, no voltage to TCC solenoid
        break;

        case slipping:
            //REG_PWM_CDTYUPD2 = output_; //pin 38/39, PI control
        break;

        case closed:
            //REG_PWM_CDTYUPD2 = 0; //pin 38/39, 12v to TCC solenoid
        break;
    }
}

int TCCcontrol::calculatePIOutput() // call at 1ms intervals...
{   
    static int counterTimer = 0;
    counterTimer++;
    if (counterTimer == 100) // ...so that the calculation is done in 100ms intervals
    {
        counterTimer = 0;
        static int iterm = 0;
        int error = *tcSlip_ - setPoint_;  

        if ((output_ < outputUpperLimit_ || error < 0) && (output_ > outputLowerLimit_ || error > 0))
        {
            iterm = error * ki_ + iterm;
            //Serial.println("calc!");
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
       // Serial.print("err: ");
       // Serial.print(error);
       // Serial.print("  iterm: ");
       // Serial.print(iterm);
       // Serial.print("  output: ");
       // Serial.println(output_);
    }
    return output_;    
}