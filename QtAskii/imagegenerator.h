#ifndef IMAGEGENERATOR_H
#define IMAGEGENERATOR_H
//#include "../characters.h"
//#include "../picture.h"
//#include "../region.h"
//#include "../dataStructures.h"
typedef struct colorMatrix_struct colorMatrix;
typedef struct image_struct image;
typedef struct characterSet_struct characterSet;
typedef struct MagickWand_struct MagickWand;
extern "C" {
    char* getOutputFile();
    void setOutputFile(char*);
    char* getInputFile();
    void setInputFile(char*);
    char* getFont();
    void setFont(char*);
    int getFontSize();
    void setFontSize(int);
    //void getFontDim(char*, int, int*, int*);
    void getFontDims(characterSet* set, float* fontWidth, float* fontHeight);
    void freeColorMatrix(colorMatrix* rm);
    void freeImage(image* rm);
    void freeCharacterSet(characterSet* rm);
    colorMatrix* generateColorMatrix(char* fileName, int fontWidth, int fontHeight);
    image* generateImage(colorMatrix* entireImage, int fontWidth, int fontHeight);
    characterSet* buildCharacterSet(char* font, int fs);
    void matchImageToCharacters(image* pic, characterSet* characterSet);
    void drawPicToDisk(image* pic, characterSet* set);
  
    MagickWand* mem_light_generateColorMatrix(char* fileName, int regionWidth, int regionHeight);

    image* mem_light_generateImage(MagickWand* imgWand, int regionWidth, int regionHeight);

  MagickWand* DestroyMagickWand ( MagickWand * wand);
  void ClearMagickWand (MagickWand *wand);
}



class ImageGenerator
{
public:
    ImageGenerator();
    static void generateAscii();
    static void setRebuildBasePicture(bool);
    static void setRebuildTiledPicture(bool);
    static void setRebuildCharacterSet(bool);
    static bool rebuildBasePicture;
    static bool rebuildTiledPicture;
    static bool rebuildCharacterSet;
private:
    static struct colorMatrix_struct* BasePicture;
    static struct image_struct* TiledPicture;
    static struct characterSet_struct* characters;
    static struct MagickWand_struct* imgWand;
  
};



#endif // IMAGEGENERATOR_H
