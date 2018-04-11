// 
// 
// 

#include "BeispielAnwendung.h"

bool stiftChangeState(Motor motor, DigitalIO_PWMout sxIOUnten, bool wantedState) {
	unsigned long startTime = millis();
	bool stiftDownSens = sxIOUnten.getValue();
	if (wantedState) {			//true -> Stift auf Papier drücken
		motor.setValues(1, 5);
		while (!stiftDownSens) {		//warten bis Stiftsensor anzeigt, dass Stift gesenkt wurde
			delay(50);
			stiftDownSens = sxIOUnten.getValue();
			Serial.println(stiftDownSens);
			if (millis() - startTime > AbbortChangeStiftStatThreshold) {
				Serial.println("Stift senken wurde abgebrochen");
				motor.setValues(1, 0);
				return 0;
			}
		}
		delay(100);				//damit Stift sicher am Boden ist
		motor.setValues(1, 0);
		return 1;
	}
	else {
		motor.setValues(0, 5);
		while (stiftDownSens) {		//warten bis Stiftsensor anzeigt, dass Stift oben ist 
			delay(50);
			stiftDownSens = sxIOUnten.getValue();
			Serial.println(stiftDownSens);
			if (millis() - startTime > AbbortChangeStiftStatThreshold) {
				Serial.println("Stift heben wurde abgebrochen");
				motor.setValues(1, 0);
				return 0;
			}
		}
		motor.setValues(1, 0);
		return 1;
	}
}

bool turnDegrees(int degrees, Motor m0, Motor m1) {		//assumed motro placement: m0 left, m1 right
	if (degrees == 0)
		return 1;

	unsigned int timeTurn = TimeTurnStartOffset + abs(degrees) * TimeTurnDegToMS;		//calculate the time needed to turn [input] degrees by multiplication with empiric factor

	if (degrees > 0) {			//if degrees is positive turn counter clockwise
		m0.setValues(0, 4);
		m1.setValues(1, 4);
		delay(timeTurn);
		m0.setValues(1, 0);
		m1.setValues(1, 0);
	}
	else {						//turn clockwise
		m0.setValues(1, 4);
		m1.setValues(0, 4);
		delay(timeTurn);
		m0.setValues(1, 0);
		m1.setValues(1, 0);
	}
}

bool goStraight(unsigned int timeMS, Motor m0, Motor m1, bool direction) {		//drive a straight line for "time[s]" time
	m0.setValues(direction, 5);
	m1.setValues(direction, 5);
	delay(timeMS);
	m0.setValues(direction, 0);
	m1.setValues(direction, 0);
}

void RunBeispielAnwendung(void* args) {
	st_BeispielSHM_e *mSHM = (st_BeispielSHM_e*)args;
	bool initPause = false;
	Motor m0(0), m1(1), m2(2), m3(3);
	DigitalIO_PWMout penRaised(0, INPUT), penDown(1, INPUT), start(2, INPUT), pause(3, INPUT), stop(4, INPUT);

	while (1) {
		if (mSHM->ptrSHMQueue->commonStart)	
		{
			mSHM->state = BEISPIEL_STATE_PAUSE;
		}

		else if (mSHM->state == BEISPIEL_STATE_PAUSE) {		//alle Motoren Stoppen
			if (!initPause) {
				for (char i = 0; i < 4; i++)
					mSHM->mMotors[i].setValues(1, 0);
				initPause = true;
			}
		}

		else if (mSHM->state == BEISPIEL_STATE_STOP) {		//zurück zum Start, dann startbereit machen
			mSHM->step = 0;
			mSHM->state = BEISPIEL_STATE_PAUSE;
		}

		else if (mSHM->state == BEISPIEL_STATE_RUN) {
			initPause = false;
			switch (mSHM->step)
			{
			case 0:		//go to starting pos (raise pen)
				if (!penRaised.getValue()) {
					stiftChangeState(m2, penDown, 0);
				}
				mSHM->step = 1;
				break;
			case 1:		//first line of "M" slanted by 15°
				stiftChangeState(m2, penDown, 1);
				turnDegrees(15, m0, m1);
				goStraight(2500, m0, m1, 1);
				mSHM->step = 2;
				break;
			case 2:			//draw first middle line of "M"
				turnDegrees(-30, m0, m1);
				goStraight(2500, m0, m1, 0);
				mSHM->step = 3;
				break;
			case 3:			//draw second middle line of "M"
				turnDegrees(30, m0, m1);
				goStraight(2500, m0, m1, 1);
				mSHM->step = 4;
				break;
			case 4:			//draw last line of "M"
				turnDegrees(-30, m0, m1);
				goStraight(2500, m0, m1, 0);
				mSHM->step = 5;
				break;
			case 5:
				mSHM->step = 0;
				break;
				//...

			default:
				break;
			}
		}
		vTaskDelay(100000);			//mind. 10 milisek warten bis nächster durchlauf
	}
}



//BeispielAnwendung::BeispielAnwendung()
//{
//	Serial.print("Falscher ctor für BeispielAnswendung genutzt");
//}

BeispielAnwendung::BeispielAnwendung(const SHM *ptrSHMQueueArg)
{
#ifdef DEBUG
	Serial.print("Ctor BeispielAnwendung");
#endif // DEBUG

	mSHM->ptrSHMQueue = ptrSHMQueueArg;
	mSHM->step = 0;
	mSHM->step = BEISPIEL_STATE_PAUSE;

	xTaskCreatePinnedToCore(
		RunBeispielAnwendung,   /* Function to implement the task */
		"BeispielAnwendung", 	/* Name of the task */
		4096,      				/* Stack size in words */
		(void*)mSHM,       	/* Task input parameter */
		1,          			/* Priority of the task */
		NULL,       			/* Task handle. */
		1);  					/* Core where the task should run */
}

BeispielAnwendung::~BeispielAnwendung()
{
#ifdef DEBUG
	Serial.print("Dtor BeispielAnwendung");
#endif // DEBUG

	//vTaskDelete(mTask);
}

void BeispielAnwendung::start()
{
	mSHM->state = BEISPIEL_STATE_RUN;
}

void BeispielAnwendung::pause()
{
	mSHM->state = BEISPIEL_STATE_PAUSE;
}

void BeispielAnwendung::stop()
{
	mSHM->state = BEISPIEL_STATE_STOP;
}

unsigned int BeispielAnwendung::getStep()
{
	return mSHM->step;
}
