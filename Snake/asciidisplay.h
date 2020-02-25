void ascii_init(void);

void ascii_ctrl_bit_set(unsigned char x);
void ascii_ctrl_bit_clear(unsigned char x);
void ascii_write_cmd(unsigned char command);
void ascii_write_data(unsigned char data);
void ascii_write_controller(unsigned char byte);
unsigned char ascii_read_controller(void);
unsigned char ascii_read_status(void);
unsigned char ascii_read_data(void);
void clear_display();
void funtion_set();
void display_control();
void entry_mode_set();
void ascii_write_char(unsigned char c);
void ascii_gotoxy(int x, int y);