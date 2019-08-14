#include <stdlib.h>
#include "screen.h"
#include "hx711/receiver_hx711.h"

extern volatile double max_value_in_experiment;
extern volatile double current_value;
extern volatile uint8_t experiment_state;

extern volatile double tenzoconst_k;
extern volatile double tenzoconst_nom;

extern volatile int32_t tenzo_zero_ofset;


void show_yes_no(char * ret, char param){
	if(param)
		sprintf(ret, "Да");
	else
		sprintf(ret, "Нет");
}

int32_t rab_koef_per = 14651;

void show_received(char * ret){
	sprintf(ret, "%10d", (int)(RECEIVE_DATA[3]));
}

void show_curr(char * ret){

	if (current_value>0.001)
		sprintf(ret, " %2d.%02d", (int)(100*current_value)/100, (int)((int)(100*current_value))%100);
	else
		sprintf(ret, "-%2d.%02d", abs((int)(100*current_value)/100), abs((int)((int)(100*current_value))%100));

//	sprintf(ret, "%2d.%02d", (int)(100*current_value)/100, (int)((int)(100*current_value))%100);
}
void show_voltage(char * ret){

	double n = (((double)RECEIVE_DATA[3]-tenzo_zero_ofset)/0xffffff)*1000.0;
	n /= 128.0;

	if (n>0.001)
		sprintf(ret, " %2d.%02d", (int)(100*n)/100, (int)((int)(100*n))%100);
	else
		sprintf(ret, "-%2d.%02d", abs((int)(100*n)/100), abs((int)((int)(100*n))%100));

//	sprintf(ret, "%2d.%02d", (int)(100*current_value)/100, (int)((int)(100*current_value))%100);
}
void show_max(char * ret){

	if (max_value_in_experiment>0.001)
		sprintf(ret, " %2d.%02d", (int)(100*max_value_in_experiment)/100, (int)((int)(100*max_value_in_experiment))%100);
	else
		sprintf(ret, "-%2d.%02d", abs((int)(100*max_value_in_experiment)/100), abs((int)((int)(100*max_value_in_experiment))%100));

//	sprintf(ret, "%2d.%02d", (int)(100*max_value_in_experiment)/100, (int)((int)(100*max_value_in_experiment))%100);
}

void show_experiment_state(char * ret){
	if (experiment_state){
		sprintf(ret, "run");
	}else{
		sprintf(ret, "stop");
	}
}

void show_tenzoconst_k(char * ret){
	sprintf(ret, "%1d.%04d", (int)(tenzoconst_k*10)/10, (int)(tenzoconst_k*10000)%10000);
}
void inc_tenzoconst_k(void){
	tenzoconst_k += 0.0001;
}
void dec_tenzoconst_k(void){
	tenzoconst_k -= 0.0001;
}

void show_tenzoconst_nom(char * ret){
	sprintf(ret, "%4d", (int)tenzoconst_nom);
}
void inc_tenzoconst_nom(void){
	tenzoconst_nom += 1;
}
void dec_tenzoconst_nom(void){
	tenzoconst_nom -= 1;
}




void show_tenzo_zero(char * ret){
	double k = tenzoconst_k;
	double f = tenzoconst_nom;
	double n = (((double)tenzo_zero_ofset)/0xffffff)*1000.0;
	n /= 128.0;
	double	current_value__ = (n*f/k);
	current_value__ = 4.9348 * current_value__ + 32.651;

//	sprintf(ret, "%2d.%02d", (int)(100*current_value__)/100, (int)((int)(100*current_value__))%100);
	sprintf(ret, "%7d", (int)(tenzo_zero_ofset));
//	sprintf(ret, "%2d.%02d", (int)(100*current_value__)/100, (int)((int)(100*current_value__))%100);

}

void set_tenzo_zero(void){
	tenzo_zero_ofset = RECEIVE_DATA[3];
}

uint16_t PWM_Speed_Backup = 0;

void inc_freq(void){
	PWM_Speed_Backup += 1; 

}
void dec_freq(void){
	PWM_Speed_Backup -= 1;  if(PWM_Speed_Backup<1) { PWM_Speed_Backup = 1; }

}
void show_freq(char * ret){
	sprintf(ret, "%2d.%1d", PWM_Speed_Backup/10, PWM_Speed_Backup%10);
}



/*   <----KEY CALLBACKS---->   */

void blueButtonClickHandler(void){
	screenSM(SCR_COMM_up);
}

void yellowButtonClickHandler(void){
	screenSM(SCR_COMM_down);
}



void redButtonClickHandler(void){
	userExperimentClick();
}
void greenButtonClickHandler(void){
	screenSM(SCR_COMM_select);
}

void blueButtonPressHandler(void){
	screenSM(SCR_COMM_up);
}

void yellowButtonPressHandler(void){
	screenSM(SCR_COMM_down);
}

void redButtonPressHandler(void){
	redButtonClickHandler();
}

void greenButtonPressHandler(void){
	screenSM(SCR_COMM_select);
}


void yellowBlueButtonClickHandler(void){

}

void redYellowButtonClickHandler(void){

}

void redBlueButtonClickHandler(void){ //красная и синяя делает запись

}

void threeButtonClickHandler(void){

}

