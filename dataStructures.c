
#include <stdlib.h>
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
  return new;
}

void freeProfileMatrix(profileMatrix* rm) {
  freeIntMatrix(rm->diff);
  free(rm->edgeScores);
  freeColorMatrix(rm->source);
  freeColor(rm->averageColor);
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
void freeCharacterMatrix(character*** rm) {
  int colIndex = 0;
  int rowIndex = 0;
  while(rm[rowIndex] != NULL) {
    while(rm[rowIndex][colIndex] != NULL) {
      free(rm[rowIndex][colIndex]);
      colIndex++;
    }
    rowIndex++;
  }
  free(rm);
}

intMatrix* createIntMatrix(profileMatrix* prof) {
  intMatrix* new = malloc(sizeof(intMatrix));
  new->cols = prof->cols;
  new->rows = prof->rows;
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
  for(int colIndex = 0; colIndex < rm->cols; colIndex++) {
    free(rm->ints[colIndex]);
  }
  free(rm->ints);
  free(rm);
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
  image* new = malloc(sizeof(image));
  new->width = entireImage->cols;;
  new->height = entireImage->rows;;
  new->numberOfRegionCols = -1;
  new->numberOfRegionRows = -1;
  new->profiles = NULL;
  new->filledCharacterss = NULL;
  return new;
}

void freeImage(image* rm) {
  int maxCols = rm->numberOfRegionCols;
  int maxRows = rm->numberOfRegionRows;
  for(int rowIndex = 0; rowIndex < maxRows; rowIndex++) {
    for(int colIndex = 0; colIndex < maxCols; colIndex++) {
      freeProfileMatrix(rm->profiles[rowIndex][colIndex]);
    }
    free(rm->profiles[rowIndex]);
    free(rm->filledCharacterss[rowIndex]);
  }
  free(rm->profiles);
  free(rm->filledCharacterss);
  free(rm);
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
