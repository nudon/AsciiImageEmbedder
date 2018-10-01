#include "characters.h"
#include "region.h"
static int asciiStart = 32;
static int asciiEnd = 126;
static int asciiUsed = 0;
static int asciiSize = 95;

//imageMagick only supports utf-8, so all the folloing are the starting utf-8 encoding values
//not the actual decimal/intCode of the unicode characters
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

FT_Library lib = NULL;

int init_FT() {
  int error = 0;
  if (lib == NULL) {
    error = FT_Init_FreeType( &lib );
    if (error) {
      lib = NULL;
      fprintf(stderr, "failed to load freeType Library\n");
    }
  }
  return error;
}

int close_FT() {
  int error = 0;
  if (lib != NULL) {
    error = FT_Done_FreeType( lib );
    if (error) {
      fprintf(stderr, "failed to destroy freeType Library\n");
    }
    else {
      lib = NULL;
    }
  }
  return error;
}

#include <unistd.h>
void printCwd() {
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    printf("Current working dir: %s\n", cwd);
  }
}

characterSet* buildCharacterSet(char* font, int fw, int fh, int fs) {
  //based on the ascii/etc used variables, builds up a characterset
  //from the used ranges of characters
  int size = 0;
  int index = 0;
  int seqNum;

  printCwd();
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

  int badCharIntCode = 0xEFBFBF;
  int badSeqNum = UTF8CodePointToSequenceNumber(badCharIntCode);
  if (codeInFont(badSeqNum, font)) {
    fprintf(stderr, "wut\n");
  }
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
	seqNum = UTF8CodePointToSequenceNumber(intCode);
	if (codeInFont(seqNum, font)) {
	  symbol = buildCharacterOfCodePoint(hickory, creator, charColors, intCode);
	  setCharacterAtIndex(charSet, index, symbol);
	  getCharacterAtIndex(charSet, index)->profile->source = NULL;
	  index++;
	}
	else {
	  fprintf(stderr, "Found a bad code\n");
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
  charSet->length = index;
  generateLightMarkScoresForCharacterSet(charSet);
  return charSet;
}

character* buildCharacterOfCodePoint(MagickWand* staff, DrawingWand* creator, colorMatrix* charColors, int intCode) {
  //takes a unicode, draws the glyph to an image provided by staff
  int x, y, writingGlyphs;
  char* codePoint = intToIMUnicode(intCode);
  x = 0;
  y = 0;
  DrawSetGravity(creator, CenterGravity);
  //_ is non-existant with comic sans
  //but a worthy sacrifice. Maybe, _ is pretty important. 
  MagickBooleanType status;
  character* completeChar;
  status = MagickAnnotateImage(staff, creator, x, y, 0, (const char*)codePoint);
  if (status == MagickTrue) {
    writingGlyphs = 0;
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
  return completeChar;
}

//transforms an integer utf8-encoding and shoves it into a char*
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

int bytesInUTF8CodePoint(u_int32_t intCode) {
  u_int32_t tempCode = intCode;
  int shiftAmount = 8;
  int bytesInCode = 0;
  while(tempCode != 0) {
    bytesInCode++;
    tempCode = tempCode >> shiftAmount;
  }
  return bytesInCode;
}

int bytesNeededForUTF8EncodingOfAnIntCode(int intCode) {
  int tempCode = intCode;
  int bitsNeeded = 0;
  int bytesNeeded = 0;
  while (tempCode != 0) {
    tempCode = tempCode >> 1;
    bitsNeeded++;
  }
  if (bitsNeeded <= 7) {
    bytesNeeded = 1;
  }
  else if (bitsNeeded <= 6 * 1 + 5) {
    bytesNeeded = 2;
  }
  else if (bitsNeeded <= 6 * 2 + 4) {
    bytesNeeded = 3;
  }
  else if (bitsNeeded <= 6 * 3 + 3) {
    bytesNeeded = 4;
  }
  else {
    //no
    bytesNeeded = 0;
    fprintf(stderr, "code would need 5 bytes to store in utf8, recheck unicodeblock start/end ranges\n");
  }
  return bytesNeeded;
}

int UTF8CodePointToSequenceNumber(u_int32_t intCode) {
  int byteMask = 63;
  int byteMaskDigits;
  int shiftAmount = 6;
  int tempCode = intCode;
  int bytesInCode = bytesInUTF8CodePoint(intCode);
  int sequenceNumber = 0;
  u_int8_t bytes[bytesInCode];
  for (int index = bytesInCode - 1; index >= 0; index--){
    bytes[index] = tempCode & 0xff;
    tempCode = tempCode >> 8;
  }
  tempCode = intCode;

  for (int index = 1; index <= bytesInCode - 1; index++){
    sequenceNumber = sequenceNumber << shiftAmount;
    sequenceNumber += bytes[index] & byteMask;
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
  else if (bytesInCode >= 5) {
    fprintf(stderr, "Error, would need 5 or more bytes to enocde, check unicodeBlock start/end values and verify they are correct\n by rfc3629, utf-8 should not need more than four bytes\n");
    //just return zero and probably crash
    byteMask = 0;
    byteMaskDigits = 2;
    sequenceNumber = 0;
  }
  sequenceNumber += (bytes[0] & byteMask) << ((bytesInCode - 1) * 6);
  return sequenceNumber;
}

u_int32_t SequenceNumberToUTF8CodePoint(int sequenceNumber) {
  int byteMask = 0x3f;
  int byteMaskDigits = 6;
  int temp = sequenceNumber;

  int bytesInCode = bytesNeededForUTF8EncodingOfAnIntCode(sequenceNumber);
  u_int8_t bytes[bytesInCode];
  
  byteMask = 0x3f;
  byteMaskDigits = 6;
  
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
  //return advancePastInvalidCharacter(ret);
  return ret;
}

u_int32_t incrementUTF8CodePoint(u_int32_t intCode) {
  int sequenceNumber = UTF8CodePointToSequenceNumber(intCode);
  u_int32_t ret;
  sequenceNumber++;
  //may have to use advancePastInvalidCharacter, will see
  ret = SequenceNumberToUTF8CodePoint(sequenceNumber);
  return ret;
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

FT_Face getFaceForFont(char* fontPath, int faceIndex) {
  int error;
  FT_Face font;
  error = FT_New_Face(lib, fontPath, faceIndex, &font);
  if (error) {
    fprintf(stderr, "failed to load font %s with face %d\n", fontPath, faceIndex);
    font = NULL;
  }
  return font;
}

int codeInFace(u_int32_t intCode, FT_Face face) {
  //FT_EXPORT( FT_UInt )
  int index;
  index = FT_Get_Char_Index( face, intCode );
  return index;
}

int codeInFont(u_int32_t intCode, char* fontPath) {
  int faceIndex = 0;
  int numFaces;
  int done = 0;
  int ret = 0;
  //documentation of FT_open_face(and siblings) says passing in a negative index
  //returns an FT_face that will indicate if the font is recognized
  //and also the number of faces in the font
  FT_Face face = getFaceForFont(fontPath, -1);
  if (face != NULL) {
    numFaces = face->num_faces;
    while(!done) {
      if (faceIndex == numFaces) {
	done = 1;
	ret = 0;
      }
      else {
	face = getFaceForFont(fontPath, faceIndex);
	if (codeInFace(intCode, face)) {
	  done = 1;
	  ret = 1;
	}
	else {
	  faceIndex++;
	}
	FT_Done_Face(face);
      }
    }
  }
  //fprintf(stderr, "returned %d for intCode %d in font %s\n", ret, intCode, fontPath);
  return ret;
}
