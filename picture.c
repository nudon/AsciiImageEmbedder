#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
//if using imageMagick 6
//#include <wand/MagickWand.h>
//this is for imageMagick 7
#include <MagickWand/MagickWand.h>

#include "region.h"


//potential bugs to run into
//realized that I'm not accounting for the spaces between lines or characters.
//might not matter, if they do would have to do some more calculations. Essentially find out how big those gaps are
//then keep that in mind when tiling the original picture into subregions.
//also those gaps are going to be variable(?)
//also wanted a way of outputting the picture in a bigger resolution. Setting super-small font sizes get's closer to generating pixel-art instead of ascii-art

//also, issue of darker regions getting represented as whitespace.
//kind of fixed that by doing something. Yeah, started assiging lightness non-zero values. like between .02 and .01
//sometimes that works, sometimes it doesn't

void test();


int indexOfChar(char* src, char search);
int match(char* src, char* search);
int matchFlag(char* token, char* flag );

char* intToIMUnicode(u_int32_t intCode);

void setFontSize(int new);

void setFontName(char* new);

void imgInit();

void imgQuit();

void scaleImageToFitFont(MagickWand* staff, int fontw, int fonth);

image* readColorMatrixIntoImage(colorMatrix* entireImage, int regCols, int regRows, int regWidth, int regHeight);

colorMatrix* readFileIntoColorMatrix(char* fileName);

colorMatrix* readWandIntoColorMatrix(MagickWand* staff, colorMatrix* toReadTo);

myColor* calculateAverageColor(colorMatrix* colorMatrix);

void matchImageToCharacters(image* pic, characterSet* characterSet);

void shovePixelWandIntoMyColor(PixelWand* aPixel, myColor* color);

characterSet* buildCharacterSet(char* font, int fw, int fh, int fs);

character* buildCharacterOfCodePoint(MagickWand* staff, DrawingWand* creator, colorMatrix* charColors, int intCode);

int findAndStoreMatch(char* token, char* search, int resultSize, char* result);

void drawPicToDisk(image* pic, char* font, int fs);

int incrementUTF8CodePoint(u_int32_t intCode);

u_int32_t advancePastInvalidCharacter(u_int32_t intCode);

int asciiStart = 32;
int asciiEnd = 126;
int asciiUsed = 0;
int asciiSize = 95;


//oh, this also isn't contigious.
//U+307f is 11100011:10000001:10111111
//U+3080 is 11100011:10000010:10000000
//will have to probably create a function for incrementing codePoints then.
//also size is no longer start - end, just had to hardcode it

//also, If you want to look at using other character sets
//fileformat.info has a nice unicode section. 
int hiraganaStart = 0xe38181;
int hiraganaEnd = 0xe3829f;
int hiraganaUsed = 0;
int hiraganaSize = 93;

//got lucky on the other ascii ranges, but katakana actually is complicated in utf8
//from U+30bf to U+30c0, binary code jumps from 11100011:10000010:10111111 to  	11100011:10000011:10000000
//my dinky way of just incrementing the int-code will break there.

int katakanaStart = 0xe382a0;
int katakanaEnd = 0xe383bf;
int katakanaUsed = 0;
int katakanaSize = 96;
// "Usage is \"%s\" fileName (opt)fontSize (opt)pixleThing (opt) distanceDecay (opt)edgeWeight (opt)colorWeight\n"
//font size, should be simple.

//pixleThing, I calculate colorDiffs by taking an hsl component, finding a difference, adding it to a total,
//then multiplying total by pixelThing. Basically weights some components higher than others. set to 1 for equality of components.
//or a fraction if crazy

//distanceDecay, the higher this is, the worse it gets. generally don't try higher than 3

//edgeWeight and colorWeight, they sort of determine closest matches
//patching things by comparing profiles and finding the profile that had the lowest score
//score is a sum of an seperate edgeScore and averageColorScore
//score is weighted, so totScore = edgeScore * edgeWeight + colorScore * colorWeight

char* flagStart = "--";
char optValueDelim = '=';

char* fontSizeOpt = "font-size";
char* fontNameOpt = "font";
char* outputFile = "output";
char* useQuickCalc = "useQuickCalc";
//still in use
char* edgeWeight = "edge";
char* colorWeight = "color";
char* saturationScale = "saturation";
char* lightnessScale = "lightness";
char* hueScale = "hue";
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
  fprintf(stderr, "%s: Controls the size of the font the image gets embedded in\n", fontSizeOpt);
  fprintf(stderr, "%s: Relates to determing how much worth is ascribed to pixels far away from an edge\nGenerally ", distanceDropoff);
  fprintf(stderr, "%s: Sets the weight assigned to edge scores when matching profiles\n", edgeWeight);
  fprintf(stderr, "%s: Sets the wieght assigned to the average colors of profiles\n", colorWeight);
  fprintf(stderr, "Using HSL color scheme, look at that if this doesn't make sense: \n");
  fprintf(stderr, "%s: Sets weight assigend to saturation differences when comparing colors\n", saturationScale);
  fprintf(stderr, "%s: Sets weight assigend to lightness differences when comparing colors\n", lightnessScale);
  fprintf(stderr, "%s: Sets weight assigend to hue differences when comparing colors\n", hueScale);
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
    if (findAndStoreMatch(token, fontSizeOpt, sizeOfValue, valueText) != -1) {
      intVal = atoi(valueText);
      setFontSize(intVal);
    }
    else if (findAndStoreMatch(token, fontNameOpt, pathLen, fontName) != -1) {
      //again, don't do anything
    }
    else if (findAndStoreMatch(token, distanceDropoff, sizeOfValue, valueText) != -1) {
      floatVal = atoi(valueText);
      setDistanceWeight(floatVal);
    }
    else if (findAndStoreMatch(token, edgeWeight, sizeOfValue, valueText) != -1) {
      floatVal = atof(valueText);
      setEdgeScoreWeight(floatVal);
    }
    else if (findAndStoreMatch(token, colorWeight, sizeOfValue, valueText) != -1) {
      floatVal = atoi(valueText);
      setColorScoreWeight(floatVal);
    }
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
    else if (findAndStoreMatch(token, hitWeight, 10, valueText) != -1) {
      floatVal = atof(valueText);
      setHitWeight(floatVal);
    }
    else if (findAndStoreMatch(token, hitDecay, 10, valueText) != -1) {
      floatVal = atof(valueText);
      setHitDecay(floatVal);
    }
    else if (findAndStoreMatch(token, missWeight, 10, valueText) != -1) {
      floatVal = atof(valueText);
      setMissWeight(floatVal);
    }
    else if (findAndStoreMatch(token, missDecay, 10, valueText) != -1) {
      floatVal = atof(valueText);
      setMissDecay(floatVal);
    }
    else if (findAndStoreMatch(token, outputFile, pathLen, outputFileName) != -1)  {
      //don't do anything?
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
  setUseQuick(0);
  setUseAverageReduce(0);
  setDistanceWeight(.25);
  setEdgeScoreWeight(1);
  setColorScoreWeight(0);
  setSaturationScale(0);
  setLightnessScale(1);
  setHueScale(1);
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
      int diffParam = averageCompareResults(entireImage);
      setDiffParam(diffParam);
      int regionsx = imageWidth / regionWidth;
      int regionsy = imageHeight / regionHeight;
      image* pic = readColorMatrixIntoImage(entireImage, regionsx, regionsy, regionWidth, regionHeight);
      asciiUsed = 1;
      //hiraganaUsed = 1;
      //katakanaUsed = 1;
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

void matchImageToCharacters(image* pic, characterSet* characterSet) {
  character* val;
  int regionCols = pic->numberOfRegionCols;
  int regionRows = pic->numberOfRegionRows;
  for(int regy = 1; regy <= regionRows; regy++) {
    for(int regx = 1; regx <= regionCols; regx++) {
      val = matchProfileToCharacter(pic->profiles[regy - 1][regx - 1],
				    characterSet);
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
  DestroyPixelWand(aPixel);
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






characterSet* buildCharacterSet(char* font, int fw, int fh, int fs) {
  //based on the ascii/etc used variables, builds up a characterset
  //from the used ranges of characters
  int size = 0;
  int index = 0;
   
  if (asciiUsed) {
    size += asciiSize;
  }
  if (hiraganaUsed) {
    size += hiraganaSize;
  }
  if (katakanaUsed) {
    size += katakanaSize;
  }
  characterSet* charSet = newCharacterSet(size);

  MagickBooleanType status;
  PixelWand* white = NewPixelWand();
  PixelSetColor(white, "white");
  MagickWand* hickory = NewMagickWand();
  DrawingWand* creator = NewDrawingWand();
  status =  MagickNewImage(hickory, fw,fh, white);
  assert(status != MagickFalse && "blew up at new Image");
  //colorMatrix is shared between characters
  colorMatrix* charColors = newColorMatrix(fw, fh);
  status = DrawSetFont(creator, font);
  assert(status != MagickFalse && "blew up setting font");
  DrawSetFontSize(creator, fs);
  PixelSetColor(white, "black");
  //for text color
  DrawSetFillColor(creator, white);

  PixelWand* clearColor = NewPixelWand();
  PixelSetColor(clearColor, "white");  
  DrawSetTextUnderColor(creator, clearColor);


  DrawSetTextEncoding(creator, "UTF-8");
  int isDone = 0;
  int codeStart, codeEnd, codeUsed;
  while(!isDone) {
    codeUsed = 0;
    if (asciiUsed) {
      codeUsed = asciiUsed;
      codeStart = asciiStart;
      codeEnd = asciiEnd;
    }
    else if (hiraganaUsed) {
      codeUsed = hiraganaUsed;
      codeStart = hiraganaStart;
      codeEnd = hiraganaEnd;
    }
    else if (katakanaUsed) {
      codeUsed = katakanaUsed;
      codeStart = katakanaStart;
      codeEnd = katakanaEnd;
    }

    if(codeUsed) {
      for(int intCode = codeStart; intCode <= codeEnd; intCode = incrementUTF8CodePoint(intCode)) {
	MagickOpaquePaintImage(hickory, clearColor, clearColor, 0, MagickTrue);
	setCharacterAtIndex(charSet, index, buildCharacterOfCodePoint(hickory, creator, charColors, intCode));
	getCharacterAtIndex(charSet, index)->profile->source = NULL;
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
  DestroyPixelWand(clearColor);
  DestroyPixelWand(white);
  //causes asserts to fail, so ?
  //DestroyImage(GetImageFromMagickWand(hickory));
  DestroyMagickWand(hickory);
  DestroyDrawingWand(creator);
  freeColorMatrix(charColors);
  return charSet;
}
 
character* buildCharacterOfCodePoint(MagickWand* staff, DrawingWand* creator, colorMatrix* charColors, int intCode) {
  //takes a unicode, draws the glyph to an image provided by staff
  int x, y, writingGlyphs;
  char* codePoint = intToIMUnicode(intCode);
  double* fm = MagickQueryFontMetrics(staff, creator, codePoint);
  //so, bounds and char dims are the reverse of what I thought they were
  //bound is actual dim of character
  //character width/height is actualy just the drawingWandDim?
  int boundx = fm[9] - fm[7];
  int boundy = fm[10] - fm[8];
  int charx = fm[0];
  int chary = fm[1];
  int diffParam = -1;
  //coordinate specified is the lower-lefthand corner of drawn images
  //this generally works, I think backTick gets killed for some reason
  //
  x = (charx - boundx) / 2;
  y = ((chary - boundy) / 2) + boundy;

  //should have read the docs more
  x = 0;
  y = 0;
  DrawSetGravity(creator, CenterGravity);
  //_ is non-existant with comic sans
  //but a worthy sacrifice. Maybe, _ is pretty important. 

  MagickAnnotateImage(staff, creator, x, y, 0, (const char*)codePoint);
  writingGlyphs = 0;
  if (writingGlyphs) {
    char fn[20];
    sprintf(fn, "%dcharTest.jpg", intCode);
    MagickWriteImage(staff, fn);
  }
  readWandIntoColorMatrix(staff, charColors);
  myColor* averageColor = calculateAverageColor(charColors);
  profileMatrix* charProfile = newProfileMatrix(charColors);
  charProfile->averageColor = averageColor;
  intMatrix* difs = createIntMatrix(charProfile);
  //might want to call averageCompare results and set param again before doing this
  //diffParam = averageCompareResults(charProfile->source);
  fillDiffMatrix(difs, charProfile, diffParam);
  calculateEdgeScores(charProfile);
  character* completeChar = newCharacter();
  completeChar->charVal = codePoint;
  completeChar->profile = charProfile;
  RelinquishMagickMemory(fm);
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
  DestroyDrawingWand(creator);
}



//so, this works.
//welp, not when using ascii codes. prematurely ends
//fixed that, now learning that there are arbitrary characters in the hiragana block that are not valid.

int incrementUTF8CodePoint(u_int32_t intCode) {
  //so, beyond the first byte, all subsequent bytes have to start with 10
  //so grab first 6 bits
  int byteMask = 0x3f;
  int byteMaskDigits = 6;

  int shiftAmount = 8;
  int tempCode = intCode;
  int bytesInCode = 0;
  int sequenceNumber = 0;
  int temp;
  
  while(tempCode != 0) {
    bytesInCode++;
    tempCode = tempCode >> shiftAmount;
  }
  u_int8_t bytes[bytesInCode];
  tempCode = intCode;
  for (int index = bytesInCode - 1; index >= 0; index--){
    bytes[index] = tempCode & 0xff;
    tempCode = tempCode >> 8;
  }
  tempCode = intCode;

  byteMask = 63;
  shiftAmount = 6;
  for (int index = 1; index <= bytesInCode - 1; index++){
    sequenceNumber = sequenceNumber << shiftAmount;
    sequenceNumber += bytes[index] & byteMask;

    //uses the previouse forloop header
    //sequenceNumber += (bytes[index] && byteMask) << (((bytesInCode -1) - index) * byteMaskDigits)
  }
  //for final thing, depends.
  if (bytesInCode == 1) {
    //nothing special, just an ascii code, increment as usual
    byteMask = 127;
    byteMaskDigits = 7;
  }
  else if (bytesInCode == 2) {
    //grab 5 bits
    byteMask = 31;
    byteMaskDigits = 5;
    
  }
  else if (bytesInCode == 3) {
    //grab 4 bits
    byteMask = 15;
    byteMaskDigits = 4;
  }
  else if (bytesInCode == 4) {
    //grab 3 bits
    byteMask = 7;
    byteMaskDigits = 3;
  }
  //using uint8-32, so this is mostly an error statement. 
  else if (bytesInCode == 5) {
    fprintf(stderr, "Error, using pretty end-range unicode characters\n I'm using uint32_t so this will not work so change that to uint64_t\n");
    //otherwise, only grab 2 bits
    byteMask = 3;
    byteMaskDigits = 2;
  }
  else if (bytesInCode == 6) {
    fprintf(stderr, "Error, using pretty end-range unicode characters\n I'm using uint32_t so this will not work so change that to uint64_t\n");
    //otherwise, only grab 1 bit
    byteMask = 1;
    byteMaskDigits = 1;
  }
  sequenceNumber += (bytes[0] & byteMask) << ((bytesInCode - 1) * 6);
  sequenceNumber++;
  //then do all of the backwards, fuck
  byteMask = 0x3f;
  byteMaskDigits = 6;
  temp = sequenceNumber;
  for (int index = bytesInCode - 1; index > 0; index--){
    bytes[index] = (temp & byteMask) + (1 << 7);
    temp = temp >> byteMaskDigits;
  }
  // last byte
  //only thing to do is set the correct prefix, then just add byte[0]
  if (bytesInCode == 1) {
    //no prefix, just assign 
    bytes[0] = temp;
  }
  else {
    if (bytesInCode >= 2) {
      //prefix is 11000000
      byteMask = 1 << 7;
      byteMask += 1 << 6;
      
    }
    if (bytesInCode >= 3) {
      //prefix is 11100000
      byteMask += 1 << 5;
    }
    if (bytesInCode >= 4) {
      //prefix is 11110000
      byteMask += 1 << 4;
    }
    //using uint8-32, so this is mostly an error statement. 
    if (bytesInCode == 5) {
      //prefix is 11111000
      byteMask += 1 << 3;
    }
    if (bytesInCode == 6) {
      byteMask += 1 << 2;
    }
    bytes[0] = byteMask + temp;
  }

  u_int32_t ret = 0;
  for (int i = 0; i < bytesInCode; i++) {
    ret = ret << 8;
    ret += bytes[i];
  }
  return advancePastInvalidCharacter(ret);
}


u_int32_t advancePastInvalidCharacter(u_int32_t intCode) {
  //in hiragana block, 2 characters that aren't used
  //if intCode is one of them, go to nearest valid one
  int ret = intCode;
  if (intCode == 0xe38297 ||
      intCode == 0xe38298) {
    ret = 0xe38299;
  }
  return ret;
}
