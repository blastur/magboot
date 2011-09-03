/* Copyright (C) 2010-2011 Magnus Olsson
 * 
 * This file is part of magboot
 * Windnode is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Windnode is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with magboot.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "uart.h"

#define STARTBIT (0)
#define STOPBIT (1)

/* Calculate baudtime using timer pre-scaler 64 */
#define BAUDTIME ((F_CPU)/(BAUD_RATE*64l))

#if (BAUDTIME > 255)
#error Baudrate is too low!
#endif

#if (BAUDTIME < 1)
#error Baudrate is too high!
#endif

static void __bit_delay(void)
{
	TCCR0B |= _BV(CS01) | _BV(CS00); /* Timer pre-scaler 64 */
	while (TCNT0 < BAUDTIME) { /* NOP */ }
	TCCR0B &= ~(_BV(CS01) | _BV(CS00));
}

static void bit_delay(void)
{
	TCNT0 = 0;
	__bit_delay();
}

static void half_bit_delay(void)
{
	TCNT0 = BAUDTIME/2;
	__bit_delay();
}

static uint8_t rx_pin(void)
{
	return (CONFIG_SWUART_RX_PIN & _BV(CONFIG_SWUART_RX_BIT)) != 0;
}

static void tx_pin(uint8_t hi)
{
	if (hi)
		CONFIG_SWUART_TX_PORT |= _BV(CONFIG_SWUART_TX_BIT);
	else
		CONFIG_SWUART_TX_PORT &= ~(_BV(CONFIG_SWUART_TX_BIT));
}

/* uart_putc() -- blocking 8N1 transmit */
void uart_putc(uint8_t ch)
{
	uint8_t bit;

	tx_pin(STARTBIT);
	bit_delay();
	for (bit = 0; bit < 8; bit++) {
		tx_pin(ch & 0x1);
		ch = ch >> 1;
		bit_delay();
	}
	tx_pin(STOPBIT);
	bit_delay();
}

static int __getc(void)
{
	uint8_t bit;
	uint8_t ch;

	half_bit_delay();
	if (rx_pin() != STARTBIT)
		return -1; /* Glitch */
	bit_delay();
	for (bit = 0; bit < 8; bit++) {
		ch = ch >> 1;
		if (rx_pin())
			ch |= 1 << 7;
		bit_delay();
	}

	if (rx_pin() != STOPBIT)
		return -2; /* Frame error */

	return (int) ch;
}

/* uart_getc() -- blocking 8N1 receive */
uint8_t uart_getc(void)
{
	while (1) {
		int ch;
		while (rx_pin() != STARTBIT) { /* NOP */ }

		ch = __getc();
		if (ch >= 0)
			return (uint8_t) ch;
	}
}

void uart_init()
{
	CONFIG_SWUART_RX_DIR  &= ~(_BV(CONFIG_SWUART_RX_BIT));
	CONFIG_SWUART_RX_PORT |= _BV(CONFIG_SWUART_RX_BIT);
	CONFIG_SWUART_TX_DIR  |= _BV(CONFIG_SWUART_TX_BIT);
	tx_pin(STOPBIT);
}

