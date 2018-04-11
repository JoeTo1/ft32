#ifndef _BEISPIELSANWENDUNG_h
#define _BEISPIELSANWENDUNG_h
// BeispielAnwendung.h
#include "ft_ESP32_IOobjects.h"
#include "ft_ESP32_SHM.h"

#define AbbortChangeStiftStatThreshold 10000	//10 Sek probieren Stift zu heben/senken bis Programmabbruch

#define TimeTurnStartOffset 100		//Time the construct needs to start turning
#define TimeTurnDegToMS 13			//Time [ms] needed to turn one degree
#define STIFT_DOWN 1
#define STIFT_UP 0

bool turnDegrees(int degrees, Motor m0, Motor m1);
bool stiftChangeState(Motor motor, DigitalIO_PWMout sxIOUnten, bool wantedState);

typedef enum : char {	//enum mit atomarem Zugriff für Threadsafety
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
	//void run();
private:
	//bool mPause;
	//bool mStop;
	st_BeispielSHM_e *mSHM;		//SHM mit Run-Task
	TaskHandle_t *mTask;				//task Handle
};


#endif

