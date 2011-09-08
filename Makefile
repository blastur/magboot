# Copyright (C) 2010-2011 Magnus Olsson
# 
# This file is part of magboot
# Windnode is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# Windnode is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with magboot.  If not, see <http://www.gnu.org/licenses/>.
# 

CC=avr-gcc
OBJDUMP=avr-objdump
OBJCOPY=avr-objcopy
STRIP=avr-strip

PROGTYPE=stk500v2
PROGPORT=/dev/avrusb0
AVRDUDE=avrdude -p $(MCU) -c $(PROGTYPE) -P $(PROGPORT)

HWUART=magboot_hw
SWUART=magboot_sw
HWUART_IHEX=magboot_hw.ihex
SWUART_IHEX=magboot_sw.ihex
HWUART_BIN=magboot_hw.bin
SWUART_BIN=magboot_sw.bin

COMMON_OBJS=magboot.o
HWUART_OBJS=hwuart.o
SWUART_OBJS=swuart.o

#OPTIMIZE = -Os -fno-inline-small-functions -fno-split-wide-types -mshort-calls
OPTIMIZE = -Os
CFLAGS += -g -Wall $(OPTIMIZE) -mmcu=$(MCU) -include config.h -DBAUD_RATE=19200
LDFLAGS += -Wl,--section-start=.text=$(BOOTADDR)

# Target definition
MCU=atmega328p
HFUSE=0xdc
BOOTADDR=0x7c00

.PHONY: all
all: $(HWUART_IHEX) $(SWUART_IHEX) $(SWUART_BIN) $(HWUART_BIN)

$(HWUART): $(COMMON_OBJS) $(HWUART_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(SWUART): $(COMMON_OBJS) $(SWUART_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(HWUART_IHEX): $(HWUART)
	$(OBJCOPY) -j .text -j .data -S -O ihex $^ $@

$(SWUART_IHEX): $(SWUART)
	$(OBJCOPY) -j .text -j .data -S -O ihex $^ $@ 

$(HWUART_BIN): $(HWUART)
	$(OBJCOPY) -j .text -j .data -S -O binary $^ $@

$(SWUART_BIN): $(SWUART)
	$(OBJCOPY) -j .text -j .data -S -O binary $^ $@

.PHONY: flash_sw
flash_sw: $(SWUART_IHEX)
	$(AVRDUDE) -e -U flash:w:$^

.PHONY: flash_hw
flash_hw: $(HWUART_IHEX)
	$(AVRDUDE) -e -U flash:w:$^

.PHONY: fusedump
fusedump: 
	$(AVRDUDE) -U hfuse:r:-:h

.PHONY: fuseprog
fuseprog:
	$(AVRDUDE) -u -U hfuse:w:$(HFUSE):m

.PHONY: flashdump
flashdump:
	$(AVRDUDE) -U flash:r:flash.bin:r

.PHONY: clean
clean:
	rm -f $(COMMON_OBJS) $(HWUART) $(HWUART_IHEX) $(HWUART_OBJS) $(HWUART_BIN) $(SWUART) $(SWUART_IHEX) $(SWUART_OBJS) $(SWUART_BIN)
