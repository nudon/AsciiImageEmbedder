CC = gcc
COMP_FLAGS = -Wall -g


region.o: region.c region.h
	$(CC) $(COMP_FLAGS) -c -o $@ $<


clear:
	rm *.o
