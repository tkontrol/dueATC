#include "../headers/ui.h"  

ui::ui():
    core_(core(10, 22, 24, 26, 49, 48)), // 
    screen_(screenControl()),
    upButton_(controlButton(upButtonPin, true)),
    downButton_(controlButton(downButtonPin, true)),
    leftButton_(controlButton(leftButtonPin, true)),
    rightButton_(controlButton(rightButtonPin, true)),
    okButton_(controlButton(okButtonPin)),
    cancelButton_(controlButton(cancelButtonPin)),
    autoModeSwitch_(controlButton(autoModeSwitchPin)),
    gearUp_(controlButton(gearUpPin)),
    gearDown_(controlButton(gearDownPin))
{

}

ui::~ui()
{

}

void ui::increaseSpeedMeasCounters()
{
  core_.increaseSpeedMeasCounters();
  clock_= clock_ + 10;
}

void ui::engineSpeedMeas()
{
  core_.engineSpeedMeas();    
}

void ui::primaryVehicleSpeedMeas()
{    
  core_.primaryVehicleSpeedMeas();
}

void ui::secondaryVehicleSpeedMeas()
{    
  core_.secondaryVehicleSpeedMeas();
}

void ui::n2SpeedMeas()
{
  core_.n2SpeedMeas();
}

void ui::n3SpeedMeas()
{
  core_.n3SpeedMeas();
}

void ui::runCoreLoop() // called in 1ms intervals
{
  loopStartTime_ = clock_;
  core_.coreloop();
  loopEndTime_ = clock_;
}

void ui::runUiLoop() //called in 100ms intervals
{
  checkForMalfunction();
  checkPendingCoreNotifications();  
  screen_.advanceTextScrolls();
  if (notificationActive_)
  {
    printNotification();
  }
  else
  {
    rightButton_.releaseBlock();
    leftButton_.releaseBlock();
    upButton_.releaseBlock();
    downButton_.releaseBlock();
    okButton_.releaseBlock();
    cancelButton_.releaseBlock();
    updateMenu();
  }
  gearUp_.releaseBlock();
  gearDown_.releaseBlock();
  readShiftSwitches();
}

void ui::readShiftSwitches()
{
  if (gearUp_.giveSingleShot())
  {
    core_.gearUpRequest();
  }
  else if (gearDown_.giveSingleShot())
  {
    core_.gearDownRequest();
  }
}

void ui::printLoopDuration()
{
  int loopDuration = (loopEndTime_ - loopStartTime_);
  screen_.setFont(u8g_font_4x6r);
  screen_.setCursor(0, 5);
  screen_.print(loopDuration);
  screen_.print(" us");
}

void ui::initUI()
{
  pinMode(upButtonPin, INPUT_PULLUP);
  pinMode(downButtonPin, INPUT_PULLUP);
  pinMode(leftButtonPin, INPUT_PULLUP);
  pinMode(rightButtonPin, INPUT_PULLUP);
  pinMode(okButtonPin, INPUT_PULLUP);
  pinMode(cancelButtonPin, INPUT_PULLUP); 
  pinMode(autoModeSwitchPin, INPUT_PULLUP);
  pinMode(gearUpPin, INPUT_PULLUP);
  pinMode(gearDownPin, INPUT_PULLUP);

  core_.initController();
  setScreenDataPointers();
  log_ = core_.giveLogPtr();
  parametersPtr_ = core_.givePtrToConfigurationSet()->parameters;
  dualAxisMapsPtr_ = core_.givePtrToConfigurationSet()->dualAxisMaps;
  singleAxisMapsPtr_ = core_.givePtrToConfigurationSet()->singleAxisMaps;
  screen_.begin();
  currentMenu_ = &mainMenu_;
  menuFuncExecuting_ = true; // start at MainScreen, because it is the first in MainMenu (which is at index 0 = default at startup)

  showNotification(1500, core_.readConfFile()); // here the data from the SD card is read to RAM

  for (int i = 0; i < parametersPtr_->size; i++)
  {
    if (parametersPtr_->parameters[i].ID == "Screen_brightness")
    {
      screen_.setBrightness(parametersPtr_->parameters[i].data);
      break;
    }
  }
  analogWrite(12, 240); // VO input to screen, CONTRAST
  core_.startupProcedure();
}

void ui::showNotification(int time, String msg)
{
   notificationMessage_ = msg;
   notificationActive_ = true;
   core_.startNotificationTimer(time);
}

void ui::checkForMalfunction()
{
  if (core_.giveMalfunctionStatus())
  {
    //fallBackMenu_ = currentMenu_; 
    //fallBackSelection_ = currentMenu_->selection;
    //currentMenu_ = &malfunctionMenu_;
    //currentMenu_->selection = 1;
  }
}

void ui::checkPendingCoreNotifications()
{
  if (core_.checkNotification().pending)
  {   
    core_.markNotificationAsReceived();   
    showNotification(core_.checkNotification().time, core_.checkNotification().message);      
  }
}

void ui::printNotification()
{
  screen_.setFont(u8g2_font_profont11_tr);
  uint8_t len = notificationMessage_.length();

  if (len <= 15)
  {
      screen_.setCursor(10, 25);
      screen_.print(notificationMessage_);
  }
  else if (len <= 30)
  {
      uint8_t index = 15;
      while (notificationMessage_.charAt(index) != ' ' && index > 0)
      {
        index--;
      }

      String line1;
      String line2;

      line1 = notificationMessage_.substring(0, index);
      line2 = notificationMessage_.substring(index);
      line2.trim();

      screen_.setFont(u8g2_font_profont11_tr);
      screen_.setCursor(10, 25);
      screen_.print(line1);
      screen_.setCursor(10, 35);
      screen_.print(line2);        
  }
  else if (len <= 60)
  {
      uint8_t index1 = 15;
      while (notificationMessage_.charAt(index1) != ' ' && index1 > 0)
      {
        index1--;
      }
      String line1;
      String line2;
      String line3;

      line1 = notificationMessage_.substring(0, index1);

      uint8_t index2 = 30;
      while (notificationMessage_.charAt(index2) != ' ' && index2 > index1)
      {
        index2--;
      }

      line2 = notificationMessage_.substring(index1, index2);
      line2.trim();
      line3 = notificationMessage_.substring(index2);
      line3.trim();

      screen_.setFont(u8g2_font_profont11_tr);
      screen_.setCursor(10, 25);
      screen_.print(line1);
      screen_.setCursor(10, 35);
      screen_.print(line2);        
      screen_.setCursor(10, 45);
      screen_.print(line3);
  }

  else if (len <= 90)
  {
      uint8_t index1 = 15;
      while (notificationMessage_.charAt(index1) != ' ' && index1 > 0)
      {
        index1--;
      }
      String line1;
      String line2;
      String line3;
      String line4;

      line1 = notificationMessage_.substring(0, index1);

      uint8_t index2 = 30;
      while (notificationMessage_.charAt(index2) != ' ' && index2 > index1)
      {
        index2--;
      }

      uint8_t index3 = 45;
      while (notificationMessage_.charAt(index3) != ' ' && index3 > index2)
      {
        index3--;
      }

      line2 = notificationMessage_.substring(index1, index2);
      line2.trim();
      line3 = notificationMessage_.substring(index2, index3);
      line3.trim();
      line4 = notificationMessage_.substring(index3);
      line4.trim();

      screen_.setFont(u8g2_font_profont11_tr);
      screen_.setCursor(10, 25);
      screen_.print(line1);
      screen_.setCursor(10, 35);
      screen_.print(line2);        
      screen_.setCursor(10, 45);
      screen_.print(line3);
      screen_.setCursor(10, 55);
      screen_.print(line4);
  }

  if (core_.hasNotificationTimePassed())
  {
    notificationActive_ = false;
    core_.markNotificationAsShown();
  }
  //notificationActive_ = !core_.hasNotificationTimePassed(); // disable timer after time has passed   
}

void ui::updateParameter(configHandler::parameter* p)
{
    if (p->ID == "Screen_brightness")
    {
      screen_.setBrightness(p->data);
    }
    else
    {
      core_.updateParameter(p);
    }
}

struct configHandler::dualAxisMap* ui::givePtrToDualAxisMap(String name)
{
    for (int i = 0; i < dualAxisMapsPtr_->size; i++)
    {
        if (name.equals(dualAxisMapsPtr_->maps[i].ID))
        {
            return &dualAxisMapsPtr_->maps[i];
        }
    }
    return nullptr;
}

struct configHandler::singleAxisMap* ui::givePtrToSingleAxisMap(String name)
{
    for (int i = 0; i < singleAxisMapsPtr_->size; i++)
    {
        if (name.equals(singleAxisMapsPtr_->maps[i].ID))
        {
            return &singleAxisMapsPtr_->maps[i];
        }
    }
    return nullptr;
}

void ui::updateMenu()
{
  static uint8_t executableFunctionNumber = 0;
  if (!menuFuncExecuting_) // if executing flag is not set, print menu
  {
    bool up = upButton_.givePulse();
    bool down = downButton_.givePulse();
    bool ok = okButton_.givePulse(); // this must be givePulse, not singleShot, because singleShot will prevent returning true later in a loop if used in a function
    bool cancel = cancelButton_.giveSingleShot();

    if (down)
    {
      if (currentMenu_->selection < currentMenu_->size - 1)
      {
        currentMenu_->selection++;
      }
      else // return to beginning of menu
      {
        currentMenu_->selection = 0;
      }      
    }
    else if (up)
    {
      if (currentMenu_->selection > 0)
      {
        currentMenu_->selection--;
      }
      else // jump to the last object of menu
      {
        currentMenu_->selection = currentMenu_->size - 1;
      }
    }
    else if (ok)
    {
      menuFuncExecuting_ = true; // enable the class-wide execution flag
      executableFunctionNumber = currentMenu_->selection; // set the selected menu object to be executable
    }
    else if (cancel && currentMenu_->hostMenu != NULL)
    {      
      currentMenu_ = currentMenu_->hostMenu; // return to host menu when pressing cancel      
    }
    drawMenu(currentMenu_);
  }
  else if ((this->*currentMenu_->menuObj[executableFunctionNumber].function)()) // if execution flag is set, execute the selected item until it returns true (cancel pressed)
  {
    //currentMenu_->selection = 0;
    menuFuncExecuting_ = false;
  }
}

void ui::firstPage()
{
  screen_.firstPage();
}

bool ui::nextPage()
{
  return screen_.nextPage(); // for picture loop
}

bool ui::showMainScreen()
{
  drawMainScreen();
  if (leftButton_.giveSingleShot())
  {
    core_.forceGearVariables();
  }
  if (upButton_.givePulse())
  {
    if (!core_.toggleAutoMan())
    {
      showNotification(3000, "No AUT mode due to some missing conf on SD!");
    }
  }
  if (downButton_.givePulse())
  {
    //core_.toggleRatioDetection();
    //showNotification(1000, "Toggled gear ratio detection");
  }

  return cancelButton_.giveSingleShot();
}

void ui::drawMainScreen()
{ 
  static int blinkCounter = 0;

  if (*dataPtrs_.shifting)
  {
      blinkCounter++;

      if (blinkCounter < 5)
      {
        screen_.setFont(u8g2_font_7Segments_26x42_mn);
        if (*dataPtrs_.currentGear == 1)
        {
          screen_.setCursor(-10, 50);
        }
        else
        {
          screen_.setCursor(0, 50);
        }
        screen_.print(*dataPtrs_.currentGear); 
      }
      
      if (blinkCounter > 10)
      {
        blinkCounter = 0;
      }
  }
  else
  {
    blinkCounter = 0;
    screen_.setFont(u8g2_font_7Segments_26x42_mn);
    if (*dataPtrs_.currentGear == 1)
    {
      screen_.setCursor(-10, 50);
    }
    else
    {
      screen_.setCursor(0, 50);
    }
    screen_.print(*dataPtrs_.currentGear); 
  }


  screen_.drawLine(0, 55, 30, 55);
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.setCursor(5, 64);

  switch(*dataPtrs_.shiftingMod)
  {
  case core::shiftingMode::MAN:
      screen_.print("MAN");
  break;
  case core::shiftingMode::AUT:
      screen_.print("AUT");
  break;
  }

  // lines for target gear display and PRND
  screen_.drawLine(30, 18, 30, 64);
  screen_.drawLine(30, 18, 49, 18);
  screen_.drawLine(49, 0, 49, 64);
  screen_.drawLine(30, 39, 49, 39);

  screen_.setFont(u8g2_font_profont22_tf);
  screen_.setCursor(35, 15);
  screen_.print(*dataPtrs_.targetGear);  

  switch(*dataPtrs_.leverPosition)
  {
  case core::leverPos::P:
      screen_.setFont(u8g2_font_profont22_tf); 
      screen_.drawStr(35, 36, "P");
  break;
    case core::leverPos::R:
      screen_.setFont(u8g2_font_profont22_tf); 
      screen_.drawStr(35, 36, "R");
  break;
    case core::leverPos::N:
      screen_.setFont(u8g2_font_profont22_tf); 
      screen_.drawStr(35, 36, "N"); 
  break;
    case core::leverPos::D:
      screen_.setFont(u8g2_font_profont22_tf); 
      screen_.drawStr(35, 36, "D");
  break;
  }

  screen_.setFont(u8g_font_4x6r);
  screen_.setCursor(32, 46);
  screen_.print("MPC");
  screen_.setCursor(32, 52);
  screen_.print(*dataPtrs_.MPC);

  screen_.setCursor(44, 49);
  screen_.print(*dataPtrs_.MPCchange);

  screen_.setCursor(32, 58);
  screen_.print("SPC");
  screen_.setCursor(32, 64);
  screen_.print(*dataPtrs_.SPC);

  screen_.setCursor(44, 60);
  screen_.print(*dataPtrs_.SPCchange);

  // Other data -section
  //setFont(u8g2_font_profont10_mf);
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(52, 6, "Speed");
  screen_.setCursor(105, 6);
  screen_.print(*dataPtrs_.vehicleSpeed);  
  screen_.setCursor(52, 16);
  screen_.print("Oil");


  //screen_.drawStr(52, 16, "Oil <");
  screen_.setFont(u8g_font_4x6r);
  if (*dataPtrs_.oilTempStatus == configHandler::oilTemp::cold)
  {
    screen_.print("<cold>");
  }
  else
  {
    screen_.print("<warm>");
  }
  screen_.setCursor(105, 16);
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  if (*dataPtrs_.leverPosition == core::leverPos::R || *dataPtrs_.leverPosition == core::leverPos::D)
  {
    screen_.print(*dataPtrs_.oilTemp);
  }
  else
  {
    screen_.print("???");
  }
  
  screen_.drawStr(52, 26, "Load");
  screen_.setCursor(105, 26);
  screen_.print(*dataPtrs_.load);
  screen_.drawStr(52, 36, "Eng");
  screen_.setCursor(100, 36);
  screen_.print(*dataPtrs_.engineSpeed);
  
  screen_.setFont(u8g_font_4x6r);
  screen_.drawStr(52, 44, "measuredGear:");

  screen_.setCursor(105, 44);
  screen_.print(*dataPtrs_.measuredGear);
  switch(*dataPtrs_.tccMod)
  {
  case TCCcontrol::TCCMode::open:
     // screen_.drawStr(105, 44, "open");
  break;
  case TCCcontrol::TCCMode::slipping:
     // screen_.drawStr(105, 44, "slipping");
  break;
    case TCCcontrol::TCCMode::closed:
    //  screen_.drawStr(105, 44, "closed");
  break;
  } 

  screen_.drawStr(52, 53, "LastShiftDur");
  screen_.setCursor(105, 53);
  screen_.print(*dataPtrs_.lastShiftDuration);

  screen_.drawStr(52, 63, "Transm.ratio:");
  screen_.setCursor(105, 63);
 // if (*dataPtrs_.dType == configHandler::driveType::load)
  //{
    screen_.print(*dataPtrs_.transmissionRatio);
 // }
  //else
  //{
  //  screen_.print("coast");
  //}
}

bool ui::showLiveData1()
{
  drawLiveData1();    
  return cancelButton_.giveSingleShot();
}

bool ui::showLiveData2()
{
  drawLiveData2();    
  return cancelButton_.giveSingleShot();
}

bool ui::showLiveData3()
{
  drawLiveData3();    
  return cancelButton_.giveSingleShot();
}

bool ui::showLiveData4()
{
  drawLiveData4();    
  return cancelButton_.giveSingleShot();
}

void ui::drawLiveData1()
{
  const int columnXPos = 100;	
  const int firstLine = 18;
  const int lineGap = 9;
	
  screen_.setFont(u8g_font_5x8);
  screen_.drawStr(35, firstLine-9, "LIVE DATA PAGE 1");
  screen_.drawLine(0, firstLine-8, 128, firstLine-8);
  screen_.drawStr(0, firstLine, "Eng spd, rpm:");
  screen_.setCursor(columnXPos, firstLine);
  screen_.print(*dataPtrs_.engineSpeed);
  screen_.drawLine(0, firstLine+1, 128, firstLine+1);

  screen_.drawStr(0, firstLine+lineGap, "Veh spd, km/h:");
  screen_.setCursor(columnXPos, firstLine+lineGap);
  screen_.print(*dataPtrs_.vehicleSpeed);  
  screen_.drawLine(0, firstLine+lineGap+1, 128, firstLine+lineGap+1);
  
  screen_.drawStr(0, firstLine+lineGap*2, "Prim veh spd, rpm:");
  screen_.setCursor(columnXPos, firstLine+lineGap*2);
  screen_.print(*dataPtrs_.primaryVehicleSpeed);  
  screen_.drawLine(0, firstLine+lineGap*2+1, 128, firstLine+lineGap*2+1);
  
  screen_.drawStr(0, firstLine+lineGap*3, "Sec veh spd, rpm:");
  screen_.setCursor(columnXPos, firstLine+lineGap*3);
  screen_.print(*dataPtrs_.secondaryVehicleSpeed);  
  screen_.drawLine(0, firstLine+lineGap*3+1, 128, firstLine+lineGap*3+1);
  
  screen_.drawStr(0, firstLine+lineGap*4, "N2 spd, rpm:");
  screen_.setCursor(columnXPos, firstLine+lineGap*4);
  screen_.print(*dataPtrs_.n2Speed);  
  screen_.drawLine(0, firstLine+lineGap*4+1, 128, firstLine+lineGap*4+1);

  screen_.drawStr(0, firstLine+lineGap*5, "N3 spd, rpm:");
  screen_.setCursor(columnXPos, firstLine+lineGap*5);
  screen_.print(*dataPtrs_.n3Speed);  
}

void ui::drawLiveData2()
{
  const int columnXPos = 100;	
  const int firstLine = 18;
  const int lineGap = 9;
	
  screen_.setFont(u8g_font_5x8);
  screen_.drawStr(35, firstLine-9, "LIVE DATA PAGE 2");
  screen_.drawLine(0, firstLine-8, 128, firstLine-8);

  screen_.drawStr(0, firstLine, "N3/N2 ratio:");
  screen_.setCursor(columnXPos, firstLine);
  screen_.print(*dataPtrs_.n3n2Ratio);
  screen_.drawLine(0, firstLine+1, 128, firstLine+1);

  screen_.drawStr(0, firstLine+lineGap, "Cardan spd, rpm:");
  screen_.setCursor(columnXPos, firstLine+lineGap);
  screen_.print(*dataPtrs_.cardanSpeed);  
  screen_.drawLine(0, firstLine+lineGap+1, 128, firstLine+lineGap+1);
  
  screen_.drawStr(0, firstLine+lineGap*2, "Transm.ratio:");
  screen_.setCursor(columnXPos, firstLine+lineGap*2);
  screen_.print(*dataPtrs_.transmissionRatio);  
  screen_.drawLine(0, firstLine+lineGap*2+1, 128, firstLine+lineGap*2+1); 
  
  screen_.drawStr(0, firstLine+lineGap*3, "TC slip, rpm:");
  screen_.setCursor(columnXPos, firstLine+lineGap*3);
  screen_.print(*dataPtrs_.tcSlip);  
  screen_.drawLine(0, firstLine+lineGap*3+1, 128, firstLine+lineGap*3+1);  
  /*
  screen_.drawStr(0, firstLine+lineGap*4, "CPU temp, C:");
  screen_.setCursor(columnXPos, firstLine+lineGap*4);
  screen_.print(cpuTemp);  
  screen_.drawLine(0, firstLine+lineGap*4+1, 128, firstLine+lineGap*4+1);
 
  screen_.drawStr(0, firstLine+lineGap*5, "Transm. ratio:");
  screen_.setCursor(columnXPos, firstLine+lineGap*5);
  screen_.print(*dataPtrs_.transmissionRatio);   */
}

void ui::drawLiveData3()
{
  const int columnXPos = 103;	
  const int firstLine = 18;
  const int lineGap = 9;
	
  screen_.setFont(u8g_font_5x8);
  screen_.drawStr(35, firstLine-9, "LIVE DATA PAGE 3");
  screen_.drawLine(0, firstLine-8, 128, firstLine-8);

  screen_.drawStr(0, firstLine, "TPS %:");
  screen_.setCursor(columnXPos, firstLine);
  screen_.print(*dataPtrs_.TPS);
  screen_.drawLine(0, firstLine+1, 128, firstLine+1);

  screen_.drawStr(0, firstLine+lineGap, "TPS voltage, mV:");
  screen_.setCursor(columnXPos, firstLine+lineGap);
  screen_.print(*dataPtrs_.TPSVoltage);  
  screen_.drawLine(0, firstLine+lineGap+1, 128, firstLine+lineGap+1);
  
  screen_.drawStr(0, firstLine+lineGap*2, "MAP, %:");
  screen_.setCursor(columnXPos, firstLine+lineGap*2);
  screen_.print(*dataPtrs_.MAP);  
  screen_.drawLine(0, firstLine+lineGap*2+1, 128, firstLine+lineGap*2+1);
  
  screen_.drawStr(0, firstLine+lineGap*3, "MAP voltage, mV:");
  screen_.setCursor(columnXPos, firstLine+lineGap*3);
  screen_.print(*dataPtrs_.MAPVoltage);  
  screen_.drawLine(0, firstLine+lineGap*3+1, 128, firstLine+lineGap*3+1);
  
  screen_.drawStr(0, firstLine+lineGap*4, "Oil temp C:");
  screen_.setCursor(columnXPos, firstLine+lineGap*4);
  screen_.print(*dataPtrs_.oilTemp);  
  screen_.drawLine(0, firstLine+lineGap*4+1, 128, firstLine+lineGap*4+1);

  screen_.drawStr(0, firstLine+lineGap*5, "Oiltemp/PN sens ohm:");
  screen_.setCursor(columnXPos, firstLine+lineGap*5);
  screen_.print(*dataPtrs_.oilTemp_PN_sens_res);   
}

void ui::drawLiveData4()
{
  const int columnXPos = 103;	
  const int firstLine = 18;
  const int lineGap = 9;
	
  screen_.setFont(u8g_font_5x8);
  screen_.drawStr(35, firstLine-9, "LIVE DATA PAGE 4");
  screen_.drawLine(0, firstLine-8, 128, firstLine-8);

  screen_.drawStr(0, firstLine, "Park switch:");
  screen_.setCursor(columnXPos, firstLine);
  screen_.print(*dataPtrs_.parkSwitch);
  screen_.drawLine(0, firstLine+1, 128, firstLine+1);

  screen_.drawStr(0, firstLine+lineGap, "Reverse switch:");
  screen_.setCursor(columnXPos, firstLine+lineGap);
  screen_.print(*dataPtrs_.reverseSwitch);  
  screen_.drawLine(0, firstLine+lineGap+1, 128, firstLine+lineGap+1);
  
  
  screen_.drawStr(0, firstLine+lineGap*2, "Gear + switch:");
  screen_.setCursor(columnXPos, firstLine+lineGap*2);
 // screen_.print(*dataPtrs_.gearPlusSwitch);  
  screen_.drawLine(0, firstLine+lineGap*2+1, 128, firstLine+lineGap*2+1);
  
  screen_.drawStr(0, firstLine+lineGap*3, "Gear - switch:");
  screen_.setCursor(columnXPos, firstLine+lineGap*3);
  //screen_.print(*dataPtrs_.gearMinusSwitch);  
  screen_.drawLine(0, firstLine+lineGap*3+1, 128, firstLine+lineGap*3+1);
   
  screen_.drawStr(0, firstLine+lineGap*4, "Brake pedal switch:");
  screen_.setCursor(columnXPos, firstLine+lineGap*4);
  screen_.print(*dataPtrs_.brakePedalSwitch);  
  screen_.drawLine(0, firstLine+lineGap*4+1, 128, firstLine+lineGap*4+1);
  /*
  screen_.drawStr(0, firstLine+lineGap*5, "Oiltemp/PN sens ohm:");
  screen_.setCursor(columnXPos, firstLine+lineGap*5);
  screen_.print(*dataPtrs_.oilTemp_PN_sens_res);   */
}

void ui::adjustPlotTime()
{
  if (rightButton_.givePulse() && plotTime_ < 50)
  {
    if (plotTime_ >= 10)
    {
      plotTime_ += 10;
    }
    else if (plotTime_ > 5)
    {
      plotTime_ = plotTime_ + 2;
    }
    else
    {
      plotTime_++;
    }    
  }

  if (leftButton_.givePulse() && plotTime_ >= 2)
  {
    if (plotTime_ > 10)
    {
      plotTime_ -= 10;
    }
    else if (plotTime_ > 5)
    {
      plotTime_ = plotTime_ - 2;
    }
    else
    {
      plotTime_--;
    }    
  }
}

bool ui::plotEngineSpeed()
{
  core_.setLoggingStatus(true);
  core_.setLoggableVariable(core::engineSpeed);
  plotDataForSingleYAxis();

  screen_.setCursor(30, 8);
  screen_.print("engine: ");
  screen_.print(log_->y1Data[0]);
  screen_.print(" rpm");

  if (cancelButton_.giveSingleShot())
  {
    core_.setLoggingStatus(false);
    return true;
  }
  return false;
}

bool ui::plotVehicleSpeed()
{
  core_.setLoggingStatus(true);
  core_.setLoggableVariable(core::vehicleSpeed);
  plotDataForSingleYAxis();

  screen_.setCursor(30, 8);
  screen_.print("veh. speed: ");
  screen_.print(log_->y1Data[0]);
  screen_.print(" km/h");

  if (cancelButton_.giveSingleShot())
  {
    core_.setLoggingStatus(false);
    return true;
  }
  return false;
}

bool ui::plotPrimAndSecSpds()
{
  core_.setLoggingStatus(true);
  core_.setLoggableVariable(core::primAndSecVehSpds);
  plotDataForSingleYAxis(false, true); // first argument false -> integer units on left axis. last argument true -> display yData2, see core.cpp

  screen_.setCursor(30, 8);
  screen_.print("prim: ");
  screen_.print(log_->y1Data[0]);
  screen_.setCursor(85, 8);
  screen_.print("sec: ");
  screen_.print(int(log_->y2Data[0]));

  if (cancelButton_.giveSingleShot())
  {
    core_.setLoggingStatus(false);
    return true;
  }
  return false;
}

bool ui::plotn2n3Speeds()
{
  core_.setLoggingStatus(true);
  core_.setLoggableVariable(core::n2Andn3Speed);
  plotDataForSingleYAxis(false, true); // first argument false -> integer units on left axis. last argument true -> display yData2, see core.cpp

  screen_.setCursor(30, 8);
  screen_.print("n2: ");
  screen_.print(log_->y1Data[0]);
  screen_.setCursor(85, 8);
  screen_.print("n3: ");
  screen_.print(int(log_->y2Data[0]));

  if (cancelButton_.giveSingleShot())
  {
    core_.setLoggingStatus(false);
    return true;
  }
  return false;
}

bool ui::plotn2n3Ratio()
{
  core_.setLoggingStatus(true);
  core_.setLoggableVariable(core::n3n2Ratio);
  plotDataForSingleYAxis(true, true); // first argument true -> float units on left axis. 
                                      // last argument true -> display yData2, which is the ratio, also yData1 is constant 0, see core.cpp

  screen_.setCursor(30, 8);
  screen_.print("n3/n2: ");
  screen_.print(log_->y2Data[0]);

  if (cancelButton_.giveSingleShot())
  {
    core_.setLoggingStatus(false);
    return true;
  }
  return false;
}

bool ui::plotTransmissionRatio()
{
  core_.setLoggingStatus(true);
  core_.setLoggableVariable(core::transmissionRatio);
  plotDataForSingleYAxis(true, true); // first argument true -> float units on left axis. 
                                      // last argument true -> display yData2, which is the ratio, also yData1 is constant 0, see core.cpp

  screen_.setCursor(30, 8);
  screen_.print("transm. ratio: ");
  screen_.print(log_->y2Data[0]);
  
  if (cancelButton_.giveSingleShot())
  {
    core_.setLoggingStatus(false);
    return true;
  }
  return false;
}

bool ui::plotTCSlipAndCtrl()
{
  core_.setLoggingStatus(true);
  core_.setLoggableVariable(core::TCSlipAndTCControl);
  plotDataForDualYAxis(false);

  screen_.setCursor(28, 8);
  screen_.print("TC slip: ");
  screen_.print(int(log_->y2Data[0]));
  screen_.setCursor(85, 8);
  screen_.print("ctrl: ");
  screen_.print(log_->y1Data[0]);
  screen_.print(" %");

  if (cancelButton_.giveSingleShot())
  {
    core_.setLoggingStatus(false);
    return true;
  }
  return false;
}

bool ui::plotOilTemp()
{
  core_.setLoggingStatus(true);
  core_.setLoggableVariable(core::oilTemp);
  plotDataForSingleYAxis();

  screen_.setCursor(30, 8);
  screen_.print("oilTemp: ");
  screen_.print(log_->y1Data[0]);
  screen_.print(" C");

  if (cancelButton_.giveSingleShot())
  {
    core_.setLoggingStatus(false);
    return true;
  }
  return false;
}

bool ui::plotTPSes()
{
  core_.setLoggingStatus(true);
  core_.setLoggableVariable(core::TPSes);
  plotDataForDualYAxis();

  screen_.setCursor(28, 8);
  screen_.print("TPS: ");
  screen_.print(int(log_->y1Data[0]));
  screen_.print(" %");
  screen_.setCursor(85, 8);
  screen_.print("TPSd: ");
  screen_.print(log_->y2Data[0]);
  screen_.print(" %");

  if (cancelButton_.giveSingleShot())
  {
    core_.setLoggingStatus(false);
    return true;
  }
  return false;

}

void ui::plotDataForSingleYAxis(bool leftAxisFloat, bool displayData2)
{    
  screen_.drawLine(0, 10, 128, 10);  

  adjustPlotTime();

  float xMin = 0;
  float yMin = 0;
  float xMax = plotTime_ * 10; // because with 0,1s measuring intervals, there is 10 measurements per 1 second, see core.h

  float xCenter = (xMax - xMin) / 2;
  float yCenter = (yLeftMax_ - yMin) / 2;

  // define the lower left corner of drawing area with these
  int xLowerPos = 17;
  int yLowerPos = 56;

  int xUpperPos = 123; // do not touch
  int yUpperPos = 15;  // do not touch

  int xSize = xUpperPos - xLowerPos;
  int ySize = yLowerPos - yUpperPos; 

  int xHalfwayPos = xLowerPos + ((xUpperPos - xLowerPos) / 2);
  int yHalwayPos = yLowerPos - ((yLowerPos - yUpperPos) / 2);

  screen_.drawLine(xLowerPos, yLowerPos, xLowerPos, yUpperPos); // vertical line, y
  screen_.drawPixel(xLowerPos - 1, yLowerPos); // lowermost dot, y
  screen_.drawPixel(xLowerPos - 1, yHalwayPos); // center dot, y
  screen_.drawPixel(xLowerPos - 1, yUpperPos); // uppermost dot, y

  screen_.drawLine(xLowerPos, yLowerPos, xUpperPos, yLowerPos); // horizontal line, x
  screen_.drawPixel(xLowerPos, yLowerPos + 1); // leftmost dot, x
  screen_.drawPixel(xHalfwayPos, yLowerPos + 1); // center dot, x
  screen_.drawPixel(xUpperPos, yLowerPos + 1); // rightmost dot, x

  screen_.setFont(u8g_font_4x6r);
  screen_.setCursor(0, 58);

  if (!leftAxisFloat)
  {
    screen_.print(int(yMin)); // min value for lowermost dot
    screen_.setCursor(0, yHalwayPos + 2);
    screen_.print(int(yCenter)); // center value for center dot
    screen_.setCursor(0, 18);
    screen_.print(int(yLeftMax_)); // max value for uppermost dot
  }
  else // use float units
  {
    screen_.print(yMin); // min value for lowermost dot
    screen_.setCursor(0, yHalwayPos + 2);
    screen_.print(yCenter); // center value for center dot
    screen_.setCursor(0, 18);
    screen_.print(yLeftMax_); // max value for uppermost dot
  }

  // x-axis
  screen_.setCursor(15, 64);
  screen_.print(int(xMin)); // min value for leftmost dot
  screen_.setCursor(xHalfwayPos - 9, 64);
  screen_.print(xCenter/10); // center value for center dot
  screen_.setCursor(115, 64);
  screen_.print(int(xMax/10)); // max value for rightmost dot

  float x = 0;
  float y1 = 0;
  float y2 = 0;

  float highestOnData = 0;
  for (int i = 0; i < xMax; i++) // go through the data array
  {
    if (log_->y1Data[i] > highestOnData)
    {
      highestOnData = float(log_->y1Data[i]);
    } 
    if (log_->y2Data[i] > highestOnData)
    {
      highestOnData = log_->y2Data[i];
    } 

    x = log_->xData[i];
    y1 = log_->y1Data[i];

    if (displayData2) // print also another variable
    {
      y2 = log_->y2Data[i]; 
    }

    if (x >= xMin && x <= xMax) // print only values which are in selected time interval
    {
      int xPositionForPixel = xLowerPos + (int((x / xMax * xSize)));

      screen_.drawPixel(xPositionForPixel, yLowerPos - (int(((y1 / yLeftMax_) * ySize))));
      screen_.drawPixel(xPositionForPixel, yLowerPos - (int(((y2 / yLeftMax_) * ySize))));
    }
  }
  yLeftMax_ = highestOnData;
}

void ui::plotDataForDualYAxis(bool leftAxisFloat, bool useSmallView)
{
  screen_.drawLine(0, 10, 128, 10);  

  adjustPlotTime();
  int plotLeftStartPosition = 0;

  if (useSmallView)
  {
    plotLeftStartPosition = 30;
  }  

  float xMin = 0;
  float yLeftMin = 0;
  float yRightMin = 0;
  float xMax = plotTime_ * 10; // because with 0,1s intervals, there is 10 measurements per 1 second, see core.cpp

  float xCenter = (xMax - xMin) / 2;
  float yLeftCenter = (yLeftMax_ - yLeftMin) / 2;
  float yRightCenter = (yRightMax_ - yRightMin) / 2;

  // define the lower left corner of drawing area with these
  int xLowerPos = plotLeftStartPosition + 17;
  int yLowerPos = 56;

  int xUpperPos = 110; // do not touch
  int yUpperPos = 15;  // do not touch

  int xSize = xUpperPos - xLowerPos;
  int ySize = yLowerPos - yUpperPos; 

  int xHalfwayPos = xLowerPos + ((xUpperPos - xLowerPos) / 2);
  int yHalfwayPos = yLowerPos - ((yLowerPos - yUpperPos) / 2);

  screen_.drawLine(xLowerPos, yLowerPos, xLowerPos, yUpperPos); // vertical line, left y
  screen_.drawPixel(xLowerPos - 1, yLowerPos); // lowermost dot, left y
  screen_.drawPixel(xLowerPos - 1, yHalfwayPos); // center dot, left y
  screen_.drawPixel(xLowerPos - 1, yUpperPos); // uppermost dot, left y

  screen_.drawLine(xUpperPos, yLowerPos, xUpperPos, yUpperPos); // vertical line, right y
  screen_.drawPixel(xUpperPos + 1, yLowerPos); // lowermost dot, right y
  screen_.drawPixel(xUpperPos + 1, yHalfwayPos); // center dot, right y
  screen_.drawPixel(xUpperPos + 1, yUpperPos); // uppermost dot, right y

  screen_.drawLine(xLowerPos, yLowerPos, xUpperPos, yLowerPos); // horizontal line, x
  screen_.drawPixel(xLowerPos, yLowerPos + 1); // leftmost dot, x
  screen_.drawPixel(xHalfwayPos, yLowerPos + 1); // center dot, x
  screen_.drawPixel(xUpperPos, yLowerPos + 1); // rightmost dot, x

  screen_.setFont(u8g_font_4x6r);
  screen_.setCursor(plotLeftStartPosition, 58);

  if (!leftAxisFloat)
  {
    screen_.print(int(yLeftMin)); // min value for lowermost dot
    screen_.setCursor(plotLeftStartPosition, yHalfwayPos + 2);
    screen_.print(int(yLeftCenter)); // center value for center dot
    screen_.setCursor(plotLeftStartPosition, 18);
    screen_.print(int(yLeftMax_)); // max value for uppermost dot
  }
  else // use float units
  { 
    screen_.print(yLeftMin); // min value for lowermost dot
    screen_.setCursor(plotLeftStartPosition, yHalfwayPos + 2);
    screen_.print(yLeftCenter); // center value for center dot
    screen_.setCursor(plotLeftStartPosition, 18);
    screen_.print(yLeftMax_); // max value for uppermost dot 
  }

  // Right y-axis
  screen_.setCursor(112, 58);
  screen_.print(int(yRightMin)); // min value for lowermost dot
  screen_.setCursor(112, yHalfwayPos + 2);
  screen_.print(int(yRightCenter)); // center value for center dot
  screen_.setCursor(112, 18);
  screen_.print(int(yRightMax_)); // max value for uppermost dot    

  // x-axis
  screen_.setCursor(plotLeftStartPosition + 15, 64);
  screen_.print(int(xMin)); // min value for leftmost dot
  screen_.setCursor(xHalfwayPos - 9, 64);
  screen_.print(xCenter/10); // center value for center dot
  screen_.setCursor(105, 64);
  screen_.print(int(xMax/10)); // max value for rightmost dot

  float x = 0;
  float y1 = 0;
  float y2 = 0;

  float highestOnData1 = 0;
  float highestOnData2 = 0;

  for (int i = 0; i < xMax; i++) // go through the data array
  {
    if (log_->y1Data[i] > highestOnData1)
    {
      highestOnData1 = float(log_->y1Data[i]);
    } 
    if (log_->y2Data[i] > highestOnData2)
    {
      highestOnData2 = log_->y2Data[i];
    } 

    x = log_->xData[i];

    y1 = log_->y1Data[i];
    y2 = log_->y2Data[i];  

      if (x >= xMin && x <= xMax) // print only values which are in selected time interval
      { 
        int xPositionForPixel = xLowerPos + (int((x / xMax * xSize)));       
        screen_.drawPixel(xPositionForPixel, yLowerPos - (int(((y1 / yRightMax_) * ySize)))); // print y1Data to right, because it has no optional float units 
        screen_.drawPixel(xPositionForPixel, yLowerPos - (int(((y2 / yLeftMax_) * ySize))));  // only left axis has optional float units
      }
  }
  yRightMax_ = highestOnData1;
  yLeftMax_ = highestOnData2;    
}


bool ui::editMPCNormalDriveMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(22, 8, "MPC NORM DRIVE");
  editDualAxisMap(givePtrToDualAxisMap("MPC_normalDrive"), true);
  return exitMapEditor();
}

bool ui::editAutoModeWantedGearMapCold()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(22, 8, "Automode, cold");
  editDualAxisMap(givePtrToDualAxisMap("Automode_wanted_gear_map_cold"));
  return exitMapEditor();
}

bool ui::editAutoModeWantedGearMapWarm()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(22, 8, "Automode, warm");
  editDualAxisMap(givePtrToDualAxisMap("Automode_wanted_gear_map_warm"));
  return exitMapEditor();
}

bool ui::editShiftSolenoidTimeMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(22, 8, "Shift sol. ctrl time");
  editDualAxisMap(givePtrToDualAxisMap("Shift_solenoid_time_map"));
  return exitMapEditor();
}

bool ui::editMPC1to2loadMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "MPC,load,1->2");
  editDualAxisMap(givePtrToDualAxisMap("MPC_1to2_load"));
  return exitMapEditor();
}

bool ui::editMPC2to3loadMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "MPC,load,2->3");
  editDualAxisMap(givePtrToDualAxisMap("MPC_2to3_load"));
  return exitMapEditor();
}

bool ui::editMPC3to4loadMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "MPC,load,3->4");
  editDualAxisMap(givePtrToDualAxisMap("MPC_3to4_load"));
  return exitMapEditor();
}

bool ui::editMPC4to5loadMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "MPC,load,4->5");
  editDualAxisMap(givePtrToDualAxisMap("MPC_4to5_load"));
  return exitMapEditor();
}

bool ui::editMPC5to4loadMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "MPC,load,5->4");
  editDualAxisMap(givePtrToDualAxisMap("MPC_5to4_load"));
  return exitMapEditor();
}

bool ui::editMPC4to3loadMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "MPC,load,4->3");
  editDualAxisMap(givePtrToDualAxisMap("MPC_4to3_load"));
  return exitMapEditor();
}

bool ui::editMPC3to2loadMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "MPC,load,3->2");
  editDualAxisMap(givePtrToDualAxisMap("MPC_3to2_load"));
  return exitMapEditor();
}

bool ui::editMPC2to1loadMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "MPC,load,2->1");
  editDualAxisMap(givePtrToDualAxisMap("MPC_2to1_load"));
  return exitMapEditor();
}

bool ui::editMPC1to2coastMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "MPC,coast,1->2");
  editDualAxisMap(givePtrToDualAxisMap("MPC_1to2_coast"));
  return exitMapEditor();
}

bool ui::editMPC2to3coastMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "MPC,coast,2->3");
  editDualAxisMap(givePtrToDualAxisMap("MPC_2to3_coast"));
  return exitMapEditor();
}

bool ui::editMPC3to4coastMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "MPC,coast,3->4");
  editDualAxisMap(givePtrToDualAxisMap("MPC_3to4_coast"));
  return exitMapEditor();
}

bool ui::editMPC4to5coastMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "MPC,coast,4->5");
  editDualAxisMap(givePtrToDualAxisMap("MPC_4to5_coast"));
  return exitMapEditor();
}

bool ui::editMPC5to4coastMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "MPC,coast,5->4");
  editDualAxisMap(givePtrToDualAxisMap("MPC_5to4_coast"));
  return exitMapEditor();
}

bool ui::editMPC4to3coastMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "MPC,coast,4->3");
  editDualAxisMap(givePtrToDualAxisMap("MPC_4to3_coast"));
  return exitMapEditor();
}

bool ui::editMPC3to2coastMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "MPC,coast,3->2");
  editDualAxisMap(givePtrToDualAxisMap("MPC_3to2_coast"));
  return exitMapEditor();
}

bool ui::editMPC2to1coastMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "MPC,coast,2->1");
  editDualAxisMap(givePtrToDualAxisMap("MPC_2to1_coast"));
  return exitMapEditor();
}

bool ui::editSPC1to2loadMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "SPC,load,1->2");
  editDualAxisMap(givePtrToDualAxisMap("SPC_1to2_load"));
  return exitMapEditor();
}

bool ui::editSPC2to3loadMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "SPC,load,2->3");
  editDualAxisMap(givePtrToDualAxisMap("SPC_2to3_load"));
  return exitMapEditor();
}

bool ui::editSPC3to4loadMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "SPC,load,3->4");
  editDualAxisMap(givePtrToDualAxisMap("SPC_3to4_load"));
  return exitMapEditor();
}

bool ui::editSPC4to5loadMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "SPC,load,4->5");
  editDualAxisMap(givePtrToDualAxisMap("SPC_4to5_load"));
  return exitMapEditor();
}

bool ui::editSPC5to4loadMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "SPC,load,5->4");
  editDualAxisMap(givePtrToDualAxisMap("SPC_5to4_load"));
  return exitMapEditor();
}

bool ui::editSPC4to3loadMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "SPC,load,4->3");
  editDualAxisMap(givePtrToDualAxisMap("SPC_4to3_load"));
  return exitMapEditor();
}

bool ui::editSPC3to2loadMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "SPC,load,3->2");
  editDualAxisMap(givePtrToDualAxisMap("SPC_3to2_load"));
  return exitMapEditor();
}

bool ui::editSPC2to1loadMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "SPC,load,2->1");
  editDualAxisMap(givePtrToDualAxisMap("SPC_2to1_load"));
  return exitMapEditor();
}

bool ui::editSPC1to2coastMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "SPC,coast,1->2");
  editDualAxisMap(givePtrToDualAxisMap("SPC_1to2_coast"));
  return exitMapEditor();
}

bool ui::editSPC2to3coastMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "SPC,coast,2->3");
  editDualAxisMap(givePtrToDualAxisMap("SPC_2to3_coast"));
  return exitMapEditor();
}

bool ui::editSPC3to4coastMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "SPC,coast,3->4");
  editDualAxisMap(givePtrToDualAxisMap("SPC_3to4_coast"));
  return exitMapEditor();
}

bool ui::editSPC4to5coastMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "SPC,coast,4->5");
  editDualAxisMap(givePtrToDualAxisMap("SPC_4to5_coast"));
  return exitMapEditor();
}

bool ui::editSPC5to4coastMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "SPC,coast,5->4");
  editDualAxisMap(givePtrToDualAxisMap("SPC_5to4_coast"));
  return exitMapEditor();
}

bool ui::editSPC4to3coastMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "SPC,coast,4->3");
  editDualAxisMap(givePtrToDualAxisMap("SPC_4to3_coast"));
  return exitMapEditor();
}

bool ui::editSPC3to2coastMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "SPC,coast,3->2");
  editDualAxisMap(givePtrToDualAxisMap("SPC_3to2_coast"));
  return exitMapEditor();
}

bool ui::editSPC2to1coastMap()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(20, 8, "SPC,coast,2->1");
  editDualAxisMap(givePtrToDualAxisMap("SPC_2to1_coast"));
  return exitMapEditor();
}

void ui::editDualAxisMap(configHandler::dualAxisMap* map, bool tracing, variableTypes vars)
{
  screen_.drawLine(0, 10, 128, 10);

  // switch editing mode
  if (!editingMode_ && okButton_.givePulse()) // enter edit mode
  {
    editingMode_ = true;
  }
  if (editingMode_)
  {
    if (cancelButton_.giveSingleShot()) // exit edit mode
    {
        editingMode_ = false;
    }    
  }

  if (!editingMode_) // navigate in map if editing mode is inactive
  {
      if (downButton_.givePulse() && editorRow_ < map->rows-1)
      {
          editorRow_++;
      }
      if (upButton_.givePulse() && editorRow_ > 0)
      {
          editorRow_--;
      }
      if (rightButton_.givePulse() && editorColumn_ < map->columns-1)
      {
          editorColumn_++;
      }
      if (leftButton_.givePulse() && editorColumn_ > 0)
      {
          editorColumn_--;
      }
  }
  else // change selected map value if editing mode is active
  {
      if (upButton_.givePulse() && map->data[editorRow_][editorColumn_] < map->maxData)
      {
          map->data[editorRow_][editorColumn_]++;
      }
      if (downButton_.givePulse() && map->data[editorRow_][editorColumn_] > map->minData)
      {
          map->data[editorRow_][editorColumn_]--;
      }
  }

  // ROLL THE MAP
  static uint8_t colEndIndex = 6; // start rolling after this amount of columns
  static uint8_t rowEndIndex = 3; // start rolling after this amount of rows
  static uint8_t colStartIndex = 0;
  static uint8_t rowStartIndex = 0;

  // calculate new locations
  if (editorColumn_ > colEndIndex-1)
  {
    colStartIndex++;
    colEndIndex++;
  }
  if (editorColumn_ < colStartIndex)
  {
    colStartIndex--;
    colEndIndex--;
  }

  if (editorRow_ > rowEndIndex-1)
  {
    rowStartIndex++;
    rowEndIndex++;
  }
  if (editorRow_ < rowStartIndex)
  {
    rowStartIndex--;
    rowEndIndex--;
  }

  int rowVal = *dataPtrs_.oilTemp;
  int colVal = *dataPtrs_.load;

   // DATA TRACING /
   uint8_t traceRowPos = 0;
   uint8_t traceColPos = 0;
  if (tracing)
  {
    traceRowPos = getClosestCell(map->rowTitles, map->rows, rowVal);
    traceColPos = getClosestCell(map->columnTitles, map->columns, colVal);
  }

  screen_.drawHLine(12, 18, 128);
  screen_.drawVLine(12, 18, 64);
  screen_.setFont(u8g2_font_trixel_square_tn);

  uint8_t colStartPos = 17;
  uint8_t colSpacing = 16;
  uint8_t colItemPos = colStartPos;

  // print column titles
  for (int i = colStartIndex; i < map->columns; i++)
  {
      screen_.setCursor(colItemPos, 17);
      screen_.print(map->columnTitles[i]);
      colItemPos += colSpacing;
      if (traceColPos == i+1 && tracing) // print also line to traced column
      {
        screen_.drawVLine(colItemPos+4, 18, 3);        
      }      
  }

  uint8_t rowStartPos = 27;
  uint8_t rowSpacing = 12;
  uint8_t rowTitleItemPos = rowStartPos;

  uint8_t rowDataStartPos = 17;
  uint8_t rowDataItemPos = rowDataStartPos;

  //print row titles and data
  for (int i = rowStartIndex; i < map->rows; i++)
  {      
      if (editorRow_ == i)
      {
        screen_.setFont(u8g2_font_trixel_square_tn);
      }
      screen_.setCursor(0, rowTitleItemPos);
      screen_.print(map->rowTitles[i]);   

      if (traceRowPos == i && tracing) // print also line to traced column
      {
        screen_.drawHLine(13, rowTitleItemPos-3, 2);        
      } 

      for (int j = colStartIndex; j < map->columns; j++)
      {
         screen_.setCursor(rowDataItemPos, rowTitleItemPos);
         
         if (i == editorRow_ && j == editorColumn_) // highlight the selection
         {
           if (editingMode_)
           {
              screen_.setFont(u8g2_font_profont10_mn);
           }
           else
           {
             screen_.print("*");
           }            
         }
         screen_.print(map->data[i][j]);
         screen_.setFont(u8g2_font_trixel_square_tn);  // change back to normal font if changed 

        if (i == traceRowPos && j == traceColPos && tracing) // if the cell is "active" (according to live data), draw box around it
        {
            uint8_t width = 15;
            if (map->data[i][j] > 99 || (i == editorRow_ && j == editorColumn_))
            {
              width = 18;
            }
            if (map->data[i][j] > 99 && (i == editorRow_ && j == editorColumn_))
            {
              width = 20;
            }
            screen_.drawRFrame(rowDataItemPos-4, rowTitleItemPos-8, width, 11, 1);
        }
         rowDataItemPos += colSpacing;
      }
    rowDataItemPos = rowDataStartPos;
    rowTitleItemPos += rowSpacing;      
  }
}

int ui::getClosestCell(int values[], uint8_t size, int val)
{

  int closest = 0;
    if (val <= values[0]) // smaller or as small as the smallest in row
    { 
        closest = 0;
    }
    else if(val >= values[size-1]) // bigger or as big as the biggest in row
    {
        closest = size-1;     
    }
    else // in between
    {
        for(closest = 0; closest < size; closest++)
        {
            if (val <= values[closest])
            {
                if (val < (values[closest-1] + ((values[closest] - values[closest-1]) / 2))) // if smaller than halfway of two row titles, then use previous title
                {
                    closest--;
                }
                break;
            }
        } 
    }
    return closest;
}

bool ui::exitMapEditor()
{
  if (!editingMode_ && cancelButton_.giveSingleShot())
  {
    editorRow_ = 0;
    editorColumn_ = 0;
    return true;
  }
  return false; 
}

bool ui::goToSettingsMenu()
{
  currentMenu_ = &settingsMenu_;
  return true;
}

bool ui::goToAdjustLastShiftMenu()
{
  currentMenu_ = &adjLastShiftMenu_;
  return true;
}

bool ui::increaseLastShiftMPC()
{
  core_.modifyLastShiftMaps(1, 0);
  showNotification(1000, "Increased MPC map value by 1!");
  return true;
}

bool ui::decreaseLastShiftMPC()
{
  core_.modifyLastShiftMaps(-1, 0);
  showNotification(1000, "Decreased MPC map value by 1!");
  return true;
}

bool ui::increaseLastShiftSPC()
{
  core_.modifyLastShiftMaps(0, 1);
  showNotification(1000, "Increased SPC map value by 1!");
  return true;
}

bool ui::decreaseLastShiftSPC()
{
  core_.modifyLastShiftMaps(0, -1);
  showNotification(1000, "Decreased SPC map value by 1!");
  return true;
}

bool ui::showPressureValuesOverrideView()
{
  static uint8_t selection = 1;
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(25, 8, "OVERR.PRESS.VALS");
  screen_.drawLine(0, 10, 128, 10);

  screen_.setCursor(0, 35);
  screen_.print("Active:");
  screen_.setCursor(0, 45);
  *dataPtrs_.overridePressureValues ? screen_.print("true"): screen_.print("false");
  if (selection == 1) {screen_.print("*");}

  screen_.setCursor(55, 35);
  screen_.print("MPC: ");
  screen_.setCursor(55, 45);
  screen_.print(*dataPtrs_.overridedMPCValue);
  if (selection == 2) {screen_.print("*");}

  screen_.setCursor(90, 35);
  screen_.print("SPC: ");
  screen_.setCursor(90, 45);
  screen_.print(*dataPtrs_.overridedSPCValue);
  if (selection == 3) {screen_.print("*");}
  
  if (selection > 1 && leftButton_.giveSingleShot())
  {
    selection--;
  }
  else if (selection < 3 && rightButton_.giveSingleShot())
  {
    selection++;
  }

  if (selection == 1)
  {
    if (upButton_.giveSingleShot() || downButton_.giveSingleShot())
    {
      *dataPtrs_.overridePressureValues ? *dataPtrs_.overridePressureValues = false: *dataPtrs_.overridePressureValues = true; // toggle variable
    }
  }
  else if (selection == 2)
  {
    if (upButton_.givePulse() && *dataPtrs_.overridedMPCValue < 100)
    {
      *dataPtrs_.overridedMPCValue = *dataPtrs_.overridedMPCValue + 1;
    }
    else if (downButton_.givePulse() && *dataPtrs_.overridedMPCValue > 1)
    {
      *dataPtrs_.overridedMPCValue = *dataPtrs_.overridedMPCValue - 1;
    }
  }
  else if (selection == 3)
  {
    if (upButton_.givePulse() && *dataPtrs_.overridedSPCValue < 100)
    {
      *dataPtrs_.overridedSPCValue = *dataPtrs_.overridedSPCValue + 1;
    }
    else if (downButton_.givePulse() && *dataPtrs_.overridedSPCValue > 1)
    {
      *dataPtrs_.overridedSPCValue = *dataPtrs_.overridedSPCValue - 1;
    }
  }
  return cancelButton_.giveSingleShot();
}

bool ui::goToDualAxisMapEditorMenu()
{
  currentMenu_ = &mapEditorMenu_;
  return true;
}

bool ui::goToSingleAxisMapEditorMenu()
{
  static bool runEditor = false;
  static uint8_t i = 0;

  if (okButton_.givePulse() && !runEditor)
  {
    runEditor = true;
  }

  if (!runEditor)
  {
    screen_.setFont(u8g_font_5x8);
    screen_.drawStr(20, 8, "SINGLE-AXIS MAPS");

    screen_.setCursor(50,20);
    screen_.print(i+1);
    screen_.print("/");
    screen_.print(singleAxisMapsPtr_->size);

    // shift to next parameter
    if (rightButton_.givePulse() && i+1 < singleAxisMapsPtr_->size)
    {
      i++;
    }
    // shift to previous parameter
    if (leftButton_.givePulse() && i > 0)
    {
      i--;
    }

    uint8_t len = singleAxisMapsPtr_->maps[i].ID.length();

    // divide the map ID into two lines if necessary
    uint8_t w = 4; // 4 is approx char width in px
    uint8_t c = 55; // center of the ID text
    if (len <= 20)
    {
        screen_.setFont(u8g2_font_profont10_tf);
        screen_.setCursor(c - (singleAxisMapsPtr_->maps[i].ID.length() * w / 2), 40); 
        screen_.print(singleAxisMapsPtr_->maps[i].ID);
    }
    else
    {
        uint8_t index = 20;
        while (singleAxisMapsPtr_->maps[i].ID.charAt(index) != '_' && index > 0)
        {
          index--;
        }

        String line1;
        String line2;

        line1 = singleAxisMapsPtr_->maps[i].ID.substring(0, index);
        line2 = singleAxisMapsPtr_->maps[i].ID.substring(index);

        screen_.setFont(u8g2_font_profont10_tf);
        screen_.setCursor(c - (line1.length() * w / 2), 40);
        screen_.print(line1);
        screen_.setCursor(c - (line2.length() * w / 2), 50);
        screen_.print(line2);       
    }
    if (cancelButton_.giveSingleShot())
    {
      return true;
    }
  }
  else
  {
    runEditor = editSingleAxisMap(&singleAxisMapsPtr_->maps[i]);
  }
   return false;
}

bool ui::editSingleAxisMap(configHandler::singleAxisMap* map)
{
  screen_.drawLine(0, 10, 128, 10);
  screen_.setCursor(0, 20);
  screen_.print(map->ID);

  // switch editing mode
  if (!editingMode_ && okButton_.givePulse())
  {
    editingMode_ = true;
  }
  if (editingMode_)
  {
    if (cancelButton_.giveSingleShot())
    {
      editingMode_ = false;
    }    
  }

  if (!editingMode_) // navigate in map if editing mode is inactive
  {
    if (rightButton_.givePulse() && editorColumn_ < map->columns-1)
    {
        editorColumn_++;
    }
    if (leftButton_.givePulse() && editorColumn_ > 0)
    {
        editorColumn_--;
    }
  }
  else // change selected map value if editing mode is active
  {
      if (upButton_.givePulse() && map->data[editorColumn_] < map->maxData)
      {
          map->data[editorColumn_]++;
      }
      if (downButton_.givePulse() && map->data[editorColumn_] > map->minData)
      {
          map->data[editorColumn_]--;
      }
  }

  // ROLL THE MAP
  static uint8_t colEndIndex = 4; // start rolling after this amount of columns
  static uint8_t colStartIndex = 0;

  // calculate new locations
  if (editorColumn_ > colEndIndex-1)
  {
    colStartIndex++;
    colEndIndex++;
  }
  if (editorColumn_ < colStartIndex)
  {
    colStartIndex--;
    colEndIndex--;
  }

  screen_.drawHLine(0, 33, 128);
  screen_.setFont(u8g2_font_trixel_square_tn);

  uint8_t screenWidth = 128;
  uint8_t colStartPos = 0;
  uint8_t colSpacing = 35;
  uint8_t colItemPos = colStartPos;

  // print column titles
  for (int i = colStartIndex; i < map->columns; i++)
  {
      if (colItemPos < screenWidth)
      {
        screen_.setCursor(colItemPos, 32);
        screen_.print(map->columnTitles[i]);
        colItemPos += colSpacing;    
      }
  }
  uint8_t rowStartPos = 42;
  uint8_t rowDataStartPos = 0;
  uint8_t rowDataItemPos = rowDataStartPos;

  //print data
  for (int j = colStartIndex; j < map->columns; j++)
  {
      screen_.setCursor(rowDataItemPos, rowStartPos);

      if (j == editorColumn_) // highlight the selection
      {
        if (editingMode_)
        {
          screen_.setFont(u8g2_font_profont10_mn);
        }
        else
        {
          screen_.print("*");
        }            
      }

    if (rowDataItemPos < screenWidth)
    {
      screen_.print(map->data[j]);
      rowDataItemPos += colSpacing;
    }
    screen_.setFont(u8g2_font_trixel_square_tn);  // change back to normal font if changed 
  }
  rowDataItemPos = rowDataStartPos;  
  return !exitMapEditor();
}

bool ui::showParamEditor()
{
    screen_.setFont(u8g2_font_synchronizer_nbp_tf);
    screen_.drawStr(30, 8, "PARAMETERS");
    screen_.drawLine(0, 10, 128, 10);  

    static uint8_t i = 0;
    screen_.setCursor(50,20);
    screen_.print(i+1);
    screen_.print("/");
    screen_.print(parametersPtr_->size);

    static int origData = parametersPtr_->parameters[i].data;

    // shift to next parameter
    if (rightButton_.givePulse() && i+1 < parametersPtr_->size)
    {
      if (parametersPtr_->parameters[i].data != origData)
      {
        parametersPtr_->parameters[i].data = origData;
        showNotification(1500, "Changed value discarded!");        
      } 
      i++;
      origData = parametersPtr_->parameters[i].data;
    }

    // shift to previous parameter
    if (leftButton_.givePulse() && i > 0)
    {
      if (parametersPtr_->parameters[i].data != origData)
      {
        parametersPtr_->parameters[i].data = origData;
        showNotification(1500, "Changed value discarded!");        
      } 
      i--;
      origData = parametersPtr_->parameters[i].data;
    }

    // increment value
    if (upButton_.givePulse() && parametersPtr_->parameters[i].data < parametersPtr_->parameters[i].maxData)
    {
      if (origData == parametersPtr_->parameters[i].data)
      {
        origData = parametersPtr_->parameters[i].data;
      }
      parametersPtr_->parameters[i].data++;
    }
    // decrement value
    if (downButton_.givePulse() && parametersPtr_->parameters[i].data > parametersPtr_->parameters[i].minData)
    {
      if (origData == parametersPtr_->parameters[i].data)
      {
        origData = parametersPtr_->parameters[i].data;
      }
      parametersPtr_->parameters[i].data--;
    }

    uint8_t len = parametersPtr_->parameters[i].ID.length();

    // divide the parameter ID into two lines if necessary
    uint8_t w = 4; // 4 is approx char width in px
    uint8_t c = 55; // center of the ID text
    if (len <= 20)
    {
        screen_.setFont(u8g2_font_profont10_tf);
        screen_.setCursor(c - (parametersPtr_->parameters[i].ID.length() * w / 2), 30); 
        screen_.print(parametersPtr_->parameters[i].ID);
    }
    else
    {
        uint8_t index = 20;
        while (parametersPtr_->parameters[i].ID.charAt(index) != '_' && index > 0)
        {
          index--;
        }

        String line1;
        String line2;

        line1 = parametersPtr_->parameters[i].ID.substring(0, index);
        line2 = parametersPtr_->parameters[i].ID.substring(index);

        screen_.setFont(u8g2_font_profont10_tf);
        screen_.setCursor(c - (line1.length() * w / 2), 30);
        screen_.print(line1);
        screen_.setCursor(c - (line2.length() * w / 2), 40);
        screen_.print(line2);       
    }

    screen_.setFont(u8g2_font_profont11_tr);
    screen_.setCursor(10, 55);

    if (parametersPtr_->parameters[i].unit == "bool") // if boolean parameter, replace 1/0 with true/false
    {
        if (parametersPtr_->parameters[i].data == 0)
        {
          screen_.print("false");
        }
        if (parametersPtr_->parameters[i].data == 1)
        {
          screen_.print("true");
        }
    }
    else
    {
      screen_.print(parametersPtr_->parameters[i].data); // print parameter value, if not boolean
    }
    screen_.print(" <");
    screen_.setFont(u8g_font_5x8);
    screen_.print(parametersPtr_->parameters[i].unit); //print unit
    screen_.print(">");

    if (!parametersPtr_->parameters[i].foundFromSD)
    {
      screen_.setFont(u8g_font_5x8);
      screen_.setCursor(0, 20);
      screen_.print("Not on SD");
    }

    screen_.setFont(u8g_font_5x8);
    screen_.setCursor(80,55);
    if (parametersPtr_->parameters[i].data == parametersPtr_->parameters[i].minData)
    {
      screen_.print("Min val.");
    }
    else if (parametersPtr_->parameters[i].data == parametersPtr_->parameters[i].maxData)
    {
      screen_.print("Max val.");
    }

    // if value has changed, show option to save
    if (origData != parametersPtr_->parameters[i].data)
    {
      screen_.setFont(u8g_font_5x8);
      screen_.setCursor(50,64);
      screen_.print("Press OK to save");

      if (okButton_.givePulse())
      {
         updateParameter(&parametersPtr_->parameters[i]);  //run the update function      
         origData = parametersPtr_->parameters[i].data;
         showNotification(1500, "Value saved to RAM!");
      }
    }

    // if not the first param on list, print arrow
    screen_.setFont(u8g2_font_saikyosansbold8_8u);
    if (i > 0)
    {
      screen_.drawStr(0, 30, "<");
    }

    // if not the last param on list, print arrow
    if (i+1 < parametersPtr_->size)
    {
      screen_.drawStr(120, 30, ">");
    }

    // exit the editor
    if (cancelButton_.giveSingleShot())
    {
      if (parametersPtr_->parameters[i].data != origData) // if data has changed
      {
        showNotification(1500, "Changed value discarded!");      
        parametersPtr_->parameters[i].data = origData;  
      } 
      return true;
    }
    return false;
}

bool ui::showTCCTuneView()
{
  core_.setLoggingStatus(true);
  core_.setLoggableVariable(core::TCSlipAndTCControl);
  plotDataForDualYAxis(false, true);
  screen_.setFont(u8g_font_4x6r);

  screen_.setCursor(28, 8);
  screen_.print("TC slip: ");
  screen_.print(int(log_->y2Data[0]));
  screen_.setCursor(85, 8);
  screen_.print("ctrl: ");
  screen_.print(log_->y1Data[0]);
  screen_.print(" %");

  static bool editMode = false;

  if (okButton_.givePulse() && !editMode)
  {
    editMode = true;
  }

  if (editMode)
  {
    if (cancelButton_.giveSingleShot())
    {
      editMode = false;
    }
  }

  if (!editMode) // select either of parameters
  {
    if (downButton_.givePulse() && editorRow_ < 1)
    {
        editorRow_++;
    }
    if (upButton_.givePulse() && editorRow_ > 0)
    {
        editorRow_--;
    }
  }

  screen_.setCursor(0, 22);
  screen_.print("PI-par.");

  screen_.setCursor(2, 35);
  screen_.print("Kp:");
  
  if (editMode && editorRow_ == 0)
  {
    screen_.setFont(u8g2_font_profont10_mn);
  }

  // seek Kp from parameters and print it to screen
  int i;
  for (i = 0; i < parametersPtr_->size; i++)
  {
    if (parametersPtr_->parameters[i].ID == "TCC_PI_control_P_factor")
    {
      break;
    }
  }
  screen_.print(parametersPtr_->parameters[i].data);
  screen_.setFont(u8g_font_4x6r); // fallback to normal font

  if (editorRow_ == 0)
  {
      screen_.drawFrame(0, 27, 25, 12);
      if (editMode && upButton_.givePulse() && parametersPtr_->parameters[i].data < parametersPtr_->parameters[i].maxData)
      {
        parametersPtr_->parameters[i].data++;
        updateParameter(&parametersPtr_->parameters[i]); //update parameter to controller
      }
      if (editMode && downButton_.givePulse() && parametersPtr_->parameters[i].data > parametersPtr_->parameters[i].minData)
      {
        parametersPtr_->parameters[i].data--;
        updateParameter(&parametersPtr_->parameters[i]); //update parameter to controller
      }
  }

  screen_.setCursor(2, 50);
  screen_.print("Ki:");
  if (editMode && editorRow_ == 1)
  {
    screen_.setFont(u8g2_font_profont10_mn); //highlight the selection
  }  

  // seek Ki from parameters and print it to screen
  int j;
  for (j = 0; j < parametersPtr_->size; j++)
  {
    if (parametersPtr_->parameters[j].ID == "TCC_PI_control_I_factor")
    {
      break;
    }
  }
  screen_.print(parametersPtr_->parameters[j].data);
  screen_.setFont(u8g_font_4x6r); // fallback to normal font

  if (editorRow_ == 1)
  {
      screen_.drawFrame(0, 42, 25, 12);
      if (editMode && upButton_.givePulse() && parametersPtr_->parameters[j].data < parametersPtr_->parameters[j].maxData)
      {
        parametersPtr_->parameters[j].data++;
        updateParameter(&parametersPtr_->parameters[j]); //update parameter to controller
      }
      if (editMode && downButton_.givePulse() && parametersPtr_->parameters[j].data > parametersPtr_->parameters[j].minData)
      {
        parametersPtr_->parameters[j].data--;
        updateParameter(&parametersPtr_->parameters[j]); //update parameter to controller
      }
  }

  if (cancelButton_.giveSingleShot()) 
  {
    editorRow_ = 0;
    editMode = false;
    return true;
  }
  return false; 

}

bool ui::showMalfunctionCodes()
{
  screen_.setFont(u8g2_font_synchronizer_nbp_tf);
  screen_.drawStr(25, 8, "FAULT CODES");
  screen_.drawLine(0, 10, 128, 10);  

  static bool confirmResetView = false;

  if (okButton_.giveSingleShot() && !confirmResetView)
  {
    confirmResetView = true;
  }

  if (!confirmResetView)
  { 
    int xoffset = 0;	
    const int firstLine = 18;
    const int lineGap = 9;
    
    screen_.setFont(u8g_font_5x8);
    uint8_t counter = 0;
    uint8_t activeCodes = 0;
    uint8_t descriptionCharsToDisplay = 22;

    for (uint8_t i = 0; i < sizeof(dataPtrs_.malfuncs->codes)/sizeof(dataPtrs_.malfuncs->codes[0]); i++)
    {
      if (dataPtrs_.malfuncs->codes[i] == true) {activeCodes++;}// get number of active fault codes
    }
    if (activeCodes > 5)
    {
      descriptionCharsToDisplay = 8; // in case there are more than 5, they dont fit in one column -> shorten the displayed description
    }    
    if (activeCodes == 0){showNotification(1500, "No active fault codes!"); return true;}

    for (uint8_t j = 0; j < sizeof(dataPtrs_.malfuncs->codes)/sizeof(dataPtrs_.malfuncs->codes[0]); j++) // print all active fault codes
    {
      if (dataPtrs_.malfuncs->codes[j] == true)
      {
        screen_.setCursor(xoffset, firstLine+lineGap*counter);
        screen_.print("E");
        screen_.print(j);
        screen_.print(":");
        screen_.scrollText(&dataPtrs_.malfuncs->descriptions[j], descriptionCharsToDisplay, 25);        
        counter++;
        if (counter > 5)  //...in two columns, if necessary
        {
          xoffset = 60;
          counter = 0;
        }
      }
    }
  }
  else if (dataPtrs_.malfuncs->activeMalfunctions) // ask to confirm the clear of all codes
  {
    screen_.drawStr(16, 25, "PRESS OK TO");
    screen_.drawStr(14, 40, "CLEAR ALL CODES");
    screen_.setFont(u8g_font_5x8);
    screen_.drawStr(0, 64, "Press cancel to return");

    if (okButton_.givePulse())// clear all codes and return to view
    { 
      core_.clearFaultCodes();
      showNotification(2000, "All fault codes cleared.");
      confirmResetView = false;
    } 
    if (cancelButton_.giveSingleShot()) {confirmResetView = false;} // do nothing and return to view
  }  

  if (!confirmResetView && cancelButton_.giveSingleShot()) // exit the view
  {
    confirmResetView = false;
    return true;
  }
  return false;
}

bool ui::goToMPCMapMenu()
{
  currentMenu_ = &MPCMenu_ ;
  return true;
}

bool ui::goToSPCMapMenu()
{
  currentMenu_ = &SPCMenu_ ;
  return true;
}

bool ui::goToMainMenu()
{
  currentMenu_ = &mainMenu_;
  return true;
}

bool ui::goToLiveDataMenu()
{
  currentMenu_ = &liveDataMenu_;
  return true;
}

bool ui::goToDataPlotterMenu()
{
  currentMenu_ = &dataPlotterMenu_;
  return true;
}

bool ui::buttonTests()
{
    screen_.setFont(u8g2_font_synchronizer_nbp_tf);
    screen_.drawStr(20, 8, "BUTTON TESTS");
    screen_.drawLine(0, 10, 128, 10);  

    uint8_t r = 3;
    uint8_t rs = 1;
    uint8_t rb = 4;

    uint8_t xCenter = 25;
    uint8_t yCenter = 30;

    screen_.drawCircle(xCenter, yCenter-10, r); // up
    screen_.drawCircle(xCenter, yCenter+10, r); // down
    screen_.drawCircle(xCenter-10, yCenter, r); // left
    screen_.drawCircle(xCenter+10, yCenter, r); // right

    screen_.drawCircle(xCenter+10, yCenter+25, rb); // ok
    screen_.drawCircle(xCenter-10, yCenter+25, rb); // cancel
    
    if (upButton_.isPressed())
    {
      screen_.drawCircle(xCenter, yCenter-10, rs);
    }

    if (downButton_.isPressed())
    {
      screen_.drawCircle(xCenter, yCenter+10, rs);
    }
    
    if (leftButton_.isPressed())
    {
      screen_.drawCircle(xCenter-10, yCenter, rs);
    }
    
    if (rightButton_.isPressed())
    {
      screen_.drawCircle(xCenter+10, yCenter, rs);
    }

    if (okButton_.isPressed())
    {
      screen_.drawCircle(xCenter+10, yCenter+25, rs);
    }

    if (cancelButton_.isPressed())
    {
      screen_.drawCircle(xCenter-10, yCenter+25, rs);
    }

    screen_.drawFrame(60, 20, 42, 10);

    if (autoModeSwitch_.isPressed())
    {
      screen_.drawStr(62, 28, "ON");
    }
    else
    {
        screen_.drawStr(80, 28, "OFF");
    }

    screen_.setFont(u8g_font_5x8);
    screen_.drawStr(60, 50, "Press cancel");
    screen_.drawStr(60, 60, "& ok to exit");

    if (cancelButton_.isPressed() && okButton_.isPressed())
    {
      return true;
    }

  return false;
}

bool ui::goToSDMenu()
{
  currentMenu_ = &SDMenu_;
  return true; 
}

bool ui::readConfigFile()
{
  showNotification(2500, core_.readConfFile());
  return true; 
}

bool ui::overWriteConfigFile()
{
  showNotification(2500, core_.writeConfFile());
  return true;
}

bool ui::restoreDefaultConfigFile()
{
    screen_.setFont(u8g2_font_profont11_tr);
    screen_.setCursor(0, 15);
    screen_.print("The configuration in");
    screen_.setCursor(0, 25);
    screen_.print("RAM will be overwr.");
    screen_.setCursor(0, 35);
    screen_.print("with conf from");
    screen_.setCursor(0, 45);
    screen_.print("ORICONF.CFG. Press");
    screen_.setCursor(0, 55);
    screen_.print("right (>) to confirm?");

    if(rightButton_.givePulse())
    {      
      showNotification(3000, core_.readDefaultsFile());
      return true;
    }
    else if (cancelButton_.giveSingleShot())
    {
      return true;
    }
    return false;
}

bool ui::showOther()
{
  screen_.drawOther();
  return cancelButton_.giveSingleShot();
}

bool ui::showMalfunctionCode1()
{
  // HERE PRINT STUFF ON THE SCREEN THAT MALFUNCTION HAS HAPPENED AND STH HAS TO BE DONE

  if (cancelButton_.giveSingleShot()) // if cancel is pressed, reset the fault code fall back to previous menu and selection
  {
    core_.clearFaultCodes();
    currentMenu_ = fallBackMenu_;
    currentMenu_->selection = fallBackSelection_;
    return true;
  }
  return false;
}

void ui::drawMenu(menuCollection* menu)
{
  const uint8_t startTextPos = 15;	
  const uint8_t firstLine = 21;
  const uint8_t lineGap = 10;
  uint8_t lineCounter = 0;

  screen_.setFont(u8g2_font_profont11_tr);
  screen_.drawStr(45, 8, "MENU");
  screen_.drawLine(0, 10, 128, 10);

  uint8_t maxVisibleItems = 5;
  uint8_t firstVisibleItem = 0;

  if (menu->selection >= (maxVisibleItems - 1)) // in case there is too much items (rows) in view
  {
    firstVisibleItem = menu->selection - (maxVisibleItems - 1); //change start position
  }
  for (uint8_t i = firstVisibleItem; i < firstVisibleItem + maxVisibleItems; i++)
  {
      if (menu->selection == i)
      {
        screen_.drawStr(0, firstLine+lineGap*lineCounter, "->"); // print arrow next to selected item
      }
      //char ttl[50]; // 50 will be max length of menuobj name
      //menu->menuObj[i].title.toCharArray(ttl, 50); // conversion String -> char[]
      screen_.drawStr(startTextPos, firstLine+lineGap*lineCounter, &*menu->menuObj[i].title);
      lineCounter++;
  }
}

void ui::setScreenDataPointers() // name to be changed, does not control the screen class anymore
{
    setDataPointers(core_.giveDataPointers());
}

void ui::setDataPointers(core::dataStruct data)
{
    dataPtrs_ = data;
}
