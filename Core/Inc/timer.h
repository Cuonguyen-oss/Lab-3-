
#ifndef INC_TIMER_H_
#define INC_TIMER_H_
#include "global.h"

#define CYCLE 	20
#define SEC 	1000
#define SCAN_MS 20
#define DONE_MS 1000
#define BLINK_MS 500

extern int flag1,
		   flag2,
		   flag3,
		   flag4,
		   flag5; 


void set1(int);
void set2(int);
void set3(int);
void set4(int);
void set5(int); 
void resetTimer(int);

void timerRun();


#endif /* INC_TIMER_H_ */
