# /*******************************************************************************
#  * Name        : pfind.c
#  * Author      : Mark Pipko
#  * Date        : Feburary 27, 2020
#  * Description : This program will recursively search for files whose permissions match the permissions string starting in the specified directory.
#  * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
#  ******************************************************************************/

CC     = gcc
C_FILE = $(wildcard *.c)
TARGET = $(patsubst %.c,%,$(C_FILE))
CFLAGS = -g -Wall -Werror -pedantic-errors

all:
	$(CC) $(CFLAGS) $(C_FILE) -o $(TARGET)
clean:
	rm -f $(TARGET) $(TARGET).exe
