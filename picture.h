#ifndef FILE_PICTURE_SEEN
#define FILE_PICTURE_SEEN

#include <MagickWand/MagickWand.h>
#include "dataStructures.h"

#define pathLen 255

void setFontSize(int new);

int getFontSize();

void setSpaceX(int new);

void setSpaceY(int new);

int getSpaceX();

int getSpaceY();

void setInputFile(char* file);

char* getInputFile();

void setOutputFile(char* file);

char* getOutputFile();

void setFont(char* newFont);

void mySetFont(char* newFont);

char* getFont();

colorMatrix* generateColorMatrix(char* fileName, float fontWidth, float fontHeight);

image* generateImage(colorMatrix* entireImage, int fontWidth, int fontHeight);

void scaleImageToFitFont(MagickWand* staff, float fontw, float fonth);

image* readColorMatrixIntoImage(colorMatrix* entireImage, int regCols, int regRows, int regWidth, int regHeight);

colorMatrix* readWandIntoColorMatrix(MagickWand* staff, colorMatrix*);

void matchImageToCharacters(image* pic, characterSet* characterSet);

void libInit();

void libQuit();

colorMatrix* readFileIntoColorMatrix(char* fileName);

void shovePixelWandIntoMyColor(PixelWand* aPixel, myColor* color);

myColor* calculateAverageColor(colorMatrix* colorMatrix);

void drawPicToDisk(image* pic, characterSet* charSet);

void superFastDraw (image* pic, MagickWand* staff, DrawingWand* drawer);

void fastDraw(image* pic, MagickWand* staff, DrawingWand* drawer);

void slowDraw(image* pic, MagickWand* staff, DrawingWand* drawer);

MagickWand* mem_light_generateColorMatrix(char* fileName, int regionWidth, int regionHeight);

//
image* mem_light_generateImage(MagickWand* imgWand, int regionWidth, int regionHeight);


//also need to hand in some MagickWand with img loaded into it
image* mem_light_readColorMatrixIntoImage(MagickWand* imgWand, int regCols, int regRows, int regWidth, int regHeight);



#endif
