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
#include <util/delay.h>
#include <avr/wdt.h>

#define LED_DIR  DDRB
#define LED_PORT PORTB
#define LED_BIT  PB5

int main(void)
{
	LED_DIR |= _BV(LED_BIT);
	LED_PORT ^= _BV(LED_BIT);

	wdt_enable(WDTO_250MS);

	while (1); /* Watchdog bite */

	return 0;
}
