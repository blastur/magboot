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

PROG=magboot
IHEX=magboot.ihex
SRCS=magboot.c
OBJS=$(SRCS:%.c=%.o)

#OPTIMIZE = -Os -fno-inline-small-functions -fno-split-wide-types -mshort-calls
OPTIMIZE = -Os
CFLAGS += -g -Wall $(OPTIMIZE) -mmcu=$(MCU) -DF_CPU=$(CPU_FREQ)
LDFLAGS += -Wl,--section-start=.text=$(BOOTADDR)

CFLAGS += -DBAUD_RATE=115200
CFLAGS += -DJUMP_ADDR=0x00

# Target specifics: ATmega328p @ external 16 MHz
MCU=atmega328p
CPU_FREQ=16000000L
HFUSE=0xdc
BOOTADDR=0x7c00

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(IHEX): $(PROG)
	$(OBJCOPY) -j .text -j .data -S -O ihex $^ $@ 

.PHONY: flash
flash: $(IHEX)
	$(AVRDUDE) -e -U flash:w:$(IHEX)

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
	rm -f $(OBJS) $(PROG) $(IHEX)
