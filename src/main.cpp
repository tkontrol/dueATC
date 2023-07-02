#include <Arduino.h>
#include <DueTimer.h>
#include "../headers/ui.h"

#define engineSpeedPin 22
#define primaryVehicleSpeedPin 24
#define secondaryVehicleSpeedPin 26
#define n2SpeedPin 49
#define n3SpeedPin 48

#define speedMeasInterruptInterval 10 // us

ui ui_ = ui();

int ftime = 0;
int lastTime = 0;
int dataCounter = 0;
bool ack;
bool receiving;
long receivedData;

void callEngineSpeedMeas()
{
  ui_.engineSpeedMeas();
}

void callPrimaryDriveShaftSpeedMeas()
{
  ui_.primaryVehicleSpeedMeas();  
}

void callSecondaryDriveShaftSpeedMeas()
{
  ui_.secondaryVehicleSpeedMeas();  
}

void calln2SpeedMeas()
{
  ui_.n2SpeedMeas();
}

void calln3SpeedMeas()
{
  ui_.n3SpeedMeas();
}

void increaseSpeedMeasCounters()
{
  ui_.increaseSpeedMeasCounters();
}

void coreLoop()
{
  ui_.runCoreLoop();
}

void uiLoop()
{
      // picture loop
  ui_.firstPage();
  do {    
    ui_.runUiLoop();
    ui_.printLoopDuration();
  } 
  while( ui_.nextPage() );

}

void adc_setup()
{
  PMC->PMC_PCER1 |= PMC_PCER1_PID37;                    // ADC power ON
  ADC->ADC_CR = ADC_CR_SWRST;                           // Reset ADC
  ADC->ADC_MR |=  ADC_MR_FREERUN_ON                    // ADC in free running mode
                //  | ADC_MR_STARTUP_SUT960
                  | ADC_MR_LOWRES_BITS_10               // 10 bits resolution
                  | ADC_MR_PRESCAL(1)
                  | ADC_MR_SETTLING_AST3
                  | ADC_MR_TRACKTIM(10)
                  | ADC_MR_TRANSFER(2);


  // channels: 7 = A0, 6 = A1, 5 = A2, 4 = A3, 3 = A4, 2 = A5, 1 = A6, 0 = A7, 15 = cpu temp sensor
  ADC->ADC_CHER |= ADC_CHER_CH7;                       // Enable Channel 7 = A0, 6 = A1, 5 = A2, 4 = A3, 3 = A4, 2 = A5, 1 = A6, 0 = A7, 15 = cpu temp sensor
  ADC->ADC_CHER |= ADC_CHER_CH6;                       // Enable channel 6 = A1
  ADC->ADC_CHER |= ADC_CHER_CH5;                       // Enable channel 5 = A2
  ADC->ADC_CHER |= ADC_CHER_CH15;                      // Enable channel 15 = cpu temp sensor

  ADC->ADC_ACR |= ADC_ACR_TSON;
}

void pwmsetup() {
  //set up PWM channels 0,1,2 (ports C.3,C.5,C.7), (Due pins P35,P37,P39), (PWMH0,PWMH1,PWMH2)
  REG_PIOC_PDR = 0B10101000; // PIO Disable Register for C
  REG_PIOC_ABSR = REG_PIOC_ABSR | 0B10101000; //Peripheral AB Select Register, select peripheral B for used pins
  REG_PMC_PCER1 = REG_PMC_PCER1 | 16; //Peripheral Clock Enable Register 1 (activate clock for PWM, id36, bit5 of PMC_PCSR1)
  REG_PWM_ENA = REG_PWM_SR | B111; //PWM Enable Register | PWM Status Register (activate channels 0,1,2)

  // Use 0x100000 to enable dead time
  REG_PWM_CMR0 = PWM_CMR_DTE | PWM_CMR_CPRE_MCK_DIV_1024; //Channel 0 Mode Register: Dead Time Enable DTE=1, clock divider 1024 -> ~ 825 Hz PWM
  REG_PWM_CMR1 = PWM_CMR_DTE | PWM_CMR_CPRE_MCK_DIV_1024; //Channel 1 Mode Register: Dead Time Enable DTE=1, clock divider 1024 -> ~ 825 Hz PWM
  REG_PWM_CMR2 = PWM_CMR_DTE | PWM_CMR_CPRE_MCK_DIV_1024; //Channel 2 Mode Register: Dead Time Enable DTE=1, clock divider 1024 -> ~ 825 Hz PWM

  // available range is same as duty and depends on PWM frequency (see datasheet)
  REG_PWM_DT0 = 0x000000; //Channel0 Dead Time Register for pins 34,35 (PWML0,PWMH0)
  REG_PWM_DT1 = 0x000000; //Channel1 Dead Time Register for pins 36,37 (PWML1,PWMH1)
  REG_PWM_DT2 = 0x000000; //Channel2 Dead Time Register for pins 38,39 (PWML2,PWMH2)

  REG_PWM_CPRD0 = 100; // ch0 (pin 35), pwm duty range 0-100
  REG_PWM_CPRD1 = 100; // ch1 (pin 37), pwm duty range 0-100
  REG_PWM_CPRD2 = 100; // ch2 (pin 39), pwm duty range 0-100

  REG_PWM_CDTY0 = 0;
  REG_PWM_CDTY1 = 0;
  REG_PWM_CDTY2 = 0;
}

void setup()
{

  Serial.begin(460800);

  pinMode(engineSpeedPin, INPUT);
  pinMode(primaryVehicleSpeedPin, INPUT);
  pinMode(secondaryVehicleSpeedPin, INPUT);
  pinMode(n2SpeedPin, INPUT_PULLUP);
  pinMode(n3SpeedPin, INPUT_PULLUP);

  NVIC_SetPriority(TC0_IRQn, 0); // set timer 0 to be highest priority (rpm measurements timer)
  NVIC_SetPriority(TC1_IRQn, 1); // set timer 1 to be second highest priority (core loop)
  NVIC_SetPriority(TC2_IRQn, 2); // set timer 1 to be second highest priority (UI loop)
  NVIC_SetPriority(SysTick_IRQn, 3); // make sure SysTick is lower than our timers
  NVIC_SetPriority(ADC_IRQn, 4);

  ui_.initUI();

  attachInterrupt(digitalPinToInterrupt(engineSpeedPin), callEngineSpeedMeas, RISING);
  attachInterrupt(digitalPinToInterrupt(primaryVehicleSpeedPin), callPrimaryDriveShaftSpeedMeas, RISING);
  attachInterrupt(digitalPinToInterrupt(secondaryVehicleSpeedPin), callSecondaryDriveShaftSpeedMeas, RISING);
  attachInterrupt(digitalPinToInterrupt(n2SpeedPin), calln2SpeedMeas, RISING);
  attachInterrupt(digitalPinToInterrupt(n3SpeedPin), calln3SpeedMeas, RISING); 
  Timer0.attachInterrupt(increaseSpeedMeasCounters).start(speedMeasInterruptInterval);
  Timer1.attachInterrupt(coreLoop).start(1000);
  Timer2.attachInterrupt(uiLoop).start(100000);

  //attachInterrupt(digitalPinToInterrupt(22), print, FALLING); 
  adc_setup();
  pwmsetup();
}

void loop()
{ }

