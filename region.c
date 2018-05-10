#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "region.h"

//as buggy as this is likely to be
//think I have all the function I want to write
//given a profileMatrix, I have functions to generate an edges struct
//then find a closest match to a char within a charset.

//next up, have to handle the bigger picture stuff
//getting a picture read in to begin with
//dividing picture into sections, generating colorMatrix
//sticking that in a profile matrix, calling generate edges
//then also do all of that for rasters of whatever characters I want
//sticking those into some array
//then calling  closestCharToProfile for every section of image

//had a more compact idea how to check edges
//generally have some special traverse function, which takes a start, end, *current position
//will traverse/iterate current position from start to end in a straight-ish line
//because right now I have about 2--300 lines of boiler plate loops


edges * generateEdges(profileMatrix* prof) {
  //int detectedEdges[prof->source->cols][prof->source->rows];
  int** detectedDiffs = NULL;
  fillDiffMatrix(detectedDiffs, prof);
  edges* found = findEdges(detectedDiffs, prof);
  prof->edgeScores = found;
  return found;
}



void fillDiffMatrix(int **detectedEdges, profileMatrix* prof) {
  //given a profileMatrix, will go through the colorMatrix
  //comparing adjacent pixels. it the difference is large enough
  //will set the parallell entry in detectedEdges of both pixels to some positive number
  
  //threshold for gitpixelDiff
  //like the idea of this being some value ralative to picture I'm looking at
  //so pictures with a high color/darkness conformity will get lower thresholds, while more varying images will get higher ones. 
  int param = 12;
  //had idea for param. Do the same traversal as when i compare to the param, but instead keep a cumulative sum of compare values
  //then divide by number of comparision, and you have an average comparision result.
  //could either set this average by going through the entire image and having a global param
  //or do it in params and have each param have it's own average. 

  int rows = prof->source->rows;
  int cols = prof->source->cols;
  //int detectedEdges[cols][rows];
  for (int c = 0; c < cols; c++) {
    for (int r = 0; r < rows; r++) {
      detectedEdges[c][r] = 0;
    }
  }

  //this loop, goes through and compares adjacent pixels
  
  int testC, testR, testNumber;
  myColor* current, *compare;
  for (int colIndex = 0; colIndex < cols; colIndex++) {
    for (int rowIndex = 0; rowIndex < rows; rowIndex++) {
      current = getPixel(prof->source, colIndex, rowIndex);
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
	  compare = getPixel(prof->source, testC, testR);
	}
	else if (testNumber == 3) {
	  //pixel below
	  testC = colIndex;
	  compare = getPixel(prof->source, testC, testR);
	}
	else if (testNumber == 4) {
	  //pixel diagnol bottom left
	  testC = colIndex - 1;
	  compare = getPixel(prof->source, testC, testR);
	  //...
	  //don't set to zero,  I increment testNumber 
	  testNumber = -10;
	}
	compare = getPixel(prof->source, testC, testR);
	//eventually...
	if (compare != NULL && getPixelDif(current, compare) > param) {
	  //detect edge
	  detectedEdges[colIndex][rowIndex] = 1;
	  detectedEdges[testC][testR] = 1;
	}
	testNumber++;
      }            
    }
  } 
}


edges* findEdges(int **filledDiff, profileMatrix* prof) {
  //takes a diff matrix(filled by fillDiffMatrix)
  //tries to group non-zero entry in matrix to certain edges
  
  edges* foundEdges = initEdges();
  int row, col, rowMax, colMax, rowDim, colDim;
  rowDim = prof->source->rows;
  colDim = prof->source->cols;
  
  
  //going to need to be some coordination between where I'm initializing indexes
  //and how advanceEdgeCheck changes them
  //standard, start in the upper left sides. having forward slash start in upper right

  edgeCheck checkType = topCheck;
  float edgeScore = 0;
  int numRuns = 0;
  while(checkType != forwardCheck) {
    if (checkType == topCheck) {
      row = 0;
      col = 0;
      rowMax = row;
      colMax = colDim;
    }
    else if (checkType == bottomCheck) {
      row = rowDim;
      col = 0;
      rowMax = row;
      colMax = colDim;
    }
    else if (checkType == leftCheck) {
      row = 0;
      col = 0;
      rowMax = rowDim;
      colMax = col;
    }
    else if (checkType == rightCheck) {
      row = 0;
      col = colDim;
      rowMax = rowDim;
      colMax = col;
    }
    else if (checkType == tableCheck) {
      row = rowDim / 2;
      col = 0;
      rowMax = row;
      colMax = colDim;
    }
    else if (checkType == poleCheck) {
      row = 0;
      col = colDim / 2;
      rowMax = rowDim;
      colMax = col;
    }
    else if (checkType == backwardCheck) {
      row = 0;
      col = 0;
      rowMax = rowDim;
      colMax = colDim;
    }
    while (row < rowMax || col < colMax) {
      edgeScore += checkForEdge(filledDiff, row, col, rowDim, colDim,  checkType);
      advanceEdgeCheck(&row, &col, rowDim, colDim, checkType);
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
    edgeScore = 0;
    numRuns = 0;    
  }
  
  //forward check is special, because it's starting colPosition is greater than it's end position.
  //so I can't put it in the loop, because I'd change the while conditional.
  //could have the col = -colDim, then when in checkForEdge hand in abs(colDim)
  //kind of goofy though 

  checkType = forwardCheck;
  edgeScore = 0;
  numRuns = 0;
  
  row = 0;
  col = colDim;
  rowMax = rowDim;
  colMax = 0;
  //while (row < rowMax || col < colMax) {
  while (row < rowMax || col >= colMax) {
    edgeScore += checkForEdge(filledDiff, row, col, rowDim, colDim,  checkType);
    advanceEdgeCheck(&row, &col, rowMax, colMax, checkType);
    numRuns++;
  }
  edgeScore = edgeScore/numRuns;
  foundEdges->forward = edgeScore;
  

    
  return foundEdges;
}

float checkForEdge(int **diff, int row, int col, int rowDim, int colDim,  edgeCheck whichCheck) {
  //checks for edges along a line roughly orthogonal to the edge whichCheck
  //general idea, have a decay, so successive hits/missess are worth less
  //attempting to prevent case of a strong grouping of diffs in matrix in a corner
  //giving a high/low  edge score. 
  float edgeHits = 0;
  float edgeMisses = 0;

  float hitAmount = 1;
  float missAmount = 1;

  float hitWeight = 1;
  float missWeight = 1;


  //on successive (as in, multiple within a loop) hits,
  // weight *= decay
  //was also toying with the idea of making things less the greater offset got.
  //would need to do something like offDecay = (checkRadius - offset);
  //then have weight *= decay * offDecay
  float hitDecay = .75;
  float missDecay = .60;


  int checkRadius;
  //sets/pairs of checkradius for specific edges
  int topBottomRads = (rowDim / 4) + 1;
  int leftRightRads = (colDim / 4) + 1;
  //diag rads, they get two because of reasons
  int diagRadsVert = (colDim / 2) + 1;
  int diagRadsHorz = (rowDim / 2) + 1;

  switch(whichCheck) {
  case topCheck:
    checkRadius = topBottomRads;
    for(int rowOffset = 0; rowOffset < checkRadius; rowOffset++) {
      if (diff[col][row + rowOffset] != 0) {
	edgeHits += hitWeight * hitAmount;
	hitWeight *= hitDecay;
      }
      else {
	edgeMisses += missWeight * missAmount;
	missWeight *= missDecay;
      }
    }
    break;
  case bottomCheck:
    checkRadius = topBottomRads;
    for(int rowOffset = 0; rowOffset < checkRadius; rowOffset++) {
      if (diff[col][row - rowOffset] != 0) {
	edgeHits += hitWeight * hitAmount;
	hitWeight *= hitDecay;
      }
      else {
	edgeMisses += missWeight * missAmount;
	missWeight *= missDecay;
      }
    }
    break;
  case leftCheck:
    checkRadius = leftRightRads;
    for(int colOffset = 0; colOffset < checkRadius; colOffset++) {
      if (diff[col + colOffset][row] != 0) {
	edgeHits += hitWeight * hitAmount;
	hitWeight *= hitDecay;
      }
      else {
	edgeMisses += missWeight * missAmount;
	missWeight *= missDecay;
      }
    }
    break;
  case rightCheck:
    checkRadius = leftRightRads;
    for(int colOffset = 0; colOffset < checkRadius; colOffset++) {
      if (diff[col - colOffset][row] != 0) {
	edgeHits += hitWeight * hitAmount;
	hitWeight *= hitDecay;
      }
      else {
	edgeMisses += missWeight * missAmount;
	missWeight *= missDecay;
      }
    }
    break;
  case tableCheck:
    checkRadius = topBottomRads;
    for(int rowOffset = 0; rowOffset < checkRadius; rowOffset++) {
      if (diff[col][row - rowOffset] != 0) {
	edgeHits += hitWeight * hitAmount;
	hitWeight *= hitDecay;
      }
      else {
	edgeMisses += missWeight * missAmount;
	missWeight *= missDecay;
      }
      if (diff[col][row + rowOffset] != 0) {
	edgeHits += hitWeight * hitAmount;
	hitWeight *= hitDecay;
      }
      else {
	edgeMisses += missWeight * missAmount;
	missWeight *= missDecay;
      }
    }
    break;
  case poleCheck:
    checkRadius = leftRightRads;
    for(int colOffset = 0; colOffset < checkRadius; colOffset++) {
      if (diff[col - colOffset][row] != 0) {
	edgeHits += hitWeight * hitAmount;
	hitWeight *= hitDecay;
      }
      else {
	edgeMisses += missWeight * missAmount;
	missWeight *= missDecay;
      }
      if (diff[col + colOffset][row] != 0) {
	edgeHits += hitWeight * hitAmount;
	hitWeight *= hitDecay;
      }
      else {
	edgeMisses += missWeight * missAmount;
	missWeight *= missDecay;
      }
    }
    
    break;
  case forwardCheck:;
    //diagnol checking
    //trace out a line that is orthagonal to diagnol line
    //current way is same as how I advanceEdgeCheck for diagnol edges
    //just have the controlling ratio inverted.
    int ratio = colDim / rowDim;
    int rowOffset = 0;
    int colOffset = 0;
    while (rowOffset < diagRadsVert || colOffset < diagRadsHorz) {
      if (diff[col - colOffset][row - rowOffset] != 0) {
	edgeHits += hitWeight * hitAmount;
	hitWeight *= hitDecay;
      }
      else {
	edgeMisses += missWeight * missAmount;
	missWeight *= missDecay;
      }
      if (diff[col + colOffset][row + rowOffset] != 0) {
	edgeHits += hitWeight * hitAmount;
	hitWeight *= hitDecay;
      }
      else {
	edgeMisses += missWeight * missAmount;
	missWeight *= missDecay;
      }
      if (rowOffset == 0) {
	rowOffset++;
      }
      else if (colOffset >= diagRadsHorz) {
	rowOffset++;
      }
      else if (rowOffset >= diagRadsVert) {
	colOffset++;
	  }
      else if (colOffset / rowOffset > ratio) {
	rowOffset++;
      }
      else {
	colOffset++;
      }
      break;
    case backwardCheck:;    
      //diagnol checking
      //checks from top left to bottom right
      int ratio = colDim / rowDim;
      int rowOffset = 0;
      int colOffset = 0;
      while (rowOffset < diagRadsVert || colOffset < diagRadsHorz) {
	if (diff[col + colOffset][row - rowOffset] != 0) {
	  edgeHits += hitWeight * hitAmount;
	  hitWeight *= hitDecay;
	}
	else {
	  edgeMisses += missWeight * missAmount;
	  missWeight *= missDecay;
	}
	if (diff[col - colOffset][row + rowOffset] != 0) {
	  edgeHits += hitWeight * hitAmount;
	  hitWeight *= hitDecay;
	}
	else {
	  edgeMisses += missWeight * missAmount;
	  missWeight *= missDecay;
	}
	//just copied code from forward check
	if (rowOffset == 0) {
	  rowOffset++;
	}
	else if (colOffset >= diagRadsHorz) {
	  rowOffset++;
	}
	else if (rowOffset >= diagRadsVert) {
	  colOffset++;
	    }
	else if (colOffset / rowOffset > ratio) {
	  rowOffset++;
	}
	else {
	  colOffset++;
	}
      }
      
      break;
    default:
      
      break;
      
    }
  
  }
  return hitAmount / missAmount;
}

void advanceEdgeCheck(int* row, int* col, int rowDim, int colDim, edgeCheck whichCheck) {
  //iterates row/col indexs by one to follow along an edge given by whichCheck 
  float ratio = (float)rowDim / colDim;
  switch(whichCheck) {
  case topCheck:
    (*col)++;
    break;
  case bottomCheck:
    (*col)++;
    break;
  case leftCheck:
    (*row)++;
    break;
  case rightCheck:
    (*row)++;
    break;
  case tableCheck:
    //ditto
    (*col)++;

    break;
  case poleCheck:
    //start in top middleish
    //really simple advance
    (*row)++;

    break;
  case forwardCheck:
    //does forward check, from top right to bottom left.
    //two approaches, could either to complicated stuff
    //or just run it through the same code for backward check, then colIndex = colMax - colIndex
    //wait no, colIndex = colDim - colIndex;
    //that will not work either, on repeated calls it'd just do stupid things
    if (*col == colDim) {
      (*col)--;
    }
    else if (*row >= rowDim) {
      (*col)--;
    }
    else if (*col <= 0) {
      (*row)++;
    }
    else if ((float)*row / (colDim - *col)  > ratio) {
      (*col)--;
    }
    else {
      (*row)++;
    }


    break;
  case backwardCheck:
    //does backward check, from top left to bottom right.
    //col/row start both at zero
    if (*col == 0) {
      (*col)++;
    }
    else if (*row >= rowDim) {
      (*col)++;
    }
    else if (*col >= colDim) {
      (*row)++;
    }
    else if ((float)*row / *col > ratio) {
      (*col)++;
    }
    else {
      (*row)++;
    }
    break;  
  default:

    break;
      
  }
}

char* closestCharToProfile(profileMatrix* subSect, character** charSet) {
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
