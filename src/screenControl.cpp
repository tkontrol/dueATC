#include "../headers/screenControl.h"  

screenControl::screenControl() : U8G2_ST7920_128X64_1_SW_SPI(U8G2_R0,9,10,11,8)
{

}

screenControl::~screenControl()
{

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

void screenControl::advanceTextScrolls() // runs on every uiLoop (100ms)
{
  scrollCounter_++;
}

void screenControl::scrollText(String *text, uint8_t maxChars, int uiLoopsBetweenCharSrolls)
{
  static uint8_t firstChar = 0;
  static uint8_t textLength = 0;

  if (textLength < text->length()) // in case there are multiple texts running at the same time on the screen, the longest one dictates
  {
    textLength = text->length();
  }

  if (text->length() <= maxChars) // if text fits to given area without scrolling, print it directly
  {
    print(text->c_str());
  } 
  else 
  {
    print (text->substring(firstChar, firstChar + maxChars));
    if (scrollCounter_ >= uiLoopsBetweenCharSrolls) // roll the text
    {
      scrollCounter_ = 0;
      firstChar++;
    }
    if (firstChar == textLength) // start scrolling from the beginning
    {
      scrollCounter_ = 0;
      firstChar = 0;
    }
  } 
}