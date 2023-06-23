#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#define META_SIZE sizeof(struct block_meta)
#define align4(x) (((((x)-1)>>2)<<2)+4)


struct block_meta {
  size_t size;
  struct block_meta *next;
  int free;
  // int magic; //remove the comment while debugging
};


void *global_base = NULL;
// a global base pointer acting as head of linked list



// fnding a free block and returning a pointer to it
struct block_meta *find_free_block(struct block_meta **last, size_t size){
  struct block_meta *current = global_base;
  while (current != NULL && !(current->free && current->size >= size)){
    *last = current;
    current = current->next;
  }
  return current;
}


// requesting a new block from os if no free block is found
struct block_meta *request_space(struct block_meta *last, size_t size){
  struct block_meta *block;
  block = sbrk(0);
  void *request = sbrk(size + META_SIZE);

  if(request == (void*) -1) //no new space found
    return NULL;

  if(last) // link list is empty
    last->next = block; //global base pointer is set to this head later in code
  
  block->size = size;
  block->next = NULL;
  block->free = 0;
  //block->magic = 0x12345678;
  return block;
}


void *malloc(size_t size){
  struct block_meta *block;
  //size = align4(size);
  
  if (size <= 0)
    return NULL;

  if (global_base == NULL){ // first call
    block = request_space(NULL, size);
    if (block == NULL)
      return NULL; // no space found for new block
  }
  else{
    struct block_meta *last = global_base;
    block = find_free_block(&last, size);
    if (block == NULL)
      return NULL; // free block not found
    else{
      // add code for splitting block here if it has more memory than required
      block->free = 0;
      //block->magic = 0x77777777;
    }
  }
  return (block+1); // +1 here increments by META_SIZE since block points to block_meta
}


struct block_meta *get_block_ptr(void *ptr){
  return (struct block_meta*)ptr - 1;
}


void free(void *ptr){
  if (ptr == NULL)
    return;
  struct block_meta *block_ptr = get_block_ptr(ptr);
  block_ptr-> free = 1;
  //block_ptr->magic = 0x55555555;
}


void *realloc(void *ptr, size_t size){
  if (ptr == NULL)
    return malloc(size);

  struct block_meta *block_ptr = get_block_ptr(ptr);
  if (block_ptr->size >= size)
    return ptr;
  
  void *new_ptr;
  new_ptr = malloc(size);
  if(new_ptr == NULL){
    printf("realloc failed");
    return NULL;
  }
  memcpy(new_ptr, ptr, block_ptr->size);
  free(ptr);
  return new_ptr;
}

void *calloc(size_t nelem, size_t elsize){
  size_t size = nelem * elsize;
  void *ptr = malloc(size);
  memset(ptr, 0, size);
  return ptr;
}


























