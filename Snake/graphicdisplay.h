void graphic_init(void);
void graphic_ctrl_bit_clear(uint8_t x);
void graphic_ctrl_bit_set(uint8_t x);

void select_controller(uint8_t controller);

void graphic_wait_ready(void);

unsigned char graphic_read(unsigned char controller);

void graphic_write(uint8_t value, uint8_t controller);
void graphic_write_command(uint8_t cmd, uint8_t controller);
void graphic_write_data(uint8_t data, uint8_t controller);

void graphic_clear_screen(void);
void pixel(unsigned int x, unsigned int y, unsigned int set);