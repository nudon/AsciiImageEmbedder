#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "region.h"
#include "picture.h"
//#include "dataStructures.h"

extern int diffYes;
extern int diffNo;
extern int diffErr;

static int diffParam = -1;
static int useQuick = 0;
static int useAverageReduce = 0;
static int autoGenColorScale = 0;
static float edgeScoreWeight = 1;
static float colorScoreWeight = 1;
static float distanceWeight = -1;
static float saturationScale = 1;
static float lightnessScale = 1;
static float hueScale = 1;


static float globHitDecay = 1;
static float globHitWeight = 1;
static float globMissDecay = 1;
static float globMissWeight = 1;

static character* closestCharacterToProfile(profileMatrix* subSect,  characterSet* charSet);

void setDiffParam(int new) {
  if (new > 0) {
    diffParam = new;
  }
}

void setUseQuick(int new) {
  useQuick = new;
}

void setUseAverageReduce(int new) {
  useAverageReduce = new;
}

void setAutoGenColorScale(int new) {
  autoGenColorScale = new;
}

int getAutoGenColorScale() {
  return autoGenColorScale;
}


void setEdgeScoreWeight(float new) {
  edgeScoreWeight = new;
}

void setColorScoreWeight(float new) {
  colorScoreWeight = new;
}

void setDistanceWeight(float new) {
  if (new > 0) {
    distanceWeight = new;
  }
}

void setSaturationScale(float new) {
  if (new >= 0) {
    saturationScale = new;
  }
}

void setLightnessScale(float new) {
  if (new >= 0) {
    lightnessScale = new;
  }
}

void setHueScale(float new) {
  if (new >= 0) {
    hueScale = new;
  }
}

void setHitDecay(float new) {
  globHitDecay = new;
}

void setHitWeight(float new){
  globHitWeight = new;
}

void setMissDecay(float new) {
  globMissDecay = new;
}
void setMissWeight(float new) {
  globMissWeight = new;
}

void fillDiffMatrix(  intMatrix* detectedEdges, profileMatrix* prof, int locDiffParam) {
  //given a profileMatrix, will go through the colorMatrix
  //comparing adjacent pixels. it the difference is large enough
  //will set the parallell entry in detectedEdges of both pixels to some positive number
  
  //diffParam should be set by main program as averageCompareResult, or something to that affect
  //param could either be for entire image, or individual regions
  
  int param;
  if (locDiffParam >= 0) {
    param = locDiffParam;
  }
  else if (diffParam >= 0) {
    param = diffParam;
  }
  else {
    fprintf(stderr, "neitgher parameter or global diffParam was a valid number. Using a hardcoded parameter\n");
    param = 10;
  }

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

profileMatrix* generateProfileFromColor(colorMatrix* colors) {
  int diffParam = -1;
  diffParam = averageCompareResults(colors);
  profileMatrix* profile = newProfileMatrix(colors);
  myColor* averageColor = calculateAverageColor(colors);
  profile->averageColor = averageColor;
  intMatrix* difs = createIntMatrix(profile);
  fillDiffMatrix(difs, profile, diffParam);
  calculateEdgeScores(profile);
  return profile;
}


character* matchProfileToCharacter(profileMatrix* prof,  characterSet* charSet) {
  //given a profileMatrix with a color matrix, will generate a diff matrix
  //then generate an edge scores based on diff matrix
  //then find a closest match to a character in charSet
  int locDiffParam = -1;
  if (prof->edgeScores == NULL) {
    if (prof->diff == NULL) {
      intMatrix* diff = createIntMatrix(prof);
      prof->diff = diff;
      if (prof->source != NULL) {
	//locDiffParam = averageCompareResults(prof->source);
	//autoSetColorComponentScale(prof->source);
	fillDiffMatrix(diff, prof, locDiffParam);
      }
      else {
	fprintf(stderr,"Error, given a profile with no colorMatrix\n");
	return NULL;
      }
    }
    
  }
  return closestCharacterToProfile(prof, charSet);
}

edges* calculateEdgeScores(profileMatrix* prof) {
  edges* ret = NULL;
  if (useQuick) {
    ret = quickCalcEdges(prof);
  }
  else {
    ret = betterPopulateEdges(prof);
  }
  return ret;
}

static character* closestCharacterToProfile(profileMatrix* subSect,  characterSet* charSet) {
  //given a profile, finds a closest match to a profile found in charSet
  float score;
  float min = -1;
  character* temp = NULL;
  character* match = NULL;
  for (int index = 0; index < charSet->length; index++) {
    temp = getCharacterAtIndex(charSet, index);
    score = compareProfiles(subSect, temp->profile);
    if (score < min || match == NULL) {
      min = score;
      match = temp;
    }
    index++;
  }
  if (match == NULL) {
    printf("segfaults inbound because no match\n");
  }
  return match;
}


//so this doesn't work at all
//think it's because I'm not calculating misses
//also, I think anything with lots of space get's high scores in everything
//maybe implement a distance dropoff?
//well, actually I can run the program with dist, hitDecay, and missDecay all equal to 1. and the output looked the same. So technichally I don't think I even need any of the functionality of betterCalcEdges.
//so, fixed the issue, which was that i wasn't even looking at the diff values before incrementing, so everything had same score, so everything was spaces
//actually implemented everything and it looks good. So good that it should be default.
//think in cases where characters represent larger areas the slower one does better, or in collages, but often I have fontsize super tiny so there's not much benefit
edges* quickCalcEdges(profileMatrix* prof) {
  //idea here, just travers diffMatrix
  //if an index is within some bounds, add to edgeScore
  int colDim = prof->cols;
  int rowDim = prof->rows;
  int vertColCheckRad = (colDim / 4) + 1;
  int horzRowCheckRad = (rowDim / 4) + 1;
  int diagColCheckRad = (colDim / 4) + 1;
  int diagRowCheckRad = (rowDim / 4) + 1;
  int topScore = 0;
  int bottomScore = 0;
  int leftScore = 0;
  int rightScore = 0;
  int tableScore = 0;
  int poleScore = 0;
  int forwardScore = 0;
  int backwardScore = 0;
  float dimRat = colDim / rowDim;
  float curRat = 0;
  float shiftRat = 0;
  int diffVal;
  //the int matrix is the one matrix I made which has the inner index as rows instead of columns
  for (int colIndex = 0; colIndex < colDim; colIndex++) {
    for (int rowIndex = 0; rowIndex < rowDim; rowIndex++) {
      //just going to be 8 if statements. Could be worse
      //might be worse for diags, will see. worstace just do a regular traversal and  no that's dumb
      //will figure that out. might just have to disable

      //also, can't dothe successive hits being worth less.
      //just increment
      //thought something was off. One idea was to add statements to decrement a score instead of just counting hits
      //know what even cooler than that? actually only incrementing an edge score if the diff at the index is set. 
      diffVal = getDiffAtIndex(prof->diff, colIndex, rowIndex);
      if (colIndex <= vertColCheckRad) {
	if (diffVal == diffYes) {
	  leftScore++;
	}
	else if (diffVal == diffNo) {
	  leftScore--;
	}

      }
      else if (colIndex >= colDim - vertColCheckRad) {
	if (diffVal == diffYes) {
	  rightScore++;
	}
	else if (diffVal == diffNo) {
	  rightScore--;
	}
      }
      if (rowIndex <= horzRowCheckRad) {
	if (diffVal == diffYes) {
	  topScore++;
	}
	else if (diffVal == diffNo) {
	  topScore--;
	}
      }
      else if (rowIndex >= rowDim - horzRowCheckRad) {
	if (diffVal == diffYes) {
	  bottomScore++;
	}
	else if (diffVal == diffNo) {
	  bottomScore--;
	}
      }
      if (abs(colIndex - (colDim / 2)) <= diagColCheckRad) {
	if (diffVal == diffYes) {
	  poleScore++;
	}
	else if (diffVal == diffNo) {
	  poleScore--;
	}
      }
      if (abs(rowIndex - (rowDim / 2)) <= diagRowCheckRad) {
	if (diffVal == diffYes) {
	  tableScore++;
	}
	else if (diffVal == diffNo) {
	  tableScore--;
	}
      }
      //diagnols are going to be hard
      //somehow find a quick way if a point is in upper/lower triangle
      //if I had that, could find which region current point is, add the diagCheckRads, and see if region changed
      //seems doable, more work involving index ratios
      //instead of comparing index ratios, could throw indexes into atan or something and get an angle
      //if do all the comparison on if current angle is greater/less than angle of atan(colDim, rowDim)
      //would be a race between floatingPoint division and functionCalls + the trig implementation
      //I think floatingPoint math is still faster

      //floatingPointExceptions are not fun
      if (rowIndex == 0) {
	backwardScore++;
      }
      else {
	curRat = colIndex / rowIndex;
	//in upper righthand corner of matrix
	if (curRat > dimRat) {
	  shiftRat = ((float)(colIndex - diagColCheckRad)) / (rowIndex + diagRowCheckRad);
	  //shitfing the diagCheckRad switched which side of matrix it's on
	  //so it's within the diagCheckRadius
	  if (shiftRat < dimRat) {

	    if (diffVal == diffYes) {
	      	    backwardScore++;
	    }
	    else if (diffVal == diffNo) {
	      backwardScore--;
	    }
	  }
	}
	
	//in lowerLeft
	else {
	  shiftRat = ((float)(colIndex + diagColCheckRad)) / (rowIndex - diagRowCheckRad);
	  if (shiftRat > dimRat) {
	    if (diffVal == diffYes) {
	      backwardScore++;
	    }
	    else if (diffVal == diffNo) {
	      backwardScore--;
	    }
	  }
	}
      }

      //backwards checking, need to flip something about y axis
      //take just replace colIndex with colDim - colIndex, should do it
      //otherwise same code, just change the score changes and any other occurence of colRim
      if (rowIndex == 0) {

      }
      else {
	curRat = (colDim - colIndex) / (rowIndex);
	if (curRat > dimRat) {
	  shiftRat = ((float)((colDim - colIndex) - diagColCheckRad)) / (rowIndex + diagRowCheckRad);
	  //shitfing the diagCheckRad switched which side of matrix it's on
	  //so it's within the diagCheckRadius
	  if (shiftRat < dimRat) {
	    if (diffVal == diffYes) {
	      forwardScore++;
	    }
	    else if (diffVal == diffNo) {
	      forwardScore--;
	    }
	  }
	}
	//in lowerLeft
	else {
	  shiftRat = ((float)((colDim - colIndex) + diagColCheckRad)) / (rowIndex - diagRowCheckRad);
	  //adding diagCheckRads switched side of matrix, current is withiin checkRrad
	  if (shiftRat > dimRat) {
	    if (diffVal == diffYes) {
	      forwardScore++;
	    }
	    else if (diffVal == diffNo) {
	      forwardScore--;
	    }
	  }
	}
      }

      if (diffVal == diffErr) {
	fprintf(stderr, "Diff val was neither yes or no\n");
      }

    }
  }
  edges* ret = initEdges();
  ret->top = topScore;
  ret->bottom = bottomScore;
  ret->right = rightScore;
  ret->left = leftScore;
  ret->table = tableScore;
  ret->pole = poleScore;
  ret->forward = forwardScore;
  ret->backward = backwardScore;
  prof->edgeScores = ret;
  return ret;
}

//better, but slower
edges* betterPopulateEdges(profileMatrix* prof) {
  //given a profile with diffs
  //will carry out multiple traversals and orthogonal traversals
  //to calculate and assign edgeScores

  intMatrix* diffMatrix = prof->diff;
  edges* foundEdges = initEdges();
  float edgeScore;
  edgeCheck checkType = topCheck;
  int colStart, rowStart, colEnd, rowEnd, colDim, rowDim;
  int colOff, rowOff, colCur, rowCur, numRuns;

  rowDim = prof->source->rows - 1;
  colDim = prof->source->cols - 1;
  //the -1 keeps traversal within matrix
  //really these shouls be called rowMax
  //othersise one final check is done outside matrix
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
  if (useAverageReduce) {
    averageReduceEdgeScores(foundEdges);
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
  //so decay amount^numLoops = c
  //some root math

  float edgeHits = 0;
  float edgeMisses = 0;

  float hitAmount = 1;
  float missAmount = 1;

  float hitWeight = globHitWeight;
  float missWeight = globMissWeight;
  float hitDecay = globHitDecay;
  float missDecay = globMissDecay;

  int rowOrthOff = 0;
  int colOrthOff = 0;
  
  int colCheckRad;
  int rowCheckRad;

  int lengthOfPath;
  float maxDistanceWeight = distanceWeight;
  if (distanceWeight < 0) {
     maxDistanceWeight = 5;
  }
  float distanceDecay;
  float distanceDecayAmount;
  

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
  lengthOfPath = colCheckRad + rowCheckRad;
  //want distanceDacay ^ lengthOfPath = maxDistanceWeight
  distanceDecayAmount = pow(maxDistanceWeight, (double)1/(lengthOfPath));
  distanceDecay = 1;
  //distanceDecay = 1;
  //distanceDecayAmount = 1;
  

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
	edgeHits += hitWeight * hitAmount * distanceDecay;
	hitWeight *= hitDecay;
    }
    else if (diff == diffNo){
	edgeMisses += missWeight * missAmount * distanceDecay;
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
	edgeHits += hitWeight * hitAmount * distanceDecay;
	hitWeight *= hitDecay;
    }
    else if (diff == diffNo){
	edgeMisses += missWeight * missAmount * distanceDecay;
	missWeight *= missDecay;
    }
    else if (diff == diffErr) {
      bothOOB++;
    }
    else {
      //printf("Have unspecified number %d in col:%d row:%d\n", diff, colCur + colOrthOff, rowCur + rowOrthOff);
    }
    distanceDecay *= distanceDecayAmount;
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
  float totScore, edgeScore, avgColorScore; 
  edgeScore = compareEdges(p1->edgeScores, p2->edgeScores);
  avgColorScore = getPixelDif(p1->averageColor, p2->averageColor);
  avgColorScore = compareLightmarks(p1->mark, p2->mark);
  edgeScore *= edgeScoreWeight;
  avgColorScore *= colorScoreWeight;
  totScore = edgeScore + avgColorScore;
  //so, looking nice, only issue is dark regions are usually whitespace because crushed blacks
  //observation is, things with high edge scores in everything  are similar to those with low edge scores,
  //try and do something with that
  //question is, how? maybe do some stats, like take standard deviation or something of all edges
  //compare that instead? doesn't exactly work out.
  //would seem like there's just a special case, where
  //suddenlyidea
  //for edges, take average of edgeScores, then subtract off from each component.
  //negative edges are ?, but otherwise seems like a good idea.
  //didn't totally work, but seemed cool.
  return totScore;
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

float compareLightmarks(lightmark* lm1, lightmark* lm2) {
  float delta = 0;
  delta += fabs(lm1->differenceFromMostLight - lm2->differenceFromMostLight);
  delta += fabs(lm1->differenceFromMostDark - lm2->differenceFromMostDark);
  delta = fabs(lm1->spanPercentile - lm2->spanPercentile);
  return delta;
}

void averageReduceEdgeScores(edges* edges) {
  float totalScore = edges->top + edges->bottom + edges->left + edges->right + edges->table + edges->pole + edges->forward + edges->backward;
  int numOfEdges = 8;
  float average = totalScore / numOfEdges;
  float* temp;
  edgeCheck whichCheck = topCheck;
  while(whichCheck != doneChecking) {
    if (whichCheck == topCheck) {
      temp = &(edges->top);
      whichCheck = bottomCheck;
    }
    else if (whichCheck == bottomCheck) {
      temp = &(edges->bottom);
      whichCheck = leftCheck;
    }
    else if (whichCheck == leftCheck) {
      temp = &(edges->left);
      whichCheck = rightCheck;
    }
    else if (whichCheck == rightCheck) {
      temp = &(edges->right);
      whichCheck = tableCheck;
    }
    else if (whichCheck == tableCheck) {
      temp = &(edges->table);
	whichCheck = poleCheck;
    }
    else if (whichCheck == poleCheck) {
      temp = &(edges->pole);
      whichCheck = forwardCheck;
    }
    else if (whichCheck == forwardCheck) {
      temp = &(edges->forward);
      whichCheck = backwardCheck;
    }
    else if (whichCheck == backwardCheck) {
      temp = &(edges->backward);
      whichCheck = doneChecking;
    }
    *temp = *temp - average;
  }
  
}

float averageCompareResults(colorMatrix* colors) {

  //basically copy-pasted code from fillDiffMatrix
  //if there's any bugs in one, there's bugs in the other  
  int rows = colors->rows;
  int cols = colors->cols;
  int testC, testR, testNumber;
  myColor* current, *compare;
  long results = 0;
  int numRuns = 0;
  for (int colIndex = 0; colIndex < cols; colIndex++) {
    for (int rowIndex = 0; rowIndex < rows; rowIndex++) {
      current = getColor(colors, colIndex, rowIndex);
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
	compare = getColor(colors, testC, testR);
	//eventually...
	if (compare != NULL) {
	  results += getPixelDif(current, compare);
	  numRuns++;
	}
	testNumber++;
      }            
    }
  }
  return results/numRuns;
}

int getPixelDif(myColor* c1, myColor* c2) {
  int difference = 0;
  if (c1 != NULL && c2 != NULL) {
    difference = saturationDif(c1, c2) * saturationScale;
    difference += hueDif(c1, c2) * hueScale;
    difference += lightnessDif(c1, c2) * lightnessScale;
  }
  else {
    fprintf(stderr,"Comparing null pixel(s)\n");
    difference = 0;
  }
  
  return difference;
}

void autoSetColorComponentScale(colorMatrix* source) {
  int rows = source->rows;
  int cols = source->cols;
  int testC, testR, testNumber;
  myColor* current, *compare;
  long totHueDif = 0;
  long totLightnessDif = 0;
  long totSaturationDif = 0;
  int totalCompares = 0;
  for (int colIndex = 0; colIndex < cols; colIndex++) {
    for (int rowIndex = 0; rowIndex < rows; rowIndex++) {
      current = getColor(source, colIndex, rowIndex);
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
  	compare = getColor(source, testC, testR);
	//eventually...
	if (compare != NULL) {
	  totSaturationDif += saturationDif(current, compare);
	  totHueDif += hueDif(current, compare);
	  totLightnessDif += lightnessDif(current,  compare);
	  totalCompares++;
	}
	testNumber++;
      }            
    }
  }
  if (totHueDif != 0) {
    setHueScale(((float)totHueDif / totalCompares));
  }
  if (totLightnessDif != 0) {
    setLightnessScale((float)totLightnessDif / totalCompares);
  }
  if (totSaturationDif != 0) {
    setSaturationScale(((float)totSaturationDif / totalCompares));
  }
}


  
int saturationDif(myColor* c1, myColor* c2) {
  return (int)(fabs(c1->sat - c2->sat));
}

int lightnessDif(myColor* c1, myColor* c2) {
  return (int)(fabs(c1->lightness - c2->lightness));
}
  
int hueDif(myColor* c1, myColor* c2) {
  int temp = 0;
  temp = (int)(fabs(c1->hue - c2->hue));
  if (temp > (255 / 2)) {
    temp = 255 - temp;
  }
  return temp;
}



int sameIntMatrix(intMatrix* m1, intMatrix* m2) {
  //to be used primarily in sniffing out bad characters in fonts
  //only issue is that with one font there have been a left and right justified ? rendered on errors
  //not sure what determines how that's alligned, but it's a source of error
  //apparently 0xEF 0xBF 0xBF should be an intCode for an invalid character
  //unless fonts are cheeky and supply a glyph for the invalid character, should be fine for some of the ?'s
  int ret = 0, rows = m1->rows, cols = m1->cols, val1, val2;
  if (m1->rows == m2->rows && m1->cols == m2->cols) {
    ret = 1;
    for (int rowIndex = 0; rowIndex < rows; rowIndex++) {
      for (int colIndex = 0; colIndex < cols; colIndex++) {
	val1 = getDiffAtIndex(m1, colIndex, rowIndex);
	val2 = getDiffAtIndex(m2, colIndex, rowIndex);
	if ( val1 != val2) {
	  ret = 0;
	  colIndex = cols;
	  rowIndex = rows;
	}
      }
    }
  }
  return ret;
}
