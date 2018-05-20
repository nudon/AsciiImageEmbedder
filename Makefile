CC = gcc
MATH_CONFIG = "-lm"
IMG_MAGICK_CONFIG = `pkg-config --cflags --libs MagickWand`
TOT_CONFIG = $(MATH_CONFIG) $(IMG_MAGICK_CONFIG)
COMP_FLAGS = -Wall -g

all: picture.o region.o dataStructures.o
	$(CC) $(COMP_FLAGS) -o askii picture.o region.o dataStructures.o  $(TOT_CONFIG)	

picture.o: picture.c region.h
	$(CC) $(COMP_FLAGS)  -c -o $@ $< $(IMG_MAGICK_CONFIG)	

region.o: region.c region.h
	$(CC) $(COMP_FLAGS) -c -o $@ $< $(MATH_CONFIG)

dataStructures.o: dataStructures.c dataStructures.h
	$(CC) $(COMP_FLAGS) -c -o $@ $< 


clear:
	rm *.o
