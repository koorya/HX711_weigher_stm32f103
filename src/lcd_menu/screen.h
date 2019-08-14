#include <stdint.h>
#include <stdio.h>
#include "screen_types.h"
#include "menu_callback.h"








extern void screenWork(void);

extern char updateScreen(void);

extern void menuInit(void);

extern void initScreen(uint32_t interval, volatile ITEM_TYPE * curr_item);
extern void initWindow(volatile WINDOW_TYPE * window, char *name, volatile WINDOW_TYPE * par_window);

extern void voidFunct(void);
extern void voidFunct_c(char *);
extern void back_menu_calback(void);

extern void initItemLabel(volatile ITEM_TYPE * 		item, 
							char *									name, 
							volatile WINDOW_TYPE * 	par_window, 
							char 										x_pos, 
							char 										y_pos); 

extern void initItemSubmenu(volatile ITEM_TYPE * 		item, 
							char *									name, 
							volatile WINDOW_TYPE * 	par_window, 
							volatile WINDOW_TYPE * 	chld_window, 
							char 										x_pos, 
							char 										y_pos);
							
extern void initItemParameter(volatile ITEM_TYPE * 		item, 
							char *									name, 
							volatile WINDOW_TYPE * 	par_window, 
							ITEM_OBJ * 							obj,
							char 										x_pos, 
							char 										y_pos,	
							PARAM_VIEW_ENUM_TYPE 		view_mode);	
							
							

extern ITEM_OBJ * initObj(ITEM_OBJ * obj, void (*dec) (void), void (*inc) (void), void (*show) (char *), OBJ_TYPE_ENUM type);

extern ITEM_OBJ * initObjChangeable(ITEM_OBJ * obj, void (*inc) (void), void (*dec) (void), void (*show) (char *));
extern ITEM_OBJ * initObjNotchangeable(ITEM_OBJ * obj, void (*show) (char *));

extern void screenSM(SCREEN_COMMAND_TYPE cmd);
