#include <stdlib.h>
#include <stdio.h>
//uinsg #include <wand/MagickWand.h> wasn't working
#include <ImageMagick-7/MagickWand/MagickWand.h>
#include "region.h"

//next up, have to handle the bigger picture stuff
//getting a picture read in to begin with
//dividing picture into sections, generating colorMatrix
//sticking that in a profile matrix, calling generate edges
//then also do all of that for rasters of whatever characters I want
//sticking those into some array
//then calling  closestCharToProfile for every section of image

//think I got some rough code for everything but reading and generating profiles of characters
//ideally I don't have to manaully rip individual characters as images
//ideally I just pick a font, and some function will scan in everything.
//seems like imageMagick had some base text-writing features
//would use that to open up some temp file
//clear screen, draw the character
//analyze region & build profile
//then repeat on a different character
//uses the drawAnotation, which is restricted to to taking chars as input
//looked at drawSetTextEncoding, which if called with "UTF-8" as arg
//expects input to be in form of unicode-bit-sequences.
//also apparantly one can type \xyz and that escapes as unicode
//also would seem that I have to use monospace font, If I actually want to have recognizable output textno

//also, just realized that I'm not accounting for the spaces between lines or characters.
//might not matter, if they do would have to do some more calculations. Essentially find out how big those gaps are
//then keep that in mind when tiling the original picture into subregions. 
double diffThreshHold;

void imgConvInit();

void imgConvClose();

colorMatrix* readFileIntoColorMatrix(char* fileName);

double averageCompareResults(colorMatrix* colors);

int main(int argc, char * argv[]) {
  char* fileName;
  char* fontToUse;

  int imageWidth;
  int imageHeight;
  int regionWidth;
  int regionHeight;
  int remainingWidth;
  int remainingHeight;

  int fontWidth;
  int fontHeight;
  //maybe just use ratios, because vector fonts
  
  
  if (argc > 1) {
    fileName = argv[1];

    image* pic = malloc(sizeof(image));
    //load file name using whatever image library I'm using
    
    //figure out how to tile the image using regions of aspect ratio == font
    //likely going to have some hanging/excess.
    // regionWidth = something
    // regionHeight = something
    
    
    //how to scale the image up/down for good tiling?
    //just have iterative ideas for scaling image up enough to tile ImgX % fontX == 0
    //then check if ImgY % fontY == 0
    //think it will eventually converge so both are true
    //but that's just a vague feeling
    remainingWidth = imageWidth % regionWidth;
    remainingHeight = imageHeight % regionHeight;
    while(remainingHeight != 0 || remainingWidth != 0) {
      if (remainingHeight != 0) {
	//scale image so new image height = imageHeight + remainingHeight
      }
      if (remainingWidth != 0) {
	//scale image so new width is width + rem width
      }
 
    }

    //will probably load rescaled image?
    colorMatrix* entireImage = readFileIntoColorMatrix(fileName);
    setDiffParam(averageCompareResults(entireImage));
    //set imageWidth/height to new values
    //if it doesn't work then just treat the excess region as something special
    //such that it won't show up as any edge
    //could also just stretch image to required dimensions.
    int regionsx = imageWidth / regionWidth;
    int regionsy = imageHeight / regionHeight;

    profileMatrix* aProfile;
    colorMatrix* regionColors;
    myColor* aColor;
    //then read pixels into myColor and divide picture into regions.
    pic->profiles = malloc(sizeof(profileMatrix**) * regionsy);
    for(int regy = 1; regy * regionHeight <= imageHeight; regy++) {
      pic->profiles[regy] = malloc(sizeof(profileMatrix*) * regionsx); 
      for(int regx = 1; regx * regionWidth <= imageWidth; regx++) {
	regionColors = newColorMatrix(regionWidth, regionHeight);
	for(int suby = 0; suby < regionHeight; suby++) {
	  for(int subx = 0; subx < regionWidth; subx++) {
	    //grab pixel at (x = ((regx - 1) * regionWidth) + subx,
	    //               y = ((regy - 1) * regionHeight) + suby)
	    //store in aColor
	    aColor = getColor(entireImage,
			      (regy - 1) * regionHeight + suby,
			      (regx - 1) * regionWidth + subx);
			      
	    //copy into region
	    setColor(regionColors, subx, suby, aColor);
	    
	  }
	}
	aProfile = newProfileMatrix(regionColors);
	pic->profiles[regy][regx] = aProfile;
      }
    }
    char val;
    for(int regy = 1; regy * regionHeight <= imageHeight; regy++) {
      for(int regx = 1; regx * regionWidth <= imageWidth; regx++) {
	val = closestCharToProfile(pic->profiles[regy][regx], NULL);
	printf("%c", val);
      }
      printf("\n");
    }
	
    //then send profiles to fillDiffMatrix
    //then send DiffMatrix and profile to findEdges
    //then hand edge to closestCharToProfile
    //then assign char some position in an array.
    //then print array
  }
  else {
    //not enough args
    fprintf(stderr, "Give a file name\n");
  }
}




void imgConvInit() {
  MagickWandGenesis();
}

void imgConvClose() {
  MagickWandTerminus();
}


colorMatrix* readFileIntoColorMatrix(char* fileName) {
  MagickWand* birch = NewMagickWand();
  PixelWand* aPixel;
  MagickSetFirstIterator(birch);
  MagickReadImage(birch, fileName);

  int height = (int)MagickGetImageHeight(birch);
  int width = (int)MagickGetImageWidth(birch);
  colorMatrix* colors = newColorMatrix(height, width);
  myColor* color = newColor();
  for(int colIndex = 0; colIndex < width; colIndex++) {
    for(int rowIndex = 0; colIndex < height; rowIndex++) {
      MagickGetImagePixelColor(birch, colIndex, rowIndex, aPixel);
      PixelGetHSL(aPixel, &(color->hue), &(color->sat), &(color->lightness));
      //unsure whether to use regular getColor or getColorQuantum
      color->red = PixelGetRed(aPixel);
      color->green = PixelGetGreen(aPixel);
      color->blue =  PixelGetBlue(aPixel);
      setColor(colors, rowIndex, colIndex, color);
    }
  }
  free(color);
  return colors;
}

double averageCompareResults(colorMatrix* colors) {

  //basically copy-pasted code from fillDiffMatrix
  //if there's any bugs in one, there's bugs in the other  
  int rows = colors->rows;
  int cols = colors->cols;
    int testC, testR, testNumber;
  myColor* current, *compare;
  int results;
  int numRuns;
  for (int colIndex = 0; colIndex < cols; colIndex++) {
    for (int rowIndex = 0; rowIndex < rows; rowIndex++) {
      current = getColor(colors, colIndex, rowIndex);
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
	compare = getColor(colors, testC, testR);
	//eventually...
	if (compare != NULL) {
	  results = getPixelDif(current, compare);
	  numRuns++;
	}
	testNumber++;
      }            
    }
  }
  return results/numRuns;
}


character* makecharacter(char value) {
  character* new = malloc(sizeof(character));
  
}
