#include "../headers/analogMeasurement.h"  

analogMeasurement::analogMeasurement(uint8_t ADCRegisterNum, int maxChangeBetweenMeasurements):
ADCRegisterNum_(ADCRegisterNum),
maxChangeBetweenMeasurements_(maxChangeBetweenMeasurements)
{

}

analogMeasurement::~analogMeasurement()
{

}

int analogMeasurement::giveVoltage()
{
    mem_ = ADC->ADC_CDR[ADCRegisterNum_] / 1023.0 * 3300;
    useLowPass();
    //voltage_ = mem_;
    return voltage_ ;
}

void analogMeasurement::useLowPass()
{   
    int cv = mem_; // store current value
    if (voltage_ != 0 && cv > voltage_ + maxChangeBetweenMeasurements_) // if current voltage(cv) is larger than accepted than last measured voltage
    {
        voltage_ = voltage_ + maxChangeBetweenMeasurements_; // increase last one with largest accepted change
        return;
    }
    else if (voltage_ != 0 && cv <= voltage_ - maxChangeBetweenMeasurements_) // if current voltage(cv) is shorter than accepted than last measured voltage
    {        
        voltage_ = voltage_ - maxChangeBetweenMeasurements_; // shorten last one with largest accepted change
        return;
    }
    voltage_ = cv; //if diff < maxChangeBetweenMeasurements_, use current voltage directly
}