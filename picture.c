#include <stdlib.h>
#include <stdio.h>
#include <math.h>
//uinsg #include <wand/MagickWand.h> wasn't working
#include <ImageMagick-7/MagickWand/MagickWand.h>

#include "region.h"


//potential bugs to run into
//realized that I'm not accounting for the spaces between lines or characters.
//might not matter, if they do would have to do some more calculations. Essentially find out how big those gaps are
//then keep that in mind when tiling the original picture into subregions.
//also would seem that I have to use monospace font, If I actually want to have recognizable output textno

char* intToPFUnicode(int intCode);

char* intToIMUnicode(u_int32_t intCode);

void utf8Things();

double diffThreshHold;

void imgInit();

void imgQuit();

void scaleImageToFitFont(MagickWand* staff, int fontw, int fonth);

image* readColorMatrixIntoImage(colorMatrix* entireImage, int regCols, int regRows, int regWidth, int regHeight);

colorMatrix* readFileIntoColorMatrix(char* fileName);

double averageCompareResults(colorMatrix* colors);

colorMatrix* readWandIntoColorMatrix(MagickWand* staff, colorMatrix* toReadTo);

myColor* calculateAverageColor(colorMatrix* colorMatrix);

void matchImageToCharacters(image* pic, character** characterSet);

void shovePixelWandIntoMyColor(PixelWand* aPixel, myColor* color);

character** buildCharacterSet(char* font, int fw, int fh, int fs);

character* buildCharacterOfCodePoint(MagickWand* staff, DrawingWand* creator, colorMatrix* charColors, int intCode);

void drawPicToDisk(image* pic, char* font, int fs);

int asciiStart = 32;
int asciiEnd = 126;
int asciiUsed = 0;

int hiraganaStart = 0x3040;
int hiraganaEnd = 0x309f;
int hiraganaUsed = 0;

int katakanaStart = 0x30a0;
int katakanaEnd = 0x30ff;
int katakanaUsed = 0;

//so, close to finally getting to run this and have infinite segfaults
//things I need to to, implement setting unicode character ranges to use
//setting up generation of characterSets
//figureing out how imageMagick's draw annotaion expects utf input to look like
//also issues of getting aspect ration of font, and setting size of font/regions.

int main(int argc, char * argv[]) {
  //int unicodeQuote = 0x2018;
  //char* anoTemp = "\u2018";
  //printf("%s\n", "\u2018");
  //printf("\u2018\n");
  //work();
  char* fileName;
  char* fontToUse;
  //have damase and unifont as utf-8 things
  //DejaVu sans mono is also an option
  //mona font is also nice if using sjis, though that is spooky because not monospace?
  fontToUse = "Dejavu-san-mono";
  //fontToUse = "mono";


  int fontSize;
  //double fontRatio; //as width/heifht
  //fontRatio = .5;
  int fontWidth;
  int fontHeight;
  //maybe just use ratios, because vector fonts
  int imageWidth;
  int imageHeight;
  int regionWidth;
  int regionHeight;
  
  if (argc > 1) {
    if (argc > 2) {
      fontSize = atoi(argv[2]);
    }
    else {
      fontSize = 20;
    }
    fileName = argv[1];
    imgInit();

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
    assert(status != MagickFalse && "blew up at setting color of pixel");
    //for text color
    DrawSetFillColor(creator, white);

    double *fm = NULL;
    char* str = "M";
  
    fm = MagickQueryFontMetrics(staff, creator, str);
    fontWidth = fm[4];  //maybe use fm[9] - fm[7]
    fontHeight = fm[5];  //maybe use fm[10] - fm[8]

      //
      
    regionWidth = fontWidth;
    regionHeight = fontHeight;
    printf("Regions dimension are: %d , %d\n", regionWidth, regionHeight);
    //load file name using whatever image library I'm using
    MagickWand* birch = NewMagickWand();
    MagickSetFirstIterator(birch);
    status = MagickReadImage(birch, fileName);
    if (status != MagickFalse) {
      scaleImageToFitFont(birch, fontWidth, fontHeight);
      imageWidth = (int)MagickGetImageWidth(birch);
      imageHeight = (int)MagickGetImageHeight(birch);
      
      colorMatrix* entireImage = readWandIntoColorMatrix(birch, NULL);
      setDiffParam(averageCompareResults(entireImage));
      int regionsx = imageWidth / regionWidth;
      int regionsy = imageHeight / regionHeight;
      image* pic = readColorMatrixIntoImage(entireImage, regionsx, regionsy, regionWidth, regionHeight);
      asciiUsed = 1;
      character** characterSet = buildCharacterSet(fontToUse, fontWidth, fontHeight, fontSize);
      matchImageToCharacters(pic, characterSet);
      drawPicToDisk(pic, fontToUse, fontSize);
      freeImage(pic);
    }
    else {
      printf("Unable to open file %s\n", fileName);
    }
    DestroyMagickWand(staff);
    DestroyMagickWand(birch);
    RelinquishMagickMemory(fm);
    imgQuit();
  }
  else {
    fprintf(stderr, "Give a file name\n");
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
  //gickWriteImage(staff, "scaledThing.png");
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
			    (regx - 1) * regWidth + subx,
			    (regy - 1) * regHeight + suby);
	  
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

void matchImageToCharacters(image* pic, character** characterSet) {
  character* val;
  int regionCols = pic->numberOfRegionCols;
  int regionRows = pic->numberOfRegionRows;
  for(int regy = 1; regy <= regionRows; regy++) {
    for(int regx = 1; regx <= regionCols; regx++) {
      val = matchProfileToCharacter(pic->profiles[regy - 1][regx - 1], characterSet);
      //generally won't work, need to have a function for turning charVals into actual chars
      //will probably need to call wprintf or do special cases if charVal is valid ascii
      //still nice to know that everything looks like spaces though
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
  return colors;
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

double averageCompareResults(colorMatrix* colors) {

  //basically copy-pasted code from fillDiffMatrix
  //if there's any bugs in one, there's bugs in the other  
  int rows = colors->rows;
  int cols = colors->cols;
    int testC, testR, testNumber;
  myColor* current, *compare;
  int results = 0;
  int numRuns = 0;
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
	  results += getPixelDif(current, compare);
	  numRuns++;
	}
	testNumber++;
      }            
    }
  }
  return results/numRuns;
}




character** buildCharacterSet(char* font, int fw, int fh, int fs) {
  //based on the ascii/etc used variables, builds up a characterset
  //from the used ranges of characters
  int size = 0;
  int index = 0;
   
  if (asciiUsed) {
    size += asciiEnd - asciiStart + 1;
  }
  if (hiraganaUsed) {
    size += hiraganaEnd - hiraganaStart + 1;
  }
  if (katakanaUsed) {
    size += katakanaEnd - katakanaStart + 1;
  }
  character** charSet = malloc(sizeof(character*) * size + 1);  

  MagickBooleanType status;
  PixelWand* white = NewPixelWand();
  PixelSetColor(white, "white");
  MagickWand* hickory = NewMagickWand();
  DrawingWand* creator = NewDrawingWand();
  status =  MagickNewImage(hickory, fw,fh, white);
  assert(status != MagickFalse && "blew up at new Image");
  colorMatrix* charColors = newColorMatrix(fw, fh);
  status = DrawSetFont(creator, font);
  assert(status != MagickFalse && "blew up setting font");
  DrawSetFontSize(creator, fs);
  status = PixelSetColor(white, "black");
  assert(status != MagickFalse && "blew up at setting color of pixel");
  //for text color
  DrawSetFillColor(creator, white);

  //fm = MagickQueryFontMetrics(hickory, creator, str);
  //get this to get info about font. 
  //MagickScaleImage(hickory, ceil(fm[9] - fm[7]), ceil(fm[10] - fm[8]))
  PixelWand* clearColor = NewPixelWand();
  PixelSetColor(clearColor, "white");  
  DrawSetTextUnderColor(creator, clearColor);
  
  int isDone = 0;
  int usingUTF8 = 0;
  int codeStart, codeEnd, codeUsed;
  while(!isDone) {
    codeUsed = 0;
    if (asciiUsed) {
      codeUsed = asciiUsed;
      codeStart = asciiStart;
      codeEnd = asciiEnd;
    }
    else if (hiraganaUsed) {
      usingUTF8 = 1;
      codeUsed = hiraganaUsed;
      codeStart = hiraganaStart;
      codeEnd = hiraganaEnd;
    }
    else if (katakanaUsed) {
      usingUTF8 = 1;
      codeUsed = katakanaUsed;
      codeStart = katakanaStart;
      codeEnd = katakanaEnd;
    }

    if(codeUsed) {
      for(int intCode = codeStart; intCode <= codeEnd; intCode++) {
	MagickOpaquePaintImage(hickory, clearColor, clearColor, 0, MagickTrue);
	charSet[index] = buildCharacterOfCodePoint(hickory, creator, charColors, intCode);
	index++;
      }
    }
    else {
      isDone = 1;
    }
    
    if (asciiUsed) {
      asciiUsed = 0;
    }
    else if (hiraganaUsed) {
      hiraganaUsed = 0;
    }
    else if (katakanaUsed) {
      katakanaUsed = 0;
    }
  }
  charSet[size] = (character*) NULL;
  
  return charSet;
}
 

int tempCount = 1;
character* buildCharacterOfCodePoint(MagickWand* staff, DrawingWand* creator, colorMatrix* charColors, int intCode) {
  //takes a unicode, draws the glyph to an image provided by staff
  //then will run other functions to build an edgescore
  //some cordinate to draw. ideally they are zero but I doubt they are
  //may need to pass as a value, since probably dependent on fm
  int x, y;
  x = 0;
  y = 14;
  char* codePoint = intToIMUnicode(intCode);
  //char fn[7];
  //sprintf(fn, "%d.jpg", tempCount++);
  MagickAnnotateImage(staff, creator, x, y, 0, (const char*)codePoint);
  //MagickWriteImage(staff, fn);
  readWandIntoColorMatrix(staff, charColors);
  myColor* averageColor = calculateAverageColor(charColors);
  profileMatrix* charProfile = newProfileMatrix(charColors);
  charProfile->averageColor = averageColor;
  intMatrix* difs = createIntMatrix(charProfile);
  //might want to call averageCompare results and set param again before doing this
  fillDiffMatrix(difs, charProfile);
  betterPopulateEdges(charProfile);
  character* completeChar = newCharacter();
  completeChar->charVal = codePoint;
  completeChar->profile = charProfile;
  return completeChar;
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


int sizeOfIMCharCode = 5; //5 with nul, 4 otherwise because IM usees utf-8


char* intToIMUnicode(u_int32_t intCode) {
  //not sure what it's expecting
  //so the shell scripting libraries took \x1234 or something
  //that was also from 2008 and not using the magickWand api, so
  //beleive it just wants a multi-byte character
  //just carve up the ints into byte regions
  //and shove those bytes into indicies in char*
  //generally expects things like \x12\x32 where that stuff is hex
  //so to convert an int code into that
  //take code, carve up byes, then put into indicies of charCode;
  u_int8_t byteBlock;
  u_int32_t copy = intCode;
  copy = copy;
  char copyThing[sizeOfIMCharCode];
  for(int i = 0; i < sizeOfIMCharCode; i++) {
    copyThing[i] = '\0';
  }
  //copyThing[sizeOfIMCharCode - 1] = '\0';
  for(int index = sizeOfIMCharCode - 2; index >= 0; index--) {
    byteBlock = intCode & 0xff;
    intCode = intCode >> 8;
    copyThing[index] = byteBlock;
    index--;    
  }
  int actualSize = sizeOfIMCharCode;
  while(copyThing[sizeOfIMCharCode - actualSize] == 0 && actualSize > 1) {
    actualSize--;
  }
  char* charCode = malloc(sizeof(char) * actualSize);
  int cci = 0;
  for(int i = 0; i < sizeOfIMCharCode; i++) {
    if (copyThing[i] != 0) {
      charCode[cci] = copyThing[i];
      cci++;
    }
  }
  charCode[actualSize - 1] = '\0';
  return charCode;
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
  MagickNewImage(staff, pic->width, pic->height, white);
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
  MagickWriteImage(staff, "output.jpg");
  DestroyMagickWand(staff);
  DestroyDrawingWand(creator);
}
