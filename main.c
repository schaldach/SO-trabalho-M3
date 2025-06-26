#include "filesystem.h"
#include "filesystem.c"

void reverse_string(char* str, int str_len){
    char temp;
    int i=0; 
    int j=str_len-1;

    while(i<j){
        temp = str[j];
        str[j] = str[i];
        str[i] = temp;
        i++;
        j = j-1;
    }
}

int main(){
    Directory* root_directory = get_root_directory();

    // current_directory_node será a variável atualizada durante o loop
    TreeNode* current_directory_node = malloc(sizeof(TreeNode));
    current_directory_node->data.directory = root_directory;
    current_directory_node->type = DIRECTORY_TYPE;
    current_directory_node->name = strdup("");

    char input[100];
    char command[100];
    char argument[100];
    char path[100];
    char* sep = "/";
    char content[] = "dsdsadasdas";

    printf("--------\n");
    printf("ls = listar os arquivos e diretórios na pasta atual\n");
    printf("cd {nome} = navegar para diretório\n");
    printf("touch {nome} = criar arquivo\n");
    printf("mkdir {npme} = criar diretório\n");
    printf("rmdir {nome} = excluir diretório vazio\n");
    printf("rm {nome} = excluir arquivo\n");
    printf("--------\n\n");


    while(true){
        // definindo variáveis para ficar menos verboso
        Directory* current_directory = current_directory_node->data.directory;
        BTreeNode* current_directory_bnode = current_directory_node->data.directory->tree->root;

        TreeNode* dir_temp = current_directory_node;
        path[0] = '/';
        path[1] = '\0';
        while(dir_temp->data.directory->parent != NULL){
            char* dir_name = strdup(dir_temp->name);
            reverse_string(dir_name, strlen(dir_name));
            strcat(path, dir_name);
            strcat(path, sep);
            dir_temp = dir_temp->data.directory->parent;
        }
        reverse_string(path, strlen(path));

        printf("%s $ ", path);

        // lendo input
        fgets(input, 100, stdin);

        // separando input em comando e argumento
        int commandLength = strcspn(input, " ");
        bool spaceFound = false;
        for(int i=0; i<strlen(input); i++){
            if(!spaceFound){
                if(input[i] != ' ' && input[i] != '\n'){
                    command[i] = input[i];
                }
                else{
                    spaceFound = true;
                    command[i] = '\0';
                    commandLength = i;
                }
            }
            else{
                if(input[i] != ' ' && input[i] != '\n'){
                    argument[i-commandLength-1] = input[i];
                }
                else{
                    argument[i-commandLength-1] = '\0';
                    break;
                }
            }
        }
        // printf("comando: %s, argumento: %s\n", command, argument);

        short commandCode = -1;
        if(strcmp(command, "ls") == 0) commandCode = 0;
        if(strcmp(command, "cd") == 0) commandCode = 1;
        if(strcmp(command, "touch") == 0) commandCode = 2;
        if(strcmp(command, "mkdir") == 0) commandCode = 3;
        if(strcmp(command, "rm") == 0) commandCode = 4;
        if(strcmp(command, "rmdir") == 0) commandCode = 5;

        switch(commandCode){
            // ls
            case 0:
                list_directory_contents(current_directory);
            break;

            // cd
            case 1:
                TreeNode* target_directory_node = change_directory(current_directory, argument);
                if(target_directory_node != NULL) current_directory_node = target_directory_node;
            break;

            // touch
            case 2:
                TreeNode* new_file = create_txt_file(argument, content);
                btree_insert(current_directory_bnode, new_file);
                if(current_directory_bnode->parent != NULL){
                    current_directory_node->data.directory->tree->root = current_directory_node->data.directory->tree->root->parent;
                }
            break;

            // mkdir
            case 3:
                TreeNode* new_dir = create_directory(argument);
                new_dir->data.directory->parent = current_directory_node;
                btree_insert(current_directory_bnode, new_dir);
                if(current_directory_bnode->parent != NULL){
                    current_directory_node->data.directory->tree->root = current_directory_node->data.directory->tree->root->parent;
                }
            break;

            case 4:
            break;

            case 5:
            break;
        }
        printf("\n");
    }
}
