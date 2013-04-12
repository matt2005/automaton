/* automaton
 * (c) 2013 Justin Gottula
 * The source code of this project is distributed under the terms of the
 * simplified BSD license. See the LICENSE file for details.
 */


#ifndef AUTOMATON_IO_UART_H
#define AUTOMATON_IO_UART_H


#include "std.h"


#define UART_LF_TO_CRLF 1


enum uart_num {
	UART_PC      = 0,
	UART_STN1110 = 1,
};

/* bit 12 is the double-speed bit */
enum uart_divisor {
	UART_DIV_2400   = 520,
	UART_DIV_4800   = 259,
	UART_DIV_9600   = 129,
	UART_DIV_14400  = 86,
	UART_DIV_19200  = 64,
	UART_DIV_28800  = 42,
	UART_DIV_38400  = 32,
	UART_DIV_57600  = 21,
	UART_DIV_76800  = 15,
	UART_DIV_115200 = 10,
	UART_DIV_250K   = 4,
	UART_DIV_1_25M  = 0,
	
	UART_DIV_230400 = _BV(12) | 21,
	UART_DIV_500K   = _BV(12) | 4,
	UART_DIV_2_50M  = _BV(12) | 0,
};
_Static_assert(F_CPU == 20000000UL, "UART baud rate divisors are wrong");


void uart_init(uint8_t dev, uint16_t divisor, uint16_t timeout_tx_ms,
	uint16_t timeout_rx_ms);
void uart_stop(uint8_t dev);

bool uart_flush(uint8_t dev, uint16_t timeout_ms);

bool uart_write(uint8_t dev, char chr);
bool uart_read(uint8_t dev, char *chr);


#endif
