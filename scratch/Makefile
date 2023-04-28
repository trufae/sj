#  ___  ___
# |  _||   |  the Small Interpreter
# |_  | | |
# |___||___|
#


all:
	gcc -g -o si si.c
	echo '{a=1;print(a+2);}' | ./si | grep 3
	echo '{a=1024;print(a-2);}' | ./si | grep 1022
	echo '{a="Hello world";}' | SI_AST=1 ./si | grep 'STR "Hell'
	echo '{print("Hello world");}' | ./si | grep ^Hello
	echo '{system("ls ./");}' | ./si | grep Makefile
	echo '{/* comment here */print(/* or there */ "Hello");}' | ./si | grep Hello
	echo '{/* nested /* comments */ */print("nested");}' | ./si | grep nested
# echo '{foo=(num, msg){print("pop");foo(32)}' | ./si
	echo '{print(32); // oneline comment \n \
		print(64);}' | ./si | grep 64

DESTDIR?=
PREFIX?=/usr/local
BINDIR=$(PREFIX)/bin

install:
	ln -fs $(shell pwd)/si $(DESTDIR)/$(BINDIR)/si
