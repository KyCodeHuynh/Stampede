# Makefile for the Stampede web server
# Ky-Cuong L. Huynh
# 22 January 2016

NAME = Stampede
CC = gcc
FLAGS = -std=c99 -Wall -Wextra

SRC = server.c 
TESTSRC = $(SRC) client.c

all:
	$(CC) $(FLAGS) -o $(NAME) $(SRC)

test:
	$(CC) $(FLAGS) -o $(NAME) $(TESTSRC)

clean:
	rm -rf *.o a.out $(NAME)