//these will store whatever metadata I want

typedef
struct {
  //some color
} myColor;

//just a subset of pixels
typedef
struct {
  int rows;
  int cols;
  myColor *** cells;
} colorMatrix;


typedef
struct {
  int rows;
  int cols;
  //float instead?
  int *** cells;
  colorMatrix* source;
} profileMatrix;





//things for each character 
typedef
struct {
  char character;
  //some vector format of character?
  //or maybe a large enough raster?
  profileMatrix * profile;
} character;

//will contain image, some dimensions, probably some aspect ratio things as well,
//as well as heigh/width of subRects
//the filled chars will be some mirror array of the subRect array, which will get filled with respective matches
typedef
struct {
  //SDL_Surface/texture
  int width;
  int height;
  //matrix ***?
  profileMatrix*** profiles;
  char*** filledChars;
} image;

// so, have matrix. in general it contains metainformation like value of a pixel or group of pixel
// have some parameters, like how many pixels should a character represent. in general picDim * char / pixel  = dim of pic in chars
// another parameter, how fine grained should the profile matrix be. Should it store metaDate in a 1-1 relationship with pixels? or group nearby pixels into 1 cell
// at any rate profile matrix has to have a matching aspect ratio to the pixel region it covers. 1-1 will work, other options. Not actually sure why I'd want grouping
// characters, in display they are usually rectangularish. some aspect ratio  issues. Would either stretch image or just do weird stuff on boundaries
// also some issues with dimensions of image. if it's a prime number or otherwise hard to tesselate into rectangles, might need to scale it up/down
// so issue of aspect ratios and having image dimensions that lend to tesselation
// 
// then, need two algs
// first, looking at an arrangement of pixels and generating a profile of it
// second, need to find a closest match of another profile
// depends heavily on how I define the metadate stored in the profile matrixes

// for metadata, thinking of just storing value/lightness of pixel
// comparision could then come down to 2 things:
// 1, just counting the number of "matches" between profiles, so just seeing how close the profileMatrix cells are in value
// 2, which is similarities  in profile matrices. 2 matrices could have a checkerboard pattern, but be offest so no tiles match. but the two tiles look visually alike.
// generally seems like just matching between average grayscale/darkness values is a decent way
// in that case, may need a seconday matrix to store, for each profile, how different it is from adjacent cells. or other things like that, 
// in 2nd case, doing some kind of edge detection might be a good idea. have some way of comparing differences of colors, if difference gets too high say it's a new edge?
//idea was to reduce lines/edges into horizontal, vertical, diagnol, and also contain some orientation. would compare the number and types of edges between to profiles

//guess it would be a good idea to pick an image library first
//I know SDL has some stuff for it, but might be overkill
//Imagemagick also has some stuff. used that a bit for primitive file conversion
//those are the only 2 I know really. 

//rough stuff for edge detection
typedef
struct {
  int top;
  int bottom;
  int left;
  int right;
  int table;
  int pole;
  int forward;
  int backward;

} edges;

typedef enum {  topCheck,  bottomCheck, leftCheck, rightCheck, tableCheck, poleCheck, forwardCheck, backwardCheck} edgeCheck;

edges * detectEdge(profileMatrix* prof) {
  int detectedEdges[prof->source->cols][prof->source->rows];
  fillDiffMatrix(detectedEdges, prof);

    //anyways, as long as I'm only considering a reduced set of edges to idenfiy, could just have several different for loops that walk across the array, sush as diagonally, and checking if cells within a certain array are flipped.
  //so, walk down some imaginary line in array. have another parameter for for far to look from line for deteced edges. good to have parameter be relative to dim of profile
  //keep track of how many edges within boundary along line were detected, if thats above another parameter(or maybe some number depending on dim of profileMatrix/colorMatrix
  //then say you found a certain kind of edge.
   
}



void fillDiffMatrix(int **detectedEdges, profileMatrix* prof) {
  //given a profileMatrix, will go through the colorMatrix
  //comparing adjacent pixels. it the difference is large enough
  //will set the parallell entry in detectedEdges of both pixels to some positive number
  
  //threshold for gitpixelDiff
  int param = 12;

  int rows = prof->source->rows;
  int cols = prof->source->cols;
  //int detectedEdges[cols][rows];
  for (int c = 0; c < cols; c++) {
    for (int r = 0; r < rows; r++) {
      detectedEdges[c][r] = 0;
    }
  }
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


edges* findEdges(int filledDiff[][], profileMatrix* prof) {
  edges* foundEdges = initEdges();
  int row, col, ratio, rowMax, colMax;
  rowMax = prof->source->row;
  colMax = prof->source->col;
  ratio = rowMax / colMax;

  //going to need to be some coordination between where I'm initializing indexes
  //and how advanceEdgeCheck changes them
  //standard, start in the upper left sides. having forward slash start in upper right
  edgeCheck checkType = topCheck;

  int sumHits = 0;
  int numRuns = 0;
  row = 0;
  col = 0;
  while (row < rowMax || col < colMax) {
    
    //look at nearby cells
    checkForDiffs(foundEdges, checkType);
    sumHits += advanceEdgeCheck(&row, &col, rowMax, colMax, checkType);
    numRuns++;
  }
  int edgeScore = sumHits/numRuns;
}

int checkForDiffs(edges* edges, int diff[][],  int row, int col, int rowDim, int colDim,  edgeCheck whichCheck) {
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
  int topBottomRads = (rowDim / 4) + 1;
  int leftRightRads = (colDim / 4) + 1;
  //diag rads, kind of complicated. 
  int diagRadsVert = (colDim / 2) + 1;
  //int diagRadsHorz = (rowDim / 2) + 1;

  switch(whichCheck) {
  case topCheck:
    checkRadius = topBottomRads;
    for(int rowOffset = 0; rowOffset < checkRadius; rowOffset++) {
      if (diff[col][row + rowOffset] != 0) {
	//found what might be an edge, maybe break loop idk
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
	//found what might be an edge, maybe break loop idk
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
	//found what might be an edge, maybe break loop idk
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
	//found what might be an edge, maybe break loop idk
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
	//found what might be an edge, maybe break loop idk
	edgeHits += hitWeight * hitAmount;
	hitWeight *= hitDecay;
      }
      else {
	edgeMisses += missWeight * missAmount;
	missWeight *= missDecay;
      }
      if (diff[col][row + rowOffset] != 0) {
	//found what might be an edge, maybe break loop idk
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
	//found what might be an edge, maybe break loop idk
	edgeHits += hitWeight * hitAmount;
	hitWeight *= hitDecay;
      }
      else {
	edgeMisses += missWeight * missAmount;
	missWeight *= missDecay;
      }
      if (diff[col + colOffset][row] != 0) {
	//found what might be an edge, maybe break loop idk
	edgeHits += hitWeight * hitAmount;
	hitWeight *= hitDecay;
      }
      else {
	edgeMisses += missWeight * missAmount;
	missWeight *= missDecay;
      }
    }
    
    break;
  case forwardCheck:

    //sligtly like this better than doing horizontal/vertical,
    //I believe there is some double counting going on however.
    //wait I did this wrong. right now I'm tracing out diagnol path flipped across y dim.
    //want to instead be checking in a path that's orthogonal to diagnol.
    //I believe that means I just flip all the cols with rows in the iterating bits of code
    
    //diagnol checking
    int ratio = colDim / rowDim;
    while (rowOffset < diagRadsVert || colOffset < diagRadsHorz) {
      if (diff[col - colOffset][row - rowOffset] != 0) {
	//found what might be an edge, maybe break loop idk
	edgeHits += hitWeight * hitAmount;
	hitWeight *= hitDecay;
      }
      else {
	edgeMisses += missWeight * missAmount;
	missWeight *= missDecay;
      }
      if (diff[col + colOffset][row + rowOffset] != 0) {
	//found what might be an edge, maybe break loop idk
	edgeHits += hitWeight * hitAmount;
	hitWeight *= hitDecay;
      }
      else {
	edgeMisses += missWeight * missAmount;
	missWeight *= missDecay;
      }
      //does a similar diagnol search, similar to how I do advanceEdgeCheck for diags.
      if (rowOffset == 0) {
	rowOffset++;
      }
      else if (colOffset >= diagRadsHorz) {
	rowOffset++;
      }
      else if (rowOffset >= diagRadsVert) {
	colOffset++
	  }
      else if (colOffset / rowOffset > ratio) {
	rowOffset++;
      }
      else {
	colOffset++;
      }
    
    break;
  case backwardCheck:
    
    //diagnol checking
    int ratio = colDim / rowDim;
    while (rowOffset < diagRadsVert || colOffset < diagRadsHorz) {
      if (diff[col + colOffset][row - rowOffset] != 0) {
	//found what might be an edge, maybe break loop idk
	edgeHits += hitWeight * hitAmount;
	hitWeight *= hitDecay;
      }
      else {
	edgeMisses += missWeight * missAmount;
	missWeight *= missDecay;
      }
      if (diff[col - colOffset][row + rowOffset] != 0) {
	//found what might be an edge, maybe break loop idk
	edgeHits += hitWeight * hitAmount;
	hitWeight *= hitDecay;
      }
      else {
	edgeMisses += missWeight * missAmount;
	missWeight *= missDecay;
      }
      //does a similar diagnol search, similar to how I do advanceEdgeCheck for diags.
      if (rowOffset == 0) {
	rowOffset++;
      }
      else if (colOffset >= diagRadsHorz) {
	rowOffset++;
      }
      else if (rowOffset >= diagRadsVert) {
	colOffset++
	  }
      else if (colOffset / rowOffset > ratio) {
	rowOffset++;
      }
      else {
	colOffset++;
      }
    }
    break;
  case default:

    break;
      
  }
  
}

void advanceEdgeCheck(int* row, int* col, int rowMax, int colMax, edgeCheck whichCheck) {
  //absoulutely brain numbing stuff
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
    (*row)++
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
    if (col == 0) {
      (*col)++;
    }
    else if (row >= rowMax) {
      (*col)++;
    }
    else if (col >= colMax) {
      (*row)++;
    }
    else if (row / col > ratio) {
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
    if (col == 0) {
      (*col)++;
    }
    else if (row >= rowMax) {
      (*col)++;
    }
    else if (col >= colMax) {
      (*row)++;
    }
    else if (row / col > ratio) {
      (*col)++;
    }
    else {
      (*row)++;
    }
    break;  
  case default:

    break;
      
  }
}

edges* initEdges() {
  edges* new = malloc(sizeof(edges));
  new = {.top = 0, .bottom = 0, .left = 0, .right = 0, .left = 0, .table = 0, .pole = 0, .forward = 0, .backward = 0};
  return new;
}

//taken from http://sdl.beuc.net/sdl.wiki/Pixel_Access
uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    uint8 *p = (uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(uint16 *)p;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(uint32 *)p;
        break;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

SDL_Color* uint32ToColor(Uint32 rgba) {
  myColor* color = malloc(sizeof(myColor));
  int r,g,b,a, br,bg,bb,ba, sr, sg, sb, sa;
  int mask = 0xff;
  //bytemask for each component
  sr = 24;
  sg = 16;
  sb = 8;
  sa = 0;
  
  r = ((mask << sr) | rgba) >> sr;
  g = ((mask << sg) | rgba) >> sg;
  b = ((mask << sb) | rgba) >> sb;
  a = ((mask << sa) | rgba) >> sa;
}
