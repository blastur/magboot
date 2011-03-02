#! /usr/bin/env python
# Copyright (c) 2010-2011 Magnus Olsson (magnus@minimum.se)
# See LICENSE for details

"""magboot - AVR serial bootloader
  
usage: magboot <port> <device> <[command1] [command2] ... [commandN]>

commands:
-a <addr>	Load (byte) address (autoincremented by -w)
-w <file>	Write contents of <file> to loaded address (use - for stdin)
-i		Verify device signature
-j		Jump to loaded address
-r		Reset device (will bypass bootloader on next boot)

supported devices:
atmega328p
atmega168p
"""

import serial
import getopt
import sys
from struct import *
from array import array

global ser
global devicelist
global dev

atmega328p = {
	'signature': "\x1E\x95\x0F",
	'pagesize': 128,
}

atmega168p = {
	'signature': "\x1E\x94\x0B",
	'pagesize': 128,
}

devicelist = {
	'atmega328p': atmega328p,
	'atmega168p': atmega168p,
}

def usage(*args):
	sys.stdout = sys.stderr
	print __doc__
	for msg in args:
		print msg
	sys.exit(2)

def do_cmd(str, expect_reply = True):
	ser.write(str)
	if not expect_reply:
		print "> OK"
		return

	reply = ser.read()
	if (len(reply) == 0):
		print "> FAILED (TIMEOUT)"
		sys.exit(1)
		
	if (reply == 'Y'):
		print "> OK"
		return

	if (reply == 'N'):
		print "> FAILED"
	else:
		print "> FAILED (UNKNOWN):"
		while (len(reply) == 1):
			print reply
			reply = ser.read()
	sys.exit(1)

def cmd_device_id():
	print "ID"
	do_cmd('I' + dev['signature'])

def cmd_load_addr(addr):
	print "LOAD_ADDR"
	# Little-endian, 16-bit uint load address
	load_addr = pack('<H', int(addr, 16))
	do_cmd('A' + load_addr)

def cmd_write_file(fname):
	print "WRITE_FILE"

	if (fname == "-"):
		f = sys.stdin
	else:
		f = open(fname, "rb")

	eof = False
	
	while (not eof):
		buf = array('c')
		bytecount = 0
		while (bytecount < dev['pagesize']):
			data = f.read(1)
			if (len(data) == 0):
				eof = True
				break
			buf.append(data)
			bytecount = bytecount + 1

		# Zerofill remaining part of page
		if (bytecount != dev['pagesize']):
			while (bytecount < dev['pagesize']):
				buf.append(chr(0))
				bytecount = bytecount + 1

		do_cmd('W' + buf.tostring())

def cmd_reset():
	print "RESET"
	do_cmd('R', False)

def cmd_jump():
	print "JUMP"
	do_cmd('J', False)

if __name__ == "__main__":
	if len(sys.argv) < 4:
		usage();

	try:
		dev = devicelist[sys.argv[2]]
	except KeyError:
		print "Unsupported device '" + sys.argv[2] + "'"
		sys.exit(3)

	ser = serial.Serial(port=sys.argv[1], baudrate=115200, timeout=4)

	try:
		opts, args = getopt.getopt(sys.argv[3:], 'a:w:ijpr')
	except getopt.error, msg:
		usage(msg)

	for o, a in opts:
		if o == '-a': cmd_load_addr(a)
		if o == '-w': cmd_write_file(a)
		if o == '-i': cmd_device_id()
		if o == '-j': cmd_jump()
		if o == '-r': cmd_reset()

	ser.close()

