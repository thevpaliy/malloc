#ifndef __BUCKET_H__
#define __BUCKET_H__
typedef enum { false, true } bool;
typedef struct page
{
  size_t size;
  bool free;
  struct page* next;
} page_t;

typedef struct bucket
{
  size_t size;
  bool red;
  struct page* head;    
  struct bucket* parent;
  struct bucket* left;
  struct bucket* right;
} bucket_t;

extern bucket_t* create(size_t size);
extern bucket_t* find(bucket_t* root, size_t size);
extern bucket_t* remove(bucket_t* root, size_t size);
extern bucket_t* add(bucket_t* root, size_t size);
#endif
