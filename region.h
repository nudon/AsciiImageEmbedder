//these will store whatever metadata I want
#include "dataStructures.h"

// so, have matrix. in general it contains metainformation like value of a pixel or group of pixel
// then, need two algs
// first, looking at an arrangement of pixels and generating a profile of it
// second, need to find a closest match of another profile
// depends heavily on how I define the metadate stored in the profile matrixes

//metadata, likely to come down to two things. one, just the grascale darkness/ or hsv/hsl value/lightness value. 
// the otheer, doing some kind of edge detection?
//idea was to reduce lines/edges into horizontal, vertical, diagnol, would compare the edge profiles to find a similar match

//kind of discrepencany in naming scheme. col/row max indicates dimension of profile matrix
//standard for where to start specific edge Checks
//listed as whichEdge, rowStart, colStart
//top     , 0         , 0
//bottom  , rowMax    , 0,
//left    , 0         , 0
//right   , 0         , colMax
//table   , rowMax / 2, 0,
//pole    , 0         , colMax / 2
//forward , 0,        , colMax
//backWard, 0        , 0

//standard for where to end specific edge Checks
//listed as whichEdge, rowEnd, colEnd
//top     , 0         , colMax
//bottom  , rowMax    , colMax,
//left    , rowMax    , 0
//right   , rowMax    , colMax
//table   , rowMax / 2, colMax
//pole    , rowMax    , colMax / 2
//forward , rowMax    , 0
//backWard, rowMax    , colMax


void setUseQuick(int new);
void setUseAverageReduce(int new);
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

character* matchProfileToCharacter(profileMatrix* prof,  characterSet* charSet);

edges* calculateEdgeScores(profileMatrix* prof);

edges* betterPopulateEdges(profileMatrix* prof);

edges* quickCalcEdges(profileMatrix* prof);

void averageReduceEdgeScores(edges* edges);

float betterGenerateEdgeScore(intMatrix* diffMatrix, int colCur, int rowCur, int colDim, int rowDim, edgeCheck whichCheck);

character* newCharacter();

void fillDiffMatrix(  intMatrix* detectedEdges, profileMatrix* prof, int diffParam);

float compareProfiles(profileMatrix* p1, profileMatrix* p2);

float compareEdges(edges* e1, edges* e2);

float averageCompareResults(colorMatrix* colors);

int getPixelDif(myColor* c1, myColor* c2);
int getPixelDifByArgs(myColor* c1, myColor* c2, float argSatScale, float argHueScale, float argLightScale);

int getNonColorPixelDiff(myColor* c1, myColor* c2);
