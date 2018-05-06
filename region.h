
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

typedef enum {  topCheck,  bottomCheck, leftCheck, rightCheck, tableCheck, poleCheck, forwardCheck, backwardCheck} edgeCheck;

typedef
struct {
  int rows;
  int cols;
  //float instead?
  edges* edgeScores;
  int *** cells;
  colorMatrix* source;
} profileMatrix;





//things for each character 
typedef
struct {
  char character;
  //some vector format of character?
  //or maybe a large enough raster?
  //maybe not as fields, but I need those one way or another
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
// characters, in display they are usually rectangularish. some aspect ratio  issues. Would either stretch image or just do weird stuff on boundaries
// also some issues with dimensions of image. if it's a prime number or otherwise hard to tesselate into rectangles, might need to scale it up/down
// so issue of aspect ratios and having image dimensions that lend to tesselation
// 
// then, need two algs
// first, looking at an arrangement of pixels and generating a profile of it
// second, need to find a closest match of another profile
// depends heavily on how I define the metadate stored in the profile matrixes

//metadata, likely to come down to two things. one, just the grascale darkness/ or hsv/hsl value/lightness value. 
// in 2nd case, doing some kind of edge detection might be a good idea. have some way of comparing differences of colors, if difference gets too high say it's a new edge?
//idea was to reduce lines/edges into horizontal, vertical, diagnol, would compare the edge profiles to find a similar match

//guess it would be a good idea to pick an image library first
//I know SDL has some stuff for it, but might be overkill
//Imagemagick also has some stuff. used that a bit for primitive file conversion
//those are the only 2 I know really. 




//kind of discrepencany in naming scheme. col/row max indicates dimension of profile matrix
//standard for where to start specific edge Checks
//listed as whichEdge, rowStart, colStart
//top     , 0         , 0
//bottom  , rowMax    , 0,
//left    , 0         , 0
//right   , 0         , colMax
//table   , rowMax / 2, 0,
//pole    , 0         , colMax / 2
//forward , 0,        , colMax
//backWard, 0        , 0

//standard for where to end specific edge Checks
//listed as whichEdge, rowEnd, colEnd
//top     , 0         , colMax
//bottom  , rowMax    , colMax,
//left    , rowMax    , 0
//right   , rowMax    , colMax
//table   , rowMax / 2, colMax
//pole    , rowMax    , colMax / 2
//forward , rowMax    , 0
//backWard, rowMax    , colMax


//generates a matrix which marks pixels that are sufficiently different
void fillDiffMatrix(int **detectedEdges, profileMatrix* prof);

//given a profile, will generate a reductive representation of edges within picture
edges* findEdges(int ** filledDiff, profileMatrix* prof);

//will look around vicinity of cell, calculating misses/hits of edges
//then return an edge score, 
float checkForEdge(int **diff, int row, int col, int rowDim, int colDim,  edgeCheck whichCheck);

void advanceEdgeCheck(int* row, int* col, int rowMax, int colMax, edgeCheck whichCheck);

char* closestCharToProfile(profileMatrix* subSect, character** charSet);

float compareProfiles(profileMatrix* p1, profileMatrix* p2);

float compareEdges(edges* e1, edges* e2);


edges* initEdges();

myColor* getPixel(colorMatrix*, int, int);

int getPixelDif(myColor *, myColor*);
