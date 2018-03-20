#include "bucket.h"
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>

#define LIMIT 32
#define INITIAL_CAPACITY 10
#define PAGE (x) (sizeof(page_t) + x)

bucket_t* i_balance(bucket_t* root, bucket_t* node);
bucket_t* d_balance(bucket_t* root, bucket_t* node);
bucket_t* transplant(bucket_t* root, bucket_t* x, bucket_t* y);
page_t*   create_page(size_t size);
void l_rotation(bucket_t* node);
void r_rotation(bucket_t* node);

void p_remove(page_t* node)
{
  while(node != NULL){
    page_t* temp = node->next;
    node->next = NULL;
  }
}

void b_remove(bucket_t* node)
{
  node->parent = NULL;
  node->right = NULL;
  node->left = NULL;
  p_remove(node->head);
}

bucket_t* create(size_t size)
{
  bucket_t* bucket = sbrk(sizeof(bucket_t) + PAGE(size) * INITIAL_CAPACITY);
  if(bucket == NULL) {
    return NULL;
  }
  bucket->parent = NULL;
  bucket->right = NULL;
  bucket->left = NULL;
  bucket->occupied = NULL;
  bucket->size = size;
  bucket->red = true;
  bucket->head = bucket + sizeof(bucket_t);
  page_t* page = head;
  for(int index = 1; index < INITIAL_CAPACITY; index++) {
    page->next = NULL;
    page->prev = NULL;
    page->parent = bucket;
    page_t* temp = bucket->head + PAGE(size) * index;
    page->next = temp;
    temp->prev = page;
    page = temp;
  }
  page->parent = bucket;
  page->next = NULL;
  return bucket;
}

page_t* create_page(size_t size)
{
    page_t* page = sbrk(PAGE(size));
    if(page == NULL)
      return NULL;
    page->parent = NULL;
    page->next = NULL;
    page->prev = NULL;
    return page;
}

bucket_t* add(bucket_t* root, bucket_t* target)
{
  if(root != NULL) {
    bucket_t* node = root;
    bucket_t* parent = node;
    size_t size = target->size;
    while(node != NULL) {
      parent = node;
      node = node->size > size ? node->left : node->right;
    }
    if(parent->size < size)
      parent->right = node;
    else
      parent->left = node;
    node->parent = parent;
    root = i_balance(root, target);
  }
  return root;
}

bucket_t* search(bucket_t* root, size_t size)
{
  while(root != NULL) {
    if(root->size >= size)
      return root;
    root = root->size < size ? root->left : root->right;
  }
  return NULL;
}

bucket_t* min(bucket_t* root)
{
  bucket_t* node = root;
  while(root != NULL) {
    node = root;
    root = root->left;
  }
  return node;
}

bucket_t* max(bucket_t* root)
{
  bucket_t* node = root;
  while(root != NULL) {
    node = root;
    node = node->right;
  }
  return node;
}

bucket_t* remove(bucket_t* root, size_t size)
{
  bucket_t* target = search(root, size);
  if(target != NULL) {
    bucket_t* successor = NULL;
    if(target-> left && target->right) {
      successor = target->right;
      while(successor->left)
        successor= successor->left;
      if(successor->parent != target){
        successor->parent->left = successor->right;
        if(successor->right != NULL)
          successor->right->parent = successor->parent;
        successor->right = target->right;
        successor->right->parent = successor;
      }
    } else {
      successor = target->left ?
         target->left : target->right;
    }
    if(target->parent != NULL){
      if(target->parent->left == target)
        target->parent->left = successor;
      else
        target->parent->right = successor;
    }
    if(successor != NULL)
      successor->parent = target->parent;
    root = root != target ? root : successor;
    b_remove(target);
    d_balance(root, target);
  }
  return root;
}

void l_rotation(bucket_t* node)
{
  if(node->parent != NULL) {
    bucket_t* parent = node->parent;
    bucket_t* grandparent = parent->parent;
    bucket_t* left = node->left;
    node->parent = grandparent;
    node->left = parent;
    parent->right = left;
    parent->parent = node;
    if(left != NULL)
      left->parent = parent;
    if(grandparent != NULL) {
      if(grandparent->left == parent)
        grandparent->left = node;
      else
        grandparent->right = node;
    }
  }
}

void r_rotation(bucket_t* node)
{
  if(node->parent != NULL) {
    bucket_t* parent = node->parent;
    bucket_t* grandparent = parent->parent;
    bucket_t* right = node->right;
    parent->parent = node;
    parent->left = right;
    node->parent = grandparent;
    node->right = parent;
    if(right != NULL)
      right->parent = parent;
    if(grandparent != NULL) {
      if(grandparent->left == parent)
        grandparent->left = node;
      else
        grandparent->right = node;
    }
  }
}

bucket_t* i_balance(bucket_t* root, bucket_t* node)
{
  if(root != NULL) {
    while(node->parent != NULL && node->parent->red) {
      bucket_t* parent = node->parent;
      bucket_t* grandparent = parent->parent->parent;
      if(grandparent == NULL)
        return root;
      bucket_t* uncle = grandparent->left != parent
              ? grandparent->left : grandparent->right;
      if(uncle == NULL || !uncle->red) {
        if(grandparent->right != parent) {
          if(parent->left != node) {
            l_rotation(node);
            parent = node;
          }
          r_rotation(parent);
        }else {
          if(parent->right != node) {
            r_rotation(node);
            parent = node;
          }
          l_rotation(parent);
        }
        parent->red = false;
        node = parent;
        root = node->parent != NULL ? root : node;
      }else {
        grandparent->red = grandparent->parent != NULL;
        uncle->red = false;
        parent->red = false;
        node = grandparent;
      }
    }
  }
  return root;
}

bucket_t* transplant(bucket_t* root, bucket_t* x , bucket_t* y)
{
  if(x->parent == NULL)
    root = y;
  else if(x->parent->left == x)
    x->parent->left = y;
  else
    x->parent->right = y;
  y->parent = x->parent;
  return root;
}

bucket_t* remove(bucket_t* root, size_t size)
{
  bucket_t* target = search(root, size);
  if(target != NULL) {
    bucket_t* node = target;
    bucket_t* x = target->left;
    bool color = node->red;
    if(target->left == NULL) {
      x = target->right;
      root = transplant(root, target, target->right);
    }else if(target->right == NULL) {
      root = transplant(root, target, target->left);
    }else {
      node = min(node->right);
      color = node->red;
      x = node->right;
      if(node->parent != target) {
        node->parent->left = node->right;
        if(node->right != NULL)
          node->right->parent = node->parent;
        node->right = target->right;
        node->right->parent = node;
      }
      root = transplant(root, target, node);
      node->left = target->left;
      node->left->parent = node;
      node->red = target->red;
    }
    b_remove(target);
    if(!color) {
      root = d_balance(root, x);
    }
  }
  return root;
}

bool is_black(bucket_t* node)
{
  return node == NULL || !node->red;
}

bucket_t* d_balance(bucket_t* root, bucket_t* node)
{
  while(node != NULL && !node->red) {
    bucket_t* parent = node->parent;
    if(parent != NULL) {
      if(parent->right != node){
        bucket_t* temp = parent->right;
        if(temp != NULL && temp->red) {
          temp->red = false;
          parent->color = true;
          l_rotation(temp);
        }
      }
    }
  }
}

bucket_t* remove(bucket_t* root, page_t* node)
{
  if(node != NULL) {
    bucket_t* parent = node->parent;
    if(parent != NULL) {
      if(node->prev != NULL)
        node->prev->next = node->next;
      if(node->next != NULL)
        node->next->prev = node->prev;
      if(parent->occupied == node)
        parent->occupied = node->next;
      page_t* head = parent->head;
      if(head != NULL)
        head->prev = node;
      node->next = head;
      parent->head = node;
      parent->count += 1;
    }
  }
  return root;
}

page_t* get(bucket_t* bucket)
{
  if(bucket != NULL) {
    page_t* page = bucket->head;
    if(page == NULL) {
      page = create_page(bucket->size);
      if(page == NULL)
        return NULL;
    }
    page_t* occupied = bucket->occupied;
    if(occupied != NULL)
      occupied->prev = page;
    page->next = occupied;
    bucket->occupied = page;
    return page;
  }
  return NULL;
}
