//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//  Конфигурация интерфейса SPI микроконтроллера STM32F10xxxx
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#ifndef GLCD_H	// Блокирую повторное включение этого модуля
#define GLCD_H 

#define glcd_Width  128 // Ширина в пикселях 
#define glcd_Height 64  // Высота в пикселях

#define   Command         0                
#define   Data            1
            
#define   H 1        
#define   V 0

#define       R 0
#define   not_R 1

#define   FonOFF_InversOFF 0   // Рисует на пустом месте белым по синему, фон не тронут.
#define   FonOFF_InversON  1   // Рисует пустым по закрашенному, фон не тронут.
#define   FonON_InversOFF  2   // Буква использует весь фон. Рисует по пустому.
#define   FonON_InversON   3   // Весь фон, рисует по закрашенному пустотой. 

#define glcd_Backlight_On() (GPIOA->BSRR = ( GPIO_BSRR_BS10 ));
#define glcd_Backlight_Off() (GPIOA->BSRR = ( GPIO_BSRR_BR10 ));

// ==================================================

// Инициализация интерфейса SPI
void SPI_Init_user(void);

// Отправка данных в буфер SPI
char spi_Send(uint8_t raw);

// Отправка данных/команды в GLCD ST7920
char glcd_Send(uint8_t raw, uint8_t cmd);

// Ожидание "аля-трамвай"
void glcd_Delay(uint32_t tick);

// Инициализация графического режима
void GLCD_Init(void);

// Очистка графического дисплея (аппаратная)
char glcd_Clear_HW(void);

// Очистка графического дисплея (программная)
char glcd_Clear_FW(void);

// Очистить виртуальный буфер экрана
void glcd_Buffer_Clear(void);

// Вывод виртуального буфера экрана на дисплей
char glcd_Show(void);

// Отображает символ по координатам Х(столбец) и Y(строка), стиль отображения
void glcd_Char(uint8_t Symbol, uint8_t X, uint8_t Y, uint8_t type_font);

// Отправка строки с своим шрифтом по заданным координатам.
void glcd_String(char String[], uint8_t X, uint8_t Y, uint8_t type_font);

// Нарисовать пиксель в виртуальном буфере экрана Х(ширина) и Y(высота) от левого верхнего угла
void glcd_Pixel(int16_t X, int16_t Y, uint8_t State);

// Нарисовать линию в виртуальном буфере экрана
void glcd_Line(uint8_t Napr, uint8_t IniLin, uint8_t Dl, uint8_t st);

// Нарисовать прямоугольник в виртуальном буфере экрана
void glcd_Rect(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2);

// Вывод "дробного" числа на экран (точность 2 знака).
void glcd_Float(uint32_t dig, char temp[]);

// Отображение курсора меню на экране
void Menu_Cursor(uint8_t menu);

// Вывод "дробного" числа на экран (точность 2 знака).
void glcd_Float(uint32_t dig, char temp[]);


#endif
//
//
