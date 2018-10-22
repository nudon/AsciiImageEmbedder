#ifndef FILE_REGION_SEEN
#define FILE_REGION_SEEN
#include "dataStructures.h"

// so, have matrix. in general it contains metainformation like value of a pixel or group of pixel
// then, need two algs
// first, looking at an arrangement of pixels and generating a profile of it
// second, need to find a closest match of another profile
// depends heavily on how I define the metadate stored in the profile matrixes

//metadata, likely to come down to two things. one, just the grascale darkness/ or hsv/hsl value/lightness value. 
// the otheer, doing some kind of edge detection?
//idea was to reduce lines/edges into horizontal, vertical, diagnol, would compare the edge profiles to find a similar match


/*
//datasctructures so my c++ files can do things
extern colorMatrix* entireImage;
extern image* pic;
extern characterSet* charSet;  
*/

void setUseQuick(int new);
void setUseAverageReduce(int new);
void setAutoGenColorScale(int new);
int getAutoGenColorScale();
void setDiffParam(int new);
void setUseQuick(int new);
void setEdgeScoreWeight(float new);
void setColorScoreWeight(float new);
void setDistanceWeight(float new);
void setSaturationScale(float new);
void setLightnessScale(float new);
void setHueScale(float new);
void setHitDecay(float new);
void setHitWeight(float new);
void setMissDecay(float new);
void setMissWeight(float new);

int traverse(int startx, int starty, int* offx, int* offy, int endx, int endy);
int  orthogonalTraverse(int startx, int starty, int* offx, int* offy, int endx, int endy);

profileMatrix* generateProfileFromColor(colorMatrix* colors);

character* matchProfileToCharacter(profileMatrix* prof,  characterSet* charSet);

edges* calculateEdgeScores(profileMatrix* prof);

edges* betterPopulateEdges(profileMatrix* prof);

edges* quickCalcEdges(profileMatrix* prof);

void averageReduceEdgeScores(edges* edges);

float betterGenerateEdgeScore(intMatrix* diffMatrix, int colCur, int rowCur, int colDim, int rowDim, edgeCheck whichCheck);

character* newCharacter();

void fillDiffMatrix(  intMatrix* detectedEdges, profileMatrix* prof, int diffParam);


float compareProfiles(profileMatrix* p1, profileMatrix* p2);

float compareLightmarks(lightmark* lm1, lightmark* lm2);

float compareEdges(edges* e1, edges* e2);

float averageCompareResults(colorMatrix* colors);

int getPixelDif(myColor* c1, myColor* c2);
int getPixelDifByArgs(myColor* c1, myColor* c2, float argSatScale, float argHueScale, float argLightScale);
int saturationDif(myColor* c1, myColor* c2);
int lightnessDif(myColor* c1, myColor* c2);
int hueDif(myColor* c1, myColor* c2);


int getNonColorPixelDiff(myColor* c1, myColor* c2);


void autoSetColorComponentScale(colorMatrix* source);

int sameIntMatrix(intMatrix* m1, intMatrix* m2);

#endif
