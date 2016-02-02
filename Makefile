# Makefile for the Stampede web server
# Ky-Cuong L. Huynh
# 22 January 2016

NAME = Stampede
TEST_NAME = Client-Stampede
CC = gcc
FLAGS = -std=c99 -Wall -Wextra

SRC = server.c simple_http.c utility.c
TESTSRC = client.c

all: server test
	
server:
	$(CC) $(FLAGS) -o $(NAME) $(SRC)

test:
	$(CC) $(FLAGS) -o $(TEST_NAME) $(TESTSRC)

clean:
	rm -rf *.o a.out $(NAME) $(TEST_NAME)
