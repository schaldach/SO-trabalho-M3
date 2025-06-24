#include "filesystem.h"
#include "filesystem.c"

int main(){
    // Btree* root;
    // root.createBtree();
    while(true){
        // lendo input
        printf("--------\n");
        printf("ls = listar os arquivos e diretórios na pasta atual\n");
        printf("cd {nome} = navegar para diretório\n");
        printf("touch {nome} = criar arquivo\n");
        printf("mkdir {npme} = criar diretório\n");
        printf("rmdir {nome} = excluir diretório vazio\n");
        printf("rm {nome} = excluir arquivo\n");
        printf("--------\n");
        printf("Digite o comando: \n");
        char input[100];
        fgets(input, 100, stdin);

        // separando input em comando e argumento
        bool spaceFound = false;
        int commandLength = strcspn(input, " ");
        char command[commandLength];
        char argument[100-commandLength-1];
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
                argument[i-commandLength-1] = input[i];
            }
        }
        printf("comando: %s, argumento: %s\n", command, argument);

        short commandCode;
        if(strcmp(command, "ls") == 0) commandCode = 0;
        if(strcmp(command, "cd") == 0) commandCode = 1;
        if(strcmp(command, "touch") == 0) commandCode = 2;
        if(strcmp(command, "mkdir") == 0) commandCode = 3;
        if(strcmp(command, "rm") == 0) commandCode = 4;
        if(strcmp(command, "rmdir") == 0) commandCode = 5;

        // switch(commandCode){

        // }
    }
}
