//ILI9340 library from Adafruit
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9340.h"

///////////////////////////////////
//////////////Accelerometer////////
#include <Wire.h>
const int MPU=0x68;  // I2C address of the MPU-6050
int16_t Tmp,GyX,GyY,GyZ; //Accelerometer datas
///////////END ACCELEROMETER///////
///////////////////////////////////

//////////////////////////
//////////DISPLAY/////////
#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

//SPI Pin ; those are for Arduino UNO
#define _sclk 13
#define _miso 12
#define _mosi 11
#define _cs 10
#define _dc 9
#define _rst 8

Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _rst);
/////////////END DISPLAY///////////
////////////////////////////////////

unsigned long distance;
float currentSpeed,meanSpeed;
unsigned long lastDuration, lastRising;
unsigned long lastStopMillis, lastStartMillis, timeStoppped;

boolean start = false;
boolean detected = false;

long diameter = 2150;



void setup()
{
        Serial.begin(9600);
        tft.begin(); //Screen initialization
	//attachInterrupt(0,UpdateSpeed,RISING);

	/////////ACCELEROMETER////////
	Wire.begin();
	Wire.beginTransmission(MPU);
  	Wire.write(0x6B);  // PWR_MGMT_1 register
  	Wire.write(0);     // set to zero (wakes up the MPU-6050)
	Wire.endTransmission(true);
	/////////////////////////////
}



void loop(){
        Measurement();
	if(millis() - lastRising >5000 && start){

		lastStopMillis = millis();
                currentSpeed = 0.0;
		start = false;
                Serial.println("A l'arret");

	}

	if(millis() - lastRising < 5000 && !start)
	{
		lastStartMillis = millis();
		timeStoppped = timeStoppped + lastStartMillis - lastStopMillis;
                start = true;
                Serial.println("C'est parti !!");

	}

}

void Measurement()
{
	int raw = analogRead(A0);   // Range : 0..1024
        if(raw > 600 && detected ==false)
        {
        	detected = true;
        	UpdateSpeed();
        	UpdateMeanSpeed();
        	Display(raw);
        	
        	///ACCELEROMETER///
        	Wire.beginTransmission(MPU);
		Wire.write(0x41);  // starting with register 0x41 (ACCEL_XOUT_H)
  		Wire.endTransmission(false);
  		Wire.requestFrom(MPU,8,true);  // request a total of 2*4 registers
  		Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  		GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  		GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  		GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
        }
        
        else if (raw < 600)
        detected = false;
}

void UpdateSpeed()
{ ////CALL BY INTERRUPTION
	lastDuration = millis() - lastRising;

	lastRising = millis();

	currentSpeed = (float)diameter / (float)lastDuration ;

	distance = distance+diameter;
}


void UpdateMeanSpeed()
{

	meanSpeed = (float)distance/((float)millis()-(float)timeStoppped);

}

void Display(int raw)
{
	Serial.print("Speed: ");
	Serial.println(currentSpeed);

	Serial.print("last duration: ");
	Serial.println(lastDuration);

        Serial.print("Mean Speed: ");
	Serial.println(meanSpeed);

        Serial.print("Distance: ");
	Serial.println(distance);

        Serial.print("Output: ");
	Serial.println(raw);
	
	Serial.print(" | Tmp = "); Serial.print(Tmp/340.00+36.53);  //equation for temperature in degrees C from datasheet
  	Serial.print(" | GyX = "); Serial.print(GyX);
  	Serial.print(" | GyY = "); Serial.print(GyY);
  	Serial.print(" | GyZ = "); Serial.println(GyZ);
             
        Serial.print("Awaken Time: ");
        Serial.println(millis());
}
