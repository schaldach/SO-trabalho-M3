#include "head.h"

int main(){
    char input[100];
    
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
        fgets(input, 100, stdin);

        bool spaceFound = false;
        int commandLength = strcspn(input, " ");
        char command[commandLength];
        char argument[100-commandLength-1];
        for(int i=0;i<strlen(input);i++){
            if(!spaceFound){
                if(input[i] != ' '){
                    command[i] = input[i];
                }
                else{
                    spaceFound = true;
                    command[i] = '\0';
                    commandLength = i;
                }
            }
            else if(commandLength != strlen(input)){
                argument[i-commandLength-1] = input[i];
            }
        }
        printf("comando: %s, argumento: %s\n", command, argument);
    }
}
