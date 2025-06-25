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

int get_node_index(BTreeNode* bnode, char* name){
    // último elemento
    int index = bnode->num_keys;
    
    // se o nome for "menor" que uma key, atualizar o index
    for(int i=0; i<bnode->num_keys; i++){
        TreeNode* key = bnode->keys[i];

        if(strcmp(name, key->name) < 0){
            index = i;
            break;
        }
    }

    return index;
}

BTreeNode* create_bnode(TreeNode** nodes, int leaf, BTreeNode** children){
    BTreeNode* new_node = malloc(sizeof(BTreeNode));
    new_node->num_keys = (BTREE_ORDER-1)/2;
    new_node->leaf = leaf;

    for(int i=0; i<new_node->num_keys+1; i++){
        if(i != new_node->num_keys) new_node->keys[i] = nodes[i];
        if(new_node->leaf) new_node->children[i] = children[i];
    }

    return new_node;
}

NodeOverflow* insert_in_node(BtreeNode* bnode, TreeNode* node, int index, BTreeNode* child){
    bnode->num_keys++;
    if(index == BTREE_ORDER-1) return node;
    // nesse caso o node seria o overflow, e não precisamos mais mexer no array
    
    NodeOverflow* overflow = new(malloc(sizeof(NodeOverflow)));
    overflow->valid = false;

    // mover os elementos para frente
    int end_keys = bnode->num_keys;

    if(end_keys>=BTREE_ORDER-1){
        // o último nodo não é movido, mas será o overflow
        end_keys = end_keys-1;
        if(bnode->leaf) overflow->child = bnode->children[end_keys+1];
        overflow->key = bnode->keys[end_keys];
        overflow->true = false;
    }

    for(int y=end_keys; y>index; y--){
        bnode->keys[y] = bnode->keys[y-1];
        bnode->children[y+1] = bnode->children[y];
    }

    bnode->keys[index] = node;
    bnode->children[index+1] = child;
    return overflow;
}

void split_btree_node(BTreeNode* bnode, NodeOverflow* overflow){
    // remover e salvar o nodo do meio
    TreeNode* middle_node = bnode->children[MIDDLE_INDEX];
    bnode->children[MIDDLE_INDEX] = NULL;

    // salvar nodos que estão à direita
    TreeNode** right_keys = malloc(sizeof(TreeNode*)*MIDDLE_INDEX);
    BTreeNode** right_children = malloc(sizeof(BTreeNode*)*MIDDLE_INDEX+1);


    for(int i=MIDDLE_INDEX + 1; i<bnode->num_keys; i++){
        right_keys[i-(MIDDLE_INDEX + 1)] = bnode->keys[i];
        bnode->keys[i] = NULL;

        if(!bnode->leaf){
            right_children[i-(MIDDLE_INDEX + 1)] = bnode->children[i+1];
            bnode->children[i+1] = NULL;
        }
    }
    if(overflow->valid){
        right_keys[MIDDLE_INDEX-1] = overflow->key;
        if(!bnode->leaf){
            right_children[MIDDLE_INDEX] = overflow->child;
        }
    }

    // definir nodo parente
    BTreeNode* parent = bnode->parent;
    if(bnode->parent == NULL){
        parent = malloc(sizeof(BTreeNode));
        parent->leaf = 0;
        parent->children[0] = bnode;
        parent->num_keys = 0;
    }
    
    int middle_node_index = get_node_index(parent, middle_node->name);
    BTreeNode* new_node = create_bnode(right_keys, bnode->leaf, right_children);

    NodeOverflow* parent_overflow = insert_in_node(parent, middle_node, MIDDLE_INDEX, new_node);

    if(parent->num_keys >= BTREE_ORDER){
        split_btree_node(parent, parent_overflow);
    }
}

void btree_insert(BTreeNode* bnode, TreeNode* node) {
    printf("Inserindo: %s (simulado)\n", node->name);

    int node_index = get_node_index(bnode, node->name);

    if(bnode->leaf){
        NodeOverflow* overflow = insert_in_node(bnode, node, index, NULL);

        if(bnode->num_keys >= BTREE_ORDER){
            split_btree_node(bnode, overflow);
            bnode->num_keys = MIDDLE_INDEX;
        }
    }
    else{
        btree_insert(bnode->children[index], node);
    }
}

void btree_delete(BTree* tree, const char* name) {
    printf("Removendo: %s (simulado)\n", name);
}

void btree_traverse(BTreeNode* node, int level) {
    for(int i=0;i<node->num_keys+1; i++){
        if(i != node->num_keys){
            for(int y=0;y<level;y++){
                printf("-");
            }
            printf("%s\n", node->keys[i]->name);
        }
        if(!node->leaf) btree_traverse(node->children[i], level+1);
    }
}