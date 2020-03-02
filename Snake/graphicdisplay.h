void graphic_init(void);
void graphic_ctrl_bit_clear(uint8_t x);
void graphic_ctrl_bit_set(uint8_t x);

void select_controller(uint8_t controller);

void graphic_wait_ready(void);

unsigned char graphic_read(unsigned char controller);

void graphic_write(uint8_t value, uint8_t controller);
void graphic_write_command(uint8_t cmd, uint8_t controller);
void graphic_write_data(uint8_t data, uint8_t controller);

static volatile unsigned int backBuffer[1024];

void graphic_clear_screen(void);
void pixel(unsigned x, unsigned y);
void graphic_draw_screen(void);
void clear_backbuffer();

