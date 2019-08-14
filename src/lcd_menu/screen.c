#include "screen.h"
#include "gldc/GLCD.h"







volatile ITEM_TYPE * findNextSelectableItem(volatile ITEM_TYPE * item);
volatile ITEM_TYPE * findPrewSelectableItem(volatile ITEM_TYPE * item);
char isSelectableItem(volatile ITEM_TYPE * item);
void screenSM(SCREEN_COMMAND_TYPE cmd);

void voidFunct(void){
}

void voidFunct_c(char *a){
	a[0] = 0;
}

void back_menu_calback(void){
	screenSM(SCR_COMM_back);
}
volatile uint32_t upd_interval;
volatile uint32_t upd_counter;


volatile ITEM_TYPE *CURRENT_ITEM;

volatile SCREEN_STATE_TYPE screen_state;

void initWindow(volatile WINDOW_TYPE * window, char *name, volatile WINDOW_TYPE * par_window){
	window->firs_item_ptr = 0;
	window->selected_item_ptr = 0;
	window->name = name;
	window->parent_window = par_window;
}

void insertItemToList(volatile WINDOW_TYPE * par_window, volatile ITEM_TYPE * item){
	volatile ITEM_TYPE * 		item_ptr = par_window->firs_item_ptr;
	if(item_ptr){
		item->prew_item = item_ptr->prew_item;
		item->next_item = item_ptr;
		item_ptr->prew_item->next_item = item;
		item_ptr->prew_item = item;
		if(!isSelectableItem(item->parent_window->selected_item_ptr))
			if (isSelectableItem(item))
				item->parent_window->selected_item_ptr = item;
	
	}else{
		par_window->firs_item_ptr = item;
		par_window->selected_item_ptr = item;
		item->prew_item = item;
		item->next_item = item;		
	}

}

void initItemSubmenu(volatile ITEM_TYPE * 		item, 
							char *									name, 
							volatile WINDOW_TYPE * 	par_window, 
							volatile WINDOW_TYPE * 	chld_window, 
							char 										x_pos, 
							char 										y_pos)
{
	if(!item) return;
	if(!par_window) return;		
	item->view_mode = line;	
	item->name = name;
	item->next_item = 0;
	item->parent_window = par_window;



	item->child_window = chld_window;
//	if(chld_window)
//		chld_window->parent_window = par_window;
	item->x_pos = x_pos;
	item->y_pos = y_pos;
	
	item->item_type = submenu;
	item->obj = 0;
	insertItemToList(par_window, item);
}

void initItemParameter(volatile ITEM_TYPE * 		item, 
							char *									name, 
							volatile WINDOW_TYPE * 	par_window, 
							ITEM_OBJ * 							obj,
							char 										x_pos, 
							char 										y_pos,
							PARAM_VIEW_ENUM_TYPE 		view_mode){
	if(!item) return;
	if(!par_window) return;							
	item->view_mode = view_mode;
	item->name = name;
	item->next_item = 0;
	item->parent_window = par_window;



	item->child_window = 0;

	item->x_pos = x_pos;
	item->y_pos = y_pos;
	
	item->item_type = parameter;
	item->obj = obj;
								
	insertItemToList(par_window, item);
}
							
void initItemLabel(volatile ITEM_TYPE * 		item, 
							char *									name, 
							volatile WINDOW_TYPE * 	par_window, 
							char 										x_pos, 
							char 										y_pos)
{
	if(!item) return;
	if(!par_window) return;							
	item->name = name;
	item->next_item = 0;
	item->parent_window = par_window;



	item->child_window = 0;

	item->x_pos = x_pos;
	item->y_pos = y_pos;
	
	item->item_type = labelitem;
	item->obj = 0;	
	
	insertItemToList(par_window, item);
}

ITEM_OBJ * initObj(ITEM_OBJ * obj, void (*dec) (void), void (*inc) (void), void (*show) (char *), OBJ_TYPE_ENUM type){
	obj->dec = dec;
	obj->inc = inc;
	obj->show = show;
	obj->type = type;
	obj->set = voidFunct;
	return obj;
}

ITEM_OBJ * initObjChangeable(ITEM_OBJ * obj, void (*inc) (void), void (*dec) (void), void (*show) (char *)){
	return initObj(obj, dec, inc, show, changeable);
}

ITEM_OBJ * initObjNotchangeable(ITEM_OBJ * obj, void (*show) (char *)){
	return initObj(obj, voidFunct, voidFunct, show, notchangeable);
}



void initScreen(uint32_t interval, volatile ITEM_TYPE * curr_item){
	upd_interval = interval;
	upd_counter = upd_interval;
	screen_state = SELECT_ITEM;
	
	if(isSelectableItem(curr_item)){
		CURRENT_ITEM = curr_item;
	}else{
		CURRENT_ITEM = findNextSelectableItem(curr_item);
		if(CURRENT_ITEM)
			if(CURRENT_ITEM->parent_window)
				CURRENT_ITEM->parent_window->selected_item_ptr = CURRENT_ITEM;
	}
}

void screenWork(void){
	if(upd_counter > 0){
		upd_counter--;
	}
}
char strLength(char *str){
	unsigned char ret = 0;
	while(str[ret++]);
	return ret-1;
}
char updateScreen(void){
	if(upd_counter > 0)
		return 0;
	upd_counter = upd_interval;

	glcd_Buffer_Clear();

	volatile WINDOW_TYPE * curr_window = CURRENT_ITEM->parent_window;
	volatile ITEM_TYPE * curr_item = curr_window->firs_item_ptr->prew_item;
//	glcd_String(curr_window->name, 2, 0, FonON_InversOFF); //вывод названия окна
	do{ 
		curr_item = curr_item->next_item;
		char *temp;
		char temp_str[10] = "00";
		temp = temp_str;
		if(curr_item->item_type == parameter){
			curr_item->obj->show(temp);
		}else if(curr_item->item_type == submenu){
			sprintf(temp, ">");
		}else if(curr_item->item_type == labelitem){
			temp_str[0] = 0;
		}
		char item_fon = FonOFF_InversOFF;
		char value_fon = FonOFF_InversOFF;
		if(curr_item == CURRENT_ITEM ){
			if(screen_state == SELECT_ITEM){
				item_fon = FonON_InversON;
			}else if(screen_state == MODIFY_VALUE){
				value_fon = FonON_InversON;
			}
		}
		if(curr_item->view_mode == line){
			glcd_String(curr_item->name, curr_item->x_pos, curr_item->y_pos, item_fon);
			glcd_String (temp, curr_item->x_pos+strLength(curr_item->name), curr_item->y_pos, value_fon);
		}else if(curr_item->view_mode == column){
			glcd_String(curr_item->name, curr_item->x_pos, curr_item->y_pos, item_fon);
			glcd_String (temp, curr_item->x_pos, curr_item->y_pos+1, value_fon);
		}else if(curr_item->view_mode == unvisible_value){
			glcd_String(curr_item->name, curr_item->x_pos, curr_item->y_pos, item_fon);
		}else if(curr_item->view_mode == unvisible_name){
			glcd_String (temp, curr_item->x_pos, curr_item->y_pos, value_fon);
		}

	}while(curr_item != curr_window->firs_item_ptr->prew_item);

	return glcd_Show();
}

char isSelectableItem(volatile ITEM_TYPE * item){
		if(!item){
		}else if(item->item_type == submenu){
			return 1;
		}else if(item->item_type == parameter){
			if(item->obj)
				if(item->obj->type == changeable)
					return 1;
		}
		return 0;
}
volatile ITEM_TYPE * findNextSelectableItem(volatile ITEM_TYPE * item){
	volatile ITEM_TYPE * tmp_item = item->next_item;
	for(; tmp_item; tmp_item = tmp_item->next_item){
		if(isSelectableItem(tmp_item))
			return tmp_item;
	}
	return item;
}

volatile ITEM_TYPE * findPrewSelectableItem(volatile ITEM_TYPE * item){
	volatile ITEM_TYPE * tmp_item = item->prew_item;
	for(; tmp_item; tmp_item = tmp_item->prew_item){
		if(isSelectableItem(tmp_item))
			return tmp_item;
	}
	
	return item;
}

void screenSM(SCREEN_COMMAND_TYPE cmd){
	if(screen_state == SELECT_ITEM){
		if(cmd == SCR_COMM_select){
			if(CURRENT_ITEM->item_type == parameter){
				if(CURRENT_ITEM->obj->type == changeable){
					screen_state = MODIFY_VALUE;
				}else if(CURRENT_ITEM->obj->type == notchangeable){
				}
			}else if(CURRENT_ITEM->item_type == submenu){
				if(CURRENT_ITEM->child_window)
					if(CURRENT_ITEM->child_window->firs_item_ptr){
						CURRENT_ITEM = CURRENT_ITEM->child_window->selected_item_ptr;
						screen_state = SELECT_ITEM;
					}				
			}
		}else if(cmd == SCR_COMM_up){
			CURRENT_ITEM = findPrewSelectableItem(CURRENT_ITEM);
		}else if(cmd == SCR_COMM_down){
			CURRENT_ITEM = findNextSelectableItem(CURRENT_ITEM);
		}else if(cmd == SCR_COMM_back){
			if(CURRENT_ITEM->parent_window->parent_window){
				if(CURRENT_ITEM->obj)
					CURRENT_ITEM->obj->set();///////
				CURRENT_ITEM = CURRENT_ITEM->parent_window->parent_window->selected_item_ptr;
			}
		}else if(cmd == SCR_COMM_defoult){
		}
		CURRENT_ITEM->parent_window->selected_item_ptr = CURRENT_ITEM;
	}else if(screen_state == MODIFY_VALUE){
		if(cmd == SCR_COMM_select){
				screen_state = SELECT_ITEM;
		}else if(cmd == SCR_COMM_up){
			if(CURRENT_ITEM->item_type == parameter){
				CURRENT_ITEM->obj->inc();
			}else if(CURRENT_ITEM->item_type == submenu){
			}
		}else if(cmd == SCR_COMM_down){
			if(CURRENT_ITEM->item_type == parameter){
				CURRENT_ITEM->obj->dec();
			}else if(CURRENT_ITEM->item_type == submenu){
			}
		}else if(cmd == SCR_COMM_back){
			screen_state = SELECT_ITEM;
		}else if(cmd == SCR_COMM_defoult){
		}
	}
}

