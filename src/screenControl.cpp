#include "../headers/screenControl.h"  

screenControl::screenControl() : U8G2_ST7920_128X64_1_SW_SPI(U8G2_R0,9,10,11,8)
{

}

screenControl::~screenControl()
{

}

void screenControl::drawLiveData2()
{	
  const int firstLine = 18;

  setFont(u8g_font_5x8);
  drawStr(0, firstLine-9, "<<");
  drawStr(35, firstLine-9, "LIVE DATA 2");
  drawLine(0, firstLine-8, 128, firstLine-8);
}

void screenControl::drawOther()
{
  setFont(u8g2_font_HelvetiPixelOutline_te);
  drawStr(15, 20, "Other func!");
}

void screenControl::setBrightness(int brightness) {
  brightness_ = brightness;
  int pwmValue = 0;
  pwmValue = float(brightness) * 1.7; // set max brightness to 170 PWM = 3,33v = max !!!
  analogWrite(7, pwmValue);
}

int screenControl::giveBrightness()
{
  return brightness_;
}
/*
void screenControl::setDataPointers(core::dataStruct data)
{
    engineSpeed_ = data.engineSpeed;
    vehicleSpeed_ = data.vehicleSpeed;
    n2Speed_ = data.n2Speed;
    n3Speed_ = data.n3Speed;
    cardanSpeed_ = data.cardanSpeed;
    n3n2Ratio_ = data.n3n2Ratio;
} */