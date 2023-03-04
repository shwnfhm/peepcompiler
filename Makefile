#######
# Makefile for peep compiler
#######
CC = gcc
CFLAGS = -O0 -g -Wall

all: peepcc test-i2p

peepcc:
	$(CC) $(CFLAGS) peepcc.c infix-to-postfix.c stack.c -o peepcc

test-i2p:
	$(CC) $(CFLAGS) test-i2p.c infix-to-postfix.c stack.c -o test-i2p 

clean:
	rm -f *.o *~ peepcc
	rm -f *.o *~ test-i2p
	rm -rf *.dSYM