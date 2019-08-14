#define W(A) (&(menu_window[A]))
#define I(A) (&(item[A]))

typedef enum{
	MAIN = 0,
	MODE,
	PULS_MODE,
	PID_MODE,
	LAZY_PID_MODE,
	LIMITS,
	DATA_MANAGE,
	FORMULAS
} WINDOW_ENUM;

typedef enum{
	SCR_COMM_up,
	SCR_COMM_down,
	SCR_COMM_select,
	SCR_COMM_back,
	SCR_COMM_defoult
} SCREEN_COMMAND_TYPE;

typedef enum{
	SELECT_ITEM,
	MODIFY_VALUE
} SCREEN_STATE_TYPE;

typedef enum{
	parameter,
	submenu,
	labelitem
} ITEM_TYPE_ENUM;

typedef enum{
	changeable,
	notchangeable
} OBJ_TYPE_ENUM;

typedef enum{
	line,
	column,
	unvisible_value,
	unvisible_name,
} PARAM_VIEW_ENUM_TYPE;

typedef struct ITEM_OBJ{
	void (*dec) (void);
	void (*inc) (void);
	void (*show) (char *);
	void (*set) (void);
	OBJ_TYPE_ENUM type;
} ITEM_OBJ;

typedef struct ITEM_TYPE{
	char *name;
	ITEM_TYPE_ENUM item_type;
	ITEM_OBJ * obj;
	uint32_t value;//for delete
	volatile struct ITEM_TYPE * next_item;
	volatile struct ITEM_TYPE * prew_item;
	volatile struct WINDOW_TYPE * parent_window;
	volatile struct WINDOW_TYPE * child_window;
	char x_pos;
	char y_pos;
	PARAM_VIEW_ENUM_TYPE view_mode;
} ITEM_TYPE;

typedef struct WINDOW_TYPE{
	char *name;
	volatile ITEM_TYPE * firs_item_ptr;
	volatile ITEM_TYPE * selected_item_ptr;
	volatile struct WINDOW_TYPE * parent_window;
} WINDOW_TYPE;




