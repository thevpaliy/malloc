#include "malloc.h"
#include "bucket.h"

bucket_t* root;

void*
malloc(size_t size)
{
  if(root != NULL) {
    bucket_t* node = find(root, size);
    if(node == NULL) {
      node = create(size);
      root = add(root, node);
    }
    page_t* page = get(node);
    return page != NULL ? (page + 1) : NULL;
  }
}

void 
free(void* pointer)
{
  if(pointer != NULL) {
    root = remove(pointer - sizeof(page_t));
  }
}
