#ifndef __BUCKET_H__
#define __BUCKET_H__
#include <stdlib.h>

typedef enum { false, true } bool;

typedef struct page
{
  struct bucket* parent;
  struct page* next;
  struct page* prev;
} page_t;

typedef struct bucket
{
  size_t size;
  size_t count;
  bool red;
  struct page* head;
  struct page* occupied;
  struct bucket* parent;
  struct bucket* left;
  struct bucket* right;
} bucket_t;

extern bucket_t* create(size_t size);
extern bucket_t* find(bucket_t* root, size_t size);
extern bucket_t* remove(bucket_t* root, size_t size);
extern bucket_t* add(bucket_t* root, bucket_t* target);
extern bucket_t* remove(page_t* node);
extern page_t*   get(bucket_t* node);

#endif
