#include "stm32f10x.h"
#include "keyboard.h"
#include "screen.h"


#define T_CLICK ((1*SYSTICKINTMS)/1000+1)
#define T_PRESS ((30*SYSTICKINTMS)/1000+1)
#define T_PRESS_INT ((32*SYSTICKINTMS)/1000+1)

#define RED_BUTTON_MASK (0x1<<2)
#define GREEN_BUTTON_MASK (0x1<<3)
#define BLUE_BUTTON_MASK (0x1<<0)
#define YELLOW_BUTTON_MASK (0x1<<1)


typedef enum {
	NO_PRESS,
	CLICK,
	PRESS
} STATE_TYPE;

typedef enum {
	EVENT_CLICK_SET_OF_BUTTONS,
	EVENT_PRESS_BUTTON,
	EVENT_CLICK_BUTTON,
	NO_EVENT
} EVENT_TYPE;

typedef struct {
	STATE_TYPE state;
	uint32_t t;
} BUTTON_SM;

volatile char keys_state_now, keys_state_backward; 
volatile BUTTON_SM red_b_sm, yellow_b_sm, green_b_sm, blue_b_sm;


// Задержки в мкс
void delay_us_KEYBOARD(uint32_t tick)
{ 
	uint32_t wait = 0;
	
	while(tick) 
 {
  __ASM("NOP"); 
	wait +=1; 
  tick--;
 } 
}

void initKeyboard(void){
	keys_state_now = 0;
	keys_state_backward = 0;
	red_b_sm.state = yellow_b_sm.state = green_b_sm.state = blue_b_sm.state = NO_PRESS;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8 | GPIO_Pin_7 | GPIO_Pin_6;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

}



char scanKey(char * ret_value){
//	static char bt_state_high, bt_state_low, sm = 0;
	/*
	if(sm == 0){
		GPIOC->BSRR = GPIO_BSRR_BR11 | GPIO_BSRR_BS12;
	}else if(sm == 2){
		bt_state_low = ((GPIOB->IDR >> 14) & 0x03);
		GPIOC->BSRR = GPIO_BSRR_BS11 | GPIO_BSRR_BR12;
	}else if(sm == 4){		
		bt_state_high = ((GPIOB->IDR >> 14) & 0x03)<<2;
		GPIOC->BSRR = GPIO_BSRR_BR11 | GPIO_BSRR_BS12;
		*ret_value = bt_state_high | bt_state_low;
		sm = 0;
		return 1;
	}
	*/
//	sm++;
	*ret_value = 0x00;
	*ret_value |= (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6)) <<0;
	*ret_value |= (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)) <<1;
	*ret_value |= (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8)) <<2;
	*ret_value |= (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9)) <<3;

	return 1;
//	return 0;
}

EVENT_TYPE stMachineWork(volatile BUTTON_SM *button, char key_value, char key_mask, volatile char *keys_state_backward, volatile  char *keys_state_now){
	if(button->state == NO_PRESS){
		if(key_value){
			if(button->t++ > T_CLICK){
				button->state = CLICK;
				*keys_state_backward |= *keys_state_now |= key_mask;
			}
		}else{
			button->t = 0;
		}
	}else if(button->state == CLICK){
		if(key_value){
			if(button->t++ > T_PRESS)
				button->state = PRESS;
		}else{
			button->state = NO_PRESS;
			*keys_state_now &= ~key_mask;
			if(*keys_state_backward ^ key_mask){					//если были нажаты какие-то еще кнопки
				if(!(*keys_state_now))
					return EVENT_CLICK_SET_OF_BUTTONS;
			}else{
				*keys_state_backward = 0;
				return EVENT_CLICK_BUTTON;
			}
		}
	}else if(button->state == PRESS){
		if(key_value){
			if(button->t++ > T_PRESS_INT){
				button->t = T_PRESS;
				if(!(*keys_state_backward ^ key_mask)){
					return EVENT_PRESS_BUTTON;
				}
			}
		}else{
			button->state = NO_PRESS;
			*keys_state_now &= ~key_mask;
			if(*keys_state_backward ^ key_mask){					//если были нажаты какие-то еще кнопки
				if(!(*keys_state_now))
					return EVENT_CLICK_SET_OF_BUTTONS;
			}else{
				*keys_state_backward = 0;
			}
		}
	}
	return NO_EVENT;
}


void clickSetOfButtons(volatile char *keys_state_backward){
	if(*keys_state_backward == (RED_BUTTON_MASK | GREEN_BUTTON_MASK)){
		redGreenButtonClickHandler();
	}else if(*keys_state_backward == (RED_BUTTON_MASK | YELLOW_BUTTON_MASK)){
		redYellowButtonClickHandler();
	}else if(*keys_state_backward == (RED_BUTTON_MASK | BLUE_BUTTON_MASK)){
		redBlueButtonClickHandler();
	}else if(*keys_state_backward == (YELLOW_BUTTON_MASK | GREEN_BUTTON_MASK)){
		yellowGreenButtonClickHandler();
	}else if(*keys_state_backward == (YELLOW_BUTTON_MASK | BLUE_BUTTON_MASK)){
		yellowBlueButtonClickHandler();
	}else if(*keys_state_backward == (GREEN_BUTTON_MASK | BLUE_BUTTON_MASK)){
		greenBlueButtonClickHandler();
	}else if(*keys_state_backward == (YELLOW_BUTTON_MASK | GREEN_BUTTON_MASK | RED_BUTTON_MASK)){
		threeButtonClickHandler();
	}else if(*keys_state_backward == (YELLOW_BUTTON_MASK | BLUE_BUTTON_MASK | RED_BUTTON_MASK)){
		threeButtonClickHandler();
	}else if(*keys_state_backward == (GREEN_BUTTON_MASK | BLUE_BUTTON_MASK | RED_BUTTON_MASK)){
		threeButtonClickHandler();
	}else if(*keys_state_backward == (YELLOW_BUTTON_MASK | GREEN_BUTTON_MASK | BLUE_BUTTON_MASK | RED_BUTTON_MASK)){
		threeButtonClickHandler();
	}
}

void keyboadrWork(void){

	char b_st;
	if(!scanKey(&b_st))
		return;

	EVENT_TYPE event = NO_EVENT;
	event = stMachineWork(&red_b_sm, b_st & RED_BUTTON_MASK, RED_BUTTON_MASK, &keys_state_backward, &keys_state_now);
	if(event == EVENT_CLICK_BUTTON){
		redButtonClickHandler();
	}else if(event == EVENT_PRESS_BUTTON){
		redButtonPressHandler();
	}else if(event == EVENT_CLICK_SET_OF_BUTTONS){
		clickSetOfButtons(&keys_state_backward);
	}
	
	event = stMachineWork(&green_b_sm, b_st & GREEN_BUTTON_MASK, GREEN_BUTTON_MASK, &keys_state_backward, &keys_state_now);
	if(event == EVENT_CLICK_BUTTON){
		greenButtonClickHandler();
	}else if(event == EVENT_PRESS_BUTTON){
		greenButtonPressHandler();
	}else if(event == EVENT_CLICK_SET_OF_BUTTONS){
		clickSetOfButtons(&keys_state_backward);		
	}

	event = stMachineWork(&yellow_b_sm, b_st & YELLOW_BUTTON_MASK, YELLOW_BUTTON_MASK, &keys_state_backward, &keys_state_now);
	if(event == EVENT_CLICK_BUTTON){
		yellowButtonClickHandler();
	}else if(event == EVENT_PRESS_BUTTON){
		yellowButtonPressHandler();
	}else if(event == EVENT_CLICK_SET_OF_BUTTONS){
		clickSetOfButtons(&keys_state_backward);
	}

	event = stMachineWork(&blue_b_sm, b_st & BLUE_BUTTON_MASK, BLUE_BUTTON_MASK, &keys_state_backward, &keys_state_now);
	if(event == EVENT_CLICK_BUTTON){
		blueButtonClickHandler();
	}else if(event == EVENT_PRESS_BUTTON){
		blueButtonPressHandler();
	}else if(event == EVENT_CLICK_SET_OF_BUTTONS){
		clickSetOfButtons(&keys_state_backward);
	}	
	
	
	if(!keys_state_now) //если все кнопки отпущены, то сбросить состояние нажатых кнопок
		keys_state_backward = 0;
}

void usart3_init_keyboard(void)
{

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);


//	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN; // PB10-Tx PB11-Rx (gpioa к шине тактирования, альтернативные функции к шине тактирования)
//	RCC->APB1ENR |= RCC_APB1ENR_USART3EN; //USART3 - PB10-Tx PB11-Rx , тактирование от APB1 (36MHz для этого проекта)
/*
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
*/
	//Tx PB10 (no remap, usart2)
	GPIOB->CRH	&= ~GPIO_CRH_CNF10 & ~GPIO_CRH_MODE10;
	GPIOB->CRH |= GPIO_CRH_CNF10_1 | GPIO_CRH_MODE10; //CNF2 0b10 PP alt, MODE2 0b11 Output mode max speed 50 MHz

	//Rx PB11
	GPIOB->CRH &= ~GPIO_CRH_CNF11 & ~GPIO_CRH_MODE11;
	GPIOB->CRH |= GPIO_CRH_CNF11_0 ; //input
	USART3->BRR =  0xA93; //13.300Hz //тут надо пересвчитать по формуле, целую и дробную часть
	USART3->CR1 = 0;
	USART3->CR1 |= 	USART_CR1_RXNEIE 	| //включаем прерывание по приему,
									USART_CR1_TCIE		| //включаем прерывание по отправке,
									USART_CR1_M 			| // 9 бит
									USART_CR1_PCE 		| // parity
									USART_CR1_PS			| // odd parity нечетный
									USART_CR1_TE 			|	//Transmitter enable
									USART_CR1_RE 			; //Receiver enable

	USART3->CR2 = 0;
	USART3->CR3 = 0;
	USART3->CR1 |= USART_CR1_UE; //включаем UART3
	NVIC_EnableIRQ (USART3_IRQn); //включаем прерывание
}


void keyscan_work(uint8_t code){
	static uint8_t last_code = 0;
	if(last_code == 0xf0){ //значит кнопка отпущена, ничего не делаем
	}else if(code == 0xE0){ //значит начало сложного символа, ничего не делаем
	}else if(code == 0x70){//0
	//сброс на заводские
		threeButtonClickHandler();
	}else if(code == 0x69){//1
	}else if(code == 0x72){//2
		screenSM(SCR_COMM_down);
	}else if(code == 0x7A){//3
	}else if(code == 0x6B){//4
		screenSM(SCR_COMM_back);
	}else if(code == 0x73){//5
		screenSM(SCR_COMM_select);
	}else if(code == 0x74){//6
		screenSM(SCR_COMM_select);
	}else if(code == 0x6C){//7
	}else if(code == 0x75){//8
		screenSM(SCR_COMM_up);
	}else if(code == 0x7D){//9
	}else if(code == 0x71){//.
	}else if(code == 0x79){//+
	//увеличить натяжение

	}else if(code == 0x7B){//-
	//ослабить натяг 

	}else if(code == 0x4A){// "/"
	//логирование вкл выкл

	}else if(code == 0x7C){// *
		//записать настройки

	}else if(code == 0x77){//NumLock
	//пульсация вкл выкл

	}else if(code == 0x66){// BackSpace
	//пид вкл выкл

	}else if(code == 0x5A){// Enter
		//сброс ошибки, запуск, остановка

	}else{
	}
	last_code = code;
}


void __attribute__ ((weak)) redButtonClickHandler(void){
}
void __attribute__ ((weak)) redButtonPressHandler(void){
}
void __attribute__ ((weak)) blueButtonClickHandler(void){
}
void __attribute__ ((weak)) blueButtonPressHandler(void){
}
void __attribute__ ((weak)) greenButtonClickHandler(void){
}
void __attribute__ ((weak)) greenButtonPressHandler(void){
}
void __attribute__ ((weak)) yellowButtonClickHandler(void){
}
void __attribute__ ((weak)) yellowButtonPressHandler(void){
}

void __attribute__ ((weak)) redGreenButtonClickHandler(void){
}
void __attribute__ ((weak)) redYellowButtonClickHandler(void){
}
void __attribute__ ((weak)) redBlueButtonClickHandler(void){
}

void __attribute__ ((weak)) yellowGreenButtonClickHandler(void){
}
void __attribute__ ((weak)) yellowBlueButtonClickHandler(void){
}
void __attribute__ ((weak)) greenBlueButtonClickHandler(void){
}

void __attribute__ ((weak)) threeButtonClickHandler(void){
}

