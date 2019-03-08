#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "cli_driver.h"
#include "picture.h"
#include "region.h"
#include "characters.h"




colorMatrix* entireImage;
image* pic;
characterSet* charSet;  

int main(int argc, char * argv[]) {
  //outputFileName = malloc(sizeof(char) * pathLen);
  //fontName = malloc(sizeof(char) * pathLen);
  char* fileName;
  float fontWidth;
  float fontHeight;
  int regionWidth;
  int regionHeight;
  setDefaultOpts();
  char* fontPath;
  char* outputFileName;
  int fontSize;

  if (argc > 1) {
    fileName = argv[1];
    libInit();
    parseArgs(argc, argv);
    fontPath = getFont();
    outputFileName = getOutputFile();
    fontSize = getFontSize();
    printf("Using font found at %s\n",fontPath );
    //for colormatrix
    //first, get metrics of font
    charSet = buildCharacterSet(fontPath, fontSize);
    fontWidth = charSet->avgWidth;
    fontHeight = charSet->avgHeight;
    regionWidth = ceil(fontWidth);
    regionHeight = ceil(fontHeight);
    printf("Regions dimension are: %d , %d\n", regionWidth, regionHeight);
    //read filename into a colormatrix
    MagickWand* entireImage = mem_light_generateColorMatrix(fileName, regionWidth, regionHeight);
    
    //colorMatrix* entireImage = generateColorMatrix(fileName, regionWidth, regionHeight);
    if (entireImage != NULL) {
      pic = mem_light_generateImage(entireImage, regionWidth, regionHeight);
      DestroyMagickWand(entireImage);
      
      //pic = generateImage(entireImage, regionWidth, regionHeight);
      //freeColorMatrix(entireImage);
      //test(pic);
      matchImageToCharacters(pic, charSet);
      fprintf(stderr, "writing picture  %s on disk\n", outputFileName);
      drawPicToDisk(pic, charSet);

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
