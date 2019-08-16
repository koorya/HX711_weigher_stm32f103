
#include <stddef.h>
#include <stdlib.h>
#include "stm32f10x.h"
#include "./lcd_menu/keyboard.h"
#include "./lcd_menu/screen.h"
#include "gldc/GLCD.h"
#include "hx711/receiver_hx711.h"
#include "usart/usart.h"


USART_InitTypeDef USART_InitStructure;
SPI_InitTypeDef SPI_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;
DMA_InitTypeDef DMA_InitStructure;


volatile double max_value_in_experiment = 0;
volatile double current_value = 0;
volatile uint8_t experiment_state = 0;

volatile double tenzoconst_k = 2.0286;
volatile double tenzoconst_nom = 100.0;
volatile int32_t tenzo_zero_ofset = 29526;

void userExperimentClick(void){
	if(experiment_state == 1){
		experiment_state = 0;
	}else{
		experiment_state = 1;
		max_value_in_experiment = 0;
	}
}

void maintaskWork(void){
	if (isReceived()){


		double k = tenzoconst_k;
		double f = tenzoconst_nom;
		uint32_t set_point = 1.0;

		double n = (((double)RECEIVE_DATA[3]-tenzo_zero_ofset)/0xffffff)*1000.0;
		n /= 128.0;


		current_value = (n*f/k);
		current_value = 5.104 * current_value + 31.685; //фомич посчитал

		if(experiment_state == 1){

			char usart_send_str[128];
			if (current_value>0)
				sprintf(usart_send_str, "current value: %2d.%02d\r\n", (int)(100*current_value)/100, (int)((int)(100*current_value))%100);
			else
				sprintf(usart_send_str, "current value:-%2d.%02d\r\n", abs((int)(100*current_value)/100), abs((int)((int)(100*current_value))%100));

			sendStringByUSART(usart_send_str);

			if(current_value > max_value_in_experiment){
				max_value_in_experiment = current_value;
				set_point = max_value_in_experiment * 0.7; // 70%
			}else if(max_value_in_experiment - current_value > set_point){
				experiment_state = 0;
			}
		}
	}
}

int main(void){

   	RCC_HCLKConfig( RCC_SYSCLK_Div2);
	RCC_PCLK2Config(RCC_HCLK_Div1);
	RCC_PCLK1Config(RCC_HCLK_Div1);

	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);



	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);




	initUsart();



	GLCD_Init();
	menuInit();

	screenSM(SCR_COMM_down);
	initKeyboard();

	usart3_init_keyboard();

	initHX711drv();

	uint16_t i = 1;
	while (1){
		updateScreen();
		i++;

	}

}


