﻿/*
Ausgangstreiber für ESP32-Fischertechnik-Anbindung
Autor: Johannes Marquart
*/
#ifndef FT_ESP32_IOOBJECTS_H
#define FT_ESP32_IOOBJECTS_H

#include <Arduino.h>
#include <SparkFunSX1509.h>

//Festlegen Anzahl Ports
constexpr size_t MOTOR_QTY = 4;	//Anzahl der Motoren
constexpr size_t LAMP_QTY = 4;	//Anzahl der Lampen
constexpr size_t DAIN_QTY = 2;	//Anzahl der Eingänge (digital/analog)
constexpr size_t DIO_PWMO_QTY = 8;

//Zuweisung Ports
//const int PORT_M_DIR[MOTOR_QTY] = { 17, 4, 14, 12 };	//Output-Pins Motor-Richtung

const byte SX1509_PORT_M_DIR[MOTOR_QTY] = { 0, 2, 4, 6 };	//Motor Richtung - Output am SX1509

const int ESP32_PORT_M_PWM[MOTOR_QTY] = { 4, 2, 13, 15 };	//Output-Pins Motor-Drehzahl
//const int PIN_M_INH = 27;	//Output-Pin Einschalten Motortreiber

const int ESP32_PORT_M_ENCODER[MOTOR_QTY] = { 32, 33, 25, 26};	//InputPins Motor Encoder

const int ESP32_PORT_L_PWM[LAMP_QTY] = { 16, 2, 13, 15 };	//Output-Pins Lampe, werden hier über den selben Treiber angesteuert
//const int PIN_L_INH = 27;	//Output-Pin Einschalten Lampentreiber

const int ESP32_PORT_IN[DAIN_QTY] = { 34, 35};	//Input-Pins Ditital/Analog
																//Zuweisung Ports auf SX1509
const byte SX1509_PIN_M_INH = 4;	//Output-Pin Einschalten Motortreiber

const byte SX1509_PORT_L_PWM[LAMP_QTY] = { 1, 3, 5, 7 };	//Output-Pins Lampe, werden hier �ber den selben Treiber angesteuert
const byte SX1509_PIN_L_INH = 4;	//Output-Pin Einschalten Lampentreiber

const byte SX1509_PORT_DIO_PWMO[DIO_PWMO_QTY] = { 8, 9, 10, 11, 12, 13, 14, 15 };	//DIO/PWMout Pins auf SX1509

																					//SX1509 Variables
const byte SX1509_I2C_ADDRESS = 0x3E;		//wenn keine ADD-Jumper auf SX1509 aktiviert sind
const byte SX1509_I2C_PIN_SDA = 21;
const byte SX1509_I2C_PIN_SCL = 22;
const byte SX1509_PIN_RESET = 16;
const byte SX1509_PIN_EXTI = 17;

extern SX1509 sx1509Object;				//i2c SDA = PIN 21, SCL = PIN 22

void Init_SparkFun();

class Motor
{
public:
	Motor();	//Standardkonstruktor, setzt alles auf 0;
	Motor(unsigned int motorNr);	//Konstruktor, Motor-Nr (0..3), weist zu: Pin-Nr für PWM, Pin-Nr für Richtung
	void setValues(bool, unsigned int);	//neue Motorwerte setzen (Richtung, Drehzahl)
	void reRun();	//bei Aufruf werden erneut die Pins und PWM mit den Attributen gesetzt
	//evtl. eine Methode Stop einbauen
private:
	unsigned int mMotorNr;	//Motornummer 0..3, wird bei Erstellung des Objekts angelegt
	unsigned int mPortNrPWM;	//Portnummer für PWM, wird bei Erstellung des Objekts zugewiesen
	unsigned int mPortNrDir;	//PortNr für Richtung, wird bei Erstellung des Objekts zugewiesen
	bool mRechtslauf;	//Drehrichtung: rechts = ture, links = false
	unsigned int mDrehzahl;	//aktuelle Geschwindigkeit (von 0 bis 8)
	unsigned int mLedcChannel;
};

class Lampe
{
public:
	Lampe();
	Lampe(unsigned int);	//Konstruktor, Lampe-Nr (0..7), weist zu: Pin-Nr für PWM
	void setValues(unsigned int);	//neue Lampenwerte setzen (Aktiv, Helligkeit)
	void reRun();	//bei Aufruf werden erneut die Pins und PWM mit den Attributen gesetzt
private:
	unsigned int mLampeNr;	//LampenNr 0..7, wird bei Erstellung des Objekts angelegt
	unsigned int mPortNrPWM;	//Portnummer für PWM, wird bei Erstellung des Objekts zugewiesen
	unsigned int mHelligkeit;	//aktuelle Helligkeit (von 0 bis 8)
};

class Encoder
{
public:

private:

};

class DigitalAnalogIn
{
public:
	DigitalAnalogIn();
	DigitalAnalogIn(unsigned int);
	unsigned int getValueAnalog();
	unsigned int getValueDigital();
	void setValueDigital(bool);	//Digitalen Ausgang setzen (HIGH/LOW)
private:
	unsigned int mInputNummer;
	unsigned int mInputPortNr;
};

//Klasse um zus�tzliche IOs vom SX1509 zu nutzen
class DigitalIO_PWMout
{
public:
	DigitalIO_PWMout(byte io, byte inOut);		//io von 0-7, inOut-Constants von ARDUINO nutzen
	bool getValue();							//liest Digitalen Input (setzt Pin zu erst auf direction = INPUT)
	void setValueDig(bool val);					//setzt digitalen Output (setzt Pin zu erst auf direction = OUTPUT)
	void setPWM(unsigned char pwmVal);			//setzt Pin auf PWM (Frequenz fest f�r A4990 eingestellt - in init zu sehen)
private:
	DigitalIO_PWMout();
	byte mIOPin;			//Pin am SX1509
	byte mIONumber;			//Nummer des IOs (0-7)
	byte mDirection;		//input oder output
};


#endif