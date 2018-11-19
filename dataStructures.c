
#include <stdlib.h>
#include <math.h>
#include "dataStructures.h"

int diffYes = 1;
int diffNo = 0;
int diffErr = -1;

profileMatrix* newProfileMatrix(colorMatrix* colors) {
  profileMatrix* new = NULL;
  new = malloc(sizeof(profileMatrix));
  new->source = colors;
  new->rows = colors->rows;
  new->cols = colors->cols;
  new->edgeScores = NULL;
  new->diff = NULL;
  new->mark = NULL;
  return new;
}

void freeLightmark(lightmark* rm) {
  //color field is malloc'd elsewere, don't free here
  free(rm);
}

void freeProfileMatrix(profileMatrix* rm) {
  freeIntMatrix(rm->diff);
  free(rm->edgeScores);
  freeColorMatrix(rm->source);
  freeColor(rm->averageColor);
  freeLightmark(rm->mark);
  free(rm);
}

colorMatrix* newColorMatrix(int cols, int rows) {
  colorMatrix* new = malloc(sizeof(colorMatrix));
  new->rows = rows;
  new->cols = cols;
  new->cells = malloc(sizeof(myColor**) * rows);
  for(int rowIndex =0; rowIndex < rows; rowIndex++) {
    new->cells[rowIndex] = malloc(sizeof(myColor*) * cols);
    for(int colIndex = 0; colIndex < cols; colIndex++) {
      new->cells[rowIndex][colIndex] = newColor();      
    }
  }
  return new;
}

void freeColorMatrix(colorMatrix* rm) {
  if (rm != NULL) {
    int rowMax = rm->rows;
    int colMax = rm->cols;
    for (int rowIndex = 0; rowIndex < rowMax; rowIndex++) {
      for (int colIndex = 0; colIndex < colMax; colIndex++) {
	freeColor(rm->cells[rowIndex][colIndex]);
      }
      free(rm->cells[rowIndex]);
    }
    free(rm->cells);
    free(rm);
  }
}

character*** newCharacterMatrix(int cols, int rows) {
  character*** new = malloc(sizeof(character**) * rows);
  for(int rowIndex = 0; rowIndex < rows; rowIndex++) {
    new[rowIndex] = malloc(sizeof(character*) * cols);
    for (int colIndex = 0; colIndex < cols; colIndex++) {
      new[rowIndex][colIndex] = NULL;
    }
  }
  return new;
}


//was lazy and didn't make this a struct
//so this will be fun
//actually this sucks, because shared characters get double freed
//same solution as freeImage, just free the row/collumn mallocs
//freeCharSet should handle the characters assigned
//don't even call this then, just freem rows/columns in freeImage
//when I know the dimensions of the matrix. OR hand those in, either way
void freeCharacterMatrix(character*** rm, int cols, int rows) {
  for(int rowIndex = 0; rowIndex < rows; rowIndex++){
    //don't actually need this, not freeing characters here
    for(int colIndex = cols; colIndex < cols; colIndex++) {
    }
    free(rm[rowIndex]);
  }
  free(rm);
}

intMatrix* createIntMatrix(profileMatrix* prof) {
  return createIntMatrixByDim(prof->cols, prof->rows);
}

intMatrix* createIntMatrixByDim(int cols, int rows) {
  intMatrix* new = malloc(sizeof(intMatrix));
  new->cols = cols;
  new->rows = rows;
  new->ints = malloc(sizeof(int*) * new->cols);
  for(int colIndex = 0; colIndex < new->cols; colIndex++) {
    new->ints[colIndex] = malloc(sizeof(int) * new->rows);
    for(int rowIndex = 0; rowIndex < new->rows; rowIndex++) {
      new->ints[colIndex][rowIndex] = 0;
    }
  }
  return new;
}

void freeIntMatrix(intMatrix* rm) {
  if (rm != NULL) {
    for(int colIndex = 0; colIndex < rm->cols; colIndex++) {
      free(rm->ints[colIndex]);
    }
    free(rm->ints);
    free(rm);
  }
}

myColor* newColor() {
  myColor* new = malloc(sizeof(myColor));
  *new = (myColor){.hue = 0, .sat = 0, .lightness = 0, .red =0, .green = 0, .blue = 0};
  return new;
}

void freeColor(myColor* rm) {
  free(rm);
}

void addColorToColor(myColor* dest, myColor* src) {
  dest->hue += src->hue;
  dest->sat += src->sat;
  dest->lightness += src->lightness;
  dest->red += src->red;
  dest->green += src->green;
  dest->blue += src->blue;
}

void divideColor(myColor* dest, int denom) {
  dest->hue /= denom;
  dest->sat /= denom;
  dest->lightness /= denom;
  dest->red /= denom;
  dest->green /= denom;
  dest->blue /= denom;
}



image* newImage(colorMatrix* entireImage) {
  return newImageByDim(entireImage->cols, entireImage->rows);
}

image* newImageByDim(int cols, int rows) {
  image* new = malloc(sizeof(image));
  new->width = cols;
  new->height = rows;
  new->numberOfRegionCols = -1;
  new->numberOfRegionRows = -1;
  new->profiles = NULL;
  new->filledCharacterss = NULL;
  return new;
}

void freeImage(image* rm) {
  int maxCols = rm->numberOfRegionCols;
  int maxRows = rm->numberOfRegionRows;
  freeCharacterMatrix(rm->filledCharacterss, maxCols, maxRows);
  for(int rowIndex = 0; rowIndex < maxRows; rowIndex++) {
    for(int colIndex = 0; colIndex < maxCols; colIndex++) {
      freeProfileMatrix(rm->profiles[rowIndex][colIndex]);
      //no, free these while traversing characterSet
      //otherwise double frees occur
      //freeCharacter(rm->filledCharacterss[rowIndex][colIndex]);
    }
    free(rm->profiles[rowIndex]);
  }
  free(rm->profiles);
  free(rm);
}

characterSet* newCharacterSet(int size) {
  characterSet * new = malloc(sizeof(characterSet));
  new->characters = malloc(sizeof(character*) * size);
  new->length = size;
  for (int index = 0; index < size; index++) {
    new->characters[index] = NULL;
  }
  return new;
}

void freeCharacterSet(characterSet* rm) {
  for(int index = 0; index < rm->length; index++) {
    freeCharacter(getCharacterAtIndex(rm, index));
  }
  free(rm->characters);
  free(rm);
}

character* getCharacterAtIndex(characterSet* set, int index) {
  return set->characters[index];
}

void setCharacterAtIndex(characterSet* set, int index, character* newCharacter) {
  set->characters[index] = newCharacter;
}

character* newCharacter() {
  character* new = malloc(sizeof(character));
  new->charVal = NULL;
  new->profile = NULL;
  return new; 
}

character* makecharacter(char* value) {
  character* new = malloc(sizeof(character));
  new->charVal = value;
  new->profile = NULL;
  return new;
}

void freeCharacter(character* rm) {
  free(rm->charVal);
  freeProfileMatrix(rm->profile);
  free(rm);
}


edges* initEdges() {
  edges* new = malloc(sizeof(edges));
  *new = (edges){.top = 0, .bottom = 0, .left = 0, .right = 0, .left = 0, .table = 0, .pole = 0, .forward = 0, .backward = 0};
  return new;
}


//getters and settters

int getDiffAtIndex(intMatrix* diffMatrix, int col, int row) {
  if (col >= diffMatrix->cols || row >= diffMatrix->rows ||
      col < 0 || row < 0) {
    return diffErr;
  }
  else {
    return diffMatrix->ints[col][row];
  }
}

void setDiffAtIndex(intMatrix* diffMatrix, int col, int row, int val) {
  if (col > diffMatrix->cols || col > diffMatrix->rows) {
    fprintf(stderr, "Index out of bound\n");
  }
  else {
    diffMatrix->ints[col][row] = val;
  }
}


void setColor(colorMatrix* matrix, int col, int row, myColor* tobe) {
  if (row < matrix->rows && col < matrix->cols) {
    cloneColor(matrix->cells[row][col], tobe);
  }
  else {
    fprintf(stderr, "Given a bad index for colorMatrix in setColor\n");
  }
}

myColor* getColor(colorMatrix* matrix, int col, int row) {
  if (row < matrix->rows && col < matrix->cols &&
      row >= 0 && col >= 0) {
    return matrix->cells[row][col];
  }
  else {
    return NULL;
  }
}

void cloneColor(myColor* dest, myColor* src) {
  //copy value of fields in src to dest
  *dest = *src;
}

void generateLightMarkScoresForImage(image* pic) {
  gen_list* picList = createLightmarkListFromImage(pic);
  fillOutFields(picList);
  freeGen_list(picList);
}

void generateLightMarkScoresForCharacterSet(characterSet* charSet) {
  gen_list* charList = createLightmarkListFromCharacterSet(charSet);
  fillOutFields(charList);
  freeGen_list(charList);
}

void generateLightMarkScores(image* pic, characterSet* charSet){
  gen_list* picList = createLightmarkListFromImage(pic);
  gen_list* charList = createLightmarkListFromCharacterSet(charSet);
  fillOutFields(picList);
  fillOutFields(charList);
  //free lists, but lightmarks remain in profiles in pic and charset
  freeGen_list(picList);
  freeGen_list(charList);
}

//stuff for lightmarks
lightmark* newLightmark(myColor* avgColor) {
  lightmark* new = malloc(sizeof(lightmark));
  new->color = avgColor;
  new->differenceFromMostLight = 0;
  new->differenceFromMostDark = 0;
  return new;
}

gen_list* createLightmarkListFromImage(image* pic) {
  //iterater over profileMatricies, generate lightmarks, set fields, fill list
  int cols = pic->numberOfRegionCols;
  int rows = pic->numberOfRegionRows;
  lightmark* mark;
  gen_list* list = createGen_list();
  
  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < cols; col++) {
      mark = attatchLightmarkToProfile(pic->profiles[row][col]);
      //still not sure if I want to use wrappers to I don't have to touch typecasting directly. 
      appendToGen_list(list, createGen_node(mark));
    }
  }
  return list;
}

gen_list* createLightmarkListFromCharacterSet(characterSet* set) {
  //iteracte over set,  do the things
  int length = set->length;
  lightmark* mark;
  gen_list* list = createGen_list();
  for(int index = 0; index < length; index++) {
    mark = attatchLightmarkToProfile(set->characters[index]->profile);
    appendToGen_list(list, createGen_node(mark));
  }
  return list;
}

lightmark* attatchLightmarkToProfile(profileMatrix* prof) {
  lightmark* new = newLightmark(prof->averageColor);
  prof->mark = new;
  return new;
}

//nead to create a list, traverst list for most light and dark
//then retraverse list and fill out fields.
void fillOutFields(gen_list* l) {
  myColor Dark;
  myColor Light;
  findLADExtremes(l, &Dark, &Light);
  storeLightmarkDifferences(l, &Dark, &Light);
  storeSpanningInfo(l, &Dark, &Light);
}


void findLADExtremes(gen_list* l, myColor* darkColor, myColor* lightColor) {
  myColor* dark, *light;
  dark = NULL;
  light = NULL;
  float darkVal, lightVal, tempVal;
  lightmark* currData;
  gen_node* current = l->start;
  while(current != NULL) {
    currData = (lightmark*)current->stored;
    if (dark == NULL) {
      //need to change to current->data, likely need casting
      dark = currData->color;
      light = currData->color;
      darkVal = dark->lightness;
      lightVal = light->lightness;
    }
    else {
      tempVal = currData->color->lightness;
      if (tempVal > lightVal) {
	light = currData->color;
	lightVal = tempVal;
      }
      else if (tempVal < darkVal) {
	dark = currData->color;
	darkVal = tempVal;
      }
    }
    current = current->next;
  }
  *darkColor = *dark;
  *lightColor = *light;
}


void storeLightmarkDifferences(gen_list* l, myColor* darkColor, myColor* lightColor) {
  float darkVal, lightVal, tempVal;
  lightVal =lightColor->lightness;
  darkVal = darkColor->lightness;
  lightmark* currData;
  gen_node* current = l->start;
  while(current != NULL) {
    currData = (lightmark*)current->stored;
    tempVal = currData->color->lightness;
    currData->differenceFromMostLight = fabs(lightVal - tempVal);
    currData->differenceFromMostDark = fabs(darkVal - tempVal);
    current = current->next;
  }
}

void storeSpanningInfo(gen_list* l, myColor* darkColor, myColor* lightColor) {
  //take darkval and lightval, take difference, store in span
  //compute difference from currColor->lightness and darkval, store in colorSpan
  //store colorSpan / span in spanPercentile
  float darkVal, lightVal, span, colorSpan, tempVal ;
  lightVal =lightColor->lightness;
  darkVal = darkColor->lightness;
  span = lightVal - darkVal;
  lightmark* currData;
  gen_node* current = l->start;
  while(current != NULL) {
    currData = (lightmark*)current->stored;
    tempVal = currData->color->lightness;
    colorSpan = tempVal - darkVal;
    currData->setSpan = span;
    currData->spanPercentile = colorSpan / span * 100;
    current = current->next;
  }
}

