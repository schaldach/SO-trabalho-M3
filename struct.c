//Tipos de dados (estruturas)
typedef enum { FILE_TYPE, DIRECTORY_TYPE } NodeType;

typedef struct File {
char* name;
char* content;
size_t size;
} File;

typedef struct Directory Directory;

typedef struct TreeNode {
char* name;
NodeType type;
union {
  File* file;
  Directory* directory;
} data;
} TreeNode;

struct Directory {
BTree* tree; // Árvore B com TreeNode*
};

typedef struct BTree BTree;

//Manipulação da Árvore B
BTree* btree_create(int t); // t: grau mínimo da árvore B
void btree_insert(BTree* tree, TreeNode* node);
void btree_delete(BTree* tree, const char* name);

TreeNode* btree_search(BTree* tree, const char* name);
void btree_traverse(BTree* tree); // Exibe itens do diretório

//Funções de Arquivos
TreeNode* create_txt_file(const char* name, const char* content);
void delete_txt_file(BTree* tree, const char* name);

//Funções de Diretórios
TreeNode* create_directory(const char* name);
void delete_directory(BTree* tree, const char* name);

//Sistema de Navegação
Directory* get_root_directory();
void change_directory(Directory** current, const char* path);
void list_directory_contents(Directory* dir);
