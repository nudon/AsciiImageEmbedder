#include "imagegenerator.h"
#include <math.h>

struct colorMatrix_struct* ImageGenerator::BasePicture = nullptr;
struct image_struct* ImageGenerator::TiledPicture = nullptr;
struct characterSet_struct* ImageGenerator::characters = nullptr;

bool ImageGenerator::rebuildBasePicture = true;
bool ImageGenerator::rebuildTiledPicture = true;
bool ImageGenerator::rebuildCharacterSet = true;


ImageGenerator::ImageGenerator()
{
    this->BasePicture = nullptr;
    this->TiledPicture = nullptr;
    this->characters = nullptr;

}

 void ImageGenerator::generateAscii() {
    //generation, need filename, font width/height, fontName, fontsize, and output filename
    //char* output = getOutputFile();
    char* fileName = getInputFile();
    char* fontName = getFont();
    int fontSize = getFontSize();
    int fontWidth = 0, fontHeight = 0;
    float floatW, floatH;
    colorMatrix_struct* bp = ImageGenerator::BasePicture;
    image_struct* tp = ImageGenerator::TiledPicture;
    characterSet_struct* cs = ImageGenerator::characters;
    if (rebuildCharacterSet) {
        if (cs != nullptr) {
            freeCharacterSet(cs);
        }
        cs = buildCharacterSet(fontName, fontSize);
    }
    getFontDims(cs, &floatW, &floatH);
    fontWidth = ceil(floatW);
    fontHeight = ceil(floatH);
    if (rebuildBasePicture) {
        if (bp != nullptr) {
            freeColorMatrix(bp);
        }
        bp = generateColorMatrix(fileName, fontWidth, fontHeight);
        rebuildTiledPicture = true;
    }
    if (rebuildTiledPicture) {
        if (tp != nullptr) {
            freeImage(tp);
        }
        tp = generateImage(bp, fontWidth, fontHeight);
    }


    if (tp != nullptr && cs != nullptr) {
        matchImageToCharacters(tp, cs);
        drawPicToDisk(tp, cs);
    }
    ImageGenerator::BasePicture = bp;
    ImageGenerator::TiledPicture = tp;
    ImageGenerator::characters = cs;
    ImageGenerator::rebuildBasePicture = false;
    ImageGenerator::rebuildTiledPicture = false;
    ImageGenerator::rebuildCharacterSet = false;
}

 void ImageGenerator::setRebuildBasePicture(bool val) {
     ImageGenerator::rebuildBasePicture = val;
 }

 void ImageGenerator::setRebuildTiledPicture(bool val) {
     ImageGenerator::rebuildBasePicture = val;
 }

 void ImageGenerator::setRebuildCharacterSet(bool val) {
     ImageGenerator::rebuildBasePicture = val;
 }
