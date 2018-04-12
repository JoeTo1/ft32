// 
// 
// 

#include "BeispielAnwendung.h"

bool BeispielAnwendung::mStiftChangeState(bool wantedState) {
	unsigned long startTime = millis();
	bool stiftDownSens = mPenDown.getValue();
	if (wantedState) {			//true -> Stift auf Papier drücken
		mMotorPen.setValues(1, 5);
		while (!stiftDownSens) {		//warten bis Stiftsensor anzeigt, dass Stift gesenkt wurde
			delay(50);
			stiftDownSens = mPenDown.getValue();
			if (millis() - startTime > AbbortChangeStiftStatThreshold) {
				Serial.println("Stift senken wurde abgebrochen");
				mMotorPen.setValues(1, 0);
				return 0;
			}
		}
		delay(100);				//damit Stift sicher am Boden ist
		mMotorPen.setValues(1, 0);
		return 1;
	}
	else {
		mMotorPen.setValues(0, 5);
		while (stiftDownSens) {		//warten bis Stiftsensor anzeigt, dass Stift oben ist 
			delay(50);
			stiftDownSens = mPenDown.getValue();
			if (millis() - startTime > AbbortChangeStiftStatThreshold) {
				Serial.println("Stift heben wurde abgebrochen");
				mMotorPen.setValues(1, 0);
				return 0;
			}
		}
		mMotorPen.setValues(1, 0);
		return 1;
	}
}

bool BeispielAnwendung::mTurnDegrees(int degrees) {		//assumed motro placement: m0 left, m1 right
	if (degrees == 0) {
		return 1;
	}

	unsigned int timeTurn = TimeTurnStartOffset + abs(degrees) * TimeTurnDegToMS;		//calculate the time needed to turn [input] degrees by multiplication with empiric factor

	bool stiftChanged = false;
	if (mPenDown.getValue())
	{
		stiftChanged = mStiftChangeState(STIFT_UP);	
	}

	if (degrees > 0) {			//if degrees is positive turn counter clockwise
		timeTurn += timeTurn * 0.3;			//empiric value because it turns positive slower than negative
		mMotorLeft.setValues(0, TurnSpeedOfBeispiel);
		mMotorRight.setValues(1, TurnSpeedOfBeispiel);
		delay(timeTurn);
		mMotorLeft.setValues(1, 0);
		mMotorRight.setValues(1, 0);
	}
	else {						//turn clockwise
		mMotorLeft.setValues(1, TurnSpeedOfBeispiel);
		mMotorRight.setValues(0, TurnSpeedOfBeispiel);
		delay(timeTurn);
		mMotorLeft.setValues(1, 0);
		mMotorRight.setValues(1, 0);
	}

	if (stiftChanged) {
		mStiftChangeState(STIFT_DOWN);
	}

	return 1;
}

bool BeispielAnwendung::mGoStraight(unsigned int timeMS, bool direction) {		//drive a straight line for "time[s]" time
	if (direction) {
		mMotorLeft.setValues(1, StraightSpeedOfBeispiel);
		mMotorRight.setValues(1, StraightSpeedOfBeispiel);
		delay(timeMS);
		mMotorLeft.setValues(1, 0);
		mMotorRight.setValues(1, 0);
		return 1;
	}
	else {
		timeMS += timeMS * 0.1;		//empiric value, because it goeas backwards slower than forwards
		mMotorLeft.setValues(0, StraightSpeedOfBeispiel);
		mMotorRight.setValues(0, StraightSpeedOfBeispiel);
		delay(timeMS);
		mMotorLeft.setValues(1, 0);
		mMotorRight.setValues(1, 0);
		return 1;
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

	mSHMQueue = ptrSHMQueueArg;

	mStep = 0;
	mState = BEISPIEL_STATE_PAUSE;

	mMotorLeft = Motor(0);
	mMotorRight = Motor(1);
	mMotorPen = Motor(2);
	mPenDown = DigitalIO_PWMout(0, INPUT);
	mStart = DigitalIO_PWMout(1, INPUT);
	mPause = DigitalIO_PWMout(2, INPUT);
	mStop = DigitalIO_PWMout(3, INPUT);

	mCycles = 0;
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
	mState = BEISPIEL_STATE_RUN;
}

void BeispielAnwendung::pause()
{
	mState = BEISPIEL_STATE_PAUSE;
}

void BeispielAnwendung::stop()
{
	mState = BEISPIEL_STATE_STOP;
}

unsigned int BeispielAnwendung::getStep()
{
	return mStep;
}

void BeispielAnwendung::run()
{

	if (mState == BEISPIEL_STATE_PAUSE) {		//alle Motoren Stoppen
		if (!mInitPause) {
			mMotorLeft.setValues(1, 0);
			mStiftChangeState(STIFT_UP);
			mMotorPen.setValues(1, 0);
			mMotorRight.setValues(1, 0);
			mInitPause = true;
		}
	}

	else if (mState == BEISPIEL_STATE_STOP) {		//zurück zum Start, dann startbereit machen
		mStep = 0;
		mState = BEISPIEL_STATE_PAUSE;
	}

	else if (mSHMQueue->commonStart)
	{
		mState = BEISPIEL_STATE_PAUSE;
		return;
	}

	else if (mState == BEISPIEL_STATE_RUN) {
		mCycles++;
		mInitPause = false;
		switch (mStep)
		{
		case 0:		//go to starting pos (raise pen)
			if (mPenDown.getValue()) {
				mStiftChangeState(STIFT_UP);
			}
			mStep = 1;
			break;
		case 1:		//first line of "M" slanted by 15°
			mTurnDegrees(-15);
			mStiftChangeState(1);
			mGoStraight(2500, 1);
			mStep = 2;
			break;
		case 2:			//draw first middle line of "M"
			mTurnDegrees(30);
			mGoStraight(2500, 0);
			mStep = 3;
			break;
		case 3:			//draw second middle line of "M"
			mTurnDegrees(-30);
			mGoStraight(2500, 1);
			mStep = 4;
			break;
		case 4:			//draw last line of "M"
			mTurnDegrees(30);
			mGoStraight(2500,  0);
			mStep = 5;
			break;
		case 5:
			//mStep = 0;
			mState = BEISPIEL_STATE_PAUSE;
			break;
			//...

		default:
			break;
		}
	}

	//Ausgabe zur Debughilfe usw.
	
	Serial.print(millis());
	Serial.print(" - Zyklus ");
	Serial.print(mCycles);
	Serial.println(" durchlaufen.");
	vTaskDelay(100);			//mind. 10 milisek warten bis nächster durchlauf
}
