

void show_received(char * ret);

void show_curr(char * ret);
void show_voltage(char * ret);
void show_max(char * ret);

void show_tenzoconst_k(char * ret);
void inc_tenzoconst_k(void);
void dec_tenzoconst_k(void);
void show_tenzoconst_nom(char * ret);
void inc_tenzoconst_nom(void);
void dec_tenzoconst_nom(void);

void show_tenzo_zero(char * ret);
void set_tenzo_zero(void);

extern void inc_freq(void);
extern void dec_freq(void);
extern void show_freq(char * ret);


extern void show_experiment_state(char * ret);
extern void userExperimentClick(void);
