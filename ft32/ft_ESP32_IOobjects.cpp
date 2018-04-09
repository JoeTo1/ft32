﻿/*
Ausgangstreiber für ESP32-Fischertechnik-Anbindung
Autor: Johannes Marquart
*/
#include "ft_ESP32_IOobjects.h"

/*
//Prototypen zur Simulation für VisualStudio
void ledcAttachPin(int, int) {}
void ledcSetup(int, int, int) {}
void ledcWrite(int, int) {}
void pinMode(int, int) {}
void digitalWrite(int, int) {}
unsigned int digitalRead(unsigned int) { return 0; }
unsigned int analogRead(unsigned int) { return 0; }
void delay(double) {}
//Ende der Prototypen für VS
*/

SX1509 sx1509Object;				//i2c SDA = PIN 21, SCL = PIN 22
int initRecCalls = 0;

void Init_SparkFun()
{
	static bool IS_INIT = false;
	if (!IS_INIT)
	{
		byte rstPin = SX1509_PIN_RESET;
		delay(1000);
		if (!sx1509Object.begin(SX1509_I2C_ADDRESS))
		{		
			Serial.println("SX1509 I2C Address" + SX1509_I2C_ADDRESS);
			Serial.println("I2C_Problem");
			initRecCalls++;
			pinMode(rstPin, OUTPUT);

			//reset Pin des SX nutzen um eine Reset zu versuchen
			digitalWrite(rstPin, HIGH);
			delay(10);
			digitalWrite(rstPin, LOW);

			Init_SparkFun();
			return;                                                   //If we fail to communicate, loop forever.
		}
		else
		{
			Serial.print("I2C_OK, unsuccessful tries: ");
			Serial.println(initRecCalls);
		}
		sx1509Object.clock(INTERNAL_CLOCK_2MHZ, 4);
		for (byte i = 0; i < 15; i++)
		{
			sx1509Object.pinMode(i, OUTPUT);
		}

		IS_INIT = true;
	}
}

Motor::Motor()
{
	//Abschalten des Motortreibers, welcher von diesem Objekt versorgt wird.
	//Evtl. noch undefinierte Pins können so kein falsches Signal an den Motortreiber geben
	sx1509Object.pinMode(SX1509_PIN_M_INH, OUTPUT);
	sx1509Object.digitalWrite(SX1509_PIN_M_INH, LOW);
	
	mMotorNr = 0;
	mPortNrPWM = 0;
	mPortNrDir = 0;
	mRechtslauf = true;
	mDrehzahl = 0;
}

Motor::Motor(unsigned int motorNr)
{
	//Abschalten des Motortreibers, evtl. noch undefinierte Pins können so kein falsches Signal an den Motortreiber geben
	sx1509Object.pinMode(SX1509_PIN_M_INH, OUTPUT);
	sx1509Object.digitalWrite(SX1509_PIN_M_INH, LOW);
	
	//Initialisieren des Motorobjektes
	mMotorNr = motorNr;
	
	mPortNrPWM = ESP32_PORT_M_PWM[mMotorNr];
	mPortNrDir = SX1509_PORT_M_DIR[mMotorNr];
	mRechtslauf = true;
	mDrehzahl = 0;
	mLedcChannel = mMotorNr * 2;

	////Zuweisen PWM-Generator zu Pin. Generator 0,2,4,6 für Drehzahl
	ledcAttachPin(mPortNrPWM, mLedcChannel);	//Pin-Nr für Drehzahl, PWM-Generator Nr
	ledcSetup(mLedcChannel, 21700, 8);	//PWM-Generator Nr, 21.7 kHz PWM, 8-bit resolution (0..255)
	ledcWrite(mLedcChannel, 0);	//frühzeitiges Definieren des PWM-Generators (PWM-Generator Nr., PWM-Wert (0..255))

	sx1509Object.digitalWrite(mPortNrDir, 1);	//fr�hzeitiges Definieren des Dir-Pins	
}

void Motor::setValues(bool rechtslauf, unsigned int drehzahl)
{
	mRechtslauf = rechtslauf;
	mDrehzahl = drehzahl;
	
	//Serial.begin(9600); -> sollte in der aufrufenden Instanz schon initialisiert sein
	Serial.print("Motor ");
	Serial.print(mMotorNr);
	Serial.print(" dreht in Richtung ");
	Serial.print(mRechtslauf);
	Serial.print(" mit Drehzahl ");
	Serial.println(mDrehzahl);

	//Berechnen der PWM-Werte
	int drehzahl_pwm;
	if (mDrehzahl < 1)
	{
		drehzahl_pwm = 0;
	}
	else if (mDrehzahl >7)
	{
		drehzahl_pwm = 255;
	}
	else
	{
		drehzahl_pwm = mDrehzahl * 256 / 8;
	}

	//Zuweisen der Richtung an den richtigen Pin entsprechend der Motornr.
	if (mRechtslauf)
	{
		//digitalWrite(mPortNrDir, HIGH);
		sx1509Object.digitalWrite(mPortNrDir, 1);	//Generator f�r Richtung wird auf max. (255) gesetzt
	}
	else
	{
		//digitalWrite(mPortNrDir, LOW);
		sx1509Object.digitalWrite(mPortNrDir, 0);	//Generator f�r Richtung wird auf 0 gesetzt
		//!!! Unbedingt im Datenblatt des Motortreibers nachsehen, wie PWM und Richtung zusammenhängen !!!

		drehzahl_pwm = 255 - drehzahl_pwm;	//wenn der Motor rückwärts läuft, ist die PWM invertiert (255 = min, 0 = max)
	}

	ledcWrite(mLedcChannel, drehzahl_pwm);
	
	sx1509Object.digitalWrite(SX1509_PIN_M_INH, HIGH);	//Einschalten Motortreiber
	
	Serial.print("Dir: ");
	Serial.print(mPortNrDir);
	Serial.print(" PWM: ");
	Serial.print(mPortNrPWM);
	Serial.print(" Val: ");
	Serial.println(drehzahl_pwm);
}

void Motor::reRun()
{
	if(mDrehzahl > 0)
	{
		setValues(mRechtslauf, mDrehzahl);
	}
}

Lampe::Lampe()
{
	//Abschalten des Lampentreibers, welcher von diesem Objekt versorgt wird.
	//Evtl. noch undefinierte Pins können so kein falsches Signal an den Lampentreiber geben
	sx1509Object.pinMode(SX1509_PIN_L_INH, OUTPUT);
	sx1509Object.digitalWrite(SX1509_PIN_L_INH, LOW);
	
	mLampeNr = 0;
	mPortNrPWM = 0;
	mHelligkeit = 0;
}

Lampe::Lampe(unsigned int lampeNr)
{
	//Abschalten des Motortreibers, evtl. noch undefinierte Pins können so kein falsches Signal an den Motortreiber geben
	sx1509Object.pinMode(SX1509_PIN_L_INH, OUTPUT);
	sx1509Object.digitalWrite(SX1509_PIN_L_INH, LOW);
	
	//Initialisieren des Lampenobjektes
	mLampeNr = lampeNr;
	
	//Folgender Abschnitt erlaubt es pro 'pololu a4990 dual-motor-driver' 4 Lampen unabhängig voneinander anzusteuern
	if(mLampeNr % 2 == 0)	//Lampen 0,2,4,... werden durch Pins der PWM-Reihe angesteuert
	{
		mPortNrPWM = ESP32_PORT_L_PWM[mLampeNr/2];
	}
	else	//Lampen 1,3,5,... werden durch Pins der DIR-Reihe angesteuert
	{
		mPortNrPWM = SX1509_PORT_M_DIR[mLampeNr/2];
	}
	mHelligkeit = 0;

	//Zuweisen des PWM-Generators an einen Port entsprechend der Lampennummer...
	ledcAttachPin(mPortNrPWM, mLampeNr);	//Pin-Nr, PWM-Generator Nr
	ledcSetup(mLampeNr, 21700, 8);	//PWM-Generator Nr, 21.7 kHz PWM, 8-bit resolution (0..255)
	//pinMode(PORT_M_DIR[mLampeNr], OUTPUT);	//Pin-Nr für "Richtungsangabe", Ausgang
	/*da hier Lampen am selben Treiber angeschlossen sind wie die Motoren, benötigen sie eine "Richtung"
	für eine korrekte Interpretation der PWM-Signale
	*/
	
	//digitalWrite(PORT_M_DIR[mLampeNr], HIGH);	//frühzeitiges Definieren des Dir-Pins
	ledcWrite(mLampeNr, 0);	//frühzeitiges Definieren des PWM-Generators (PWM-Generator Nr., PWM-Wert (0..255))
	
}

void Lampe::setValues(unsigned int helligkeit)
{
	//Serial.begin(9600);
	mHelligkeit = helligkeit;
	Serial.print("Lampe ");
	Serial.print(mLampeNr);
	Serial.print(" leuchtet mit Helligkeit ");
	Serial.println(mHelligkeit);
	
	//Berechnen der PWM - Werte
	int helligkeit_pwm;
	if (mHelligkeit > 7)
	{
		helligkeit_pwm = 255;
	}
	else if (mHelligkeit < 1)
	{
		helligkeit_pwm = 0;
	}
	else
	{
		helligkeit_pwm = mHelligkeit * 256 / 8;
	}

	if(mLampeNr%2 == 1)	//bei den Lampen 1,3,5,7 sind laut Datenblatt die PWM invertiert (255 = min, 0 = max)
	{
		helligkeit_pwm = 255 - helligkeit_pwm;
	}
	
	//Zuweisen des PWM-Werts an den richtigen Port entsprechend der Lampennummer
	ledcWrite(mLampeNr, helligkeit_pwm);
	//digitalWrite(PORT_M_DIR[mLampeNr], HIGH);	//Richtungsangabe, siehe Beschreibung im Konstruktor
	
	sx1509Object.digitalWrite(SX1509_PIN_L_INH, HIGH);	//Einschalten Lampentreiber
	
	Serial.print("PWM: ");
	Serial.print(mPortNrPWM);
	Serial.print(" Val: ");
	Serial.println(helligkeit_pwm);
}

void Lampe::reRun()
{
	if(mHelligkeit > 0)
	{
		setValues(mHelligkeit);
	}
}

DigitalAnalogIn::DigitalAnalogIn()
{
	mInputNummer = 0;
}

DigitalAnalogIn::DigitalAnalogIn(unsigned int inputNummer)
{
	mInputNummer = inputNummer;
	mInputPortNr = ESP32_PORT_IN[mInputNummer];
}

unsigned int DigitalAnalogIn::getValueDigital()
{
	pinMode(mInputPortNr, INPUT_PULLUP);	//Pin-Modus einrichten: Input mit Pull-Up Widerstand
	bool eingabe = !digitalRead(mInputPortNr);	//Inverse Logik: Schalter gedrückt = 1 (= Port liegt auf Masse)
	return (unsigned int) eingabe;
}

unsigned int DigitalAnalogIn::getValueAnalog()
{
	pinMode(mInputPortNr, INPUT);	//Pin-Modus einrichten: Input ohne Pull-Up Widerstand
	unsigned int eingabe = analogRead(mInputPortNr);
	return eingabe;
}

void DigitalAnalogIn::setValueDigital(bool ledLevel)
{
	pinMode(mInputPortNr, OUTPUT);	//Pin_Modus einrichten: Output
	Serial.print("Setze LED ");
	Serial.print(mInputNummer);
	Serial.print(" auf ");
	if (ledLevel)
	{
		digitalWrite(mInputPortNr, HIGH);	//Pin auf HIGH setzen
		Serial.println(" HIGH");
	}
	else
	{
		digitalWrite(mInputPortNr, LOW);	//Pin auf LOW setzen
		Serial.println(" LOW");
	}
}

DigitalIO_PWMout::DigitalIO_PWMout()
{
#ifdef DEBUG
	Serial.println("Ctor DIO_PWMO N/A");
#endif // DEBUG

	//Aufrufen um sicherzustellen, dass SX1509 Initialisiert ist
	Init_SparkFun();

	Serial.println("DigitalIO_PWMout mit parameterlosem Ctor initialisiert");
	mIONumber = 0;
}

DigitalIO_PWMout::DigitalIO_PWMout(byte io, byte inOut)
{
#ifdef DEBUG
	Serial.println("Ctor DIO_PWMO " + io);
#endif // DEBUG

	//Aufrufen um sicherzustellen, dass SX1509 Initialisiert ist
	Init_SparkFun();

	mIONumber = io;
	mDirection = inOut;
	mIOPin = SX1509_PORT_DIO_PWMO[io];

	sx1509Object.pinMode(mIOPin, mDirection);
}

bool DigitalIO_PWMout::getValue()
{
	if (mDirection != INPUT) {
		sx1509Object.pinMode(mIOPin, INPUT);
		mDirection = INPUT;
		Serial.println("SX1509 IO Output wurde in 'getValue' zu Input ge�ndert, IONumber: " + mIONumber);
		Serial.println("IOPin: " + mIOPin);
	}

	return sx1509Object.digitalRead(mIOPin);
}

void DigitalIO_PWMout::setValueDig(bool val)
{
	if (mDirection != OUTPUT) {
		sx1509Object.pinMode(mIOPin, OUTPUT);
		mDirection = OUTPUT;
		Serial.println("SX1509 IO Input wurde in 'setValueDig' zu Output ge�ndert, IONumber: " + mIONumber);
		Serial.println("IOPin: " + mIOPin);
	}

	if (val)
		sx1509Object.digitalWrite(mIOPin, HIGH);
	else
		sx1509Object.digitalWrite(mIOPin, LOW);
}

void DigitalIO_PWMout::setPWM(unsigned char pwmVal)
{
	if (mDirection != OUTPUT) {
		sx1509Object.pinMode(mIOPin, OUTPUT);
		mDirection = OUTPUT;
		Serial.println("SX1509 IO Input wurde in 'setPWM' zu Output ge�ndert, IONumber: " + mIONumber);
		Serial.println("IOPin: " + mIOPin);
	}

	sx1509Object.analogWrite(mIOPin, pwmVal);
}
