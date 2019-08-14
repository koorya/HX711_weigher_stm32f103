#include "stm32f10x.h"

uint16_t * data_pointers_array[42] = {0};



void bkpWork(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	PWR_BackupAccessCmd(ENABLE);
	BKP_WriteBackupRegister(BKP_DR1, 0xff);
	PWR_BackupAccessCmd(DISABLE);

}
