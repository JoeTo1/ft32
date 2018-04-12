#ifndef _BEISPIELSANWENDUNG_h
#define _BEISPIELSANWENDUNG_h
// BeispielAnwendung.h
#include "ft_ESP32_IOobjects.h"
#include "ft_ESP32_SHM.h"

#define AbbortChangeStiftStatThreshold 10000	//10 Sek probieren Stift zu heben/senken bis Programmabbruch

#define TurnSpeedOfBeispiel	5			//Configured speed used in the example for turning on the spot
#define StraightSpeedOfBeispiel 7		//Configured speed used in the example for straight line driving
#define TimeToDriveHeightOfText	1000	//Time the construct needs to drive the maximum height at the configured speed
#define TimeTurnStartOffset 100			//Time the construct needs to start turning
#define TimeTurnDegToMS 13				//Time [ms] needed to turn one degree
#define STIFT_DOWN 1
#define STIFT_UP 0

typedef enum : char {	//enum mit atomarem Zugriff für Threadsafety - nach Ende des multitaskings nicht mehr nötig (aber immernoch speicherschonender)
	BEISPIEL_STATE_PAUSE,
	BEISPIEL_STATE_STOP,
	BEISPIEL_STATE_RUN
} e_BeispielState_t;

typedef struct {
	e_BeispielState_t state;
	unsigned char step;
	Motor mMotors[4];
	const SHM *ptrSHMQueue;
} st_BeispielSHM_e;

/*
Steps (for writing MAXI):
0 - Initstep: Go to starting position
1 - Draw First vertical line of M
2 - Draw first slanted line of M
3 - Draw second slanted line of M
4 - Draw second vertical line of M

5 - drive to start of A
6 - rising line of A
7 - falling line of A
(8 - horizontal line of A)

9 - drive to start of X
10 - rising line of X
11 - falling line of X

12 - drive to start of I
13 - draw I line
14 - draw I dot
*/

class BeispielAnwendung {
public:
	//BeispielAnwendung();
	BeispielAnwendung(const SHM *ptrSHMQueueArg);		//benötigt SHM ptr zum SW-Queue SHM um zu überprüfen, ob Cody++ gerade aktiv ist --> Beispielanwendung wird gestoppt
	~BeispielAnwendung();
	void start();
	void pause();
	void stop();
	unsigned int getStep();
	void run();
	bool mTurnDegrees(int degrees);
private:
	//bool mPause;
	//bool mStop;
	TaskHandle_t *mTask;				//task Handle
	Motor mMotorLeft, mMotorRight, mMotorPen;
	DigitalIO_PWMout mPenDown, mStart, mStop, mPause;
	e_BeispielState_t mState;
	unsigned char mStep;
	bool mInitPause;			//used to only init pause-State once (otherwise Motors of other programs could be stopped)
	long int mCycles;
	const SHM *mSHMQueue;
	bool mStiftChangeState(bool wantedState);
	bool mGoStraight(unsigned int timeMS, bool direction);
};


#endif

