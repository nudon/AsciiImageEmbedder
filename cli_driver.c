#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "cli_driver.h"
#include "picture.h"
#include "region.h"
#include "characters.h"


//thinking how to speed up
//most of time in bigger pictures is in rendering to a picture
//thinking it's because I'm doing that one character at a time
//breaking it down into rendering by rows, or even all at once, should be quicker

//for future ideas, could extend to image collages, essentially replacing characters with other pictures
//could extend to make a gui deal so it's easeir to use. qt4/5 seems good, seems like i'll need to write some stuff in c++
//should still be able to have most of my code in c, just figure out how to link thins in compiler stage
//could add things for gifs as well(would want to speed up drawing to image process)

//also, realized that mona doesn't cover all of hiragana blocks. It's probable that when I'm building the character set I can draw each character and determine through return code if there was some error. If so, then I could exluce the character from further use. Otherwise solving font-specific gaps is impossible

colorMatrix* entireImage;
image* pic;
characterSet* charSet;  

int main(int argc, char * argv[]) {
  //outputFileName = malloc(sizeof(char) * pathLen);
  //fontName = malloc(sizeof(char) * pathLen);
  char* fileName;
  int fontWidth;
  int fontHeight;
  int regionWidth;
  int regionHeight;
  setDefaultOpts();
  char* fontPath = getFont();
  char* outputFileName = getOutputFile();
  int fontSize = getFontSize();

  if (argc > 1) {
    fileName = argv[1];
    libInit();
    parseArgs(argc, argv);
    printf("Using font found at %s\n",fontPath );
    //for colormatrix
    //first, get metrics of font
    getFontDim(fontPath,fontSize, &fontWidth, &fontHeight);      
    regionWidth = fontWidth;
    regionHeight = fontHeight;
    printf("Regions dimension are: %d , %d\n", regionWidth, regionHeight);
    //read filename into a colormatrix
    entireImage = generateColorMatrix(fileName, fontWidth, fontHeight);
    if (entireImage != NULL) {
      pic = generateImage(entireImage, fontWidth, fontHeight);
      //characterset
      charSet = buildCharacterSet(fontPath, fontWidth, fontHeight, fontSize);
      matchImageToCharacters(pic, charSet);
      fprintf(stderr, "writing picture  %s on disk\n", outputFileName);
      drawPicToDisk(pic, fontPath, fontSize);
      freeImage(pic);
      freeCharacterSet(charSet);
    }
    else {
      printf("Unable to open file %s\n", fileName);
    }

    libQuit();
  }
  else {
    fprintf(stderr, "Usage is \"%s\" fileName (opt)fontSize (opt)pixleThing (opt) distanceDecay (opt)edgeWeight (opt)colorWeight\n", argv[0]);
  }
}


//parsing functions

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


void parseArgs(int argc, char* argv[]) {
  char* token;
  int sizeOfValue = 10;
  char valueText[sizeOfValue];
  char fileName[pathLen];
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
    else if (findAndStoreMatch(token, fontNameOpt, pathLen, fileName) != -1) {
      setFont(fileName);
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
    else if (findAndStoreMatch(token, outputFile, pathLen, fileName) != -1)  {
      setOutputFile(fileName);
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


void setDefaultOpts() {
  setFontSize(10);
  setSpaceX(0); // 1 on my terminal
  setSpaceY(0); // 3 on my terminal
  setUseQuick(0);
  setUseAverageReduce(0);
  setAsciiUsed(1);
  setHiraganaUsed(0);
  setKatakanaUsed(0);
  setEdgeScoreWeight(1);
  setColorScoreWeight(1);
  setSaturationScale(0);
  setLightnessScale(1);
  setHueScale(0);
  setDistanceWeight(1);
  setHitDecay(.2);
  setMissDecay(.8);
  setHitWeight(1);
  setMissWeight(1);
  //strcpy(outputFileName, "output.jpg");
  setOutputFile("output.jpg");
  setFont("comicSans.ttf");
}
