#ifndef FILE_MYLIST_SEEN
#define FILE_MYLIST_SEEN
#include <stdlib.h>

//some idea for general structures in general
//would be nice to be able to have a free function as a field

//also, maybe have a general type enum in a seperate file
//so general lists and matricies and whatever can be compatible
//that or embrace the void*

struct gen_node_struct{
  void* stored;
  struct gen_node_struct * next;
  struct gen_node_struct * prev;
};

typedef struct gen_node_struct gen_node;

typedef
struct {
  gen_node* start;
  gen_node* end;
} gen_list;

//not sure what to do here
//can either keep these static for internal use
//and just write seperate functions which take some specific datatype
//and do void* casting on their own
//or just let these be used anywhere

void appendToGen_list(gen_list* list, gen_node* new);
void prependToGen_list(gen_list* list, gen_node* new);
void removeFromGen_list(gen_list* list, gen_node* node);



gen_list* createGen_list();

gen_node* createGen_node(void* data);

void freeGen_node(gen_node* old);

void freeGen_list(gen_list* old);

#endif
