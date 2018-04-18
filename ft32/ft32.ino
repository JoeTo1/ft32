#include <Wire.h>
#include <SparkFunSX1509.h>
#include "AssetHandler.h"
#include "WebsocketHandler.h"
#include "NetworkHandler.h"
#include "ft_ESP32_SHM.h"
#include "ft_ESP32_SW_Queue.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ft_ESP32_IOobjects.h"
#include "BeispielAnwendung.h"

#define DEBUG

//AssetHandler *nAssetHandler;
//WebsocketHandler *wsHandler;
//NetworkHandler nHandler;
//
SHM *ptrSHM;
//SW_queue mySW_queue;

Adafruit_SSD1306 display(4);

BeispielAnwendung *myBeispiel;

//void initQueue_static(void* arg) {
//    SHM *pSHM=(SHM*) arg;
//    while(1)
//    {
//      if(true==pSHM->commonStart)
//      {
//        mySW_queue.SW_work(pSHM);
//      }
//      else
//      {
//        delay(1);
//      }
//    }
//    vTaskDelete(NULL);
//}

//DigitalIO_PWMout *stiftUnten;
//DigitalIO_PWMout *stiftUnten1;
//DigitalIO_PWMout *stiftUnten2;
//DigitalIO_PWMout *stiftUnten3;
//DigitalIO_PWMout *stiftUnten4;
//DigitalIO_PWMout *stiftUnten5;
//DigitalIO_PWMout *stiftUnten6;
//DigitalIO_PWMout *stiftUnten7;

void setup() {
    Serial.begin(115200);
	Init_SparkFun();
	delay(1000);
    ptrSHM= new SHM;

	//stiftUnten = new DigitalIO_PWMout(0, INPUT_PULLUP);
	//stiftUnten1 = new DigitalIO_PWMout(1, INPUT_PULLUP);
	//stiftUnten2 = new DigitalIO_PWMout(2, OUTPUT);
	//stiftUnten3 = new DigitalIO_PWMout(3, INPUT_PULLUP);
	//stiftUnten4 = new DigitalIO_PWMout(4, OUTPUT);
	//stiftUnten5 = new DigitalIO_PWMout(5, OUTPUT);
	//stiftUnten6 = new DigitalIO_PWMout(6, OUTPUT);
	//stiftUnten7 = new DigitalIO_PWMout(7, OUTPUT);

 //   //nHandler.createUniqueAP("Espap-", "12345678");
 //   //nAssetHandler = new AssetHandler();
 //   //wsHandler = new WebsocketHandler(ptrSHM);

 //   //Serial.println("[main] Starting queue task");

 //   //xTaskCreatePinnedToCore(
 //   //  initQueue_static,   	/* Function to implement the task */
 //   //  "initQueue_static", 	/* Name of the task */
 //   //  4096,      			/* Stack size in words */
 //   //  (void*)ptrSHM,       	/* Task input parameter */
 //   //  0,          			/* Priority of the task */
 //   //  NULL,       			/* Task handle. */
 //   //  1);  					/* Core where the task should run */

	//SHM an Beispiel uebergeben, damit gestoppt werden kann, wenn Cody++ Programm aktiviert wurde
	myBeispiel = new BeispielAnwendung(ptrSHM);
	myBeispiel->start();

	//stiftUnten4.setValueDig(HIGH);
	//stiftUnten5.setValueDig(HIGH);
	//stiftUnten6.setValueDig(HIGH);
	//stiftUnten7.setValueDig(HIGH);

}


//Motor myMotor2(2);
//Motor myMotor3(3);
//int motorSpeed = 0;
//bool dir = 0;
//bool dir1 = 1;
//


void loop() {
    //nAssetHandler->handleAssetRequests();
    //wsHandler->handleWebSocketRequests();
	myBeispiel->run();


	////stiftUnten2.setValueDig(HIGH);
	////stiftUnten4.setValueDig(HIGH);


	//delay(500);
	//Serial.println(" ");

	//Serial.println("-------------------------------------");

	//Serial.println(millis());

	//Serial.println(stiftUnten->getValue());
	//if (stiftUnten->mDirection == INPUT_PULLUP) {
	//	Serial.println("INPUT_PULLUP");
	//}
	//if (stiftUnten->mDirection == INPUT_PULLDOWN) {
	//	Serial.println("INPUT_PULLDOWN");
	//}
	//if (stiftUnten->mDirection == INPUT) {
	//	Serial.println("INPUT");
	//}
	//if (stiftUnten->mDirection == OUTPUT) {
	//	Serial.println("OUTPUT");
	//}
	//Serial.println(stiftUnten1->getValue());
	////Serial.println(stiftUnten2.getValue());
	////Serial.println(stiftUnten3.getValue());	
	////Serial.println(stiftUnten4.getValue());
	////Serial.println(stiftUnten5.getValue());
	////Serial.println(stiftUnten6.getValue());
	////Serial.println(stiftUnten7.getValue());

	//Serial.println("-------------------------------------");

	



	

}


