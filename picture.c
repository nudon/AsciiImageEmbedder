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

int getSpaceX() {
  return formatSpaceX;
}

int getSpaceY() {
  return formatSpaceY;
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

//had some naming conflict with using setFont in QtCreator
//aslo font dir has a different path relative to binary
void mySetFont(char* newFont) {
  char* fontDir = "../../fonts/";
  memcpy(fontPath, fontDir, strlen(fontDir) + 1);
  strcat(fontPath, newFont);
  fprintf(stderr, "font in %s\n", fontPath);
}

char* myGetFont() {
  return getFont();
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

void scaleImageToFitFont(MagickWand* staff, float fontw, float fonth) {
  //currently this just stretches the image to fit dim
  float imageWidth, imageHeight, remainingWidth, remainingHeight;
  imageWidth = MagickGetImageWidth(staff);
  imageHeight = MagickGetImageHeight(staff);
  remainingWidth = fmodf(fontw - (fmodf(imageWidth, fontw)),fontw);
  remainingHeight = fmodf(fonth - (fmodf(imageHeight, fonth)),fonth);
  if (remainingHeight != 0 || remainingWidth != 00) {
    MagickScaleImage(staff, imageWidth + remainingWidth, imageHeight + remainingHeight);
  }  
}

colorMatrix* generateColorMatrix(char* fileName, float regionWidth, float regionHeight) {
  colorMatrix* entireImage = NULL;
  MagickWand* birch = NewMagickWand();
  MagickBooleanType status;
  MagickSetFirstIterator(birch);
  status = MagickReadImage(birch, fileName);
  if (status != MagickFalse) {
    scaleImageToFitFont(birch, regionWidth, regionHeight);
    entireImage = readWandIntoColorMatrix(birch, NULL);
  }
  DestroyMagickWand(birch);
  return entireImage;
}

image* generateImage(colorMatrix* entireImage, int regionWidth, int regionHeight) {
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
  //regionsx = (imageWidth) / (regionWidth);
  //regionsy = (imageHeight) / (regionHeight);
  pic = readColorMatrixIntoImage(entireImage, regionsx, regionsy, regionWidth, regionHeight);
  generateLightMarkScoresForImage(pic);
  return pic;
}

image* readColorMatrixIntoImage(colorMatrix* entireImage, int regCols, int regRows, int regWidth, int regHeight) {

  image* pic = newImage(entireImage);
  pic->filledCharacterss = newCharacterMatrix(regCols, regRows);
  pic->numberOfRegionRows = regRows;
  pic->numberOfRegionCols = regCols;
  profileMatrix* aProfile = NULL;
  colorMatrix* regionColors = NULL;
  myColor* aColor = NULL;
  profileMatrix*** profiles = malloc(sizeof(profileMatrix*) * regRows);
  pic->profiles = profiles;
  for(int regy = 1; regy <= regRows; regy++) {
    pic->profiles[regy - 1] = malloc(sizeof(profileMatrix*) * regCols);
    for(int regx = 1; regx <= regCols; regx++) {
      pic->profiles[regy - 1][regx - 1] = NULL;
      regionColors = newColorMatrix(regWidth, regHeight);
      for(int suby = 0; suby < regHeight; suby++) {
	for(int subx = 0; subx < regWidth; subx++) {
	  
	  aColor = getColor(entireImage,
			    (regx - 1) * (regWidth + formatSpaceX) + subx,
			    (regy - 1) * (regHeight + formatSpaceY) + suby);
	  
	  setColor(regionColors, subx, suby, aColor);
	}
      }
      aProfile = generateProfileFromColor(regionColors);
      
      pic->profiles[regy - 1][regx - 1] = aProfile;
    }
  }
  return pic;
}

colorMatrix* readWandIntoColorMatrix(MagickWand* staff, colorMatrix* toReadTo) {
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
      val = closestCharacterToProfile(pic->profiles[regy - 1][regx - 1], characterSet);
      pic->filledCharacterss[regy - 1][regx - 1] = val;
      //printf("%s", val->charVal);
    }
    //printf("\n");
  }
}


void libInit() {
  MagickWandGenesis();
  init_FT();
}

void libQuit() {
  MagickWandTerminus();
  close_FT();
}


void shovePixelWandIntoMyColor(PixelWand* aPixel, myColor* color) {
  //the get value return a normalized value (between 0-1)
  //restore them to standard value range [0,255]
  int ntn = 255;
  double h,s,l;
  PixelGetHSL(aPixel, &h, &s, &l);
  color->red = PixelGetRed(aPixel) * ntn;
  color->green = PixelGetGreen(aPixel) * ntn;
  color->blue =  PixelGetBlue(aPixel) * ntn;
  color->hue  = h * ntn;
  color->sat = s * ntn;
  color->lightness = l * ntn;
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



void drawPicToDisk(image* pic, characterSet* charSet) {
  char* font = charSet->font;
  int fs = charSet->fontSize;
  float fontX, fontY;
  int cols, rows, imgDimX, imgDimY;
  MagickWand* img = NewMagickWand();
  DrawingWand* drawer = NewDrawingWand();
  PixelWand* black = NewPixelWand();
  PixelWand* white = NewPixelWand();
  DrawSetFont(drawer, font);
  DrawSetFontSize(drawer, fs);
  PixelSetColor(black, "black");
  DrawSetFillColor(drawer, black);
  PixelSetColor(white, "white");
  MagickSetFirstIterator(img);
  fontX = charSet->avgWidth;
  fontY = charSet->avgHeight;
  cols = pic->numberOfRegionCols;
  rows = pic->numberOfRegionRows;
  imgDimX = cols * (fontX + (float)getSpaceX());
  imgDimY = rows * (fontY + (float)getSpaceY());
  MagickNewImage(img, imgDimX, imgDimY, white);
  DrawSetTextEncoding(drawer, "UTF-8");
  //slowDraw(pic, img, drawer);
  superFastDraw(pic, img, drawer);
  MagickWriteImage(img, outputFileName);
  DestroyPixelWand(white);
  DestroyPixelWand(black);
  DestroyMagickWand(img);
  DestroyDrawingWand(drawer);
}

void superFastDraw (image* pic, MagickWand* staff, DrawingWand* drawer){
  //fast, but you can't set font spacing and can't force fonts to behave as monospace
  int rows = pic->numberOfRegionRows;
  int cols = pic->numberOfRegionCols;
  char line[rows *( cols * sizeOfIMCharCode + 1) + 1];
  int lineIndex, len;
  int fontHeight = pic->height / pic->numberOfRegionRows;
  //prevents first row of text from getting cut off
  int yStart = (fontHeight + 1 + getSpaceY());
  char* charVal;
  DrawSetTextInterlineSpacing(drawer, getSpaceY() - 1);
  DrawSetTextInterwordSpacing(drawer, 0);
  DrawSetTextKerning(drawer, getSpaceX());
  lineIndex = 0;

  for (int rowIndex = 0; rowIndex < pic->numberOfRegionRows; rowIndex++) {
    for (int colIndex = 0; colIndex < pic->numberOfRegionCols; colIndex++) {
      charVal = pic->filledCharacterss[rowIndex][colIndex]->charVal;
      len = strlen(charVal);
      memcpy(&line[lineIndex], charVal, len);
      lineIndex += len;
    }
    line[lineIndex++] = '\n';    
  }
  line[lineIndex++] = '\0';
  MagickAnnotateImage(staff,
		      drawer,
		      0,
		      yStart,
		      0,
		      line);   
}


void slowDraw(image* pic, MagickWand* staff, DrawingWand* drawer) {
  //much slower due to individualy rendering each character(instead of rendering entire lines)
  //only use is if you want to pretend that certain fonts are mono-space
  //or want to use color rendering which is cool
  //or if wasted cpu cycles are your primary source of heat
  char* charVal;
  PixelWand* color = NewPixelWand();
  PixelWand* purp = NewPixelWand();
  PixelSetColor(purp, "purple");
  myColor* col;
  int format_size = 32;
  char format[format_size];
  
  int rows = pic->numberOfRegionRows;
  int cols = pic->numberOfRegionCols;
  int spaceY = getSpaceY();
  int spaceX = getSpaceX();
  int fontW = 0, fontH = 0;
  profileMatrix* p;
  fontW = pic->profiles[0][0]->cols;
  fontH = pic->profiles[0][0]->rows;
  for (int rowIndex = 0; rowIndex < rows; rowIndex++) {
    
    for (int colIndex = 0; colIndex < cols; colIndex++) {
      charVal = pic->filledCharacterss[rowIndex][colIndex]->charVal;
      p = getProfile(pic, rowIndex, colIndex);
      col = p->averageColor;
      snprintf(format, format_size, "rgb(%i,%i,%i)", col->red, col->green, col->blue);

      PixelSetColor(color, format);
      DrawSetFillColor(drawer, color);
      DrawRectangle(drawer,
		    (fontW + spaceX) * colIndex,
		    (fontH + spaceY) * rowIndex,
		    (fontW + spaceX) * (colIndex + 1),
		    (fontH + spaceY) * (rowIndex + 1));

      /*
      DrawSetFillColor(drawer, purp);
      
      DrawAnnotation(drawer,
		     colIndex * ( spaceX + fontW),
		     (rowIndex + 1) * ( spaceY + fontH),
		     (const unsigned char*)charVal);
      printf("%i out of %i regions donw\n", rowIndex * rows + colIndex, rows * cols);
      */
    }
  }
  MagickDrawImage(staff, drawer);

}

//working on less memory intensive solutions
//these will only keep a single section/region of colors in memory at a time
//not perfect, as imageMagick, specifically it's c api
//has to load in entire image to a MagickWand
//however their representation is much more compact then mine
MagickWand* mem_light_generateColorMatrix(char* fileName, int regionWidth, int regionHeight) {
  MagickWand* birch = NewMagickWand();
  MagickBooleanType status;
  MagickSetFirstIterator(birch);
  status = MagickReadImage(birch, fileName);
  if (status != MagickFalse) {
    scaleImageToFitFont(birch, regionWidth, regionHeight);
  }
  else {
    birch = NULL;
  }
  return birch;
}

//
image* mem_light_generateImage(MagickWand* imgWand, int regionWidth, int regionHeight) {
  int imageWidth = (int)MagickGetImageWidth(imgWand);
  int imageHeight = (int)MagickGetImageHeight(imgWand);
  
					
  int regionsx, regionsy;
  image* pic = NULL;
  
  
  //here's the regions that occur when you skip over some pixels
  regionsx = (imageWidth - formatSpaceX) / (regionWidth + formatSpaceX);
  regionsy = (imageHeight - formatSpaceY) / (regionHeight + formatSpaceY);
  //here is what should be for when the spacing is purely in output
  //regionsx = (imageWidth) / (regionWidth);
  //regionsy = (imageHeight) / (regionHeight);
  pic = mem_light_readColorMatrixIntoImage(imgWand, regionsx, regionsy, regionWidth, regionHeight);
  generateLightMarkScoresForImage(pic);
  return pic;
}



image* mem_light_readColorMatrixIntoImage(MagickWand* imgWand, int regCols, int regRows, int regWidth, int regHeight) {
  //this function, needs to read in or be passed in MagickWand w/ image loaded in
  //iterate over it once, compute the average color diffs and average color
  //then iterate over sections, copy into colorMatrix, build the diff matrixes
  //then computing the edge scores
  //then reusing the colorMatrix and also diff matrixes for the next section

  PixelWand* aPixel = NewPixelWand();
  int height = (int)MagickGetImageHeight(imgWand);
  int width = (int)MagickGetImageWidth(imgWand);
  image* pic = newImageByDim(width, height);
  pic->filledCharacterss = newCharacterMatrix(regCols, regRows);
  pic->numberOfRegionRows = regRows;
  pic->numberOfRegionCols = regCols;
  myColor* color = newColor();
  myColor* average = newColor();
  *average = (myColor){.hue = 0, .sat = 0, .lightness = 0, .red =0, .green = 0, .blue = 0};
  //so, first traversal, keep track of average color, and also the average diff score
  myColor* current = newColor();
  myColor* compare = newColor();
  long numPixels = height * width;
  int testC, testR, testNumber, offC, offR;
  long results = 0;
  int numRuns = 0;
  //printf("Traversing entire picture, getting color and diff vals\n");
  for(int colIndex = 0; colIndex < width; colIndex++) {
    for(int rowIndex = 0; rowIndex < height; rowIndex++) {
      MagickGetImagePixelColor(imgWand, colIndex, rowIndex, aPixel);
      shovePixelWandIntoMyColor(aPixel, color);
      //keep track of some average
      addColorToColor(average, color);
      
      MagickGetImagePixelColor(imgWand, colIndex, rowIndex, aPixel);
      shovePixelWandIntoMyColor(aPixel, current);
      //compute the average compare result
      testNumber = 1;
      while(testNumber > 0) {
	half_neighbor_traverse(&testNumber, colIndex, rowIndex, &offC, &offR);
	testC = colIndex + offC;
	testR = rowIndex + offR;
	MagickGetImagePixelColor(imgWand, testC, testR, aPixel);
	shovePixelWandIntoMyColor(aPixel, compare);
	//eventually...
	if (compare != NULL) {
	  results += getPixelDif(current, compare);
	  numRuns++;
	}
	testNumber++;
      }
    }
  }
  double avgComp = ((double)results)/numRuns;
  setDiffParam(avgComp);
  
  divideColor(average, numPixels);
  
  //second traversal, this time building colormatrix, diffMatrix and edgescores
  colorMatrix* section = newColorMatrix(regWidth, regHeight);
  intMatrix* diff = createIntMatrixByDim(regWidth,regHeight);
  int rowI;
  int colI;
  numPixels = regHeight * regWidth;
  profileMatrix* aProfile = NULL;
  profileMatrix*** profiles = malloc(sizeof(profileMatrix*) * regRows);
  pic->profiles = profiles;
  for(int regy = 1; regy <= regRows; regy++) {
    pic->profiles[regy - 1] = malloc(sizeof(profileMatrix*) * regCols);
    for(int regx = 1; regx <= regCols; regx++) {
      for(int suby = 0; suby < regHeight; suby++) {
	//rowI = (regy - 1) * (regHeight) + suby;
	rowI = (regy - 1) * (regHeight + formatSpaceY) + suby;
	for(int subx = 0; subx < regWidth; subx++) {
	  //colI = (regx - 1) * (regWidth) + subx;
	  colI = (regx - 1) * (regWidth + formatSpaceX) + subx;
	  MagickGetImagePixelColor(imgWand, colI, rowI, aPixel);
	  shovePixelWandIntoMyColor(aPixel, color);
	  
	  setColor(section, subx, suby, color);
	}
      }
      aProfile = mem_light_generateProfileFromColor(section, diff);

      pic->profiles[regy - 1][regx - 1] = aProfile;
    }
  }
  
  free(color);
  free(average);
  DestroyPixelWand(aPixel);
  freeColorMatrix(section);
  freeIntMatrix(diff);

  return pic;
}
