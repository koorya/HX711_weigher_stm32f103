#include <stdint.h>
#define SYSTICKINTMS 5000



extern void redButtonClickHandler(void);
extern void redButtonPressHandler(void);
extern void blueButtonClickHandler(void);
extern void blueButtonPressHandler(void);
extern void greenButtonClickHandler(void);
extern void greenButtonPressHandler(void);
extern void yellowButtonClickHandler(void);
extern void yellowButtonPressHandler(void);

extern void redGreenButtonClickHandler(void);
extern void redYellowButtonClickHandler(void);
extern void redBlueButtonClickHandler(void);

extern void yellowGreenButtonClickHandler(void);
extern void yellowBlueButtonClickHandler(void);

extern void greenBlueButtonClickHandler(void);

extern void threeButtonClickHandler(void);


extern void initKeyboard(void);
extern void keyboadrWork(void);
extern void keyscan_work(uint8_t code);
extern void usart3_init_keyboard(void);
