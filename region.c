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

character* closestCharacterToProfile(profileMatrix* subSect,  characterSet* charSet);

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
  int testC, testR, testNumber, offC,offR;
  myColor* current, *compare;
  for (int rowIndex = 0; rowIndex < rows; rowIndex++) {
      for (int colIndex = 0; colIndex < cols; colIndex++) {
	setDiffAtIndex(detectedEdges, colIndex, rowIndex, diffNo);
    }
  }
  for (int rowIndex = 0; rowIndex < rows; rowIndex++) {
    for (int colIndex = 0; colIndex < cols; colIndex++) {
      current = getColor(prof->source, colIndex, rowIndex);
      testNumber = 1;
      while(testNumber > 0) {
	full_neighbor_traverse(&testNumber, colIndex, rowIndex, &offC, &offR);
	testC = colIndex + offC;
	testR = rowIndex + offR;
  	compare = getColor(prof->source, testC, testR);
	if (compare != NULL && getPixelDif(current, compare) > param) {
	  setDiffAtIndex(detectedEdges,colIndex,rowIndex, diffYes);
	  setDiffAtIndex(detectedEdges,testC,testR, diffYes);
	}
      }            
    }
  }
  if (prof->diff == NULL) {
    prof->diff = detectedEdges;
  }
}


//in diff, stores maximum differences between neighboring pixels
void fillDiffMatrixAlt(  intMatrix* detectedEdges, profileMatrix* prof) {
  int rows = prof->source->rows;
  int cols = prof->source->cols;
  int val;
  for (int rowIndex = 0; rowIndex < rows; rowIndex++) {
    for (int colIndex = 0; colIndex < cols; colIndex++) {
      val = max_neighbor_diff(prof, colIndex, rowIndex);
      setDiffAtIndex(detectedEdges,colIndex,rowIndex, val);
    }
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
  //fillDiffMatrixAlt(difs, profile);
  profile->diff = difs;
  profile->edgeScores = calculateEdgeScores(profile);
  return profile;
}

profileMatrix* mem_light_generateProfileFromColor(colorMatrix* colors, intMatrix* diff) {
  int diffParam = -1;
  //diffParam = averageCompareResults(colors);
  profileMatrix* profile = newProfileMatrix(colors);
  myColor* averageColor = calculateAverageColor(colors);
  profile->averageColor = averageColor;
  fillDiffMatrix(diff, profile, diffParam);
  //fillDiffMatrixAlt(difs, profile);
  profile->edgeScores = calculateEdgeScores(profile);
  //set diff/color sources to null so bad things don't happen when I free
  profile->diff = NULL;
  profile->source = NULL;
  return profile;
}

edges* calculateEdgeScores(profileMatrix* prof) {
  edges* ret = NULL;
  if (useQuick) {
    ret = quickCalcEdges(prof);
  }
  else {
    ret = betterPopulateEdges(prof);
  }
  if (useAverageReduce) {
    averageReduceEdgeScores(ret);
  }
  return ret;
}

character* closestCharacterToProfile(profileMatrix* subSect,  characterSet* charSet) {
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
    printf("no match was found, segfaults inbound\n");
  }
  return match;
}



edges* quickCalcEdges(profileMatrix* prof) {
  //idea here, just travers diffMatrix
  //if a hihg diff is within some region, add to edgeScore
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
  for (int rowIndex = 0; rowIndex < rowDim; rowIndex++) {
    for (int colIndex = 0; colIndex < colDim; colIndex++) {
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
      //diagnols
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
	fprintf(stderr, "Diff val was not set\n");
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
;

  int diff;
  int bothOOB = 0;

  
  if (whichCheck == topCheck || whichCheck == bottomCheck || whichCheck == tableCheck) {
    colCheckRad = (colDim / 4) + 1;
    rowCheckRad = 0;
  }
  else if (whichCheck == leftCheck || whichCheck == rightCheck || whichCheck == poleCheck) {
    colCheckRad = 0;
    rowCheckRad = (rowDim / 4) + 1;
  }
  else if (whichCheck == forwardCheck || whichCheck == backwardCheck) {
    colCheckRad = (colDim / 4) + 1;
    rowCheckRad = (rowDim / 4) + 1;
  }
  lengthOfPath = colCheckRad + rowCheckRad;
  //want distanceDacay ^ lengthOfPath = maxDistanceWeight
  distanceDecayAmount = pow(maxDistanceWeight, (double)1/(lengthOfPath));
  distanceDecay = 1;
  
  while(orthogonalTraverse(colCur, rowCur,
			   &colOrthOff, &rowOrthOff, 
			   colCur + colCheckRad, rowCur + rowCheckRad) != 1
	&& bothOOB != 2){
    bothOOB = 0;
    diff = getDiffAtIndex(diffMatrix,colCur + colOrthOff,rowCur + rowOrthOff);;
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
      printf("Have unspecified number %d in col:%d row:%d\n", diff, colCur + colOrthOff, rowCur + rowOrthOff);
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
      printf("Have unspecified number %d in col:%d row:%d\n", diff, colCur + colOrthOff, rowCur + rowOrthOff);
    }
    distanceDecay *= distanceDecayAmount;
  }
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
    ret = 1;
  }
  curx = startx + *offx;
  cury = starty + *offy;
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
  return totScore;
}

//kind of prototype version that does has a variable weight for edge/colorscores
float compareProfilesVar(profileMatrix* regP, profileMatrix* charP) {
  float totScore = 0;
  if (regP->neighborEdgeDiff < 0) {
    //it's a so-called meta-edge, give edgescore
    totScore = compareEdges(regP->edgeScores, charP->edgeScores);
  }
  else {
    //avgColorScore = getPixelDif(regP->averageColor, charP->averageColor);
    totScore = compareLightmarks(regP->mark, charP->mark);
  }
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
  //delta += fabs(lm1->differenceFromMostLight - lm2->differenceFromMostLight);
  //delta += fabs(lm1->differenceFromMostDark - lm2->differenceFromMostDark);
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
  int testC, testR, testNumber, offC, offR;
  myColor* current, *compare;
  long results = 0;
  int numRuns = 0;
  for (int rowIndex = 0; rowIndex < rows; rowIndex++) {
    for (int colIndex = 0; colIndex < cols; colIndex++) {
      current = getColor(colors, colIndex, rowIndex);
      testNumber = 1;
      while(testNumber > 0) {
	half_neighbor_traverse(&testNumber, colIndex, rowIndex, &offC, &offR);
	testC = colIndex + offC;
	testR = rowIndex + offR;
	
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
  for (int rowIndex = 0; rowIndex < rows; rowIndex++) {
    for (int colIndex = 0; colIndex < cols; colIndex++) {
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

void test(image* pic) {
  profileMatrix *curr, *comp;
  int testC, testR, testNumber, offC, offR,compCount = 0;
  double compTot = 0, compAvg = -1;
  for (int rowIndex = 0; rowIndex < pic->numberOfRegionRows; rowIndex++) {
    for (int colIndex = 0; colIndex < pic->numberOfRegionCols; colIndex++) {
      curr = pic->profiles[rowIndex][colIndex];
      testNumber = 1;
      while(testNumber > 0) {
	half_neighbor_traverse(&testNumber, colIndex, rowIndex, &offC, &offR);
	testC = colIndex + offC;
	testR = rowIndex + offR;
	
	comp = getProfile(pic, testR,testC);
	if (comp != NULL) {
	  //find difference between edges
	  compTot += compareEdges(curr->edgeScores, comp->edgeScores);
	  compCount++;
	  
	}
      }
    }
  }
  compAvg = compTot / compCount;
  //part 1 done
      
  
  //second, retraverse, compute average edge scores between neighbors,
  int regionCount;
  int colOff, rowOff;
  double regionTot, regionAvg;
  for (int rowIndex = 0; rowIndex < pic->numberOfRegionRows; rowIndex++) {
    for (int colIndex = 0; colIndex < pic->numberOfRegionCols; colIndex++) {
      regionCount = 0;
      regionTot = 0;
      regionAvg = 0;
      curr = pic->profiles[rowIndex][colIndex];
      testNumber = 1;
      while(testNumber > 0) {
	full_neighbor_traverse(&testNumber, colIndex, rowIndex, &colOff, &rowOff);
	testC = colIndex + colOff;
	testR = rowIndex + rowOff;
	
	comp = getProfile(pic, testR,testC);
	if (comp != NULL) {
	  //find difference between edges
	  regionTot += compareEdges(curr->edgeScores, comp->edgeScores);
	  regionCount++;
	  
	}
      }
      //computed difference
      regionAvg = regionTot / regionCount;
      curr->neighborEdgeDiff = regionAvg - compAvg;
    }
  }
}

void half_neighbor_traverse(int *testNumber, int col_i, int row_i, int* col_off, int* row_off) {
  if (*testNumber > 4) {
    *testNumber = -10;
  }
  else {
    full_neighbor_traverse(testNumber, col_i, row_i, col_off, row_off);
  }
}

void full_neighbor_traverse(int *testNumber, int col_i, int row_i, int* col_off, int* row_off) {
  switch(*testNumber) {
  case 1:
    *col_off = -1;
    *row_off = -1;
    break;
  case 2:
    *col_off = -1;
    *row_off = 0;
    break;
  case 3:
    *col_off = -1;
    *row_off = 1;
    break;
  case 4:
    *col_off = 0;
    *row_off = -1;
    break;
  case 5:
    *col_off = 0;
    *row_off = 1;
    break;
  case 6 :
    *col_off = 1;
    *row_off = -1;
    break;
  case 7:
    *col_off = 1;
    *row_off = 0;
    break;
  case 8:
    *col_off = 1;
    *row_off = 1;
    //end loop
    *testNumber = -2;
    break;
  default:
    printf("error in full traversal, unknown test number %d\n", *testNumber);
    break;
  }
  *testNumber += 1;

}

int max_neighbor_diff(profileMatrix* prof, int x, int y) {
  int testNumber = 1;
  int max = -1, temp = 0;
  myColor *curr, *test;
  curr = getColor(prof->source, x, y);
  int testC, testR, offC, offR;
  while(testNumber > 0) {
    full_neighbor_traverse(&testNumber, x, y, &offC, &offR);
    testC = x + offC;
    testR = y + offR;
    
    test = getColor(prof->source, testC, testR);
    if (test != NULL) {
      temp = abs(getPixelDif(curr, test));
      if (temp > max) {
	max = temp;
      }
    }
  }
  return max;
}
