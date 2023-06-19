#include<stdio.h>
#include <unistd.h>

#define BLOCK_SIZE 12 // not using sizeof() to avoid including size of ch data[1]
#define align4(x) (((((x)-1)>>2)<<2)+4)
// the pointers used are of size 4 bytes (32 bits) hence to align
// the address of pointer to 4 bytes. s_block has size 12 bytes so
// it is already aligned


typedef struct s_block *t_block;

struct s_block{
  size_t size; // size of the chunk
  t_block next; // pointer to next chunk
  int free; // 1 when free, 0 otherwise
  char data[1]; // data[1] indicates end of mata data
  // it is useful for future pointer arithmetics
};


void* base = NULL; // used as initial pointer in heap

// finding a block of free chunk
t_block find_block(t_block *last, size_t size){
  t_block b = base; // base is a global pointer
  while  (b && !(b->free && b->size >= size)){
    *last = b;
    b = b->next;
  }
  return b;
}


t_block extend_heap(t_block last, size_t size){
  t_block b;
  b = sbrk(0);
  if (sbrk(BLOCK_SIZE + size) == (void*)-1)
    return NULL;
  
  b->size = size;
  b->next = NULL;
  if (last != NULL)
    last->next = b;
  b->free = 0;
  return b;
}


void split_block(t_block b, size_t size){
  t_block new;
  new = b->data + size;
  new->size = b->size - size - BLOCK_SIZE;
  new->next = b->next;
  new->free = 1;
  b->size = size;
  b->next = new;
}

void *malloc(size_t size){
  t_block b, last;
  size = align4(size);
  
  if (base != NULL){
    last = base;
    b = find_block(&last, size); // why make last as pointer above ? try not doing that and removing & here
    
    if (b != NULL){
      //split block if possible
      if ((b->size - size) >= (BLOCK_SIZE + 4))
        split_block(b, size);
      b->free = 0;
    }
    else{
      //extend heap
      b = extend_heap(last, size);
      if (!b)
        return NULL;
    }
  }
  else{
    // first time 
    b = extend_heap(NULL, size);
    if (!b)
      return NULL;
    base = b;
  }
  return b->data;
}



























