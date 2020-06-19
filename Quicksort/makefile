# CC     = gcc
# TARGET = quicksort
# CFLAGS = -g -Wall -Werror -pedantic-errors

# all: quicksort

# quicksort: sort.o quicksort.o
# 	$(CC) -o $(quicksort) sort.o quicksort.o

# sort.o: sort.c quicksort.h
# 	$(CC) $(CFLAGS) -c sort.c quicksort.h

# quicksort.o: quicksort.c quicksort.h
# 	$(CC) $(CFLAGS) -c quicksort.c quicksort.h

# clean:
# 	rm -f $(TARGET) $(TARGET).exe

sort: sort.o quicksort.o
	gcc -g sort.o quicksort.o -o sort 
sort.o: sort.c
	gcc -g -Wall -Werror -pedantic-errors -c sort.c 
quicksort.o: quicksort.c quicksort.h
	gcc -g -Wall -Werror -pedantic-errors -c quicksort.c quicksort.h
clean:
	rm -f *.o sort sort.exe
