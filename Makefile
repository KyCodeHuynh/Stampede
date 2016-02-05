# Makefile for the Stampede web server
# Ky-Cuong L. Huynh
# 22 January 2016

NAME = Stampede
TEST_NAME = Test-Stampede
CLIENT_NAME = Client-Stampede
CC = gcc
FLAGS = -std=c99 -Wall -Wextra

SRC = server.c simple_http.c utility.c
CLIENTSRC = client.c
TESTSRC = simple_http_test.c simple_http.c

all: server
	
server:
	$(CC) $(FLAGS) -o $(NAME) $(SRC)

client:
	$(CC) $(FLAGS) -o $(CLIENT_NAME) $(CLIENTSRC)

test:
	$(CC) $(FLAGS) -ggdb -o $(TEST_NAME) $(TESTSRC)
	./$(TEST_NAME)

clean:
	rm -rf *.o a.out $(NAME) $(TEST_NAME)
