#include <stdlib.h>
#include "region.h"

edges * detectEdge(profileMatrix* prof) {
  //int detectedEdges[prof->source->cols][prof->source->rows];
  int** detectedDiffs = NULL;
  fillDiffMatrix(detectedDiffs, prof);
  edges* found = findEdges(detectedDiffs, prof);
    //anyways, as long as I'm only considering a reduced set of edges to idenfiy, could just have several different for loops that walk across the array, sush as diagonally, and checking if cells within a certain array are flipped.
  //so, walk down some imaginary line in array. have another parameter for for far to look from line for deteced edges. good to have parameter be relative to dim of profile
  //keep track of how many edges within boundary along line were detected, if thats above another parameter(or maybe some number depending on dim of profileMatrix/colorMatrix
  //then say you found a certain kind of edge.
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

  //calculate topEdge score
  edgeCheck checkType = topCheck;
  float edgeScore = 0;
  int numRuns = 0;
  
  row = 0;
  col = 0;
  rowMax = row;
  colMax = colDim;
  while (row < rowMax || col < colMax) {
    edgeScore += checkForEdge(filledDiff, row, col, rowMax, colMax, rowDim, colDim,  checkType);
    advanceEdgeCheck(&row, &col, rowMax, colMax, checkType);
    numRuns++;
  }
  edgeScore = edgeScore/numRuns;
  foundEdges->top = edgeScore;

  
  checkType = bottomCheck;
  edgeScore = 0;
  numRuns = 0;
  
  row = rowDim;
  col = 0;
  rowMax = row;
  colMax = colDim;
  while (row < rowMax || col < colMax) {
    edgeScore += checkForEdge(filledDiff, row, col, rowMax, colMax, rowDim, colDim,  checkType);
    advanceEdgeCheck(&row, &col, rowMax, colMax, checkType);
    numRuns++;
  }
  edgeScore = edgeScore/numRuns;
  foundEdges->bottom = edgeScore;


  checkType = leftCheck;
  edgeScore = 0;
  numRuns = 0;
  
  row = 0;
  col = 0;
  rowMax = rowDim;
  colMax = col;
  while (row < rowMax || col < colMax) {
    edgeScore += checkForEdge(filledDiff, row, col, rowMax, colMax, rowDim, colDim,  checkType);
    advanceEdgeCheck(&row, &col, rowMax, colMax, checkType);
    numRuns++;
  }
  edgeScore = edgeScore/numRuns;
  foundEdges->left = edgeScore;

    
  checkType = rightCheck;
  edgeScore = 0;
  numRuns = 0;
  
  row = 0;
  col = colDim;
  rowMax = rowDim;
  colMax = col;
  while (row < rowMax || col < colMax) {
    edgeScore += checkForEdge(filledDiff, row, col, rowMax, colMax, rowDim, colDim,  checkType);
    advanceEdgeCheck(&row, &col, rowMax, colMax, checkType);
    numRuns++;
  }
  edgeScore = edgeScore/numRuns;
  foundEdges->right = edgeScore;


  checkType = tableCheck;
  edgeScore = 0;
  numRuns = 0;
  
  row = rowDim / 2;
  col = 0;
  rowMax = row;
  colMax = colDim;
  while (row < rowMax || col < colMax) {
    edgeScore += checkForEdge(filledDiff, row, col, rowMax, colMax, rowDim, colDim,  checkType);
    advanceEdgeCheck(&row, &col, rowMax, colMax, checkType);
    numRuns++;
  }
  edgeScore = edgeScore/numRuns;
  foundEdges->table = edgeScore;


  checkType = poleCheck;
  edgeScore = 0;
  numRuns = 0;
  
  row = 0;
  col = colDim / 2;
  rowMax = rowDim;
  colMax = col;
  while (row < rowMax || col < colMax) {
    edgeScore += checkForEdge(filledDiff, row, col, rowMax, colMax, rowDim, colDim,  checkType);
    advanceEdgeCheck(&row, &col, rowMax, colMax, checkType);
    numRuns++;
  }
  edgeScore = edgeScore/numRuns;
  foundEdges->pole = edgeScore;


  //have a problem that applies to my diagnol lines
  //both of them have some start at a max, and an end at a zero
  //this kills my while conditions, and also conditions within advance check.
  //well just modify the conditions when working with diagnols.
  //that or use absolue values?. either way
  //wait no there both positive, that does not work
  //just change your conditionals. Pretty sure it's only a problem here
  //in checkForEdges, 

  checkType = forwardCheck;
  edgeScore = 0;
  numRuns = 0;
  
  row = 0;
  col = colDim;
  rowMax = rowDim;
  colMax = 0;
  while (row < rowMax || col < colMax) {
    edgeScore += checkForEdge(filledDiff, row, col, rowMax, colMax, rowDim, colDim,  checkType);
    advanceEdgeCheck(&row, &col, rowMax, colMax, checkType);
    numRuns++;
  }
  edgeScore = edgeScore/numRuns;
  foundEdges->forward = edgeScore;


  checkType = backwardCheck;
  edgeScore = 0;
  numRuns = 0;
  
  row = 0;
  col = 0;
  rowMax = rowDim;
  colMax = colDim;
  while (row < rowMax || col < colMax) {
    edgeScore += checkForEdge(filledDiff, row, col, rowMax, colMax, rowDim, colDim,  checkType);
    advanceEdgeCheck(&row, &col, rowMax, colMax, checkType);
    numRuns++;
  }
  edgeScore = edgeScore/numRuns;
  foundEdges->backward = edgeScore;
    
  return foundEdges;
}

float checkForEdge(int **diff, int row, int col, int rowMax, int colMax, int rowDim, int colDim,  edgeCheck whichCheck) {
  //bit of an odd thing, have to add a new parameter pair
  //have row/col dim, which are useful for getting aspect raio of diff, used in diagnol bits
  //also need a row/col end value, for when to end the search. Generally these aren't the same. 
  //general idea, at cell [row][col], performing edgeCheck whichCheck
  //based on that, check cells within some radius and see if there are edges.
 

  //having fun times trying to determine criteria for how to detect edges.
  //If I just count number of hits in diff within a radius, potentially all the hits are grouped in a spot
  //have successive hits be worth less than the previouse one(reset once checking a new row/col
  //that and/or have hits farther away from original cell be worth less.
 
  //one way of returning things, just keep some cumulative score throughout run
  //then have that be returned. have caller add that to some other cumulative score, and keep track of number of times checkForDiffs was called
  //then can find an average foundDiffs per run of loop, which should be some indication of wheather an edge was found.

  
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
  //sets/paird of checkradius for specific edges
  int topBottomRads = (rowDim / 4) + 1;
  int leftRightRads = (colDim / 4) + 1;
  //diag rads, kind of complicated. 
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

void advanceEdgeCheck(int* row, int* col, int rowMax, int colMax, edgeCheck whichCheck) {
  //iterates row/col indexs by one to follow along an edge given by whichCheck 
  //absoulutely brain numbing stuff
  //check out checkForEdge, near 200 lines of for loops in a switch
  int ratio = rowMax / colMax;
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
    //col = colMax, row = 0;
    //two approaches, could either to complicated stuff
    //or just run it through the same code for backward check, then colIndex = colMax - colIndex

    //copy of backCheck
    if (*col == 0) {
      (*col)++;
    }
    else if (*row >= rowMax) {
      (*col)++;
    }
    else if (*col >= colMax) {
      (*row)++;
    }
    else if (*row / *col > ratio) {
      (*col)++;
    }
    else {
      (*row)++;
    }
    //that's it

    *col = colMax - *col;

    break;
  case backwardCheck:
    //does backward check, from top left to bottom right.
    //col/row start both at zero
    if (*col == 0) {
      (*col)++;
    }
    else if (*row >= rowMax) {
      (*col)++;
    }
    else if (*col >= colMax) {
      (*row)++;
    }
    else if (*row / *col > ratio) {
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

edges* initEdges() {
  edges* new = malloc(sizeof(edges));
  *new = (edges){.top = 0, .bottom = 0, .left = 0, .right = 0, .left = 0, .table = 0, .pole = 0, .forward = 0, .backward = 0};
  return new;
}
