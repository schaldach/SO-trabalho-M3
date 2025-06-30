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
    current_directory_node->name = strdup("~");

    char input[1024*1024];
    char command[100];
    char argument[100];
    char path[100];
    char* sep = "/";
    char content[1024*1024] = "Este é o conteúdo de um arquivo padrão.";

    printf("--------\n");
    printf("ls = listar os arquivos e diretórios na pasta atual\n");
    printf("ls-r = listar os arquivos e diretórios na pasta atual e todos os subdiretórios\n");
    printf("ls-b = listar os arquivos e diretórios na pasta atual no formato da B-Tree do diretório\n");
    printf("cd {diretório} = navegar para diretório\n");
    printf("touch {arquivo} = criar arquivo\n");
    printf("mkdir {diretório} = criar diretório\n");
    printf("rm {arquivo} = excluir arquivo\n");
    printf("rmdir {diretório} = excluir diretório vazio\n");
    printf("cat {arquivo} = visualizar conteúdo do arquivo\n");
    printf("cat> {arquivo} {conteúdo} = alterar conteúdo do arquivo\n");
    printf("--------\n\n");


    while(true){
        // definindo variáveis para ficar menos verboso
        Directory* current_directory = current_directory_node->data.directory;
        BTreeNode* current_directory_bnode = current_directory_node->data.directory->tree->root;

        TreeNode* dir_temp = current_directory_node;
        path[0] = '\0';
        while(dir_temp != NULL){
            char* dir_name = strdup(dir_temp->name);
            reverse_string(dir_name, strlen(dir_name));
            strcat(path, dir_name);
            // não colocar sep se é o ultimo diretório
            if(dir_temp->data.directory->parent != NULL) strcat(path, sep);
            dir_temp = dir_temp->data.directory->parent;
        }
        reverse_string(path, strlen(path));

        printf("%s$ ", path);

        // lendo input
        fgets(input, 100, stdin);

        // separando input em comando e argumento
        int commandLength = strcspn(input, " ");
        bool spaceFound = false;
        for(int i=0; i<strlen(input); i++){
            if(!spaceFound){
                if(input[i] != ' ' && input[i] != '\n'&& input[i] != '\0'){
                    command[i] = input[i];
                }
                else{
                    spaceFound = true;
                    command[i] = '\0';
                    commandLength = i;
                }
            }
            else{
                if(input[i] != ' ' && input[i] != '\n' && input[i] != '\0'){
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
        if(strcmp(command, "ls-r") == 0) commandCode = 6;
        if(strcmp(command, "ls-b") == 0) commandCode = 7;
        if(strcmp(command, "cat") == 0) commandCode = 8;
        if(strcmp(command, "cat>") == 0) commandCode = 9;
        if(strcmp(command, "fs.img") == 0) commandCode = 10;

        switch(commandCode){
            // ls
            case 0:
                list_directory_contents(current_directory, false, false);
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

            // rm
            case 4:
                delete_txt_file(current_directory->tree, argument);
                if(current_directory_bnode->num_keys == 0 && !current_directory_bnode->leaf){
                    current_directory_node->data.directory->tree->root = current_directory_bnode->children[0];
                    free(current_directory_bnode->parent);
                    current_directory_node->data.directory->tree->root->parent = NULL;
                }
            break;

            // rmdir
            case 5:
                delete_directory(current_directory->tree, argument);
                if(current_directory_bnode->num_keys == 0 && !current_directory_bnode->leaf){
                    current_directory_node->data.directory->tree->root = current_directory_bnode->children[0];
                    free(current_directory_bnode->parent);
                    current_directory_node->data.directory->tree->root->parent = NULL;
                }
            break;

            // ls-r (recursive)
            case 6:
                list_directory_contents(current_directory, true, false);
            break;

            // ls-b (b-tree)
            case 7:
                list_directory_contents(current_directory, false, true);
            break;
            
            // cat
            case 8:
                list_file_content(current_directory, argument);
            break;

            // cat>
            case 9:
                int initial_position = strlen(command)+1+strlen(argument)+1;
                for(int i=initial_position; i<strlen(input); i++){
                    if(input[i] != ' ' && input[i] != '\n'){
                        content[i-initial_position] = input[i];
                    }
                    else{
                        content[i-initial_position] = '\0';
                        break;
                    }

                }
                change_file_content(current_directory, argument, content);
            break;

            // fs.img
            case 10:
                FILE* fptr = fopen(FSIMG_FILE, "w");
                btree_traverse_image(current_directory->tree->root, 0, fptr);
                fclose(fptr);
            break;
        }   
        printf("\n");
    }
}
