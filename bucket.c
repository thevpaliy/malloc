#include "bucket.h"
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>

bucket_t* create(size_t size)
{
  bucket_t* bucket = sbrk(sizeof(bucket_t));
  bucket->parent = NULL;
  bucket->right = NULL;
  bucket->left = NULL;
  bucket->size = size;
  bucket->red = true;
  return bucket;
}

bucket_t* add(bucket_t* root, size_t size, page_t* page)
{
  if(root != NULL) {
    bucket_t* node = root;
    bucket_t* parent = node;
    while(node != NULL) {
      parent = node;
      node = node->size > size ? node->left : node->right;
    }
    bucket_t* target = create(size);
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
    if(taget-> left && target->right) {
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
    remove(target);
    d_balance(root, target);
  }
  return root;
}

void r_rotation(bucket_t* node)
{
  if(node->parent != NULL) {
    bucket_t* parent = node->parent;
    bucket_t* grandparent = parent->parent;
    bucekt_t* right = node->right;
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
        return;
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

void remove(bucket_t* node)
{
  node->parent = NULL;
  node->right = NULL;
  node->left = NULL;
  remove(node->head);
}

void remove(page_t* node)
{
  while(node != NULL){
    page_t* temp = node->next;
    node->next = NULL;
  }
}
