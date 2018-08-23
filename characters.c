#include "characters.h"
#include "region.h"
static int asciiStart = 32;
static int asciiEnd = 126;
static int asciiUsed = 0;
static int asciiSize = 95;

static int hiraganaStart = 0xe38181;
static int hiraganaEnd = 0xe3829f;
static int hiraganaUsed = 0;
static int hiraganaSize = 93;

static int katakanaStart = 0xe382a0;
static int katakanaEnd = 0xe383bf;
static int katakanaUsed = 0;
static int katakanaSize = 96;

void setAsciiUsed(int new) {
  asciiUsed = new;
}

void setHiraganaUsed(int new) {
  hiraganaUsed = new;
}

void setKatakanaUsed(int new) {
  katakanaUsed = new;
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
  character* symbol;
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
	symbol = buildCharacterOfCodePoint(hickory, creator, charColors, intCode);
	if (symbol != NULL) {
	  setCharacterAtIndex(charSet, index, symbol);
	  getCharacterAtIndex(charSet, index)->profile->source = NULL;
	  index++;
	}
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
  MagickBooleanType status;
  character* completeChar;
  status = MagickAnnotateImage(staff, creator, x, y, 0, (const char*)codePoint);
  if (status == MagickTrue) {
    writingGlyphs = 1;
    if (writingGlyphs) {
      char fn[20];
      sprintf(fn, "%dcharTest.jpg", intCode);
      MagickWriteImage(staff, fn);
    }
    readWandIntoColorMatrix(staff, charColors);
    profileMatrix* charProfile = generateProfileFromColor(charColors);  
    completeChar = newCharacter();
    completeChar->charVal = codePoint;
    completeChar->profile = charProfile;
  }
  else {
    completeChar = NULL;
  }
  RelinquishMagickMemory(fm);
  return completeChar;
}

//int sizeOfIMCharCode = 5; //5 with nul, 4 otherwise because IM usees utf-8
//actually could be as much as 6 bytes, so 7 including null. 

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
  for(int index = sizeOfIMCharCode - 1; index >= 0; index--) {
    byteBlock = intCode & 0xff;
    intCode = intCode >> 8;
    copyThing[index] = byteBlock;
    //index--;    fffffffff
  }
  int actualSize = sizeOfIMCharCode;
  while(copyThing[sizeOfIMCharCode - actualSize] == 0 && actualSize > 1) {
    actualSize--;
  }
  char* charCode = malloc(sizeof(char) * (actualSize + 1));
  int cci = 0;
  for(int i = 0; i < sizeOfIMCharCode; i++) {
    if (copyThing[i] != 0) {
      charCode[cci] = copyThing[i];
      cci++;
    }
  }
  charCode[actualSize] = '\0';
  return charCode;
}

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
