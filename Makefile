all: castaway exec clear

castaway: castaway.o
				gcc -ansi -Wall -o castaway castaway.c -lpthread

exec: 
			./castaway

clear: 
			rm -rf castaway
			rm -rf castaway.o