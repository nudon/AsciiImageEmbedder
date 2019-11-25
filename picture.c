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
//had some conflict with using setFont in QtCreator
//also have to go back a few directeries because the executable is in ./qt/build
//was trying to figure out how to just have makefile produce executable in same dir as base askii folder

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


colorMatrix* generateColorMatrix(char* fileName, int regionWidth, int regionHeight) {
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
  fprintf(stderr, "spacing: x is %d, y is %d\n", formatSpaceX, formatSpaceX);
  regionsx = (imageWidth - formatSpaceX) / (regionWidth + formatSpaceX);
  regionsy = (imageHeight - formatSpaceY) / (regionHeight + formatSpaceY);
  regionsx = (imageWidth) / (regionWidth);
  regionsy = (imageHeight) / (regionHeight);
  pic = readColorMatrixIntoImage(entireImage, regionsx, regionsy, regionWidth, regionHeight);
  generateLightMarkScoresForImage(pic);
  return pic;
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
  //so while loop is escessive
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
  imageWidth = (int)MagickGetImageWidth(staff);
  imageHeight = (int)MagickGetImageHeight(staff);
  //fprintf(stderr, "dims at end of scaling function: width is %d, height is %d\n", imageWidth, imageHeight);
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
	  /*
	  aColor = getColor(entireImage,
			    (regx - 1) * (regWidth + formatSpaceX) + subx,
			    (regy - 1) * (regHeight + formatSpaceY) + suby);
	  */
	  aColor = getColor(entireImage,
			    (regx - 1) * (regWidth) + subx,
			    (regy - 1) * (regHeight) + suby);
	  
	  //copy into region
	  setColor(regionColors, subx, suby, aColor);
	  //start computing average
	  addColorToColor(average, aColor);	  
	}
      }
      aProfile = newProfileMatrix(regionColors);
      divideColor(average, numPixels);
      aProfile->averageColor = average;
      aProfile->diff = createIntMatrix(aProfile);
      fillDiffMatrix(aProfile->diff, aProfile, -1);
      aProfile->edgeScores = calculateEdgeScores(aProfile);
      
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
  double h,s,l;
  PixelGetHSL(aPixel, &h, &s, &l);
  //unsure whether to use regular getColor or getColorQuantum
  color->red = PixelGetRed(aPixel) * ntn;
  color->green = PixelGetGreen(aPixel) * ntn;
  color->blue =  PixelGetBlue(aPixel) * ntn;
  color->hue  = h * ntn;
  color->sat = s * ntn;
  color->lightness = l * ntn;
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



void drawPicToDisk(image* pic, characterSet* charSet) {
  char* font = charSet->font;
  int fs = charSet->fontSize;
  MagickWand* img = NewMagickWand();
  DrawingWand* drawer = NewDrawingWand();
  DrawSetFont(drawer, font);
  DrawSetFontSize(drawer, fs);
  PixelWand* black = NewPixelWand();
  PixelSetColor(black, "black");
  DrawSetFillColor(drawer, black);
  PixelWand* white = NewPixelWand();
  PixelSetColor(white, "white");
  MagickSetFirstIterator(img);
  float fontX;
  float fontY;
  //no, bad, do integer truncation later in caluclations
  //fontX = ceil(charSet->avgWidth);
  //fontY = ceil(charSet->avgHeight);
  fontX = charSet->avgWidth;
  fontY = charSet->avgHeight;
  int spacingX = getSpaceX();
  int spacingY = getSpaceY();
  int cols = pic->numberOfRegionCols;
  int rows = pic->numberOfRegionRows;
  int imgDimX = cols * (fontX + (float)spacingX);
  int imgDimY = (rows + 1) * (fontY + (float)spacingY);
  //imgDimX = cols * (fontX);
  //imgDimY = rows * (fontY);
  //  MagickNewImage(staff, pic->width, pic->height, white);
  //fprintf(stderr, "dims in image struct: width is %d, height is %d\n", pic->width, pic->height );
  //fprintf(stderr, "dims used in drawing function: width is %d, height is %d\n", imgDimX, imgDimY);
  MagickNewImage(img, imgDimX, imgDimY, white);
  DrawSetTextEncoding(drawer, "UTF-8");
  slowDraw(pic, img, drawer);
  //fastDraw(pic, img, drawer);
  //superFastDraw(pic, img, drawer);
  MagickWriteImage(img, outputFileName);
  DestroyPixelWand(white);
  DestroyPixelWand(black);
  DestroyMagickWand(img);
  DestroyDrawingWand(drawer);
}

void superFastDraw (image* pic, MagickWand* staff, DrawingWand* drawer){
  //fast, but you can't set font spacing and can't force fonts to behave as monospace
  //
  int rows = pic->numberOfRegionRows;
  int cols = pic->numberOfRegionCols;
  char line[rows *( cols * sizeOfIMCharCode + 1) + 1];
  int lineIndex, len;
  int fontHeight = pic->height / pic->numberOfRegionRows;
  //fixes drawing to image so first row isn't cut off
  int yStart = (fontHeight + 1 + getSpaceY());
  
  DrawSetTextInterlineSpacing(drawer, getSpaceY());
  DrawSetTextInterwordSpacing(drawer, 0);
  DrawSetTextKerning(drawer, getSpaceX());
  char* charVal;
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

void fastDraw(image* pic, MagickWand* staff, DrawingWand* drawer) {
  //fast, but you can't set font spacing and can't force fonts to behave as monospace
  //
  char line[pic->numberOfRegionCols * sizeOfIMCharCode];
  int lineIndex, len;
  int rows = pic->numberOfRegionRows;
  int cols = pic->numberOfRegionCols;
  int spaceY = getSpaceY();
  int spaceX = getSpaceX();
  int fontW = 0, fontH = 0;
  fontW = pic->profiles[0][0]->cols;
  fontH = pic->profiles[0][0]->rows;
  DrawSetTextInterlineSpacing(drawer, spaceY);
  DrawSetTextInterwordSpacing(drawer, 0);
  DrawSetTextKerning(drawer, spaceX);
  char* charVal;
  for (int rowIndex = 0; rowIndex < rows; rowIndex++) {
    lineIndex = 0;
    for (int colIndex = 0; colIndex < cols; colIndex++) {
      charVal = pic->filledCharacterss[rowIndex][colIndex]->charVal;
      len = strlen(charVal);
      memcpy(&line[lineIndex], charVal, len);
      lineIndex += len;
    }
    //line[lineIndex++] = '\n';
    line[lineIndex++] = '\0';
    MagickAnnotateImage(staff,
			drawer,
			0,
			rowIndex * (fontH + spaceY),
			0,
			line);   
    
  }
}

void slowDraw(image* pic, MagickWand* staff, DrawingWand* drawer) {
  //much slower due to individualy rendering each character(instead of rendering entire lines)
  //only use is if you want to pretend that certain fonts are mono-space
  //or if wasted cpu cycles are your primary source of heat
  char* charVal;
  PixelWand* color = NewPixelWand();
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
	snprintf(format, format_size, "rgb(%i,%i,%i)", col->red, col->blue, col->green);
	
	PixelSetColor(color, format);
	DrawSetFillColor(drawer, color);
	MagickAnnotateImage(staff,
			    drawer,
			    colIndex * ( spaceX + fontW),
			    (rowIndex + 1) * ( spaceY + fontH),
			    0,
			    charVal);
	
      }
    }
}

//working on less memory intensive solutions
//these will only keep a single section/region of colors in memory at a time
//not perfect, as imageMagick, specifically it's c api
//has to load in entire image to memory.
//however their representation is much more compact then mine

//just scales image and returns wand
MagickWand* mem_light_generateColorMatrix(char* fileName, int regionWidth, int regionHeight) {
  colorMatrix* entireImage = NULL;
  MagickWand* birch = NewMagickWand();
  MagickBooleanType status;
  MagickSetFirstIterator(birch);
  status = MagickReadImage(birch, fileName);
  if (status != MagickFalse) {
    scaleImageToFitFont(birch, regionWidth, regionHeight);
    //entireImage = readWandIntoColorMatrix(birch, NULL);
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
  
					
  int diffParam, regionsx, regionsy;
  image* pic = NULL;
  
  //thinking of doing this in readColorMatrixIntoImage
  //so in one function, just have two loops iterating over pixels of some image
  //diffParam = averageCompareResults(entireImage);
  //setDiffParam(diffParam);
  //also this region calculation may be wrong(?)
  //this was for old way of doing formatSpacing,
  //where I would skip over some pixels to match empty space lines
  //mismatching how I set the number of regions and how I actually grab pixels
  //could result in cropped images
  //here's the regions that occur when you skip over some pixels
  regionsx = (imageWidth - formatSpaceX) / (regionWidth + formatSpaceX);
  regionsy = (imageHeight - formatSpaceY) / (regionHeight + formatSpaceY);
  //here is what should be for when the spacing is purely in output
  regionsx = (imageWidth) / (regionWidth);
  regionsy = (imageHeight) / (regionHeight);
  fprintf(stderr, "x regions: %d , y regions: %d\n", regionsx, regionsy);
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
  int testC, testR, testNumber;
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
	if (testNumber == 1) {
	  //pixel to right
	  testC = colIndex + 1;
	  testR = rowIndex;
	}
	else if (testNumber == 2) {
	  //pixel diagnol bottom right
	  testR = rowIndex + 1;
	}
	else if (testNumber == 3) {
	  //pixel below
	  testC = colIndex;
	}
	else if (testNumber == 4) {
	  //pixel diagnol bottom left
	  testC = colIndex - 1;
	  //...
	  //don't set to zero,  I increment testNumber 
	  testNumber = -10;
	}
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
  //printf("done traversing entire picture\n\n");
  double avgComp = ((double)results)/numRuns;
  //printf("Here's the diff thing: %f\n", avgComp);
  setDiffParam(avgComp);
  
  divideColor(average, numPixels);
  
  //second traversla, this time building colormatrix, diffMatrix and edgescores
  colorMatrix* section = newColorMatrix(regWidth, regHeight);
  intMatrix* diff = createIntMatrixByDim(regWidth,regHeight);
  int rowI;
  int colI;
  //setting to negative one means use the globally defined diffParam
  //which was set above
  int locDiffParam = -1;
  numPixels = regHeight * regWidth;
  profileMatrix* aProfile = NULL;
  profileMatrix*** profiles = malloc(sizeof(profileMatrix*) * regRows);
  pic->profiles = profiles;
  for(int regy = 1; regy <= regRows; regy++) {
    pic->profiles[regy - 1] = malloc(sizeof(profileMatrix*) * regCols);
    for(int regx = 1; regx <= regCols; regx++) {
      *average = (myColor){.hue = 0, .sat = 0, .lightness = 0, .red =0, .green = 0, .blue = 0};
      //printf("Reading part of image in to a section\n");
      for(int suby = 0; suby < regHeight; suby++) {
	rowI = (regy - 1) * (regHeight) + suby;
	//rowI = (regy - 1) * (regHeight + formatSpaceY) + suby;
	for(int subx = 0; subx < regWidth; subx++) {
	  colI = (regx - 1) * (regWidth) + subx;
	  //colI = (regx - 1) * (regWidth + formatSpaceX) + subx;
	  MagickGetImagePixelColor(imgWand, colI, rowI, aPixel);
	  shovePixelWandIntoMyColor(aPixel, color);
	  
	  setColor(section, subx, suby, color);
	  addColorToColor(average, color);
	}
      }
      //printf("done reading section\n\n");
      //went over an entire section, build diffMatrix and generate edgescores
      //also have to deal with average color for section
      aProfile = newProfileMatrix(section);
      fillDiffMatrix(diff, aProfile, locDiffParam);
      //printf("calculating edge scores\n");
      edges* foundEdges = calculateEdgeScores(aProfile);
      //printf("done calculating edge scores\n\n");
      aProfile->edgeScores = foundEdges;
      //set diff/color sources to null so bad things don't happen when I free
      aProfile->diff = NULL;
      aProfile->source = NULL;
      
      divideColor(average, numPixels);
      myColor* newAverage = newColor();
      *newAverage = *average;
      aProfile->averageColor = newAverage;

      //finally set profile
      pic->profiles[regy - 1][regx - 1] = aProfile;
    }
  }
  
  //after I'm done with second traversal...
  free(color);
  free(average);
  DestroyPixelWand(aPixel);
  freeColorMatrix(section);
  freeIntMatrix(diff);

  return pic;
}
