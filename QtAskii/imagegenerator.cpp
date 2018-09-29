#include "imagegenerator.h"

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
    int fontWidt, fontHeight;
    getFontDim(fontName, fontSize, &fontWidt, &fontHeight);
    colorMatrix_struct* bp = ImageGenerator::BasePicture;
    image_struct* tp = ImageGenerator::TiledPicture;
    characterSet_struct* cs = ImageGenerator::characters;
    if (rebuildBasePicture) {
        if (bp != nullptr) {
            freeColorMatrix(bp);
        }
        bp = generateColorMatrix(fileName, fontWidt, fontHeight);
        rebuildTiledPicture = true;
    }
    if (rebuildTiledPicture) {
        if (tp != nullptr) {
            freeImage(tp);
        }
        tp = generateImage(bp, fontWidt, fontHeight);
    }

    if (rebuildCharacterSet) {
        if (cs != nullptr) {
            freeCharacterSet(cs);
        }
        cs = buildCharacterSet(fontName, fontWidt, fontHeight, fontSize);
    }
    matchImageToCharacters(tp, cs);
    drawPicToDisk(tp, fontName, fontSize);
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
