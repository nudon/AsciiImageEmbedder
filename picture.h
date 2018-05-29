#ifndef FILE_PICTURE_SEEN
#define FILE_PICTURE_SEEN

#include <MagickWand/MagickWand.h>
#include "dataStructures.h"

int indexOfChar(char* src, char search);
int match(char* src, char* search);
int matchFlag(char* token, char* flag );

void setFontSize(int new);

void setFontName(char* new);

void imgInit();

void imgQuit();

void scaleImageToFitFont(MagickWand* staff, int fontw, int fonth);

image* readColorMatrixIntoImage(colorMatrix* entireImage, int regCols, int regRows, int regWidth, int regHeight);

colorMatrix* readFileIntoColorMatrix(char* fileName);

colorMatrix* readWandIntoColorMatrix(MagickWand* staff, colorMatrix* toReadTo);

myColor* calculateAverageColor(colorMatrix* colorMatrix);

void matchImageToCharacters(image* pic, characterSet* characterSet);

void shovePixelWandIntoMyColor(PixelWand* aPixel, myColor* color);

int findAndStoreMatch(char* token, char* search, int resultSize, char* result);

void drawPicToDisk(image* pic, char* font, int fs);

#endif
