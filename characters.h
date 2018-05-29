#ifndef FILE_CHARACTERS_SEEN
#define FILE_CHARACTERS_SEEN

#include <MagickWand/MagickWand.h>
#include "dataStructures.h"
#include "picture.h"

void setAsciiUsed(int new);
void setHiraganaUsed(int new);
void setKatakanaUsed(int new);

characterSet* buildCharacterSet(char* font, int fw, int fh, int fs);

character* buildCharacterOfCodePoint(MagickWand* staff, DrawingWand* creator, colorMatrix* charColors, int intCode);

char* intToIMUnicode(u_int32_t intCode);

int incrementUTF8CodePoint(u_int32_t intCode);

u_int32_t advancePastInvalidCharacter(u_int32_t intCode);

#endif
