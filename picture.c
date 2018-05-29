#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "picture.h"
#include "region.h"
#include "characters.h"


//accounted for outputing to terminal
//just need some options to set the terminal spacing
//set to zero would turn it off
//also some options for setting param based on entire image or individual profiles

//also, issue of darker regions getting represented as whitespace.
//kind of fixed that by doing something. Yeah, started assiging lightness non-zero values. like between .02 and .01
//sometimes that works, sometimes it doesn't


//unicode block ranges



int formatSpaceX;
int formatSpaceY;

void setSpaceX(int new) {
  formatSpaceX = new;
}

void setSpaceY(int new) {
  formatSpaceY = new;
}

char* flagStart = "--";
char optValueDelim = '=';

char* fontSizeOpt = "font-size";
char* fontNameOpt = "font";
char* outputFile = "output";
char* useQuickCalc = "useQuickCalc";
char* autoGenColorScale = "auto";
char* asciiUsedOpt = "asciiUsed";
char* hiraganaUsedOpt = "hiraganaUsed";
char* katakanaUsedOpt = "katakanaUsed";
  
//still in use
char* edgeWeight = "edge";
char* colorWeight = "color";
char* saturationScale = "saturation";
char* lightnessScale = "lightness";
char* hueScale = "hue";
char* spaceX = "spacex";
char* spaceY = "spacey";
//kind of pointless, but why not
char* useAverageReduce = "useAverageReduce";


//these, kind of worthless on smaller-scale things
char* distanceDropoff = "dist";
char* hitWeight = "hitWeight";
char* missWeight = "missWeight";
char* hitDecay = "hitDecay";
char* missDecay = "missDecay";



char* help = "help";

int pathLen = 255;

char* outputFileName;

char* fontName;

void printHelpMessage() {
  fprintf(stderr, "%s=$INT: Controls the size of the font the image gets embedded in\n", fontSizeOpt);
  fprintf(stderr, "%s=$NAME: specifies which font to use in the font folder. need to include extention in\n", fontNameOpt);
  fprintf(stderr, "%s=$NAME: specifies the name of the image to output to in\n",outputFile );
  fprintf(stderr, "%s=$INT: Specifies the in-between character  spacing, so picture looks correct when viewed in application in\n", spaceX);
  fprintf(stderr, "%s=$INT: Specifies the line spacing, so picture looks correct when viewed in application in\n", spaceY);
  fprintf(stderr, "%s=$FLOAT: Sets the weight assigned to edge scores when matching profiles\n", edgeWeight);
  fprintf(stderr, "%s=$FLOAT: Sets the wieght assigned to the average colors of profiles\n", colorWeight);
  fprintf(stderr, "Using HSL color scheme, look at that if this doesn't make sense: \n");
  fprintf(stderr, "%s=$FLOAT: Sets weight assigend to saturation differences when comparing colors\n", saturationScale);
  fprintf(stderr, "%s=$FLOAT: Sets weight assigend to lightness differences when comparing colors\n", lightnessScale);
  fprintf(stderr, "%s=$FLOAT: Sets weight assigend to hue differences when comparing colors\n", hueScale);
  fprintf(stderr, "%s: Option to automatically generate color component scales \n", autoGenColorScale);
}
//also want ones for the color components


int fontSize;

void setFontSize(int new) {
  if (new > 0) {
    fontSize = new;
  }
}

void parseArgs(int argc, char* argv[]) {
  char* token;
  int sizeOfValue = 10;
  char valueText[sizeOfValue];
  int intVal;
  float floatVal;
  for (int i = 1; i < argc; i++) {
    token = argv[i];
    //font size
    if (findAndStoreMatch(token, fontSizeOpt, sizeOfValue, valueText) != -1) {
      intVal = atoi(valueText);
      setFontSize(intVal);
    }
    //font 
    else if (findAndStoreMatch(token, fontNameOpt, pathLen, fontName) != -1) {
      //again, don't do anything
    }
    //space between characters in terminal/text-viewer
    else if (findAndStoreMatch(token, spaceX, sizeOfValue, valueText) != -1) {
      intVal = atoi(valueText);
      setSpaceX(intVal);
    }
    //space between lines in terminal/text-viewer
    else if (findAndStoreMatch(token, spaceY, sizeOfValue, valueText) != -1) {
      intVal = atoi(valueText);
      setSpaceY(intVal);
    }
    //weight assigned to difference in edge scores
    else if (findAndStoreMatch(token, edgeWeight, sizeOfValue, valueText) != -1) {
      floatVal = atof(valueText);
      setEdgeScoreWeight(floatVal);
    }
    //weight assigned to differend in color
    else if (findAndStoreMatch(token, colorWeight, sizeOfValue, valueText) != -1) {
      floatVal = atoi(valueText);
      setColorScoreWeight(floatVal);
    }
    //these deal with weights assigned to components in getPixelDif
    else if (findAndStoreMatch(token, saturationScale, sizeOfValue, valueText) != -1) {
      floatVal = atof(valueText);
      setSaturationScale(floatVal);
    }
    else if (findAndStoreMatch(token, lightnessScale, sizeOfValue, valueText) != -1) {
      floatVal = atof(valueText);
      setLightnessScale(floatVal);
    }
    else if (findAndStoreMatch(token, hueScale, sizeOfValue, valueText) != -1) {
      floatVal = atof(valueText);
      setHueScale(floatVal);
    }
    else if (findAndStoreMatch(token, asciiUsedOpt, sizeOfValue, valueText) != -1) {
      intVal = atoi(valueText);
      setAsciiUsed(intVal);
    }
    else if (findAndStoreMatch(token, hiraganaUsedOpt, sizeOfValue, valueText) != -1) {
      intVal = atoi(valueText);
      setHiraganaUsed(intVal);
    }
    else if (findAndStoreMatch(token, katakanaUsedOpt, sizeOfValue, valueText) != -1) {
      intVal = atoi(valueText);
      setKatakanaUsed(intVal);
    }
    //not really used
    else if (findAndStoreMatch(token, distanceDropoff, sizeOfValue, valueText) != -1) {
      floatVal = atof(valueText);
      setDistanceWeight(floatVal);
    }
    //not really used
    else if (findAndStoreMatch(token, hitWeight, 10, valueText) != -1) {
      floatVal = atof(valueText);
      setHitWeight(floatVal);
    }
    //not really used
    else if (findAndStoreMatch(token, hitDecay, 10, valueText) != -1) {
      floatVal = atof(valueText);
      setHitDecay(floatVal);
    }
    //not really used
    else if (findAndStoreMatch(token, missWeight, 10, valueText) != -1) {
      floatVal = atof(valueText);
      setMissWeight(floatVal);
    }
    //not really used
    else if (findAndStoreMatch(token, missDecay, 10, valueText) != -1) {
      floatVal = atof(valueText);
      setMissDecay(floatVal);
    }
    //specifying output file
    else if (findAndStoreMatch(token, outputFile, pathLen, outputFileName) != -1)  {
      //don't do anything?
    }
    else if (matchFlag(token, autoGenColorScale)) {
      setAutoGenColorScale(1);
    }
    else if (matchFlag(token, useQuickCalc)) {
      setUseQuick(1);
    }
    else if (matchFlag(token, useAverageReduce)) {
      setUseAverageReduce(1);
    }
    else if (matchFlag(token, help)) {
      printHelpMessage();
      exit(EXIT_SUCCESS);
    }    
    
  }
}

int matchFlag(char* token, char* flag ) {
  return (match(token, flagStart) && match(token, flag));
}

int findAndStoreMatch(char* token, char* search, int resultSize, char* result) {
  int ret = -1;
  if (match(token, flagStart) && match(token, search)) {
    ret = indexOfChar(token, optValueDelim);
    if (ret != -1) {
      strncpy(result, token + ret + 1, resultSize);
    }
  }
  return ret;
}

int match(char* src, char* search) {
  int srcLen = strlen(src);
  int searchLen = strlen(search);
  int currentIndex = 0;
  int srcIndex = 0;
  int searchIndex = 0;
  int done = 0;
  if (srcLen >= searchLen) {
    while(!done) {
      srcIndex = currentIndex;
      if(srcIndex == searchLen) {
	return 0;
      }
      while (src[srcIndex] == search[searchIndex] ) {
	if (searchIndex + 1 == searchLen) {
	  return 1;
	}
	else if (srcIndex + 1 == srcLen) {
	  return 0;
	}
	else {
	  srcIndex++;
	  searchIndex++;
	}
      }
      currentIndex++;
    }
  }
  return 0;
}

int indexOfChar(char* src, char search) {
  int ret = -1;
  int srcLen = strlen(src);
  int index = 0;
  while(ret == -1 && index < srcLen) {
    if (src[index] == search) {
      ret = index;
    }
    index++;
  }
  return ret;
}

int main(int argc, char * argv[]) {
  outputFileName = malloc(sizeof(char) * pathLen);
  fontName = malloc(sizeof(char) * pathLen);
  char* fileName;
  char fontToUse[pathLen];
  char* fontDir = "./fonts/";
  char* defaultFontName = "comicSans.ttf";
  memcpy(fontToUse, fontDir, pathLen);
  //have damase and unifont as utf-8 things
  //DejaVu sans mono is also an option
  //mona font is also nice if using sjis, though that is spooky because not monospace?
  memcpy(fontName, defaultFontName, strlen(defaultFontName));
  fontName[strlen(defaultFontName)] = '\0';
  //fontToUse = "mono";
  
  int fontWidth;
  int fontHeight;
  int imageWidth;
  int imageHeight;
  int regionWidth;
  int regionHeight;
  setFontSize(10);
  setSpaceX(0); // 1 on my terminal
  setSpaceY(0); // 3 on my terminal
  setUseQuick(0);
  setUseAverageReduce(0);
  setAsciiUsed(1);
  setHiraganaUsed(0);
  setKatakanaUsed(0);
  setEdgeScoreWeight(1);
  setColorScoreWeight(0);
  setSaturationScale(0);
  setLightnessScale(1);
  setHueScale(1);
  setDistanceWeight(1);
  setHitDecay(.2);
  setMissDecay(.8);
  setHitWeight(1);
  setMissWeight(1);
  strcpy(outputFileName, "output.jpg");
  if (argc > 1) {
    fileName = argv[1];
    imgInit();
    parseArgs(argc, argv);
    memcpy(fontToUse + strlen(fontDir), fontName, strlen(fontName));
    fontToUse[strlen(fontDir) + strlen(fontName)] = '\0';
    printf("Using font found at %s\n", fontToUse);
    //first, get metrics of font
    MagickBooleanType status;
    PixelWand* white = NewPixelWand();
    PixelSetColor(white, "white");
    MagickWand* staff = NewMagickWand();
    DrawingWand* creator = NewDrawingWand();
    status =  MagickNewImage(staff, 50,50, white);
    assert(status != MagickFalse && "blew up at new Image");
    status = DrawSetFont(creator, fontToUse);
    assert(status != MagickFalse && "blew up setting font");
    DrawSetFontSize(creator, fontSize);
    status = PixelSetColor(white, "black");
    //for text color
    DrawSetFillColor(creator, white);

    double *fm = NULL;
    char* str = "M";
  
    fm = MagickQueryFontMetrics(staff, creator, str);
    fontWidth = fm[0];  //maybe use fm[9] - fm[7]
    fontHeight = fm[1];  //maybe use fm[10] - fm[8]
      
    regionWidth = fontWidth;
    regionHeight = fontHeight;
    printf("Regions dimension are: %d , %d\n", regionWidth, regionHeight);
    MagickWand* birch = NewMagickWand();
    MagickSetFirstIterator(birch);
    status = MagickReadImage(birch, fileName);
    if (status != MagickFalse) {
      scaleImageToFitFont(birch, fontWidth, fontHeight);
      imageWidth = (int)MagickGetImageWidth(birch);
      imageHeight = (int)MagickGetImageHeight(birch);
      
      colorMatrix* entireImage = readWandIntoColorMatrix(birch, NULL);
      if (getAutoGenColorScale()) {
	autoSetColorComponentScale(entireImage);
      }
      int diffParam = averageCompareResults(entireImage);
      setDiffParam(diffParam);
      int regionsx = (imageWidth - formatSpaceX) / (regionWidth + formatSpaceX);
      int regionsy = (imageHeight - formatSpaceY) / (regionHeight + formatSpaceY);
      image* pic = readColorMatrixIntoImage(entireImage, regionsx, regionsy, regionWidth, regionHeight);
      characterSet* charSet = buildCharacterSet(fontToUse, fontWidth, fontHeight, fontSize);
      matchImageToCharacters(pic, charSet);
      fprintf(stderr, "writing picture  %s on disk\n", outputFileName);
      drawPicToDisk(pic, fontToUse, fontSize);
      freeImage(pic);
      freeCharacterSet(charSet);
      freeColorMatrix(entireImage);
    }
    else {
      printf("Unable to open file %s\n", fileName);
    }
    DestroyPixelWand(white);
    DestroyDrawingWand(creator);
    DestroyMagickWand(staff);
    DestroyMagickWand(birch);
    RelinquishMagickMemory(fm);
    imgQuit();
  }
  else {
    fprintf(stderr, "Usage is \"%s\" fileName (opt)fontSize (opt)pixleThing (opt) distanceDecay (opt)edgeWeight (opt)colorWeight\n", argv[0]);
  }
  
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
  for (int rowIndex = 0; rowIndex < pic->numberOfRegionRows; rowIndex++) {
    for (int colIndex = 0; colIndex < pic->numberOfRegionCols; colIndex++) {
      aProfile = pic->profiles[rowIndex][colIndex];
      MagickAnnotateImage(staff,
			  creator,
			  colIndex * aProfile->cols,
			  rowIndex * aProfile->rows,
			  0,
			  pic->filledCharacterss[rowIndex][colIndex]->charVal);
      //printf("Drawing a thing");
    }
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






