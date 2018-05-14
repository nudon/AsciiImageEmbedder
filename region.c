#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "region.h"

//as buggy as this is likely to be
//think I have all the function I want to write
//given a profileMatrix, I have functions to generate an edges struct
//then find a closest match to a char within a charset.

//had a more compact idea how to check edges
//generally have some special traverse function, which takes a start, end, *current position
//will traverse/iterate current position from start to end in a straight-ish line
//because right now I have about 2--300 lines of boiler plate loops

int diffParam;

int diffYes;
int diffNo;
int diffErr;

void setDiffParam(int new) {
  diffParam = new;
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


int getDiffAtIndex(intMatrix* diffMatrix, int col, int row) {
  if (col > diffMatrix->cols || col > diffMatrix->rows) {
    return diffErr;;
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

myColor* newColor() {
  myColor* new = malloc(sizeof(myColor));
  *new = (myColor){.hue = 0, .sat = 0, .lightness = 0, .red =0, .green = 0, .blue = 0};
  return new;
}

colorMatrix* newColorMatrix(int rows, int cols) {
  colorMatrix* new = malloc(sizeof(colorMatrix));
  new->rows = rows;
  new->cols = cols;
  new->cells = malloc(sizeof(myColor**) * cols);
  for(int colIndex = 0; colIndex < cols; colIndex++) {
    new->cells[colIndex] = malloc(sizeof(myColor*) * rows);
    for(int rowIndex =0; rowIndex < rows; rowIndex++) {
      new->cells[colIndex][rowIndex] = newColor();
      
    }
  }
  return new;
}

void setColor(colorMatrix* matrix, int row, int col, myColor* tobe) {
  if (row < matrix->rows && col < matrix->cols) {
    cloneColor(matrix->cells[col][row], tobe);
  }
  else {
    fprintf(stderr, "Given a bad index for colorMatrix in setColor\n");
  }
}

myColor* getColor(colorMatrix* matrix, int row, int col) {
  if (row < matrix->rows && col < matrix->cols) {
    return matrix->cells[col][row];
  }
  else {
    return NULL;
  }
}

int getPixelDif(myColor* c1, myColor* c2) {
  //what would be a good method for this
  //want to prioritize lightness more than hue more than saturation.
  int difference = 0;
  int scaleAmount = 10;
  difference = (int)fabs(c1->lightness - c2->lightness);
  difference *= scaleAmount;
  difference += (int)fabs(c1->hue - c2->hue);
  difference *= scaleAmount;
  difference += (int)fabs(c1->sat - c2->sat);

  return difference;
  
}

void cloneColor(myColor* dest, myColor* src) {
  //copy value of fields in src to dest
  *dest = *src;
}

profileMatrix* newProfileMatrix(colorMatrix* colors) {
  profileMatrix* new = malloc(sizeof(profileMatrix));
  new->source = colors;
  new->rows = colors->rows;
  new->cols = colors->cols;
  return new;
}

void fillDiffMatrix(  intMatrix* detectedEdges, profileMatrix* prof) {
  //quantamly entagled with averageCompareResults
  //lab equipment indicates that if there is a bug in this function, there is a bug in the other as well
  
  //given a profileMatrix, will go through the colorMatrix
  //comparing adjacent pixels. it the difference is large enough
  //will set the parallell entry in detectedEdges of both pixels to some positive number
  
  //threshold for gitpixelDiff
  //like the idea of this being some value ralative to picture I'm looking at
  //so pictures with a high color/darkness conformity will get lower thresholds, while more varying images will get higher ones. 
  int param = diffParam;
  //had idea for param. Do the same traversal as when i compare to the param, but instead keep a cumulative sum of compare values
  //then divide by number of comparision, and you have an average comparision result.
  //could either set this average by going through the entire image and having a global param
  //or do it in params and have each param have it's own average. 

  int rows = prof->source->rows;
  int cols = prof->source->cols;
  int testC, testR, testNumber;
  myColor* current, *compare;
  for (int colIndex = 0; colIndex < cols; colIndex++) {
    for (int rowIndex = 0; rowIndex < rows; rowIndex++) {
      current = getColor(prof->source, colIndex, rowIndex);
      testNumber = 1;
      while(testNumber > 0) {
	if (testNumber == 1) {
	  //pixel to right
	  testC = colIndex + 1;
	  testR = rowIndex;
	}
	else if (testNumber == 2) {
	  //pixel diagnol bottom right
	  testR = rowIndex + 1;
	}
	else if (testNumber == 3) {
	  //pixel below
	  testC = colIndex;
	}
	else if (testNumber == 4) {
	  //pixel diagnol bottom left
	  testC = colIndex - 1;
	  //...
	  //don't set to zero,  I increment testNumber 
	  testNumber = -10;
	}
  	compare = getColor(prof->source, testC, testR);
	//eventually...
	if (compare != NULL && getPixelDif(current, compare) > param) {
	  //detect edge
	  setDiffAtIndex(detectedEdges,colIndex,rowIndex, 1);
	  setDiffAtIndex(detectedEdges,testC,testR, 1);
	}
	testNumber++;
      }            
    }
  } 
}


char* matchProfileToChar(profileMatrix* prof,  character** charSet) {
  //given a profileMatrix with a color matrix, will generate a diff matrix
  //then generate an edge scores based on diff matrix
  //then find a closest match to a character in charSet
  if (prof->diff == NULL) {
    intMatrix* diff = createIntMatrix(prof);
    prof->diff = diff;
    if (prof->source != NULL) {
      fillDiffMatrix(diff, prof);
    }
    else {
      printf("Error, given a profile with no colorMatrix\n");
      return NULL;
    }
  }
  if (prof->edgeScores == NULL) {
    edges* foundEdges = betterPopulateEdges(prof);
    prof->edgeScores = foundEdges;
  }
  return closestCharToProfile(prof, charSet);
}

edges* betterPopulateEdges(profileMatrix* prof) {
  //given a profile with diffs
  //will carry out multiple traversals and orthogonal traversals
  //co calculate and assign edgeScores
  intMatrix* diffMatrix = prof->diff;
  edges* foundEdges = initEdges();
  float edgeScore;
  edgeCheck checkType = topCheck;
  int colStart, rowStart, colEnd, rowEnd, colDim, rowDim;

  rowDim = prof->source->rows;
  colDim = prof->source->cols;
  
  while(checkType != doneChecking) {
    if (checkType == topCheck) {
      rowStart = 0;
      colStart = 0;
      rowEnd = rowStart;
      colEnd = colDim;
    }
    else if (checkType == bottomCheck) {
      rowStart = rowDim;
      colStart = 0;
      rowEnd = rowStart;
      colEnd = colDim;
    }
    else if (checkType == leftCheck) {
      rowStart = 0;
      colStart = 0;
      rowEnd = rowDim;
      colEnd = colStart;
    }
    else if (checkType == rightCheck) {
      rowStart = 0;
      colStart = colDim;
      rowEnd = rowDim;
      colEnd = colStart;
    }
    else if (checkType == tableCheck) {
      rowStart = rowDim / 2;
      colStart = 0;
      rowEnd = rowStart;
      colEnd = colDim;
    }
    else if (checkType == poleCheck) {
      rowStart = 0;
      colStart = colDim / 2;
      rowEnd = rowDim;
      colEnd = colStart;
    }
    else if (checkType == backwardCheck) {
      rowStart = 0;
      colStart = 0;
      rowEnd = rowDim;
      colEnd = colDim;
    }
    else if (checkType == forwardCheck) {
      rowStart = 0;
      colStart = colDim;
      rowEnd = rowDim;
      colEnd = 0;
    }
    int colOff = 0;
    int rowOff = 0;
    int colCur;
    int rowCur;
    int numRuns = 0;
    while(traverse(colStart, rowStart, &colOff, &rowOff, colEnd, rowEnd) != 1) {
      colCur = colStart + colOff;
      rowCur = rowStart + rowOff;
      edgeScore += betterGenerateEdgeScore(diffMatrix, colCur, rowCur, colDim, rowDim, checkType);
      numRuns++;
    }
    edgeScore = edgeScore/numRuns;
    if (checkType == topCheck) {
      foundEdges->top = edgeScore;
      checkType = bottomCheck;
    }
    else if (checkType == bottomCheck) {
      foundEdges->bottom = edgeScore;
      checkType = leftCheck;
    }
    else if (checkType == leftCheck) {
      foundEdges->left = edgeScore;
      checkType = rightCheck;
    }
    else if (checkType == rightCheck) {
      foundEdges->right = edgeScore;
      checkType = tableCheck;
    }
    else if (checkType == tableCheck) {
      foundEdges->table = edgeScore;
      checkType = poleCheck;
    }
    else if (checkType == poleCheck) {
      foundEdges->pole = edgeScore;
      checkType = backwardCheck;
    }
    else if (checkType == backwardCheck) {
      foundEdges->backward = edgeScore;
      checkType = forwardCheck;
    }
    else if (checkType == forwardCheck) {
      foundEdges->forward = edgeScore;
      checkType = doneChecking;
    }
  }
  return foundEdges;
}

float betterGenerateEdgeScore(intMatrix* diffMatrix, int colCur, int rowCur, int colDim, int rowDim, edgeCheck whichCheck) {
  //given a cur index, carries out a roughly orthogonal traversal
  //and computes a result based on number of 1's and 0's in diffMatrix
  
  //beyond that, maybe have a distanceFromCurrent decay
  //would have to plan it so when both orthOffsets near the checkRads
  //the decay would be some fixed number, like .1
  //so decay is proportional to distance of rectangle
  //not the number of loops

  float edgeHits = 0;
  float edgeMisses = 0;

  float hitAmount = 1;
  float missAmount = 1;

  float hitWeight = 1;
  float missWeight = 1;
  float hitDecay = .75;
  float missDecay = .60;

  int rowOrthOff = 0;
  int colOrthOff = 0;
  
  int colCheckRad;
  int rowCheckRad;

  int diff;
  //stands for both out of bound
  //could of called it BOOB
  int bothOOB;
  if (whichCheck == topCheck ||whichCheck ==  bottomCheck ||whichCheck ==  tableCheck) {
    colCheckRad = (colDim / 4) + 1;
    rowCheckRad = 0;
  }
  else if (whichCheck == leftCheck || whichCheck == rightCheck || whichCheck == poleCheck) {
    colCheckRad = 0;
    rowCheckRad = (rowDim / 4) + 1;
  }
  else if (whichCheck == forwardCheck || whichCheck == backwardCheck) {
    //some concern here
    //potentially ratios could be super skewed
    //in which case I basically check entire array for diffs
    //might be okay, 2 reasons
    //first, most font aspect ratios are about .5, so using dim / 4 should work
    //second, if fonr ratio was super odd, like .1,
    //then there would be little distinction between diagnols and straight lines anyways
    colCheckRad = (colDim / 4) + 1;
    rowCheckRad = (rowDim / 4) + 1;
  }


  while(orthogonalTraverse(colCur, rowCur,
			   &colOrthOff, &rowOrthOff, 
			   colCur + colCheckRad, rowCur + rowCheckRad) != 1
	&& bothOOB != 2){
    bothOOB = 0;
    diff = getDiffAtIndex(diffMatrix,colCur + colOrthOff,rowCur + rowOrthOff);
    if (diff == diffYes) {
	edgeHits += hitWeight * hitAmount;
	hitWeight *= hitDecay;
    }
    else if (diff == diffNo){
	edgeMisses += missWeight * missAmount;
	missWeight *= missDecay;
    }
    else if (diff == diffErr) {
      bothOOB++;
    }
    else {
      printf("Have unspecified number %d in col:%d row:%d\n", diff, colCur + colOrthOff, rowCur + rowOrthOff);
    }
    
    diff = getDiffAtIndex(diffMatrix,colCur - colOrthOff,rowCur - rowOrthOff);
    if (diff == diffYes) {
	edgeHits += hitWeight * hitAmount;
	hitWeight *= hitDecay;
    }
    else if (diff == diffNo){
	edgeMisses += missWeight * missAmount;
	missWeight *= missDecay;
    }
    else if (diff == diffErr) {
      bothOOB++;
    }
    else {
      printf("Have unspecified number %d in col:%d row:%d\n", diff, colCur + colOrthOff, rowCur + rowOrthOff);
    }
  }
  return edgeHits / edgeMisses;
}


int traverse(int startx, int starty, int* offx, int* offy, int endx, int endy) {
  //given a start, current, and end position
  //while carry out a single step of a traversal from start to end
  //returns 1 when done with traversal
  //ratios, using y/x
  int ret;
  float controlRatio;
  float currentRatio;
  int changex = 0;
  int changey = 0;
  int curx = startx + *offx;
  int cury = starty + *offy;
  if (curx != endx && cury != endy) {
    if (curx < endx) {
      changex = 1;
    }
    else if (curx > endx) {
      changex = -1;
    }
    if (cury < endy) {
      changey = 1;
    }
    else if (cury > endy) {
      changey = -1;
    }
    
    if (curx == endx) {
      *offy += 1 * changey;
    }
    else if (cury == endy) {
      *offx += 1 * changex;
    }
    else {
      if (curx == startx) {
	*offx += 1 * changex;
      }
      else {
	controlRatio = (float)abs(endy - starty) / abs(endx - startx);
	currentRatio = (float)abs(*offy)/ abs(*offx);
	if (controlRatio > currentRatio) {
	  *offy += 1 * changey;
	}
	else {
	  *offx += 1 * changex;
	}
      }
    }
    ret = 0;
  }
  else {
    //already at end
    ret = 1;
  }
  return ret;
}

int  orthogonalTraverse(int startx, int starty, int* offx, int* offy, int endx, int endy) {
  //given a start, current, and end position
  //will carry out a single step of a path roughly orthogonal to path from start to end
  //first, transform current cords
  //flip components, negate y
  int ret;
  int curx = startx + *offx;
  int cury = starty + *offy;
  int transx = cury;
  int transy = curx * -1;
  //throw into traverse
 
  ret = traverse(startx, starty, &transx, &transy, endx, endy);

  // then negate y, flip components
  transy *= -1;
  curx = transy;
  cury = transx;
  
  *offx = curx - startx;
  *offy = cury - starty;
  //voila
  
  //just running through a rotation matrix of 90 degrees
  //
  // | cosθ, -sinθ |
  // | sinθ, cosθ  |
  //since I'm only rotating 90 degrees, don't need sin or cos really
  //just
  //
  // | 0, -1 |
  // | 1,  0 |

  return ret;
}
 
char* closestCharToProfile(profileMatrix* subSect,  character** charSet) {
  //given a profile, finds a closest match to a profile found in charSet
  int index = 0;
  float score;
  float min = -1;
  character* match = NULL;
  while(charSet[index] != NULL) {
    score = compareProfiles(subSect, charSet[index]->profile);
    if (score < min || match == NULL) {
      min = score;
      match = charSet[index];
    }
    index++;
  }
  if (match == NULL) {
    printf("segfaults inbound because no match\n");
  }
  return &(match->character);
}

float compareProfiles(profileMatrix* p1, profileMatrix* p2) {
  float score = compareEdges(p1->edgeScores, p2->edgeScores);
  //also do something with darkness/lightness averages


  return score;
}

float compareEdges(edges* e1, edges* e2) {
  //will see how this work. 
  float delta = 0;
  float f1, f2;
  f1 = e1->top;
  f2 = e2->top;
  delta += fmax(fabs(f1), fabs(f2)) - fmin(fabs(f1), fabs(f2));
  f1 = e1->bottom;
  f2 = e2->bottom;
  delta += fmax(fabs(f1), fabs(f2)) - fmin(fabs(f1), fabs(f2));
  f1 = e1->left;
  f2 = e2->left;
  delta += fmax(fabs(f1), fabs(f2)) - fmin(fabs(f1), fabs(f2));
  f1 = e1->right;
  f2 = e2->right;
  delta += fmax(fabs(f1), fabs(f2)) - fmin(fabs(f1), fabs(f2));
  f1 = e1->table;
  f2 = e2->table;
  delta += fmax(fabs(f1), fabs(f2)) - fmin(fabs(f1), fabs(f2));
  f1 = e1->pole;
  f2 = e2->pole;
  delta += fmax(fabs(f1), fabs(f2)) - fmin(fabs(f1), fabs(f2));
  f1 = e1->forward;
  f2 = e2->forward;
  delta += fmax(fabs(f1), fabs(f2)) - fmin(fabs(f1), fabs(f2));
  f1 = e1->backward;
  f2 = e2->backward;
  delta += fmax(fabs(f1), fabs(f2)) - fmin(fabs(f1), fabs(f2));
  return delta;
}

edges* initEdges() {
  edges* new = malloc(sizeof(edges));
  *new = (edges){.top = 0, .bottom = 0, .left = 0, .right = 0, .left = 0, .table = 0, .pole = 0, .forward = 0, .backward = 0};
  return new;
}
