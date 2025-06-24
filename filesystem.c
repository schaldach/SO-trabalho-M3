#include "filesystem.h"
// https://webdocs.cs.ualberta.ca/~holte/T26/ins-b-tree.html
BTree* btree_create() {
    BTree* tree = malloc(sizeof(BTree));
    tree->root = NULL;
    return tree;
}

TreeNode* create_txt_file(const char* name, const char* content) {
    File* file = malloc(sizeof(File));
    file->name = strdup(name);
    file->content = strdup(content);
    file->size = strlen(content);

    TreeNode* node = malloc(sizeof(TreeNode));
    node->name = strdup(name);
    node->type = FILE_TYPE;
    node->data.file = file;
    return node;
}

TreeNode* create_directory(const char* name) {
    Directory* dir = malloc(sizeof(Directory));
    dir->tree = btree_create();

    TreeNode* node = malloc(sizeof(TreeNode));
    node->name = strdup(name);
    node->type = DIRECTORY_TYPE;
    node->data.directory = dir;
    return node;
}

void delete_txt_file(BTree* tree, const char* name) {
    printf("Arquivo '%s' deletado (simulado)\n", name);
    free(node);
}

void delete_directory(BTree* tree, const char* name) {
    printf("Diretório '%s' deletado (simulado)\n", name);
}

Directory* get_root_directory() {
    Directory* root = malloc(sizeof(Directory));
    root->tree = btree_create();
    return root;
}

void change_directory(Directory** current, const char* path) {
    printf("Mudando para o diretório: %s (simulado)\n", path);
}

void list_directory_contents(Directory* dir, level=0) {
    printf("Conteúdo do diretório:\n");
}

TreeNode* btree_search(BTree* tree, const char* name) {
    printf("Buscando: %s (simulado)\n", name);
    return NULL;
}

void btree_insert(BTreeNode* bnode, TreeNode* node) {
    printf("Inserindo: %s (simulado)\n", node->name);

    // último elemento
    int index = bnode->num_keys;
    
    // se o node for menor que uma key, atualizar o index
    for(int i=0; i<bnode->num_keys; i++){
        TreeNode* key = bnode->children[i]
        if(strcmp(node->name, key->name) < 0){
            index = i;
            break;
        }
    }

    if(bnode->leaf){
        // abrir espaço para o novo elemento
        for(int y=num_keys-1; y>index; y--){
            bnode->keys[y] = bnode->keys[y-1];
        }
        bnode->keys[index] = node;
        bnode->num_keys++;

        if(bnode->num_keys >= BTREE_ORDER){
            int middle_index = (BTREE_ORDER-1)/2;
            TreeNode* middle_node = bnode->children[middle_index];
            bnode->children[middle_index] = NULL;

            TreeNode* right_nodes[middle_index];
            for(int i=middle_index + 1; i<num_keys; i++){
                right_nodes[i-(middle_index + 1)] = bnode->children[i];
                bnode->children[i] = NULL;
            }

            BTreeNode* parent = bnode->parent;
            if(bnode->parent == NULL){
                parent = malloc(sizeof(BTreeNode));
                parent->leaf = 0;
                parent->children[0] = bnode;
            }
            split_btree_node(parent, middle_node, right_nodes);
            bnode->num_keys = middle_index;
        }
    }
    else{
        btree_insert(bnode->children[index], node);
    }
}

void split_btree_node(BTreeNode* bnode, TreeNode* middle_node, TreeNode* right_nodes[(BTREE_ORDER/2)-1]){
    // último elemento
    int index = bnode->num_keys;
    
    // se o node for menor que uma key, atualizar o index
    for(int i=0; i<bnode->num_keys; i++){
        TreeNode* key = bnode->children[i]
        if(strcmp(middle_node->name, key->name) < 0){
            index = i;
            break;
        }
    }

    BTreeNode* new_node = malloc(sizeof(BTreeNode));
    
    new_node->num_keys = (BTREE_ORDER/2)-1;
    new_node->leaf = 1;
    for(int i=0; i<new_node->num_keys; i++){
        new_node->keys[i] = right_nodes[i];
    }

    for(int y=num_keys; y>index; y--){
        bnode->children[y] = bnode->children[y-1];
        if(y!=num_keys) bnode->keys[y] = bnode->keys[y-1];
    }
    bnode->key[index] = middle_node;
    bnode->children[index+1] = new_node;

    bnode->num_keys++;
    if(bnode->num_keys >= BTREE_ORDER){
        int middle_index = (BTREE_ORDER-1)/2;
        TreeNode* middle_node = bnode->children[middle_index];
        bnode->children[middle_index] = NULL;

        TreeNode* right_nodes[middle_index];
        for(int i=middle_index + 1; i<num_keys; i++){
            right_nodes[i-(middle_index + 1)] = bnode->children[i];
            bnode->children[i] = NULL;
        }

        BtreeNode* parent = bnode->parent;
        if(parent == NULL){
            parent = malloc(sizeof(BTreeNode));
            parent->leaf = 0;
            parent->children[0] = bnode;
        }
        
        split_btree_node(parent, middle_node, right_nodes);
    }
}

void btree_delete(BTree* tree, const char* name) {
    printf("Removendo: %s (simulado)\n", name);
    node->num_keys = node->num_keys - 1;
}

void btree_traverse(BTreeNode* node, char* name, int level=0) {
    for(int i=0;i<node->num_keys; i++){
        for(int y=0;y<level;y++){
            printf("-");
        }
        printf("%s\n", node->keys[i]);
        if(!node->leaf) btree_traverse(node->children[i]);
    }
    if(!node->leaf) btree_traverse(node->children[num_keys]);
}