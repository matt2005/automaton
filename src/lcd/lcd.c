/* automaton
 * (c) 2013 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details.
 */


#include "lcd/lcd.h"
#include "lcd/hd44780.h"


enum lcd_init_param {
	LCD_INIT_FUNC_SET =
		HD44780_BUS_8BIT | HD44780_LINES_TWO | HD44780_FONT_5X8,
	LCD_INIT_ONOFF =
		HD44780_DISPLAY_ON | HD44780_CUR_HIDE | HD44780_BLINK_OFF,
	LCD_INIT_ENT_MODE =
		HD44780_ADDR_INCR | HD44780_SHIFT_DISABLE,
};


struct lcd_state {
	uint8_t ent_mode;
	uint8_t onoff;
	uint8_t func_set;
	
	uint8_t addr;
	
	struct {
		uint8_t x;
		uint8_t y;
	} cur;
};


struct lcd_state state;
const struct lcd_state state_init PROGMEM = {
	.ent_mode = LCD_INIT_ENT_MODE,
	.onoff    = LCD_INIT_ONOFF,
	.func_set = LCD_INIT_FUNC_SET,
	
	.addr = 0x00,
	
	.cur = {
		.x = 0,
		.y = 0,
	},
};

FILE *lcd = NULL;


static void lcd_send_ddaddr(void) {
	hd44780_set_ddaddr(state.addr);
}


_Static_assert(LCD_ROWS == 4 && LCD_COLS == 20, "lcd cursor assumptions wrong");
static void lcd_set_cur(void) {
	uint8_t new_addr = state.cur.x;
	if ((state.cur.y % 2) == 0) {
		new_addr += (state.cur.y * 10);
	} else {
		new_addr += 64 + ((state.cur.y - 1) * 10);
	}
	
	state.addr = new_addr;
	lcd_send_ddaddr();
}

static void lcd_cur_adv(void) {
	/* we assume that the address counter is set to increment */
	if (++state.cur.x == LCD_COLS) {
		state.cur.x = 0;
		if (++state.cur.y == LCD_ROWS) {
			state.cur.y = 0;
		}
		lcd_cur_set();
	}
}


static int lcd_file_get(FILE *f) {
	(void)f;
	
	return lcd_read();
}

static int lcd_file_put(char c, FILE *f) {
	(void)f;
	
	lcd_write(c);
	return 0;
}


void lcd_init(void) {
	hd44780_init(LCD_INIT_FUNC_SET, LCD_INIT_ONOFF, LCD_INIT_ENT_MODE);
	
	memcpy_P(&state, &state_init, sizeof(state));
	
	lcd = fdevopen(lcd_file_put, lcd_file_get);
}


void lcd_custom_store(uint8_t code, const uint8_t data[static 8]) {
	code &= 0x07;
	
	hd44780_set_cgaddr(code * 8);
	hd44780_write(8, data);
	
	/* restore the previous ddram address */
	lcd_send_ddaddr();
}

void lcd_custom_load(uint8_t code, uint8_t data[static 8]) {
	code &= 0x07;
	
	hd44780_read_cgram(code * 8, 8, data);
	
	/* restore the previous ddram address */
	lcd_send_ddaddr();
}


void lcd_goto_xy(uint8_t x, uint8_t y) {
	state.cur.x = (x % LCD_COLS);
	state.cur.y = (y % LCD_ROWS);
	lcd_cur_set();
}

void lcd_goto_x(uint8_t x) {
	state.cur.x = (x % LCD_COLS);
	lcd_cur_set();
}

void lcd_goto_y(uint8_t y) {
	state.cur.y = (y % LCD_ROWS);
	lcd_cur_set();
}


void lcd_write(char chr) {
	if (chr == '\r') {
		lcd_goto_x(0);
		return;
	} else if (chr == '\n') {
		lcd_goto_y(state.cur.y + 1);
		return;
	}
	
	hd44780_write(1, (const uint8_t *)&chr);
	
	lcd_cur_adv();
}

char lcd_read(void) {
	char chr;
	hd44780_read_ddram(state.addr, 1, (uint8_t *)&chr);
	
	lcd_cur_adv();
	return chr;
}