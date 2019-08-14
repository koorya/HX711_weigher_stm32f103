#include "stm32f10x.h"
#include "GLCD.h"
#include "font8x8.h"

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

volatile uint8_t glcd_buff[1024]; // Буфер для работы с экраном

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
SPI_TypeDef * USED_SPI;
// Инициализация интерфейса SPI
void SPI_Init_user(void)
{

	USED_SPI = SPI1;

	SPI_InitTypeDef SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	SPI_StructInit(&SPI_InitStructure);
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
//	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
//	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_Init(USED_SPI, &SPI_InitStructure);
	SPI_Cmd(USED_SPI, ENABLE);

}
//

// Отправка данных в буфер SPI
char spi_Send(uint8_t raw)
{
	uint32_t err_cnt = 100000000;
	while(!SPI_I2S_GetFlagStatus(USED_SPI, SPI_I2S_FLAG_TXE))
		if (!err_cnt--)
			return 1;
	SPI_I2S_SendData(USED_SPI, raw);
	return 0;
}
//

// Отправка данных/команды в GLCD ST7920
char glcd_Send(uint8_t raw, uint8_t cmd)
{
	uint8_t StartByte = 0;
	char err_cnt = 0;
	
	// Выставляем CS в 1 (начало передачи данных/команды)
//	GPIOA->BSRR = GPIO_BSRR_BS4;
  
	if (!cmd) { StartByte = 0xF8; } else { StartByte = 0xFA; }
  
	// отправляем стартовый бит
	err_cnt += spi_Send(StartByte);
	
	// отправляем старшую тетраду данных
	err_cnt += spi_Send(raw & 0xF0);
	
	// отправляем младшую тетраду данных
	err_cnt += spi_Send((raw & 0x0F) << 4);
	 
	// Выставляем CS в 0 (окончание передачи данных/команды)
//	GPIOA->BRR = GPIO_BRR_BR4;
	return err_cnt;
}
//

// Ожидание "аля-трамвай"
void glcd_Delay(uint32_t tick)
{ 
	uint32_t wait = 0;
 	tick = tick * 8000;
	
	while(tick) 
 {
  __ASM("NOP"); 
	wait +=1; 
  tick--;
 } 
}
//

// Инициализация графического режима
void GLCD_Init(void)
{
	// Иницилизируем SPI1 интерфейс, к которому подключен дисплей
	// Интерфейс SPI1
	// LED       - вывод PA10 - вывод работает как выход
	// MOSI (RW) - вывод PA7 - альтернативная функция
	// Reset     - вывод PA0 - вывод работает как выход
	// CLK (E)   - вывод PA5 - альтернативная функция
	// CS (RS)   - вывод PA4 - вывод работает как выход
	SPI_Init_user();
	glcd_Delay(50);
	
	// Делаем сброс контроллера дисплея и выключаем подсветку дисплея (мусор прячем :))
//	GPIOA->BRR = ( GPIO_BRR_BR0 | GPIO_BRR_BR10 );
	glcd_Delay(50);
//	GPIOA->BSRR = ( GPIO_BSRR_BS0 );
	glcd_Delay(2);
	
  glcd_Send(0x30, Command);    //8 бит, стандартный набор комманд.
	glcd_Send(0x36, Command);    //Вкл. дисплей, курсор выключен, мигать выключено. 
  glcd_Send(0x01, Command);    //Режим пониженного энергопотребления. 
	
	glcd_Delay(150);
}
//

// Очистка графического дисплея (аппаратная)
char glcd_Clear_HW(void)
{
  if(glcd_Send(0x01, Command)) return 1;
  glcd_Delay(50);
  return 0;
}
//

// Очистка графического дисплея (программная)
char glcd_Clear_FW(void)
{
  for (char Loop_ST = 0; Loop_ST<32; Loop_ST++)
  { 
    glcd_Send(0x80|Loop_ST, Command);
    for (char Loop1 = 0; Loop1<64; Loop1++)
    { 
      if(glcd_Send(0, Data))
    	  return 1;
    }
  }
  return 0;
}
//

// Очистить виртуальный буфер экрана
void glcd_Buffer_Clear(void)
{
  for (int Loop = 0; Loop<1024; Loop++)
  {
    glcd_buff[Loop]=0;
  }
}
//

// Вывод виртуального буфера экрана на дисплей
char glcd_Show(void)
{
	char err_cnt = 0;
  int LoopDataLCD = 0;                       //В этой переменной хранится №ячейки буфера. От 0 до 1024).
  for (char Stroki = 0; Stroki<64; Stroki++) // Перебераем строки сверху вниз.
  {
    if (Stroki<32)                          // Если номер строки меньше 32-й (середины).
    { 
    	err_cnt += glcd_Send(0x80|Stroki, Command);      // Ставим указатель в начало строки 0..31.
    	err_cnt += glcd_Send(0x80, Command);
    }
    else 
    {
    	err_cnt += glcd_Send((0x80|Stroki)-32, Command);     // Если 2-я половина экрана (строка 31..63.)
    	err_cnt += glcd_Send(0x80+8, Command);            // То указатель на 8-ю ячейку строки.
    }                                        // И от номера строки -32. Т.к. строк на самом деле всего 32, но они со странной адресацией.
    for (char Stolb = 0; Stolb<16; Stolb++)  // В каждой строке выдаем по 16 ячеек. 
    {      
    	err_cnt += glcd_Send(glcd_buff[LoopDataLCD], Data);
    	LoopDataLCD++;
    }
    if(err_cnt) return 1;
  };  
  return 0;
}
//

// Отображает символ по координатам Х(столбец) и Y(строка), стиль отображения
void glcd_Char(uint8_t Symbol, uint8_t X, uint8_t Y, uint8_t type_font)
{
  for (unsigned char Strok_Loop = 0; Strok_Loop < 8; Strok_Loop++)
  { 
    for (unsigned char Stilb = 0; Stilb < 8; Stilb++)
    {
      switch  (type_font) //Каким именно рисовать символ? В define есть описание.
      {
        case FonOFF_InversOFF: glcd_buff[X+16*Stilb+(Y*16*8)] |=   Font8x8Table[Symbol] [Stilb];  break;
        case FonOFF_InversON:  glcd_buff[X+16*Stilb+(Y*16*8)] &= ~(Font8x8Table[Symbol] [Stilb]); break;
        case FonON_InversOFF:  glcd_buff[X+16*Stilb+(Y*16*8)] =    Font8x8Table[Symbol] [Stilb];  break;
        case FonON_InversON:   glcd_buff[X+16*Stilb+(Y*16*8)] |= ~(Font8x8Table[Symbol] [Stilb]); break;
      }
    }    
  }
}
//

// Отправка строки с своим шрифтом по заданным координатам.
void glcd_String(char String[], uint8_t X, uint8_t Y, uint8_t type_font)
{
  unsigned char Loop_SM = 0;                         //Номер символа.
  while (String[Loop_SM] != 0x00)           //Пока не достигним конечного.
  {   
    switch  (type_font) //Каким именно стилем рисовать? В define есть описание.
    {
      case FonOFF_InversOFF: glcd_Char( String[Loop_SM], X, Y, FonOFF_InversOFF ); break;
      case FonOFF_InversON:  glcd_Char( String[Loop_SM], X, Y, FonOFF_InversON ); break;
      case FonON_InversOFF:  glcd_Char( String[Loop_SM], X, Y, FonON_InversOFF ); break;
      case FonON_InversON:   glcd_Char( String[Loop_SM], X, Y, FonON_InversON ); break;
    }
    
    X++;                        //Сдвинуть по горизонтале вправо указатель.
    if (X > 15)                 //Если дописали до конца экрана - на слудующую строку.
    {
      X = 0; Y++;
      if (Y == 9) {Y=0;};       //Если дописана последняя строка - с 0-й.
    }
    Loop_SM++;                  //Следующий символ.
  }
}
//

// Нарисовать пиксель в виртуальном буфере экрана Х(ширина) и Y(высота) от левого верхнего угла
void glcd_Pixel(int16_t X, int16_t Y, uint8_t State)
{
  // Проверяем, находится ли точка в поле отрисовки дисплея
  if ((X >= glcd_Width) || (Y >= glcd_Height) || (X < 0) || (Y < 0))
  return;
  
  uint16_t ByteIdx = Y >> 3;
  uint8_t BitIdx = Y - (ByteIdx << 3); // Высота относительно строки байт (0<=Y<=7)
  ByteIdx *= glcd_Width;  
  ByteIdx += X;
  
  if (State)
    glcd_buff[ByteIdx] |= (1 << BitIdx);
  else
    glcd_buff[ByteIdx] &= ~(1 << BitIdx);
}
//

// Нарисовать линию в виртуальном буфере экрана
void glcd_Line(uint8_t Napr, uint8_t IniLin, uint8_t Dl, uint8_t st)
{ 
  if (Napr == H)                                   //Если линия горизонтальная, то.
  {//Для горизонтали: Horiz, по оси х слева пикселей, длинна вправо, по оси y константа.
    char LoopXn = 0;                                   //Пустых кусков. 
    while (IniLin>7) {IniLin=IniLin-8; LoopXn++;};     //Вычисляем, сколько пустых кусков до первого пикселя линии.      
    if ((8-IniLin) >= Dl)                              //Если длинна линии меньше или равна колличеству свободных после сдвига пикселей в 1 байте, то просто закрасить оставшиеся. 
    {
      glcd_buff[LoopXn+st*16] |= (0xFF>>IniLin) & (0xFF<<(8-Dl));//Отправляем линию меньше 1 байта, вмещающуюся в байт экрана.
        //Пример: слева 2          0011 1111 
                                 //1111 1100  справа 2.                              
    }                           //&0011 1100  "И"
    else
    {                                                 //Иначе смотрим.
      if (Dl <=((8-LoopXn)+8)  )                      //Если линия влезает (со сдвигом) в 2 байта рядом, то нарисовать ее.
      {
        glcd_buff[LoopXn+st*16]   |= 0xFF>>IniLin;     //Отодвинуть начальные пустые биты (если есть слева.).
        glcd_buff[LoopXn+st*16+1] |= 0xFF<<(8 - (Dl-(8-IniLin)));  //Справа отодвинуть, чтобы между была длинна.
      }
      else //Если линия занимает 3 и более байта, то... 
      {    
        glcd_buff[LoopXn+st*16] |= 0xFF>>IniLin;           //Рисуем не целую часть линии (не кратную 8) слева.
        Dl = Dl - (8-IniLin);                             //Оставшиеся для >2х бит.
        char LoopXb = 0;                                  //Занятых кусков.
        while (Dl>7) {LoopXb++; Dl=Dl-8;};                //Вычисляем колличество полных пикселей.
        char ZPDLSM = LoopXb;
        while (LoopXb!=0x00)                              //Заливаем полностью закрашенные куски.
        {
          glcd_buff[LoopXn+st*16+LoopXb] = 0xFF;
          LoopXb--;
        }
        if (Dl != 0) glcd_buff[LoopXn+st*16+ZPDLSM+1] |= 0xFF<<(8-Dl);  //Рисуем не до конца заполненный конец.
      };
    }; 
  }
  else    //Если вертикаль: Vert, сверху по оси Y пикселей, длинна вниз, константа х). 
  {
    char LoopXn = 0;                                   //Пустых кусков слева до пикселя со значением линии (типа 00010000).
    while (st>7) {st=st-8; LoopXn++;};                 //Вычисляем, сколько пустых кусков до первого пикселя линии по оси х.
    char Line_const = (1<<7)>>st;                      //Рисунок линии.
    for (char Ris = 0; Ris != Dl; Ris++)
    {
      glcd_buff[LoopXn+IniLin*16+Ris*16] |= Line_const;   
    }
  }
}
//

// Отображение курсора меню на экране
void Menu_Cursor(uint8_t menu)
{
	uint8_t cursor_x = 0, cursor_y = 0;

	switch(menu)
		  {
				case 0: { cursor_x = 15;
					        cursor_y = 1;
					        break;
					      }
				
				case 1: { cursor_x = 15;
					        cursor_y = 5;
					        break;
					      }
				
				case 2: { cursor_x = 15;
					        cursor_y = 2;
					        break;
					      }
				
				case 7: { cursor_x = 15;
					        cursor_y = 2;
					        break;
					      }
				default: break;
			}
			glcd_Char(17, cursor_x, cursor_y, FonON_InversOFF);
}
//

// Вывод "дробного" числа на экран (точность 2 знака).
void glcd_Float(uint32_t dig, char temp[])
{
	if (dig/10000) { temp[0] = (dig/10000)+48; } else { temp[0] = ' '; }
	if((dig/10000)||(dig>=1000)) { temp[1] = ((dig%10000)/1000)+48; } else { temp[1] = ' '; } 
  temp[2] = ((dig%1000)/100)+48;
  temp[3] = '.';
	temp[4] = ((dig%100)/10)+48;
	temp[5] = (dig%10)+48;
}
//
