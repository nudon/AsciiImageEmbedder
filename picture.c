#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "picture.h"
#include "region.h"
#include "characters.h"


int fontSize;

void setFontSize(int new) {
  if (new > 0) {
    fontSize = new;
  }
}

int getFontSize() {
  return fontSize;
}

int formatSpaceX;
int formatSpaceY;

void setSpaceX(int new) {
  formatSpaceX = new;
}

void setSpaceY(int new) {
  formatSpaceY = new;
}

char inputFileName[pathLen];

char outputFileName[pathLen];

char fontName[pathLen];

char fontPath[pathLen];

void setInputFile(char* file) {
  strcpy(inputFileName, file);
}

char* getInputFile() {
  return inputFileName;
}

void setOutputFile(char* file) {
  strcpy(outputFileName, file);
}

char* getOutputFile() {
  return outputFileName;
}
//had some conflict with using setFont in QtCreator
void mySetFont(char* newFont) {
  setFont(newFont);
}

void setFont(char* newFont) {
  char* fontDir = "./fonts/";
  memcpy(fontPath, fontDir, strlen(fontDir) + 1);
  //strcat(fontPath, fontDir);
  strcat(fontPath, newFont);
}

char* getFont() {
  return fontPath;
}


colorMatrix* generateColorMatrix(char* fileName, int fontWidth, int fontHeight) {
  colorMatrix* entireImage = NULL;
  MagickWand* birch = NewMagickWand();
  MagickBooleanType status;
  MagickSetFirstIterator(birch);
  status = MagickReadImage(birch, fileName);
  if (status != MagickFalse) {
    scaleImageToFitFont(birch, fontWidth, fontHeight);
    entireImage = readWandIntoColorMatrix(birch, NULL);
  }
  DestroyMagickWand(birch);
  return entireImage;
}

image* generateImage(colorMatrix* entireImage, int fontWidth, int fontHeight) {
  int regionWidth = fontWidth;
  int regionHeight = fontHeight;
  int imageWidth = entireImage->cols;
  int imageHeight = entireImage->rows;
  int diffParam, regionsx, regionsy;
  image* pic = NULL;
  if (getAutoGenColorScale()) {
    autoSetColorComponentScale(entireImage);
  }
  
  //for image
  diffParam = averageCompareResults(entireImage);
  setDiffParam(diffParam);
  regionsx = (imageWidth - formatSpaceX) / (regionWidth + formatSpaceX);
  regionsy = (imageHeight - formatSpaceY) / (regionHeight + formatSpaceY);
  pic = readColorMatrixIntoImage(entireImage, regionsx, regionsy, regionWidth, regionHeight);
  generateLightMarkScoresForImage(pic);
  generateLightMarkScoresForImage(pic);
  return pic;
}




void getFontDim(char* fontToUse, int size, int* fontWidth, int* fontHeight) {
    MagickBooleanType status;
    PixelWand* white = NewPixelWand();
    PixelSetColor(white, "white");
    MagickWand* staff = NewMagickWand();
    DrawingWand* creator = NewDrawingWand();
    status =  MagickNewImage(staff, 50,50, white);
    assert(status != MagickFalse && "blew up at new Image");
    status = DrawSetFont(creator, fontToUse);
    assert(status != MagickFalse && "blew up setting font");
    DrawSetFontSize(creator, size);
    status = PixelSetColor(white, "black");
    //for text color
    DrawSetFillColor(creator, white);

    double *fm = NULL;
    char* str = "M";
  
    fm = MagickQueryFontMetrics(staff, creator, str);
    *fontWidth = fm[0];  //maybe use fm[9] - fm[7]
    *fontHeight = fm[1];  //maybe use fm[10] - fm[8]

    DestroyPixelWand(white);
    DestroyDrawingWand(creator);
    DestroyMagickWand(staff);
    RelinquishMagickMemory(fm);
}

void scaleImageToFitFont(MagickWand* staff, int fontw, int fonth) {
  //how to scale the image up/down for good tiling?
  //just have iterative ideas for scaling image up enough to tile ImgX % fontX == 0
  //then check if ImgY % fontY == 0
  //think it will eventually converge so both are true
  //but that's just a vague feeling
  int imageWidth, imageHeight, remainingWidth, remainingHeight;
  imageWidth = (int)MagickGetImageWidth(staff);
  imageHeight = (int)MagickGetImageHeight(staff);
  remainingWidth = (fontw - (imageWidth % fontw)) % fontw;
  remainingHeight = (fonth - (imageHeight % fonth)) % fonth;
  //currently this just stretches the image to fit dim
  //so while loop is escessive, but w/e
  while(remainingHeight != 0 || remainingWidth != 0) {
    imageWidth = (int)MagickGetImageWidth(staff);
    imageHeight = (int)MagickGetImageHeight(staff);
    remainingWidth = (fontw - (imageWidth % fontw)) % fontw;
    remainingHeight = (fonth - (imageHeight % fonth)) % fonth; 
    if (remainingHeight != 0) {
      //scale image so new image height = imageHeight + remainingHeight
      MagickScaleImage(staff, imageWidth, imageHeight + remainingHeight);
    }
    else if (remainingWidth != 0) {
      //scale image so new width is width + rem width
      MagickScaleImage(staff, imageWidth + remainingWidth, imageHeight);
    }
  }
}




image* readColorMatrixIntoImage(colorMatrix* entireImage, int regCols, int regRows, int regWidth, int regHeight) {

  image* pic = newImage(entireImage);
  pic->filledCharacterss = newCharacterMatrix(regCols, regRows);
  pic->numberOfRegionRows = regRows;
  pic->numberOfRegionCols = regCols;
  profileMatrix* aProfile = NULL;
  colorMatrix* regionColors = NULL;
  myColor* aColor = NULL;
  myColor* average;
  int numPixels = regHeight * regWidth;
  //then read pixels into myColor and divide picture into regions.
  profileMatrix*** profiles = malloc(sizeof(profileMatrix*) * regRows);
  pic->profiles = profiles;
  for(int regy = 1; regy <= regRows; regy++) {
    pic->profiles[regy - 1] = malloc(sizeof(profileMatrix*) * regCols);
    for(int regx = 1; regx <= regCols; regx++) {
      pic->profiles[regy - 1][regx - 1] = NULL;
      regionColors = newColorMatrix(regWidth, regHeight);
      average = newColor();
      for(int suby = 0; suby < regHeight; suby++) {
	for(int subx = 0; subx < regWidth; subx++) {
	  aColor = getColor(entireImage,
			    (regx - 1) * (regWidth + formatSpaceX) + subx,
			    (regy - 1) * (regHeight + formatSpaceY) + suby);
	  
	  //copy into region
	  setColor(regionColors, subx, suby, aColor);
	  //start computing average
	  addColorToColor(average, aColor);	  
	}
      }
      aProfile = newProfileMatrix(regionColors);
      divideColor(average, numPixels);
      aProfile->averageColor = average;
      pic->profiles[regy - 1][regx - 1] = aProfile;
    }
  }
  return pic;
}

colorMatrix* readWandIntoColorMatrix(MagickWand* staff, colorMatrix* toReadTo) {
  //assume staff already has an Image
  PixelWand* aPixel = NewPixelWand();
  int height = (int)MagickGetImageHeight(staff);
  int width = (int)MagickGetImageWidth(staff);
  colorMatrix* colors;
  if (toReadTo != NULL) {
    colors = toReadTo;
  }
  else {
    colors = newColorMatrix(width, height);
  }
  myColor* color = newColor();
  for(int colIndex = 0; colIndex < width; colIndex++) {
    for(int rowIndex = 0; rowIndex < height; rowIndex++) {
      MagickGetImagePixelColor(staff, colIndex, rowIndex, aPixel);
      shovePixelWandIntoMyColor(aPixel, color);
      setColor(colors, colIndex, rowIndex, color);
    }
  }
  free(color);
  DestroyPixelWand(aPixel);
  return colors;
}

void matchImageToCharacters(image* pic, characterSet* characterSet) {
  character* val;
  int regionCols = pic->numberOfRegionCols;
  int regionRows = pic->numberOfRegionRows;
  for(int regy = 1; regy <= regionRows; regy++) {
    for(int regx = 1; regx <= regionCols; regx++) {
      val = matchProfileToCharacter(pic->profiles[regy - 1][regx - 1],
				    characterSet);
      pic->filledCharacterss[regy - 1][regx - 1] = val;
      printf("%s", val->charVal);
    }
    printf("\n");
  }
}


void imgInit() {
  MagickWandGenesis();
}

void imgQuit() {
  MagickWandTerminus();
}


colorMatrix* readFileIntoColorMatrix(char* fileName) {
  MagickWand* birch = NewMagickWand();
  MagickSetFirstIterator(birch);
  MagickReadImage(birch, fileName);

  return readWandIntoColorMatrix(birch, NULL);
}


void shovePixelWandIntoMyColor(PixelWand* aPixel, myColor* color) {
  //normalizeToNonnormalize
  int ntn = 255;
  //the get value return a normalized value (between 0-1)
  //restore them to full size
  PixelGetHSL(aPixel, &(color->hue), &(color->sat), &(color->lightness));
  //unsure whether to use regular getColor or getColorQuantum
  color->red = PixelGetRed(aPixel);
  color->green = PixelGetGreen(aPixel);
  color->blue =  PixelGetBlue(aPixel);
  color->hue *= ntn;
  color->sat *= ntn;
  color->lightness *= ntn;
  color->red *= ntn;
  color->green *= ntn;
  color->blue *= ntn;
  //thats it
}

myColor* calculateAverageColor(colorMatrix* colorMatrix) {
  myColor* average = newColor();
  int rows = colorMatrix->rows;
  int cols = colorMatrix->cols;
  int numPixels = rows * cols;
  for(int rowIndex = 0; rowIndex < rows; rowIndex++) {
    for(int colIndex = 0; colIndex < cols; colIndex++) {
      addColorToColor(average, getColor(colorMatrix, colIndex, rowIndex));
    }
  }
  divideColor(average, numPixels);
  return average;
}




void drawPicToDisk(image* pic, char* font, int fs) {
  MagickWand* staff = NewMagickWand();
  DrawingWand* creator = NewDrawingWand();
  DrawSetFont(creator, font);
  DrawSetFontSize(creator, fs);
  PixelWand* black = NewPixelWand();
  PixelSetColor(black, "black");
  DrawSetFillColor(creator, black);
  PixelWand* white = NewPixelWand();
  PixelSetColor(white, "white");
  MagickSetFirstIterator(staff);
  MagickWand* test = NewMagickWand();
  MagickNewImage(test, 50,50,white);
  char* str = "M";
  double * fm = MagickQueryFontMetrics(test, creator, str);
  int fontX = fm[0];
  int fontY = fm[1];
  //  MagickNewImage(staff, pic->width, pic->height, white);
  MagickNewImage(staff, pic->numberOfRegionCols * fontX, pic->numberOfRegionRows * fontY, white);
  DrawSetTextEncoding(creator, "UTF-8");
  profileMatrix* aProfile;
  //some issues with grouping up characters into a single annotate
  //with grouping into a single annotate, not sure if newlines would be correct
  //just going to od a single row then
  //for that, need to copy contents into a single char*
  //spacing on characters is off,
  //also getting some garbage at the end of lines
  //will probably have to look at annotate image options for setting up monospace
  //though maybe that's just a property of fonts.
  //dejavusansmono looks better, but there's still garbage at end of line
  //that's cuz I'm not null terminating line.
  //got that sorted
  char line[pic->numberOfRegionCols * sizeOfIMCharCode];
  char* charVal;
  int lineIndex, len;
  for (int rowIndex = 0; rowIndex < pic->numberOfRegionRows; rowIndex++) {
    lineIndex = 0;
    for (int colIndex = 0; colIndex < pic->numberOfRegionCols; colIndex++) {
      charVal = pic->filledCharacterss[rowIndex][colIndex]->charVal;
      len = strlen(charVal);
      memcpy(&line[lineIndex], charVal, len);
      lineIndex += len;
      aProfile = pic->profiles[rowIndex][colIndex];
            MagickAnnotateImage(staff,
			  creator,
			  colIndex * aProfile->cols,
			  rowIndex * aProfile->rows,
			  0,
			  charVal);
     
     //printf("Drawing a thing");
    }
    //char* lines = "hello everyone nice to meet you";
    aProfile = pic->profiles[rowIndex][0];
    line[lineIndex] = '\0';
    /*MagickAnnotateImage(staff,
			creator,
			0,
			rowIndex * aProfile->rows,
			0,
			line);   
    */
    //printf("\n");
  }
  MagickWriteImage(staff, outputFileName);
  DestroyPixelWand(white);
  DestroyPixelWand(black);
  DestroyMagickWand(staff);
  DestroyMagickWand(test);
  DestroyDrawingWand(creator);
  RelinquishMagickMemory(fm);
}






