#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "region.h"


//as buggy as this is likely to be
//think I have all the function I want to write
//given a profileMatrix, I have functions to generate an edges struct
//then find a closest match to a char within a charset.

//had a more compact idea how to check edges
//generally have some special traverse function, which takes a start, end, *offset
//will traverse/iterate current position from start to end in a straight-ish line

int diffParam;

int diffYes = 1;
int diffNo = 0;
int diffErr = -1;

//so, fixed a lot of bugs
//had a goof in orthogoal travers, was computing the current position and handing that in as offsets
//caused premature ends
//also, was never initializing bothOOb to zero, which also caused premature ends
//still getting some odd scores of infinity, but it's looking nice besides that
//If I could fix that, could feasibly have edge detection. 
//yeah,just made my edge score be edgehits - edgeMisses instead of dividing
//changed my compare function as well
//actually kind of cool. now my program thinks everything looks like @ symbols.
//cool as fuck that I am getting some output though.
//still have to implement something for how dark/light a region is, then tweek things

static character* closestCharacterToProfile(profileMatrix* subSect,  character** charSet);

void setDiffParam(int new) {
  diffParam = new;
}
void fillDiffMatrix(  intMatrix* detectedEdges, profileMatrix* prof) {
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
      setDiffAtIndex(detectedEdges, colIndex, rowIndex, diffNo);
    }
  }
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
	  setDiffAtIndex(detectedEdges,colIndex,rowIndex, diffYes);
	  setDiffAtIndex(detectedEdges,testC,testR, diffYes);
	}
	testNumber++;
      }            
    }
  }
  if (prof->diff == NULL) {
    prof->diff = detectedEdges;
  }
}


character* matchProfileToCharacter(profileMatrix* prof,  character** charSet) {
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
      //printf("Error, given a profile with no colorMatrix\n");
      return NULL;
    }
  }
  if (prof->edgeScores == NULL) {
    edges* foundEdges = betterPopulateEdges(prof);
    prof->edgeScores = foundEdges;
  }
  return closestCharacterToProfile(prof, charSet);
}

static character* closestCharacterToProfile(profileMatrix* subSect,  character** charSet) {
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
  return match;
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
  int colOff, rowOff, colCur, rowCur, numRuns;

  rowDim = prof->source->rows - 1;
  colDim = prof->source->cols - 1;
  //the -1 keeps traversal within matrix
  //othersise one final check is done outside matrix
  //not hits, no misses, returns 0/0, nan, which messes up cumulative score
  //printf("\nONTO A NEW PROFILE\n");
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
    colOff = 0;
    rowOff = 0;
    numRuns = 0;
    edgeScore = 0;
    while(traverse(colStart, rowStart, &colOff, &rowOff, colEnd, rowEnd) != 1) {
      colCur = colStart + colOff;
      rowCur = rowStart + rowOff;
      edgeScore += betterGenerateEdgeScore(diffMatrix, colCur, rowCur, colDim, rowDim, checkType);
      numRuns++;
    }
    edgeScore = edgeScore/numRuns;
    //printf("\n\n\n\n\nFinished edgeScore is %f\n\n\n\n", edgeScore);
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
  prof->edgeScores = foundEdges;
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
  //you know what would be cool? if I initialized it to zero
  int bothOOB = 0;


  if (whichCheck == topCheck ||whichCheck ==  bottomCheck ||whichCheck ==  tableCheck) {
    colCheckRad = (colDim / 4) + 1;
    rowCheckRad = 0;
    //printf("Checking horizontally\n");
  }
  else if (whichCheck == leftCheck || whichCheck == rightCheck || whichCheck == poleCheck) {
    colCheckRad = 0;
    rowCheckRad = (rowDim / 4) + 1;
    //printf("Checking vertically\n");
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
    //printf("Checking diagnolly\n");
  }

  //printf("start cords are: %d , %d\n", colCur, rowCur);
  //printf("end cords are  : %d , %d\n", colCur + colCheckRad, rowCur + rowCheckRad);
  //had an error here
  //checked bothOOB != 2 last, so if previouse 
  while(orthogonalTraverse(colCur, rowCur,
			   &colOrthOff, &rowOrthOff, 
			   colCur + colCheckRad, rowCur + rowCheckRad) != 1
	&& bothOOB != 2){
    bothOOB = 0;
    //printf("current offsets are: %d , %d\n", colOrthOff, rowOrthOff);
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
      //printf("Have unspecified number %d in col:%d row:%d\n", diff, colCur + colOrthOff, rowCur + rowOrthOff);
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
      //printf("Have unspecified number %d in col:%d row:%d\n", diff, colCur + colOrthOff, rowCur + rowOrthOff);
    }
  }
  //printf("\n\n\nCalculated edge score of %f \n\n", edgeHits - edgeMisses);
  //need to figure out what to do if there are no misses.
  //one idea, could just subtrace the two instead of dividing
  //that would be simple.
  //only issue Is I need to fix my compare, because that takes abs of everything
  return edgeHits - edgeMisses;
}


//following rotate and unrotate
//just running through a rotation matrix of 90 degrees
//
// | cosθ, -sinθ |
// | sinθ, cosθ  |
//since I'm only rotating 90 degrees, don't need sin or cos really
//just
//
// | 0, -1 |
// | 1,  0 |
void rotate9t(int* ox, int* oy) {
  int rotx = *oy;
  int roty = *ox * -1;

  *ox = rotx;
  *oy = roty;
}

void unrotate9t(int* rotx, int* roty) {
  int ox = *roty * -1;
  int oy = *rotx;

  *rotx = ox;
  *roty = oy;
}

int  orthogonalTraverse(int startx, int starty, int* offx, int* offy, int endx, int endy) {
  //given a start, current, and end position
  //will carry out a single step of a path roughly orthogonal to path from start to end
  
  int ret;
  rotate9t(offx, offy);

  int curx = *offx;
  int cury = *offy;

  ret = traverse(startx, starty, &curx, &cury, endx, endy);

  
  *offx = curx;
  *offy = cury;
  unrotate9t(offx, offy);

  return ret;
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
  //printf("Starting regular traversal\nStart pos   are: %d, %d\ncurrent pos   : %d, %d\n end cords are  : %d, %d\n",startx, starty, curx, cury, endx, endy);
  if (curx != endx || cury != endy) {
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
    //printf("done with traversal\n");
    ret = 1;
  }
  curx = startx + *offx;
  cury = starty + *offy;
  //printf("after moving currents: %d, %d\n", curx, cury);
  return ret;
}


float compareProfiles(profileMatrix* p1, profileMatrix* p2) {
  float score = compareEdges(p1->edgeScores, p2->edgeScores);
  //also do something with darkness/lightness averages


  return score;
}

float compareEdges(edges* e1, edges* e2) {
  //compares two edges
  //
  float delta = 0;
  float f1, f2;
  edgeCheck whichCheck = topCheck;
  while(whichCheck != doneChecking) {
    if (whichCheck == topCheck) {
        f1 = e1->top;
	f2 = e2->top;
	whichCheck = bottomCheck;
    }
    else if (whichCheck == bottomCheck) {
        f1 = e1->bottom;
	f2 = e2->bottom;
	whichCheck = leftCheck;
    }
    else if (whichCheck == leftCheck) {
        f1 = e1->left;
	f2 = e2->left;
	whichCheck = rightCheck;
    }
    else if (whichCheck == rightCheck) {
        f1 = e1->right;
	f2 = e2->right;
	whichCheck = tableCheck;
    }
    else if (whichCheck == tableCheck) {
        f1 = e1->table;
	f2 = e2->table;
	whichCheck = poleCheck;
    }
    else if (whichCheck == poleCheck) {
        f1 = e1->pole;
	f2 = e2->pole;
	whichCheck = forwardCheck;
    }
    else if (whichCheck == forwardCheck) {
        f1 = e1->forward;
	f2 = e2->forward;
	whichCheck = backwardCheck;
    }
    else if (whichCheck == backwardCheck) {
        f1 = e1->backward;
	f2 = e2->backward;
	whichCheck = doneChecking;
    }
    delta += fabs(f1 - f2);
  }
  return delta;
}

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

int getPixelDif(myColor* c1, myColor* c2) {
  //what would be a good method for this
  //want to prioritize lightness more than hue more than saturation.
  int difference = 0;
  int scaleAmount = 10;
  if (c1 != NULL && c2 != NULL) {
    difference = (int)fabs(c1->lightness - c2->lightness);
    difference *= scaleAmount;
    difference += (int)fabs(c1->hue - c2->hue);
    difference *= scaleAmount;
    difference += (int)fabs(c1->sat - c2->sat);
  }
  else {
    difference = 0;
  }
  return difference;
  
}

void cloneColor(myColor* dest, myColor* src) {
  //copy value of fields in src to dest
  *dest = *src;
}




