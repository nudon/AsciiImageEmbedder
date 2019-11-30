#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "options.h"
#include "picture.h"
#include "region.h"
#include "characters.h"


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
  fprintf(stderr, "--%s=$INT: Controls the size of the font the image gets embedded in\n", fontSizeOpt);
  fprintf(stderr, "--%s=$NAME: specifies which font to use in the font folder. need to include extention in\n", fontNameOpt);
  fprintf(stderr, "--%s=$NAME: specifies the name of the image to output to in\n",outputFile );
  fprintf(stderr, "--%s=$INT: Specifies the in-between character  spacing, so picture looks correct when viewed in application in\n", spaceX);
  fprintf(stderr, "--%s=$INT: Specifies the line spacing, so picture looks correct when viewed in application in\n", spaceY);
  fprintf(stderr, "--%s=$FLOAT: Sets the weight assigned to edge scores when matching profiles\n", edgeWeight);
  fprintf(stderr, "--%s=$FLOAT: Sets the wieght assigned to the average colors of profiles\n", colorWeight);
  fprintf(stderr, "Using HSL color scheme, look at that if this doesn't make sense: \n");
  fprintf(stderr, "--%s=$FLOAT: Sets weight assigend to saturation differences when comparing colors\n", saturationScale);
  fprintf(stderr, "--%s=$FLOAT: Sets weight assigend to lightness differences when comparing colors\n", lightnessScale);
  fprintf(stderr, "--%s=$FLOAT: Sets weight assigend to hue differences when comparing colors\n", hueScale);
  fprintf(stderr, "--%s: Option to automatically generate color component scales \n", autoGenColorScale);
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
      fprintf(stderr, "Setting font size to %d\n", intVal);
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
  setSpaceX(1); // 1 on my terminal
  setSpaceY(3); // 3 on my terminal
  setUseQuick(0);
  setUseAverageReduce(0);
  setAsciiUsed(1);
  setHiraganaUsed(0);
  setKatakanaUsed(0);
  setEdgeScoreWeight(1);
  setColorScoreWeight(1);
  setSaturationScale(1);
  setLightnessScale(1);
  setHueScale(0);
  setDistanceWeight(1);
  setHitDecay(.2);
  setMissDecay(.8);
  setHitWeight(1);
  setMissWeight(1);
  //strcpy(outputFileName, "output.jpg");
  setOutputFile("output.jpg");
  setFont("DejaVuSansMono.ttf");
}
