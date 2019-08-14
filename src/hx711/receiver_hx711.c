#include "stm32f10x.h"
#include "receiver_hx711.h"


volatile int32_t RECEIVE_DATA[4] = {0};
volatile uint8_t update_flag = 0;

typedef enum {
	RECEIVING,
	WAITING
} RECEIVE_STATE_ENUM;


void receiveHX711Value(void){
	static uint8_t bit_numb = 0;
	static uint32_t reveive_data[4] = {0};
	static RECEIVE_STATE_ENUM receive_state = WAITING;
	
	if(receive_state == WAITING){
		if(
//			!(GPIOA->IDR & GPIO_IDR_IDR0)  &&
//			!(GPIOA->IDR & GPIO_IDR_IDR1) &&
//			!(GPIOA->IDR & GPIO_IDR_IDR2) &&
			!(GPIOA->IDR & GPIO_IDR_IDR3)
			){
			receive_state = RECEIVING;
			TIM2->ARR = 19; //досчитывать до 20
			TIM2->PSC = 16*4 - 1; //делит на 16*4, получаетс€, что считает с периодом 1мкс
			TIM2->EGR |= TIM_EGR_UG; //на вс€кий случай
		}
	}else if(receive_state == RECEIVING){
		if(bit_numb < 50){//25
//		if(bit_numb < 52){//26
//		if(bit_numb < 54){//27

			
			if(bit_numb & 1){
				GPIOA->BSRR |= GPIO_BSRR_BS4;
			}else{
				GPIOA->BSRR |= GPIO_BSRR_BR4;
				if(bit_numb<=48){
					reveive_data[0] <<=1;
					reveive_data[1] <<=1;
					reveive_data[2] <<=1;
					reveive_data[3] <<=1;
					if(GPIOA->IDR & GPIO_IDR_IDR0)
						reveive_data[0]++;
					if(GPIOA->IDR & GPIO_IDR_IDR1)
						reveive_data[1]++;
					if(GPIOA->IDR & GPIO_IDR_IDR2)
						reveive_data[2]++;
					if(GPIOA->IDR & GPIO_IDR_IDR3)
						reveive_data[3]++;
				}
			}
			bit_numb++;
		}else{
			receive_state = WAITING;
			bit_numb = 0;
//			RECEIVE_DATA = (8388607 - (reveive_data ^ 0x800000));
			RECEIVE_DATA[0] = (reveive_data[0] ^ 0x800000);
			RECEIVE_DATA[1] = (reveive_data[1] ^ 0x800000);
			RECEIVE_DATA[2] = (reveive_data[2] ^ 0x800000);
			RECEIVE_DATA[3] = (reveive_data[3] ^ 0x800000) - 0x7fffff;
			
//			DMA1_Channel4->CCR &= ~DMA_CCR1_EN ;
//			DMA1_Channel4->CNDTR = 4*4 ; // 4 times by 4 byte
//			DMA1_Channel4->CCR |= DMA_CCR1_EN ;
			
			update_flag = 1;

			reveive_data[0] = 0;
			reveive_data[1] = 0;
			reveive_data[2] = 0;
			reveive_data[3] = 0;
			GPIOA->BSRR |= GPIO_BSRR_BR4;
			
			TIM2->ARR = 100; //100
			TIM2->PSC = 1000*4 - 1; //16 kHz
			TIM2->EGR |= TIM_EGR_UG; //на вс€кий случай
		}
	}
	TIM2->CR1 |= TIM_CR1_CEN;
}

uint8_t isReceived(void){
	if (update_flag){
		update_flag = 0;
		return 1;
	}
	return 0;
}

void TIM2_IRQHandler(){
	TIM2->SR &= ~TIM_SR_UIF;
	receiveHX711Value();
}

void initHX711drv(void){
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	GPIOA->CRL	&= ~(GPIO_CRL_CNF4 | GPIO_CRL_MODE4);
	GPIOA->CRL |= GPIO_CRL_MODE4_1 | GPIO_CRL_MODE4_0; //PP , MODE4 0b11 Output mode max speed 50 MHz
	
//	GPIOA->ODR |= GPIO_ODR_ODR4;
	
//	GPIOA->CRL |= GPIO_CRL_CNF0_0; //input floating
//	GPIOA->CRL |= GPIO_CRL_CNF1_0; //input floating
//	GPIOA->CRL |= GPIO_CRL_CNF2_0; //input floating
	GPIOA->CRL |= GPIO_CRL_CNF3_0; //input floating
	
	
	
	
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	
	
	//TIM6->ARR = 19; //досчитывать до 20
	//TIM6->PSC = 15; //делит на 16, получаетс€, что считает с периодом 1мкс
	TIM2->ARR = 99; //100
	TIM2->PSC = 1000*4 - 1; //16 kHz
//	TIM6->CR1 |= TIM_CR1_ARPE; //включить буфер arr, записывать после событи€

	TIM2->CR1 |= TIM_CR1_OPM; //one puls mode
	TIM2->CR1 |= TIM_CR1_URS; //событие (прерывание) только при обновлении счетчика
	
	TIM2->DIER |= TIM_DIER_UIE; //включаем прерывание

//	TIM6->CR1 |= TIM_CR1_CEN; //запуск счета
		
		

	NVIC_EnableIRQ(TIM2_IRQn); /* (1) */
	NVIC_SetPriority(TIM2_IRQn,0);
	
	receiveHX711Value();
	
}
