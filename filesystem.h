#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdalign.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define BTREE_ORDER 3
#define MIDDLE_INDEX (BTREE_ORDER-1)/2;

typedef enum { FILE_TYPE, DIRECTORY_TYPE } NodeType;

typedef struct File {
    char* name;
    char* content;
    size_t size;
} File;

struct Directory;

typedef struct TreeNode {
    char* name;
    NodeType type;
    union {
        File* file;
        struct Directory* directory;
    } data;
} TreeNode;

typedef struct BTreeNode {
    int num_keys;
    TreeNode* keys[BTREE_ORDER - 1];
    struct BTreeNode* children[BTREE_ORDER];
    struct BTreeNode* parent;
    int leaf;
} BTreeNode;

typedef struct BTree {
    BTreeNode* root;
} BTree;

typedef struct Directory {
    BTree* tree;
} Directory;

typedef struct NodeOverflow {
    BTreeNode* child;
    TreeNode* key;
    bool valid;
} NodeOverflow;

// // BTree Functions
// BTree* btree_create();
// void btree_insert(BTree* tree, TreeNode* node);
// void btree_delete(BTree* tree, const char* name);
// TreeNode* btree_search(BTree* tree, const char* name);
// void btree_traverse(BTree* tree);

// // File/Directory creation
// TreeNode* create_txt_file(const char* name, const char* content);
// TreeNode* create_directory(const char* name);
// void delete_txt_file(BTree* tree, const char* name);
// void delete_directory(BTree* tree, const char* name);

// // Navigation
// Directory* get_root_directory();
// void change_directory(Directory** current, const char* path);
// void list_directory_contents(Directory* dir);

#endif