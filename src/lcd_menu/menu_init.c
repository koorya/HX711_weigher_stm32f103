#include "screen.h"

void menuInit(void){
	static ITEM_OBJ obj[35];
	static WINDOW_TYPE menu_window[20];
	static ITEM_TYPE item[55];
	static uint32_t item_index = 0;
	static uint32_t obj_index = 0;
	
		initWindow(W(MAIN), "ГЛАВНОЕ", 0);
//		initWindow(W(DATA_MANAGE), "меню данных", W(MAIN));


	initItemParameter(&(item[item_index++]), 	"Макс. ", W(MAIN), initObjNotchangeable(&(obj[obj_index++]), show_max), 0, 1, line);
	initItemParameter(&(item[item_index++]), 	"Текущ.", W(MAIN), initObjNotchangeable(&(obj[obj_index++]), show_curr), 0, 2, line);
	initItemParameter(&(item[item_index++]), 	"Вольт.", W(MAIN), initObjNotchangeable(&(obj[obj_index++]), show_voltage), 0, 3, line);
	initItemParameter(&(item[item_index++]), "эксп", W(MAIN), initObjNotchangeable(&(obj[obj_index++]), show_experiment_state), 12, 0, column);
	initItemParameter(&(item[item_index++]), 	"К.пер:    ", W(MAIN), initObjChangeable(&(obj[obj_index++]), inc_tenzoconst_k, dec_tenzoconst_k, show_tenzoconst_k), 0, 4, line);
	initItemParameter(&(item[item_index++]), 	"Номинал: ", W(MAIN), initObjChangeable(&(obj[obj_index++]), inc_tenzoconst_nom, dec_tenzoconst_nom, show_tenzoconst_nom), 0, 5, line);
	initItemParameter(&(item[item_index++]), 	"фикс 0:", W(MAIN), initObjChangeable(&(obj[obj_index++]), set_tenzo_zero, set_tenzo_zero, show_tenzo_zero), 0, 6, line);

	initItemParameter(&(item[item_index++]), 	"НХ711", W(MAIN), initObjNotchangeable(&(obj[obj_index++]), show_received), 0, 7, line);


	initScreen(200, &(item[0]));

}
