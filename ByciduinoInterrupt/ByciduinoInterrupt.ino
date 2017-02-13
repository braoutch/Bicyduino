//To store some data in SRAM
//#include <avr/pgmspace.h>

// PDQ: create LCD object (using pins in "PDQ_LI9340_config.h")

//////////////////////////
//////////DISPLAY/////////
#include "SPI.h"
#include <PDQ_GFX.h>

//#if defined(__SAM3X8E__)
//    #undef __FlashStringHelper::F(string_literal)
//    #define F(string_literal) string_literal
//#endif

///////////////////////////////////
//////////////Accelerometer////////
#include <Wire.h>
const int MPU = 0x68; // I2C address of the MPU-6050
int16_t Tmp, GyX, GyY, GyZ; //Accelerometer datas
///////////END ACCELEROMETER///////
///////////////////////////////////


//SPI Pin ; those are for Arduino UNO
#define _sclk 15
#define _miso 14
#define _mosi 16
#define	ILI9340_CS_PIN		10			// <= /CS pin (chip-select, LOW to get attention of ILI9340, HIGH and it ignores SPI bus)
#define	ILI9340_DC_PIN		9			// <= DC pin (1=data or 0=command indicator line) also called RS
#define	ILI9340_RST_PIN		8
#define	ST7735_SAVE_SPCR	0			// <= 0/1 with 1 to save/restore AVR SPI control register (to "play nice" when other SPI use)

#include <PDQ_ILI9340.h>

PDQ_ILI9340 tft;

extern "C" char __data_start[];		// start of SRAM data
extern "C" char _end[];			// end of SRAM data (used to check amount of SRAM this program's variables use)
extern "C" char __data_load_end[];	// end of FLASH (used to check amount of Flash this program's code and data uses)

/////////////END DISPLAY///////////
////////////////////////////////////
uint8_t totalDistance;
float distance;
float currentSpeed, averageSpeed;
unsigned long lastDuration, lastRising, timeToDisplay;
unsigned long lastStopMillis, lastStartMillis, timeStoppped, millisVal,millisVal2;
float oldSpeed, oldAverageSpeed, oldDistance;
int oldMinutes, oldHours,oldSeconds;
boolean start = false;
boolean detected = false;
int hours, minutes,seconds;
float diameter = 2.096f;

void setup()
{
  //Serial.begin(115200);
  pinMode(3, INPUT_PULLUP); //Pin2 is connected to the reed switch
  digitalWrite(3, HIGH); //Enable internal pull-up
  //attachInterrupt(0,Measurement,FALLING);
  
  #if defined(ILI9340_RST_PIN)	// reset like Adafruit does
  FastPin<ILI9340_RST_PIN>::setOutput();
  FastPin<ILI9340_RST_PIN>::hi();
  FastPin<ILI9340_RST_PIN>::lo();
  millisVal = millis();
  delay(1);
  
  FastPin<ILI9340_RST_PIN>::hi();
  #endif

  tft.begin(); //Screen initialization
  InitializeScreen();
  Display();
}

void loop() {
  millisVal2 = millis();
    millisVal = millis();

    int reading = digitalRead(3);
    if(reading==LOW)
    Measurement();
  //Measurement();  ///First we measure ; everything is done here.
  
  if (millisVal2 - lastRising > 6000 && start) {  ///To know if we are stopped
    lastStopMillis = millisVal2;
    currentSpeed = 0.0;
    start = false;
    //Serial.println("A l'arret");
    Display();
  }

  if (millisVal2 - lastRising < 6000 && !start)  ///To know if we go again
  {
    lastStartMillis = millisVal2;
    timeStoppped = timeStoppped + lastStartMillis - lastStopMillis;
    start = true;
    //Serial.println("C'est parti !!");

  }
}

void Measurement()  ///Use the Hall effect sensor to measure
{
    millisVal = millis();
    lastDuration = millisVal - lastRising;
    if(lastDuration > 100)
    {
    UpdateSpeed();  
    UpdateMeanSpeed();
    
    //if(millis()>timeToDisplay)
     Display();
    }
        lastRising = millisVal;

}

void UpdateSpeed()      ///Measure speed and distance
{ ////CALL BY INTERRUPTION

  currentSpeed = 3.6f* diameter / ((float)lastDuration/1000) ;
  //Serial.print("currentSpeed : ");
  //Serial.println(currentSpeed);
  distance = distance + diameter;
}

void UpdateMeanSpeed()  ///Compute average speed
{
  averageSpeed = 3600*(float)distance / (float)(millisVal - timeStoppped);
}

void ComputeDuration()  ///Compute real time spent
{
float totalTime = millisVal - timeStoppped;
minutes = (totalTime/60000);
seconds = ((totalTime/60000)-minutes)*60;
if(minutes >= 60)
{
  hours = (int)(totalTime/3600000);
  minutes = minutes - (hours * 60);
}
else
{
  hours = 0;
}
}

void Display()  ///Display the dynamic things
{
//  Serial.print("Speed: ");
//  Serial.println(currentSpeed);
//
//  Serial.print("last duration: ");
//  Serial.println(lastDuration);
//
//  Serial.print("Mean Speed: ");
//  Serial.println(averageSpeed);
//
//  Serial.print("Distance: ");
//  Serial.println(distance);
//
//  Serial.print(" | Tmp = "); Serial.print(Tmp / 340.00 + 36.53); //equation for temperature in degrees C from datasheet
//  Serial.print(" | GyX = "); Serial.print(GyX);
//  Serial.print(" | GyY = "); Serial.print(GyY);
//  Serial.print(" | GyZ = "); Serial.println(GyZ);
//
//  Serial.print("Awaken Time: ");
//  Serial.println(millis());

  //SPEED VALUE
  tft.setCursor(0,0);
  tft.setTextSize(5);
  tft.setTextColor(ILI9340_BLACK);
  tft.print(oldSpeed);
  tft.setCursor(0,0);
  tft.setTextColor(ILI9340_GREEN);
  tft.print(currentSpeed);
  oldSpeed = currentSpeed;
  
  //DISTANCE VALUE
  tft.setCursor(0,100);
  tft.setTextSize(5);
  tft.setTextColor(ILI9340_BLACK);
  tft.print(oldDistance);
  tft.setCursor(0,100);
  tft.setTextColor(ILI9340_GREEN);
  tft.print(distance/1000);
  oldDistance = distance/1000;
  
  //DURATION VALUE
  ComputeDuration();
  
  tft.setCursor(0,190);
  tft.setTextSize(4);
  tft.setTextColor(ILI9340_BLACK);
  tft.print(oldHours);
  tft.setCursor(55,190);
  tft.print(oldMinutes);
  tft.setTextSize(3);
  tft.setCursor(165,195);
  tft.print(oldSeconds);
  tft.setTextSize(4);
  tft.setTextColor(ILI9340_GREEN);
  tft.setCursor(0,190);
  tft.print(hours);
  tft.setCursor(55,190);
  tft.print(minutes);
  
  tft.setTextSize(3);
  tft.setCursor(165,195);
  tft.print(seconds);
  tft.setTextSize(4);

  oldMinutes = minutes;
  oldHours = hours;
  oldSeconds=seconds;

  //AVERAGE SPEED VALUE
  tft.setCursor(0,280);
  tft.setTextSize(4);
  tft.setTextColor(ILI9340_BLACK);
  tft.print(oldAverageSpeed);
  tft.setCursor(0,280);
  tft.setTextColor(ILI9340_GREEN);
  tft.print(averageSpeed);
  oldAverageSpeed = averageSpeed;



}

void InitializeScreen()
{
  /////////STATIC PART//////////
  tft.fillScreen(ILI9340_BLACK);

  //SPEED
  tft.setCursor(180, 20);
  tft.setTextSize(2);
  tft.setTextColor(ILI9340_WHITE);
  tft.print("km/h");
  
  //DISTANCE
  tft.setCursor(0, 60);
  tft.setTextSize(2);
  tft.setTextColor(ILI9340_WHITE);
  tft.print("Distance :");
  tft.setCursor(180, 120);
  tft.print("km");

  //DURATION
  tft.setCursor(0, 150);
  tft.setTextSize(2);
  tft.setTextColor(ILI9340_WHITE);
  tft.println("Duration :");
  tft.setCursor(29,200);
  tft.print("h");
  tft.setCursor(115,200);
  tft.print("min");
  tft.setCursor(212,200);
  tft.print("s");
  
  //AVERAGESPEED
  tft.setCursor(0, 240);
  tft.setTextSize(2);
  tft.setTextColor(ILI9340_WHITE);
  tft.println("Average speed :");
  tft.setCursor(180,290);
  tft.print("km/h");
}
