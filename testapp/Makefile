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
BAUDRATE=115200
PROGPORT=/dev/avrusb0
AVRDUDE=avrdude -b $(BAUDRATE) -p $(MCU) -c $(PROGTYPE) -P $(PROGPORT)
ELFFUSE=./elf-fuse

PROG=testapp
BIN=testapp.bin
IHEX=testapp.ihex
SRCS=main.c
OBJS=$(SRCS:%.c=%.o)

MCU=atmega328p

CFLAGS  += -Wall -pedantic -Os -g -std=c99 -mmcu=$(MCU) -DF_CPU=16000000UL
LDFLAGS += -mmcu=$(MCU)

all: $(PROG) $(BIN)

$(PROG): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(BIN): $(PROG)
	$(OBJCOPY) -R .fuse -R .eeprom -R .lock -R .signature -S -O binary $^ $@ 

$(IHEX): $(PROG)
	$(OBJCOPY) -R .fuse -R .eeprom -R .lock -R .signature -S -O ihex $^ $@ 

.PHONY: flash
flash: $(IHEX)
	$(AVRDUDE) -e -U flash:w:$(IHEX)

.PHONY: clean
clean:
	rm -f $(OBJS) $(PROG) $(BIN) $(IHEX)

