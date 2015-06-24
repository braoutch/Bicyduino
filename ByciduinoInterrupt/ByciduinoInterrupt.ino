
//To store some data in SRAM
include <avr/pgmspace.h>

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
//#define _sclk 13
//#define _miso 12
//#define _mosi 11
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
unsigned long distance;
float currentSpeed, averageSpeed;
unsigned long lastDuration, lastRising, timeToDisplay;
unsigned long lastStopMillis, lastStartMillis, timeStoppped;
float oldSpeed, oldAverageSpeed, oldDistance, oldMinutes, oldHours;
boolean start = false;
boolean detected = false;
static int hours, minutes;

long diameter = 2096;



void setup()
{
  Serial.begin(115200);
  pinMode(2, INPUT_PULLUP); //Pin2 is connected to the reed switch
  digitalWrite(2, HIGH); //Enable internal pull-up
  attachInterrupt(0,Measurement,FALLING);
  
  #if defined(ILI9340_RST_PIN)	// reset like Adafruit does
  FastPin<ILI9340_RST_PIN>::setOutput();
  FastPin<ILI9340_RST_PIN>::hi();
  FastPin<ILI9340_RST_PIN>::lo();
  
  delay(1);
  
  FastPin<ILI9340_RST_PIN>::hi();
  #endif

  tft.begin(); //Screen initialization
  InitializeScreen();

  /////////ACCELEROMETER////////
//  Wire.begin();
//  Wire.beginTransmission(MPU);
//  Wire.write(0x6B);  // PWR_MGMT_1 register
//  Wire.write(0);     // set to zero (wakes up the MPU-6050)
//  Wire.endTransmission(true);
  /////////////////////////////
}



void loop() {
  //Measurement();  ///First we measure ; everything is done here.
  
  if (millis() - lastRising > 5000 && start) {  ///To know if we are stopped
    lastStopMillis = millis();
    currentSpeed = 0.0;
    start = false;
    Serial.println("A l'arret");
  }

  if (millis() - lastRising < 5000 && !start)  ///To know if we go again
  {
    lastStartMillis = millis();
    timeStoppped = timeStoppped + lastStartMillis - lastStopMillis;
    start = true;
    Serial.println("C'est parti !!");

  }

}

void Measurement()  ///Use the Hall effect sensor to measure
{
        Serial.println(lastDuration);

    lastDuration = millis() - lastRising;
      Serial.println(lastDuration);
  ///We have a measure so it's time to compute
    UpdateSpeed();  
    UpdateMeanSpeed();
    
    if(millis()>timeToDisplay)
    {
      Display();
      timeToDisplay = timeToDisplay + 5000;
      if(distance/1000000 > pgm_read_byte(&totalDistance))
      totalDistance PROGMEM = pgm_read_byte(&totalDistance)+distance/1000000;
    }

    ///ACCELEROMETER///
//    Wire.beginTransmission(MPU);
//    Wire.write(0x41);  // starting with register 0x41 (ACCEL_XOUT_H)
//    Wire.endTransmission(false);
//    Wire.requestFrom(MPU, 8, true); // request a total of 2*4 registers
//    Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
//    GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
//    GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
//    GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
    lastRising = millis();

}

void UpdateSpeed()      ///Measure speed and distance
{ ////CALL BY INTERRUPTION

  currentSpeed = (float)diameter / (float)lastDuration ;

  distance = distance + diameter;
}


void UpdateMeanSpeed()  ///Compute average speed
{

  averageSpeed = (float)distance / ((float)millis() - (float)timeStoppped);

}

void * ComputeDuration()  ///Compute real time spent
{
float totalTime = (int)millis() - (int)timeStoppped;
hours = (totalTime/3600000);
minutes = (totalTime/60000);
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
  tft.print(distance/1000000);
  oldDistance = distance/1000000;
  
  //DURATION VALUE
  ComputeDuration();
  tft.setCursor(0,190);
  tft.setTextSize(4);
  tft.setTextColor(ILI9340_BLACK);
  tft.print(oldHours);
  tft.setCursor(80,190);
  tft.print(oldMinutes);
  tft.setCursor(0,190);
  tft.setTextColor(ILI9340_GREEN);
  tft.print(hours);
  tft.setCursor(80,190);
  tft.print(minutes);
  oldMinutes = minutes;
  oldHours = hours;

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
  tft.setCursor(160, 20);
  tft.setTextSize(2);
  tft.setTextColor(ILI9340_WHITE);
  tft.print(" km/h");
  
  //DISTANCE
  tft.setCursor(0, 60);
  tft.setTextSize(2);
  tft.setTextColor(ILI9340_WHITE);
  tft.print("Distance :");
  tft.setCursor(170, 110);
  tft.print("km");

  //DURATION
  tft.setCursor(0, 150);
  tft.setTextSize(2);
  tft.setTextColor(ILI9340_WHITE);
  tft.println("Duration :");
  tft.setCursor(0,210);
  tft.print("     h       min");
  
  //AVERAGESPEED
  tft.setCursor(0, 240);
  tft.setTextSize(2);
  tft.setTextColor(ILI9340_WHITE);
  tft.println("Average speed :");
  tft.setCursor(150,290);
  tft.print("km/h");
}
