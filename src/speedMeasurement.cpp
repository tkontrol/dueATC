#include "../headers/speedMeasurement.h"  

speedMeasurement::speedMeasurement(int interruptInterval, int minRPM, int maxRPM, int maxAllowedPeriodLengthChangeBetweenMeasurements, bool readDualSignal):
    interruptInterval_(interruptInterval),
    minRPM_(minRPM),
    maxRPM_(maxRPM),
    readDualSignal_ (readDualSignal)
{
    lowerFactorForLowPass_ = float((1000 - maxAllowedPeriodLengthChangeBetweenMeasurements) / 1000);
    upperFactorForLowPass_ = float((1000 + maxAllowedPeriodLengthChangeBetweenMeasurements) / 1000);
}

speedMeasurement::~speedMeasurement()
{

}

// changes pulsesPerRev value
void speedMeasurement::setPulsesPerRev(int pulsesPerRev)
{
    calcConst_ = 60 / pulsesPerRev * 1000000; // with calcConst_, this does not need to be calculated in every giveRPM() -call
    longestPeriodToAccept_ = (60 * 1000000) / (minRPM_ * pulsesPerRev); // based on pulses per rev and minimun allowable rpm, calculate longestPeriodToAccept
    shortestPeriodToAccept_ = (60 * 1000000) / (maxRPM_ * pulsesPerRev); // based on pulses per rev and maximum allowable rpm, calculate shortestPeriodToAccept
}

// clock, run with 10us intervals
void speedMeasurement::increaseCounter()
{
     counter_ += interruptInterval_; 
}

// run on every rising edge
void speedMeasurement::updatePeriodLength()
{    
    if (readDualSignal_)
    {
        if (evenOddCounter_ == 0)
        {
            evenOddCounter_++;           
            dualMem1_ = counter_;
        }
        else
        {
            evenOddCounter_ = 0;
            dualMem2_ = counter_;
        }
        // use shorter of the values, for cases when the speed signal contains two independent period lenghts, and you are interested in the shorter one
        if (dualMem1_ < dualMem2_)
        {
            mem_ = dualMem2_;
            counter_ = 0;
        }
        else
        {
            mem_ = dualMem1_;
            counter_ = 0;
        }
    }
    else
    {
        mem_ = counter_;
        counter_ = 0;
    }    
}

// lowpass filter, to be run with 1ms intervals
void speedMeasurement::useLowPass()
{   
    int cp = mem_; // store current period (cp) length
    float diff = 0;
    diff = cp / periodLength_;
    if (diff > upperFactorForLowPass_)
    {
        periodLength_ = int(cp * upperFactorForLowPass_);
        return;
    }
    else if (diff < lowerFactorForLowPass_)
    {
        periodLength_ = int(cp * lowerFactorForLowPass_);
        return;
    }
    periodLength_ = cp;
}

// return RPM
int speedMeasurement::giveRPM()
{
    useLowPass(); // comment this if you want to disable low pass filter
    //periodLength_ = mem_; // uncomment this if you want to disable low pass filter
    int rpm = calcConst_ / periodLength_;
    if (rpm < minRPM_ || counter_ > longestPeriodToAccept_)
    {
        periodLength_ = 0;
        //counter_ = longestPeriodToAccept_; // to prevent overflow -> WHY DOES THIS CAUSE MEASUREMENT TO STUCK AT ZERO
        return 0;
    }
    else if (rpm > maxRPM_)
    {
        periodLength_ = shortestPeriodToAccept_;
        return maxRPM_;
    }
    return rpm;
}

// return counter
int speedMeasurement::giveCounter() const
{
    return counter_;
}

// return latest periodLength_;
int speedMeasurement::givePeriodLength()
{
    useLowPass();
    if (periodLength_ > longestPeriodToAccept_ || counter_ > longestPeriodToAccept_)
    {
        //periodLength_ = 0;
        return 0;
    }
    else if (periodLength_ < shortestPeriodToAccept_)
    {
        return shortestPeriodToAccept_;
    }
    return periodLength_;
}
