#ifndef _RBTREE_H_
#define _RBTREE_H_

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

// 1. 노드의 색깔 (color_t 타입 = RED-0/BLACK-1)
typedef enum {RBTREE_RED, RBTREE_BLACK} color_t;

// 2. 노드의 키 값 (key_t 타입 = int)
typedef int key_t;

// 3. 노드 (구조체 node_t)
typedef struct node_t{
    color_t color;
    key_t key;
    struct node_t *parent, *left, *right;
} node_t;

// 4. 트리 (구조체 rbtree)
typedef struct {
    node_t *root;
    node_t *nil; // for sentinel
} rbtree;

// void test_vlr(rbtree *t, node_t *now);

rbtree* new_rbtree(void);

void delete_rbtree(rbtree* t);
void delete_node(node_t* del, node_t* nil);

node_t* rbtree_insert(rbtree* t, const key_t key);
void rb_insert_fixup(node_t* z, rbtree* t);
void left_rotate(rbtree* t, node_t* x);
void right_rotate(rbtree* t, node_t* x);

node_t* rbtree_find(const rbtree* t, const key_t key);
node_t* rbtree_min(const rbtree* t);
node_t* rbtree_max(const rbtree* t);

int rbtree_to_array(const rbtree* t, key_t* arr, const size_t n);
void rbtree_inorder(const rbtree* t, node_t* n, key_t* arr, size_t size, int* sizePtr);

int rbtree_erase(rbtree* t, node_t* z);
void rbtree_erase_fixup(rbtree* t, node_t* x);
void rbtree_transplant(rbtree* t, node_t* u, node_t* v);

# endif