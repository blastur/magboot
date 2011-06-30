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
#include <stdint.h>
#include <avr/io.h>

void uart_putc(uint8_t ch)
{
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = ch;
}

uint8_t uart_getc(void)
{
	uint8_t ch;

	loop_until_bit_is_set(UCSR0A, RXC0);
	ch = UDR0;

	return ch;
}

void uart_init()
{
	UCSR0A = _BV(U2X0);
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);
	UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
	UBRR0L = (uint8_t)((F_CPU + BAUD_RATE * 4L)/(BAUD_RATE * 8L) - 1);
}

