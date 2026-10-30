

#include "button.h"

int buttonFlag[NO_BUTTON] = {0, 0, 0};

int timeout[NO_BUTTON] = {100, 100, 100};

int KeyReg0[NO_BUTTON] = {RELEASED, RELEASED, RELEASED};
int KeyReg1[NO_BUTTON] = {RELEASED, RELEASED, RELEASED};
int KeyReg2[NO_BUTTON] = {RELEASED, RELEASED, RELEASED};
int KeyReg3[NO_BUTTON] = {RELEASED, RELEASED, RELEASED};

int isButtonNoPressed(int no){
	if (buttonFlag[no] == 1){
		buttonFlag[no] = 0;
		return 1;
	}
	return 0;
}

void buttonRead(){
	for (int i = 0; i < NO_BUTTON; i++){
		KeyReg2[i] = KeyReg1[i];
		KeyReg1[i] = KeyReg0[i];
		KeyReg0[i] = HAL_GPIO_ReadPin(sw0_GPIO_Port, sw0_Pin << i);
		if ((KeyReg1[i] == KeyReg0[i]) && (KeyReg1[i] == KeyReg2[i])){
			if (KeyReg2[i] != KeyReg3[i]){ 
				KeyReg3[i] = KeyReg2[i];
				if (KeyReg0[i] == PRESSED){
					timeout[i] = 100;
					buttonFlag[i] = 1;
				}
			}
			else { 
				timeout[i]--;
				if (timeout[i] == 0){
					timeout[i] = 10;
					if (KeyReg3[i] == PRESSED){
						buttonFlag[i] = 1;
					}
				}
			}
		}
	}
}
