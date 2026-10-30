

#include "global.h"

int autoState_H = INIT;
int autoState_V = INIT;
int manState = IDLE;

int redDur = 5;
int yelDur = 2;
int grnDur = 3;

int tempRed = 1;
int tempYel = 1;
int tempGrn = 1;

int horCount = 0;
int verCount = 0;
int scan = 0;

void segmentUpdateAuto(){
	updateSegment2Digits(horCount, verCount);
}

void fsm_run(){
	fsm_auto_hor();
	fsm_auto_ver();
	fsm_man();
}

void button0Signal(){
	if (isButtonNoPressed(0) == 1){
		resetTimer(-1);
		horCount = 0, verCount = 0;
		updateSegment(IDLE, IDLE, IDLE, IDLE);
		scan = 0;
		setLedH(IDLE);
		setLedV(IDLE);
		autoState_H = IDLE;
		autoState_V = IDLE;
		manState = MAN_RED;
		tempRed = redDur;
		tempYel = yelDur;
		tempGrn = grnDur;
		set1(100);
		set3(100);
		return;
	}
}

/* Cap nhat bo dem hien thi + quet LED dung chung cho 3 trang thai ngang */
static void autoHorCommon(){
	if (flag2 == 1){
		horCount--;
		set2(SEC);
	}
	if (flag3 == 1){
		segmentUpdateAuto();
		scan = (scan == 1) ? 0 : 1;
		scan7Seg(scan);
		set3(SCAN_MS);
	}
}

void fsm_auto_hor(){
	switch(autoState_H){
	case INIT:
		set1(redDur * SEC); //timer1 is for the hor_state
		set2(SEC);          //counter 1s
		set3(SCAN_MS);      //scanning LED
		horCount = redDur;
		autoState_H = RED;
		break;
	case RED:
		setLedH(RED);
		autoHorCommon();
		if (flag1 == 1){
			set1(grnDur * SEC);
			horCount = grnDur;
			autoState_H = GRN;
		}
		button0Signal();
		break;
	case GRN:
		setLedH(GRN);
		autoHorCommon();
		if (flag1 == 1){
			set1(yelDur * SEC);
			horCount = yelDur;
			autoState_H = YEL;
		}
		button0Signal();
		break;
	case YEL:
		setLedH(YEL);
		autoHorCommon();
		if (flag1 == 1){
			set1(redDur * SEC);
			horCount = redDur;
			autoState_H = RED;
		}
		button0Signal();
		break;
	default: //IDLE
		break;
	}
}

void fsm_auto_ver(){
    switch(autoState_V){
    case INIT: 
        set4(grnDur * SEC); //timer4 is for the ver_state
        set5(SEC);
        verCount = grnDur;
        autoState_V = GRN;
        break;
    case GRN:
    	setLedV(GRN);
    	if (flag5 == 1){
    		verCount--;
    		set5(SEC);
    	}
    	if (flag4 == 1){
    		set4(yelDur * SEC);
    		verCount = yelDur;
    		autoState_V = YEL;
    	}
    	break;
    case YEL:
    	setLedV(YEL);
    	if (flag5 == 1){
    		verCount--;
    		set5(SEC);
    	}
    	if (flag4 == 1){
    		set4(redDur * SEC);
    		verCount = redDur;
    		autoState_V = RED;
    	}
    	break;
    case RED: 
    	setLedV(RED);
    	if (flag5 == 1){
    		verCount--;
    		set5(SEC);
    	}
    	if (flag4 == 1){
    		set4(grnDur * SEC);
    		verCount = grnDur;
    		autoState_V = GRN;
    	}
    	break;
    default: break;
    }
}


/* Rang buoc: redDur = grnDur + yelDur (den do = xanh + vang), moi gia tri 1..99.
 * Khi luu 1 den thi 2 den con lai duoc cap nhat theo de giu rang buoc. */
static void saveRed(int v){
	if (v <= yelDur) v = yelDur + 1;   // xanh toi thieu 1
	if (v > 99) v = 99;
	redDur = v;
	grnDur = redDur - yelDur;          // do tang n => xanh tang n
	tempRed = redDur;
	tempGrn = grnDur;
}

static void saveGrn(int v){
	if (v + yelDur > 99) v = 99 - yelDur;
	grnDur = v;
	redDur = grnDur + yelDur;          // xanh tang n => do tang n
	tempGrn = grnDur;
	tempRed = redDur;
}

static void saveYel(int v){
	if (v + grnDur > 99) v = 99 - grnDur;
	yelDur = v;
	redDur = grnDur + yelDur;          // vang doi => do doi theo, xanh giu nguyen
	tempYel = yelDur;
	tempRed = redDur;
}

void fsm_man(){
	switch(manState){
	case MAN_RED:
		updateSegment2Digits(tempRed, 22);
		if (isButtonNoPressed(0) == 1){
			//switch to man yellow and discard the change
			tempRed = redDur;
			tempYel = yelDur;
			manState = MAN_YEL;
			setLedV(IDLE);
			setLedH(IDLE);
			set1(100);
			set3(100);
		}

		if (isButtonNoPressed(1) == 1){
			tempRed = (tempRed == 99) ? 1 : tempRed + 1;
		}
		if (isButtonNoPressed(2) == 1){
			saveRed(tempRed); //save
		}
		if (flag1 == 1){ //flickering the LEDs light
			HAL_GPIO_TogglePin(red_h_GPIO_Port, red_h_Pin);
			HAL_GPIO_TogglePin(red_v_GPIO_Port, red_v_Pin);
			set1(BLINK_MS);
		}
		if (flag3 == 1){
			scan = (scan + 1)%2;
			scan7Seg(scan);
			set3(SCAN_MS);
		}
		break;
	case MAN_YEL:
		updateSegment2Digits(tempYel, 33);
		if (isButtonNoPressed(0) == 1){
			//switch to man grn and discard the change
			tempYel = yelDur;
			tempGrn = grnDur;
			manState = MAN_GRN;
			setLedV(IDLE);
			setLedH(IDLE);
			set1(100);
			set3(100);
		}

		if (isButtonNoPressed(1) == 1){
			tempYel = (tempYel == 99) ? 1 : tempYel + 1;
		}
		if (isButtonNoPressed(2) == 1){
			saveYel(tempYel); //save
		}
		if (flag1 == 1){ //flickering the LEDs light
			HAL_GPIO_TogglePin(yel_h_GPIO_Port, yel_h_Pin);
			HAL_GPIO_TogglePin(yel_v_GPIO_Port, yel_v_Pin);
			set1(BLINK_MS);
		}
		if (flag3 == 1){
			scan = (scan + 1)%2;
			scan7Seg(scan);
			set3(SCAN_MS);
		}
		break;
	case MAN_GRN:
		updateSegment2Digits(tempGrn, 44);
		if (isButtonNoPressed(0) == 1){

			//rang buoc red = grn + yel da duoc dam bao luc luu (saveRed/saveGrn/saveYel)
			tempGrn = grnDur;
			  setLedV(IDLE);
			  setLedH(IDLE);
			  resetTimer(NONE);
			  manState = IDLE;
			  autoState_H = INIT;
			  autoState_V = INIT;
			  return;
		}

		if (isButtonNoPressed(1) == 1){
			tempGrn = (tempGrn == 99) ? 1 : tempGrn + 1;
		}
		if (isButtonNoPressed(2) == 1){
			saveGrn(tempGrn); //save
		}
		if (flag1 == 1){ //flickering the LEDs light
			HAL_GPIO_TogglePin(grn_h_GPIO_Port, grn_h_Pin);
			HAL_GPIO_TogglePin(grn_v_GPIO_Port, grn_v_Pin);
			set1(BLINK_MS);
		}
		if (flag3 == 1){
			scan = (scan + 1)%2;
			scan7Seg(scan);
			set3(SCAN_MS);
		}
		break;
	case MAN_DONE:
		isButtonNoPressed(0);
		isButtonNoPressed(1);
		isButtonNoPressed(2);
		if (flag1 == 1){
			setLedV(IDLE);
			setLedH(IDLE);
			resetTimer(NONE); //reset all timer
			manState = IDLE;
			autoState_H = INIT;
			autoState_V = INIT;
		}
		break;
	default:
		break;
	}
}
