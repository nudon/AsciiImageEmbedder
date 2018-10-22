#ifndef FILE_CHARACTERS_SEEN
#define FILE_CHARACTERS_SEEN

#include <MagickWand/MagickWand.h>

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

#include "dataStructures.h"
#include "picture.h"

#define sizeOfIMCharCode 5

void setAsciiUsed(int new);
void setHiraganaUsed(int new);
void setKatakanaUsed(int new);

void getFontDimForEnabledBlocks(char* fontToUse, int size, float* fontWidth, float* fontHeight);

void printFontDimForCharp(char* str, char* fontToUse, int size);

void getAvgFontDimForUnicodeBlock(char* fontToUse, int size, int blockStart, int blockEnd, float* fontWidth, float* fontHeight);

void getFontDimForCharp(char* fontToUse, char* testText, int size, float* fontWidth, float* fontHeight);

characterSet* buildCharacterSet(char* font, int fs);

character* buildCharacterOfCodePoint(MagickWand* staff, DrawingWand* creator, colorMatrix* charColors, int intCode);

char* intToIMUnicode(u_int32_t intCode);

int bytesInUTF8CodePoint(u_int32_t intCode);

int bytesNeededForUTF8EncodingOfAnIntCode(int intCode);

int UTF8CodePointToSequenceNumber(u_int32_t intCode);

u_int32_t SequenceNumberToUTF8CodePoint(int sequenceNumber);

u_int32_t incrementUTF8CodePoint(u_int32_t intCode);

u_int32_t advancePastInvalidCharacter(u_int32_t intCode);

int init_FT();

int close_FT();

FT_Face getFaceForFont(char* fontPath, int faceIndex);

int codeInFace(u_int32_t intCode, FT_Face face);

int codeInFont(u_int32_t intCode, char* fontPath);


#endif
