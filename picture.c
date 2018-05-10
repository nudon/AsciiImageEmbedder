#include <stdio.h>
#include "region.h"

//next up, have to handle the bigger picture stuff
//getting a picture read in to begin with
//dividing picture into sections, generating colorMatrix
//sticking that in a profile matrix, calling generate edges
//then also do all of that for rasters of whatever characters I want
//sticking those into some array
//then calling  closestCharToProfile for every section of image

void main(int argc, char * argv[]) {
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
	    aColor = NULL;
	    //copy into region
	    setColor(regionColors, subx, suby, aColor);
	    
	  }
	}
	aProfile = newProfile(regionColors);
	pic->profiles[regy][regx] = aProfile;
      }
    }

    //then send profiles to fillDiffMatrix
    //then send DiffMatrix and profile to findEdges
    //then hand edge to closestCharToProfile
    //then assign char some position in an array.
    //then print array
  }
  else {
    //not enough args
    fprintf(stderr, "Give a file name");
  }
}

myColor* newColor() {
  myColor* new = malloc(sizeof(myColor));

  return new;
}

colorMatrix* newMatrix(int rows, int cols) {
  colorMatrix* new = malloc(sizeof(colorMatrix));
  new->rows = rows;
  new->cols = cols;
  new->cells = malloc(sizeof(myColor**) * cols);
  for(int colIndex = 0; colIndex < cols; colIndex++) {
    new->cells[colIndex] = malloc(sizeof(myColor*) * rows);
    for(int rowIndex =0; rowIndex < rows; rowIndex++) {
      new->cells[colIndex][rowIndex] = newColor();
      
    }
  }
  return new;
}

void setColor(colorMatrix* matrix, int row, int col, myColor* tobe) {
  if (row < matrix->rows && col < matrix->cols) {
    cloneColor(matrix[col][row], tobe);
  }
  else {
    fprintf(stderr, "Given a bad index for colorMatrix in setColor\n");
  }
}

void cloneColor(myColor* dest, myColor* src) {
  //copy value of fields in src to dest
}

void newProfileMatrix(colorMatrix* colors) {
  profileMatrix* new = malloc(sizeof(profileMatrix));
  new->source = colors;
  new->rows = colors->rows;
  new->cols = colors->cols;
  
  new->cells = malloc(sizeof(int**) * cols);
  for(int colIndex = 0; colIndex < cols; colIndex++) {
    new->cells[colIndex] = malloc(sizeof(int*) * rows);
    for(int rowIndex =0; rowIndex < rows; rowIndex++) {
      new->cells[colIndex][rowIndex] = malloc(sizeof(int));
      //
      *(new->cells[colIndex][rowIndex]) = 0;
    }
  }
  return new;
}
