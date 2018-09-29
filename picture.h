#ifndef FILE_PICTURE_SEEN
#define FILE_PICTURE_SEEN

#include <MagickWand/MagickWand.h>
#include "dataStructures.h"

#define pathLen 255

void setFontSize(int new);

int getFontSize();

void setSpaceX(int new);

void setSpaceY(int new);

void setInputFile(char* file);

char* getInputFile();

void setOutputFile(char* file);

char* getOutputFile();

void setFont(char* newFont);

void mySetFont(char* newFont);

char* getFont();

colorMatrix* generateColorMatrix(char* fileName, int fontWidth, int fontHeight);

image* generateImage(colorMatrix* entireImage, int fontWidth, int fontHeight);

void getFontDim(char* fontToUse, int size, int* fontWidth, int* fontHeight);

void scaleImageToFitFont(MagickWand* staff, int fontw, int fonth);

image* readColorMatrixIntoImage(colorMatrix* entireImage, int regCols, int regRows, int regWidth, int regHeight);

colorMatrix* readWandIntoColorMatrix(MagickWand* staff, colorMatrix*);

void matchImageToCharacters(image* pic, characterSet* characterSet);

void imgInit();

void imgQuit();

colorMatrix* readFileIntoColorMatrix(char* fileName);

void shovePixelWandIntoMyColor(PixelWand* aPixel, myColor* color);

myColor* calculateAverageColor(colorMatrix* colorMatrix);

void drawPicToDisk(image* pic, char* font, int fs);

#endif
