# Mark Pipko and Joshua Mimer
# I pledge my honor that I have abided by the Stevens Honor System. 

CC     = gcc
C_FILE = $(wildcard *.c)
TARGET = $(patsubst %.c,%,$(C_FILE))
CFLAGS = -O3 -Wall -Werror -pedantic-errors   

all:
	$(CC) $(CFLAGS) $(C_FILE) -o $(TARGET) -pthread -lm 
clean:
	rm -f $(TARGET) $(TARGET).exe
