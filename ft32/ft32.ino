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
//SHM *ptrSHM;
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


void setup() {
    Serial.begin(115200);
	Init_SparkFun();
	delay(1000);
    //ptrSHM= new SHM;

    //nHandler.createUniqueAP("Espap-", "12345678");
    //nAssetHandler = new AssetHandler();
    //wsHandler = new WebsocketHandler(ptrSHM);

    //Serial.println("[main] Starting queue task");

    //xTaskCreatePinnedToCore(
    //  initQueue_static,   	/* Function to implement the task */
    //  "initQueue_static", 	/* Name of the task */
    //  4096,      			/* Stack size in words */
    //  (void*)ptrSHM,       	/* Task input parameter */
    //  0,          			/* Priority of the task */
    //  NULL,       			/* Task handle. */
    //  1);  					/* Core where the task should run */

	/*myBeispiel = new BeispielAnwendung(ptrSHM);
	myBeispiel->start();*/

	Motor myMotor(0);
	Motor myMotor1(1);
	Motor m2(2);
	DigitalIO_PWMout stiftUnten(0, INPUT);
	stiftChangeState(m2, stiftUnten, STIFT_DOWN);
	delay(2000);
	stiftChangeState(m2, stiftUnten, STIFT_UP);
	//turnDegrees(90, myMotor, myMotor1);
}


//Motor myMotor2(2);
//Motor myMotor3(3);
//int motorSpeed = 0;
//bool dir = 0;
//bool dir1 = 1;
//
//DigitalIO_PWMout stiftUnten(0, INPUT);

void loop() {
    //nAssetHandler->handleAssetRequests();
    //wsHandler->handleWebSocketRequests();

	//delay(2000);
	//Serial.print(millis());
	//Serial.print(" - Wert von sens Stift unten: ");
	//Serial.println(stiftUnten.getValue());

	//delay(1000);

	//myMotor.setValues(0, motorSpeed);
	//myMotor1.setValues(0, motorSpeed);
	//myMotor2.setValues(0, motorSpeed);
	//myMotor3.setValues(0, motorSpeed);

	//motorSpeed--;

	//if (motorSpeed < 0) {
	//	motorSpeed = 8;
	//	/*if (dir) {
	//		dir = 0;
	//		dir1 = 1;
	//	}
	//	else {
	//		dir = 1;
	//		dir1 = 0;
	//	}*/
	//}



	

}


