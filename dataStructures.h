#ifndef MY_DATASTRUCTURES_H
#define MY_DATASTRUCTURES_H
#include <stdio.h>
#include "myList.h"

typedef
struct {
  //some color
  double hue;
  double sat;
  double lightness;
  
  double red;
  double green;
  double blue;
  
} myColor;

//just a subset of pixels
typedef
struct {
  int rows;
  int cols;
  myColor *** cells;
} colorMatrix;

//rough stuff for edge detection
typedef
struct {
  float top;
  float bottom;
  float left;
  float right;
  float table;
  float pole;
  float forward;
  float backward;
} edges;

typedef
struct {
  int rows;
  int cols;
  int** ints;
} intMatrix;

typedef
struct {
  myColor* color;
  float differenceFromMostLight;
  float differenceFromMostDark;
} lightmark;

//also some list, either general or specific
//kind of want to change this to just be profile
//since it's not a matrix. 
typedef
struct {
  int rows;
  int cols;
  //float instead?
  edges* edgeScores;
  intMatrix* diff;
  colorMatrix* source;
  myColor* averageColor;
  lightmark* mark;
} profileMatrix;


//things for each character 
typedef
struct {
  //may either point to an ascii character
  //or a multi-byte unicode codepoint
  char* charVal;
  profileMatrix * profile;
} character;

typedef
struct {
  character** characters;
  int length;
} characterSet;

//will contain image, some dimensions, probably some aspect ratio things as well,
//as well as heigh/width of subRects
//the filled chars will be some mirror array of the subRect array, which will get filled with respective matches
typedef
struct {
  //SDL_Surface/texture
  int width;
  int height;
  //matrix ***?
  int numberOfRegionCols;
  int numberOfRegionRows;
  profileMatrix*** profiles;
  character*** filledCharacterss;
} image;


typedef
enum {
  topCheck,
  bottomCheck,
  leftCheck,
  rightCheck,
  tableCheck,
  poleCheck,
  forwardCheck,
  backwardCheck,
  doneChecking
} edgeCheck;

profileMatrix* newProfileMatrix(colorMatrix* colors);
void freeProfileMatrix(profileMatrix* rm);

colorMatrix* newColorMatrix(int cols, int rows);
void freeColorMatrix(colorMatrix* rm);

character*** newCharacterMatrix(int cols, int rows);
void freeCharacterMatrix(character*** rm, int cols, int rows);

intMatrix* createIntMatrix(profileMatrix* prof);

void freeIntMatrix(intMatrix* rm);

myColor* newColor();

void freeColor(myColor* rm);

image* newImage(colorMatrix* entireImage);
 

void freeImage(image* rm);

character* newCharacter();

character* makecharacter(char* value);

characterSet* newCharacterSet(int size);

void freeCharacterSet(characterSet* rm);

character* getCharacterAtIndex(characterSet* set, int index);

void setCharacterAtIndex(characterSet* set, int index, character* newCharacter);


void freeCharacter(character* rm);


edges* initEdges();

int getDiffAtIndex(intMatrix* diffMatrix, int col, int row);
void setDiffAtIndex(intMatrix* diffMatrix, int col, int row, int val);

void setColor(colorMatrix* matrix, int row, int col, myColor* tobe);
myColor* getColor(colorMatrix* matrix, int col, int row);
void cloneColor(myColor* dest, myColor* src);
void addColorToColor(myColor* dest, myColor* src);
void divideColor(myColor* dest, int denom);


//lightmark stuff
lightmark* newLightmark(myColor* avgColor);
gen_list* createLightmarkListFromImage(image* pic);
gen_list* createLightmarkListFromCharacterSet(characterSet* set);
lightmark* attatchLightmarkToProfile(profileMatrix* prof);
void fillOutFields(gen_list* l);
void findLADExtremes(gen_list* l, myColor* darkColor, myColor* lightColor);
void storeLightmarkDifferences(gen_list* l, myColor* darkColor, myColor* lightColor);
#endif
