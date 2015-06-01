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
	//attachInterrupt(0,UpdateSpeed,RISING);
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
        }
        else if (raw < 600)
        detected = false;
}

void UpdateSpeed(){ ////CALL BY INTERRUPTION

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
        
        Serial.println(millis());
}
