#include "filesystem.h"
// https://webdocs.cs.ualberta.ca/~holte/T26/ins-b-tree.html
BTree* btree_create() {
    BTree* tree = malloc(sizeof(BTree));
    tree->root = malloc(sizeof(BTreeNode));
    tree->root->leaf = true;
    tree->root->num_keys = 0;
    tree->root->parent = NULL;
    return tree;
}

TreeNode* create_txt_file(const char* name, const char* content) {
    File* file = malloc(sizeof(File));
    file->name = strdup(name);
    file->content = strdup(content);
    file->size = strlen(file->content);

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

TreeNode* btree_search(BTreeNode* bnode, const char* name) {
    for(int i=0; i<bnode->num_keys+1; i++){
        if(i!=bnode->num_keys){
            // printf("passou por: %s\n", bnode->keys[i]->name);
            if(strcmp(name, bnode->keys[i]->name) > 0) continue;
            else if (strcmp(name, bnode->keys[i]->name) == 0) return bnode->keys[i]; 
            else if (!bnode->leaf && strcmp(name, bnode->keys[i]->name) < 0) return btree_search(bnode->children[i], name);
        }
        else if(!bnode->leaf) return btree_search(bnode->children[i], name);
    }
    return NULL;
}

int get_node_index(BTreeNode* bnode, char* name){
    if(bnode == NULL) return 0;
    // último elemento
    int index = bnode->num_keys;
    
    // se o nome for "menor" que uma key, atualizar o index
    for(int i=0; i<bnode->num_keys; i++){
        TreeNode* key = bnode->keys[i];
        if(strcmp(name, key->name) <= 0){
            index = i;
            break;
        }
    }
    // printf("index in get_node_index: %d\n", index);
    return index;
}

Directory* get_root_directory() {
    Directory* root = malloc(sizeof(Directory));
    root->tree = btree_create();
    root->parent = NULL;
    return root;
}

bool node_in_bnode(BTreeNode* bnode, char* name){
    int index = get_node_index(bnode, name);
    if (bnode!=NULL && index<bnode->num_keys && strcmp(bnode->keys[index]->name, name) == 0) return true;
    else return false;
}

TreeNode* change_directory(Directory* current, char* path) {
    if(strcmp(path, "..") == 0) return current->parent;

    TreeNode* new_directory_node = btree_search(current->tree->root, path);

    if(new_directory_node != NULL && new_directory_node->type == DIRECTORY_TYPE){
        return new_directory_node;
    }
    else{
        printf("Directory not found\n");
        return NULL;
    }
}

void btree_traverse_image(BTreeNode* node, int level, FILE *fptr) {
    for(int i=0;i<node->num_keys+1; i++){
        if(!node->leaf) btree_traverse_image(node->children[i], level, fptr);
        if(i != node->num_keys){
            for(int y=0;y<level*2+1;y++){
                fprintf(fptr, "-");
            }
            fprintf(fptr, "%s", node->keys[i]->name);
            if(node->keys[i]->type == DIRECTORY_TYPE) fprintf(fptr, " <DIR>");
            fprintf(fptr, "\n");

            if(node->keys[i]->type == DIRECTORY_TYPE){
                btree_traverse_image(node->keys[i]->data.directory->tree->root, level+1, fptr);
            }
        }
    }
}

void btree_traverse(BTreeNode* node, int level, bool recursive, bool btree) {
    for(int i=0;i<node->num_keys+1; i++){
        // pode ser level+1 pra visualizar cada btree ou level para 
        // visualizar só o sistema de pastas no geral
        if(!node->leaf) btree_traverse(node->children[i], btree?level+1:level, recursive, btree);
        if(i != node->num_keys){
            for(int y=0;y<level*2+1;y++){
                printf("-");
            }
            printf("%s", node->keys[i]->name);
            if(node->keys[i]->type == DIRECTORY_TYPE) printf(" <DIR>");
            printf("\n");

            if(node->keys[i]->type == DIRECTORY_TYPE && recursive){
                btree_traverse(node->keys[i]->data.directory->tree->root, level+1, recursive, btree);
            }
        }
    }
}

void list_directory_contents(Directory* dir, bool recursive, bool btree) {
    btree_traverse(dir->tree->root, 0, recursive, btree);
}

BTreeNode* create_bnode(TreeNode** keys, bool leaf, BTreeNode** children, BTreeNode* parent){
    BTreeNode* new_node = malloc(sizeof(BTreeNode));

    new_node->num_keys = RIGHT_NODE_SIZE;
    new_node->leaf = leaf;
    new_node->parent = parent;

    for(int i=0; i<new_node->num_keys+1; i++){
        if(i != new_node->num_keys){
            new_node->keys[i] = keys[i];
        }
        if(!new_node->leaf){
            new_node->children[i] = children[i];
            new_node->children[i]->parent = new_node;
        }
    }

    return new_node;
}

NodeOverflow* insert_in_bnode(BTreeNode* bnode, TreeNode* node, int index, BTreeNode* child){
    NodeOverflow* overflow = malloc(sizeof(NodeOverflow));
    overflow->valid = false;
    
    // nesse caso o node seria o overflow, e não precisamos mais mexer no array
    if(index == BTREE_ORDER-1){
        overflow->valid = true;
        overflow->key = node;
        if(!bnode->leaf) overflow->child = child;
        bnode->num_keys++;
        return overflow;
    }

    // mover os elementos para frente
    int end_keys = bnode->num_keys;

    if(end_keys == BTREE_ORDER-1){
        // o último nodo não é movido, mas será o overflow
        end_keys = end_keys-1;
        if(!bnode->leaf) overflow->child = bnode->children[end_keys+1];
        overflow->key = bnode->keys[end_keys];
        overflow->valid = true;
    }

    for(int y=end_keys; y>index; y--){
        bnode->keys[y] = bnode->keys[y-1];
        if(!bnode->leaf) bnode->children[y+1] = bnode->children[y];
    }

    bnode->keys[index] = node;
    if(!bnode->leaf) bnode->children[index+1] = child;
    bnode->num_keys++;
    return overflow;
}

void split_btree_node(BTreeNode* bnode, NodeOverflow* overflow){
    // remover e salvar o nodo do meio
    TreeNode* middle_node = bnode->keys[MIDDLE_INDEX];
    bnode->keys[MIDDLE_INDEX] = NULL;

    // salvar nodos que estão à direita
    TreeNode** right_keys = malloc(sizeof(TreeNode*)*(RIGHT_NODE_SIZE));
    BTreeNode** right_children = malloc(sizeof(BTreeNode*)*(RIGHT_NODE_SIZE+1));

    for(int i=MIDDLE_INDEX + 1; i<bnode->num_keys; i++){
        if(i!=bnode->num_keys-1){
            right_keys[i-(MIDDLE_INDEX + 1)] = bnode->keys[i];
            bnode->keys[i] = NULL;
        }

        if(!bnode->leaf){
            right_children[i-(MIDDLE_INDEX + 1)] = bnode->children[i];
            bnode->children[i] = NULL;
        }
    }
    if(overflow->valid){
        right_keys[RIGHT_NODE_SIZE-1] = overflow->key;
        if(!bnode->leaf) right_children[RIGHT_NODE_SIZE] = overflow->child;
        free(overflow);
    }
    bnode->num_keys = LEFT_NODE_SIZE;

    // definir nodo parente
    BTreeNode* parent = bnode->parent;
    if(parent == NULL){
        parent = malloc(sizeof(BTreeNode));
        parent->leaf = false;
        parent->children[0] = bnode;
        parent->num_keys = 0;
        parent->parent = NULL;
        bnode->parent = parent;
    }
    
    int middle_node_index = get_node_index(parent, middle_node->name);
    BTreeNode* new_bnode = create_bnode(right_keys, bnode->leaf, right_children, parent);

    free(right_children);
    free(right_keys);

    NodeOverflow* parent_overflow = insert_in_bnode(parent, middle_node, middle_node_index, new_bnode);

    if(parent->num_keys >= BTREE_ORDER){
        split_btree_node(parent, parent_overflow);
    }
}

void btree_insert(BTreeNode* bnode, TreeNode* node) {
    // printf("Inserindo: %s\n", node->name);

    int node_index = get_node_index(bnode, node->name);

    if(bnode->leaf){
        NodeOverflow* overflow = insert_in_bnode(bnode, node, node_index, NULL);

        if(bnode->num_keys >= BTREE_ORDER){
            split_btree_node(bnode, overflow);
        }
    }
    else{
        btree_insert(bnode->children[node_index], node);
    }
}

void merge_bnodes(BTreeNode* left_node, TreeNode* middle_node, BTreeNode* right_node){
    left_node->keys[left_node->num_keys] = malloc(sizeof(TreeNode));
    memcpy(left_node->keys[left_node->num_keys], middle_node, sizeof(TreeNode));
    left_node->num_keys++;

    int pre_merge_size = left_node->num_keys;
    int merge_size = left_node->num_keys*2-1;
    // printf("pre_merge: %d, merge: %d\n", pre_merge_size, merge_size);

    for(int i=left_node->num_keys; i<=merge_size; i++){
        if(!right_node->leaf) left_node->children[i] = right_node->children[i-pre_merge_size];
        if(i!=merge_size){
            // printf("inserindo %s em %d de %d\n", right_node->keys[i-pre_merge_size]->name, i, i-pre_merge_size);
            left_node->keys[i] = right_node->keys[i-pre_merge_size];
            left_node->num_keys++;
        }
    }
}

void add_to_bnode_end(BTreeNode* bnode, TreeNode* node, bool start, BTreeNode* child){
    if(start){
        // movendo para frente
        for(int i=bnode->num_keys+1;i>0;i--){
            if(i!=bnode->num_keys+1) bnode->keys[i] = bnode->keys[i-1];
            if(!bnode->leaf) bnode->children[i+1] = bnode->children[i];
        }
        bnode->keys[0] = node;
        if(!bnode->leaf) bnode->children[0] = child;
    }
    else{
        bnode->keys[bnode->num_keys] = node;
        if(!bnode->leaf) bnode->children[bnode->num_keys+1] = child;
    }
    bnode->num_keys++;
}

void remove_from_bnode_end(BTreeNode* bnode, bool start){
    if(start){
        for(int i=0;i<bnode->num_keys;i++){
            if(i!=bnode->num_keys-1) bnode->keys[i] = bnode->keys[i+1];
            bnode->children[i] = bnode->children[i+1];
        }
    }
    bnode->keys[bnode->num_keys-1] = NULL;
    if(!bnode->leaf) bnode->children[bnode->num_keys] = NULL;
    bnode->num_keys = bnode->num_keys-1;
}

void remove_from_bnode(BTreeNode* bnode, int index){
    // apagando key e child 
    // printf("remove from bnode: %d\n", index);
    free(bnode->keys[index]);
    bnode->keys[index] = NULL;

    if(!bnode->leaf){
        free(bnode->children[index+1]);
        bnode->children[index+1] = NULL;
    }

    // movendo para trás
    for(int i=index; i<bnode->num_keys; i++){
        if(i!=bnode->num_keys-1) bnode->keys[i] = bnode->keys[i+1];
        if(i!=index && !bnode->leaf) bnode->children[i] = bnode->children[i+1];
    }
    bnode->keys[bnode->num_keys-1] = NULL;
    if(!bnode->leaf) bnode->children[bnode->num_keys] = NULL;
    bnode->num_keys = bnode->num_keys-1;
}

TreeNode* find_sucessor(BTreeNode* bnode){
    if(bnode == NULL) return NULL;
    if(bnode->leaf) return bnode->keys[0];
    else return find_sucessor(bnode->children[0]);
}

TreeNode* find_predecessor(BTreeNode* bnode){
    if(bnode == NULL) return NULL;
    if(bnode->leaf) return bnode->keys[bnode->num_keys-1];
    else return find_predecessor(bnode->children[bnode->num_keys]);
}

// https://enos.itcollege.ee/~japoia/algorithms/GT/Introduction_to_algorithms-3rd%20Edition.pdf
// 520
void btree_delete(BTreeNode* bnode, char* name) {
    // printf("Removendo: %s\n", name);

    int index = get_node_index(bnode, name);
    bool is_current_bnode = node_in_bnode(bnode, name);

    // printf("index in bnode: %d, is in bnode:%d\n", index, is_current_bnode);
    if(!is_current_bnode && bnode->leaf){
        printf("Nome não se encontra no diretório\n");
        return;
    }

    // Caso 1
    if(bnode->leaf){
        remove_from_bnode(bnode, index);
        return;
    }
    // Caso 2
    if(is_current_bnode){
        // Caso 2a
        BTreeNode* left_child = bnode->children[index];
        if(left_child->num_keys > MIDDLE_INDEX){
            TreeNode* predecessor = find_predecessor(left_child);
            memcpy(bnode->keys[index], predecessor, sizeof(TreeNode));
            btree_delete(left_child, predecessor->name);
            return;
        }

        // Caso 2b
        BTreeNode* right_child = bnode->children[index+1];
        if(right_child->num_keys > MIDDLE_INDEX){
            TreeNode* sucessor = find_sucessor(right_child);
            memcpy(bnode->keys[index], sucessor, sizeof(TreeNode));
            btree_delete(right_child, sucessor->name);
            return;
        }

        // Caso 2c
        merge_bnodes(left_child, bnode->keys[index], right_child);
        remove_from_bnode(bnode, index);
        btree_delete(left_child, name);
        return;
    }
    // Caso 3
    BTreeNode* target_child = bnode->children[index];
    if(target_child->num_keys <= MIDDLE_INDEX){
        // Caso 3a
        // pegando da esquerda
        if(index>0 && bnode->children[index-1]->num_keys > MIDDLE_INDEX){
            BTreeNode* left_sibling = bnode->children[index-1];

            TreeNode* sibling_last_key = left_sibling->keys[left_sibling->num_keys-1];
            BTreeNode* sibling_last_key_child_pointer = left_sibling->children[left_sibling->num_keys];

            add_to_bnode_end(target_child, bnode->keys[index-1], true, sibling_last_key_child_pointer);
            sibling_last_key_child_pointer->parent = target_child;
            bnode->keys[index-1] = sibling_last_key;
            remove_from_bnode_end(left_sibling, false);
        }

        // pegando da direita
        else if(index<bnode->num_keys && bnode->children[index+1]->num_keys > MIDDLE_INDEX){
            BTreeNode* right_sibling = bnode->children[index+1];

            TreeNode* sibling_first_key = right_sibling->keys[0];
            BTreeNode* sibling_first_key_child_pointer = right_sibling->children[0];

            add_to_bnode_end(target_child, bnode->keys[index], false, sibling_first_key_child_pointer);
            sibling_first_key_child_pointer->parent = target_child;
            bnode->keys[index] = sibling_first_key;
            remove_from_bnode_end(right_sibling, true);
        }

        // Caso 3b
        else{
            // usar o sibling da esquerda
            if(index>0){
                BTreeNode* left_sibling = bnode->children[index-1];
                merge_bnodes(left_sibling, bnode->keys[index-1], bnode->children[index]);
                remove_from_bnode(bnode, index-1);
                index = index - 1; // para que a chamada recursiva seja sobre o left_sibling
            }
            // usar o sibling da direita
            else{
                BTreeNode* right_sibling = bnode->children[index+1];
                merge_bnodes(bnode->children[index], bnode->keys[index], right_sibling);
                remove_from_bnode(bnode, index);
            }
        }
    }
    btree_delete(bnode->children[index], name);
}

void delete_txt_file(BTree* tree, char* name) {
    // printf("deletando arquivo %s\n", name);

    TreeNode* target_file_node = btree_search(tree->root, name);

    if(target_file_node != NULL && target_file_node->type == FILE_TYPE){
        btree_delete(tree->root, name);
    }
    else printf("Arquivo não achado\n");
}

void delete_directory(BTree* tree, char* name) {
    // printf("Deletando diretório %s\n", name);

    TreeNode* target_directory_node = btree_search(tree->root, name);

    if(target_directory_node == NULL || target_directory_node->type != DIRECTORY_TYPE){
        printf("Diretório não achado\n");
        return;
    }

    if(target_directory_node->data.directory->tree->root->num_keys == 0){
        btree_delete(tree->root, name);
    }
    else printf("Diretório não está vazio\n");
}

void list_file_content(Directory* dir, char* file_name){
    TreeNode* target_file_node = btree_search(dir->tree->root, file_name);

    if(target_file_node != NULL && target_file_node->type == FILE_TYPE){
        printf("%s\n", target_file_node->data.file->content);
    }
    else printf("Arquivo não achado\n");
}

void change_file_content(Directory* dir, char* file_name, char* file_content){
    TreeNode* target_file_node = btree_search(dir->tree->root, file_name);

    if(target_file_node != NULL && target_file_node->type == FILE_TYPE){
        target_file_node->data.file->content = strdup(file_content);
    }
    else printf("Arquivo não achado\n");
}