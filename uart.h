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
#ifndef _UART_H_
#define _UART_H_

#include <stdint.h>

void uart_putc(uint8_t);
uint8_t uart_getc(void);
void uart_init();

#endif
