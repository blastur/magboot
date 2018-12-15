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
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/boot.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>

#include "uart.h"

#define LED_DIR     DDRB
#define LED_PORT    PORTB
#define LED_BIT		PB5

#define CMD_OK() uart_putc('Y')
#define CMD_FAIL() uart_putc('N')

typedef void (*jump_t)(void);

static bool cmd_load_addr(uint16_t *addr)
{
	/* 16-bit addr, little-endian */
	*addr = uart_getc();
	*addr += uart_getc() << 8;
	return false;
}

static uint16_t checksum(uint8_t *data, size_t size)
{
	uint16_t *words = (uint16_t *) data;
	size_t num_words = size / 2;
	uint32_t sum = 0;
	uint8_t i;

	for (i = 0; i < num_words; i++)
		sum += words[i];

	/* Fold */
	while (sum >> 16)
		sum = (sum & 0xFFFF) + (sum >> 16);

	return (uint16_t) sum;
}

static bool cmd_write_page(uint16_t *addr)
{
	uint16_t i;
	uint16_t page = *addr;
	uint8_t buf[SPM_PAGESIZE]; /* Pagesize + 16-bit checksum */
	uint16_t expected_csum, actual_csum;

	boot_page_erase(page);
	
	expected_csum = uart_getc();
	expected_csum += uart_getc() << 8;
	
	for(i = 0; i < SPM_PAGESIZE; i++)
		buf[i] = uart_getc();

	actual_csum = checksum(buf, sizeof(buf));

	if (expected_csum != actual_csum)
		return true;

	boot_spm_busy_wait();
	for (i = 0; i < SPM_PAGESIZE; i += 2) {
		uint16_t w = buf[i];
		w += buf[i+1] << 8;
		boot_page_fill(page + i, w);
	}

	boot_page_write(page);
	boot_spm_busy_wait();

	boot_rww_enable();

	/* Auto-increment address */
	*addr += SPM_PAGESIZE;

	return false;
}

static bool cmd_device_id(void)
{
	bool fail = false;

	/* Always read full signature, even if fail is detected */
	if (uart_getc() != SIGNATURE_0)
		fail = true;
	if (uart_getc() != SIGNATURE_1)
		fail = true;
	if (uart_getc() != SIGNATURE_2)
		fail = true;

	return fail;
}

static void jump(uint16_t addr)
{
	jump_t func;

	/* Clear WDRF to prevent "infinite reset loop". Do not rely on application
	 * to clear it, since it may not be WDT-aware and cannot account for the WDT
	 * usage of Magboot. The downside of this is however that an application can
	 * never detect WDT reset during initialization.
	 */
	MCUSR &= ~(_BV(WDRF));
	wdt_disable();
	func = (jump_t) addr;
	func();
}

static bool cmd_reset(void)
{
	wdt_enable(WDTO_15MS);
	while (1);

	return false; /* Unreachable */
}

int main(void) {
	uint16_t addr = 0;
	bool fail;

	if (bit_is_clear(MCUSR, EXTRF)) {
		/* Bypass magboot if reset caused by watchdog, power-on or brown-out */
		jump(0);
	} else
		MCUSR &= ~(_BV(EXTRF));

	wdt_enable(WDTO_4S);

	LED_DIR |= _BV(LED_BIT);
	LED_PORT |= _BV(LED_BIT);
	uart_init();

	while(1) {
		switch (uart_getc()) {
			/* Device ID */
			case 'I':
				fail = cmd_device_id();
				break;

			/* Address */
			case 'A':
				fail = cmd_load_addr(&addr);
				break;

			/* Write page */
			case 'W':
				fail = cmd_write_page(&addr);
				break;

			/* Reset */
			case 'R':
				fail = cmd_reset();
				break;

			default:
				fail = true;
				break;
		}

		if (fail)
			CMD_FAIL();
		else
			CMD_OK();
		wdt_reset();
	}
}
