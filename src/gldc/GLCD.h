//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//  ������������ ���������� SPI ���������������� STM32F10xxxx
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#ifndef GLCD_H	// �������� ��������� ��������� ����� ������
#define GLCD_H 

#define glcd_Width  128 // ������ � �������� 
#define glcd_Height 64  // ������ � ��������

#define   Command         0                
#define   Data            1
            
#define   H 1        
#define   V 0

#define       R 0
#define   not_R 1

#define   FonOFF_InversOFF 0   // ������ �� ������ ����� ����� �� ������, ��� �� ������.
#define   FonOFF_InversON  1   // ������ ������ �� ������������, ��� �� ������.
#define   FonON_InversOFF  2   // ����� ���������� ���� ���. ������ �� �������.
#define   FonON_InversON   3   // ���� ���, ������ �� ������������ ��������. 

#define glcd_Backlight_On() (GPIOA->BSRR = ( GPIO_BSRR_BS10 ));
#define glcd_Backlight_Off() (GPIOA->BSRR = ( GPIO_BSRR_BR10 ));

// ==================================================

// ������������� ���������� SPI
void SPI_Init_user(void);

// �������� ������ � ����� SPI
char spi_Send(uint8_t raw);

// �������� ������/������� � GLCD ST7920
char glcd_Send(uint8_t raw, uint8_t cmd);

// �������� "���-�������"
void glcd_Delay(uint32_t tick);

// ������������� ������������ ������
void GLCD_Init(void);

// ������� ������������ ������� (����������)
char glcd_Clear_HW(void);

// ������� ������������ ������� (�����������)
char glcd_Clear_FW(void);

// �������� ����������� ����� ������
void glcd_Buffer_Clear(void);

// ����� ������������ ������ ������ �� �������
char glcd_Show(void);

// ���������� ������ �� ����������� �(�������) � Y(������), ����� �����������
void glcd_Char(uint8_t Symbol, uint8_t X, uint8_t Y, uint8_t type_font);

// �������� ������ � ����� ������� �� �������� �����������.
void glcd_String(char String[], uint8_t X, uint8_t Y, uint8_t type_font);

// ���������� ������� � ����������� ������ ������ �(������) � Y(������) �� ������ �������� ����
void glcd_Pixel(int16_t X, int16_t Y, uint8_t State);

// ���������� ����� � ����������� ������ ������
void glcd_Line(uint8_t Napr, uint8_t IniLin, uint8_t Dl, uint8_t st);

// ���������� ������������� � ����������� ������ ������
void glcd_Rect(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2);

// ����� "��������" ����� �� ����� (�������� 2 �����).
void glcd_Float(uint32_t dig, char temp[]);

// ����������� ������� ���� �� ������
void Menu_Cursor(uint8_t menu);

// ����� "��������" ����� �� ����� (�������� 2 �����).
void glcd_Float(uint32_t dig, char temp[]);


#endif
//
//
