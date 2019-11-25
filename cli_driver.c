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


image* mem_light_picture(char* filename, int reg_width, int reg_height);

image* regular_picture(char* filename, int reg_width, int reg_height);


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
    pic = regular_picture(fileName, regionWidth, regionHeight);
    //pic = mem_light_picture(fileName, regionWidth, regionHeight;)
    
    matchImageToCharacters(pic, charSet);
    fprintf(stderr, "writing picture  %s on disk\n", outputFileName);
    drawPicToDisk(pic, charSet);
    
    freeImage(pic);
    freeCharacterSet(charSet);
    libQuit();
  }
  else {
    fprintf(stderr, "Usage is \"%s\" fileName (opt)fontSize (opt)pixleThing (opt) distanceDecay (opt)edgeWeight (opt)colorWeight\n", argv[0]);
  }
}

image* mem_light_picture(char* filename, int reg_w, int reg_h) {
  MagickWand* entireImage = mem_light_generateColorMatrix(filename, reg_w, reg_h);
  image* pic = NULL;
  if (entireImage != NULL) {
    pic = mem_light_generateImage(entireImage, reg_w, reg_h);
  }
  DestroyMagickWand(entireImage);
  return pic;
}

image* regular_picture(char* filename, int reg_w, int reg_h) {
  colorMatrix* entireImage = generateColorMatrix(filename, reg_w, reg_h);
  image* pic = NULL;
  if (entireImage != NULL) {
    pic = generateImage(entireImage, reg_w, reg_h);
  }
  freeColorMatrix(entireImage);
  //?
  //test(pic);
  return pic;
}
