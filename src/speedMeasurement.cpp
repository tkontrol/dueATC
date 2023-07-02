#include "../headers/speedMeasurement.h"  

speedMeasurement::speedMeasurement(int interruptInterval, int minRPM, int maxRPM):
    interruptInterval_(interruptInterval),
    minRPM_(minRPM),
    maxRPM_(maxRPM)
{
}

speedMeasurement::~speedMeasurement()
{

}

// changes pulsesPerRev value
void speedMeasurement::setPulsesPerRev(int pulsesPerRev)
{
    calcConst_ = 60 / pulsesPerRev * 1000000; // now this does not need to be calculated in every giveRPM() -call
    longestPeriodToAccept_ = (60 * 1000000) / (minRPM_ * pulsesPerRev); // based on pulses per rev and minimun allowable rpm, calculate longestPeriodToAccept
    shortestPeriodToAccept_ = (60 * 1000000) / (maxRPM_ * pulsesPerRev); // based on pulses per rev and minimun allowable rpm, calculate longestPeriodToAccept
}

// clock, run with 10us intervals
void speedMeasurement::increaseCounter()
{
     counter_ += interruptInterval_; 
}

// run on every rising edge
void speedMeasurement::calcPeriodLength() // run on rising edge of signal, update the variable
{    
    mem_ = counter_;
    counter_ = 0;
}

// lowpass filter, run with 1ms intervals
void speedMeasurement::useLowPass()
{   
    int cp = mem_; // store current period length
    if (periodLength_ != 0 && cp > periodLength_ + 100) // if current period (cp) is 100us longer than last one
    {
        periodLength_ = periodLength_ + 100; // increase last one with 100us
        return;
    }
    else if (periodLength_ != 0 && cp <= periodLength_ - 100) // if current period (cp) is 100us shorter than last one
    {        
        periodLength_ = periodLength_ - 100; // shorten last one with 100us
        return;
    }
    periodLength_ = cp; //if diff < 100, use current period as periodLength_
}

// return RPM
int speedMeasurement::giveRPM()
{
    useLowPass();
    // periodLength_ = mem_;
    int rpm = calcConst_ / periodLength_;
    if (rpm < minRPM_ || counter_ > longestPeriodToAccept_)
    {
        periodLength_ = 0;
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
