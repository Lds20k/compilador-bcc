#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "cgraph/list/list.h"

#define FILE_ARGUMENT_MANDATORY "File argument is mandatory!"

#define SUM "SUM\0"
#define SUBTRACTION "SUBTRACTION\0"
#define MULTIPLICATION "MULTIPLICATION\0"
#define DIVISION "DIVISION\0"
#define INDETERMINATE "INDETERMINATE\0"

typedef struct operator_s
{
    char* name;
    char* characters;
} operator_t;

list_t* tokenization(char* string){
    list_t* tokens = create_list();
    for (int i = 0; string[i] != '\0'; i++){
        operator_t operator;
        operator.characters = (char*) calloc(1, sizeof(char));
        strncpy(operator.characters, &string[i], 1);

        switch (string[i]) {
        case '+': 
            operator.name = SUM;
            break;
        case '-':
            operator.name = SUBTRACTION;
            break;
        case '*':
            operator.name = MULTIPLICATION;
            break;
        case '/':
            operator.name = DIVISION;
            break;
        default: 
            operator.name = INDETERMINATE;
            break;
        }

        if(add_to_list(tokens, "operator_t", sizeof(operator_t), &operator)){
            destroy_list(tokens);
            return NULL;
        }
    }
    return tokens;
}

int main(int argc, char const *argv[]){
    list_t* tokens = NULL;

    if(argc < 2){
        fprintf(stderr, "[arguments error] %s\n", FILE_ARGUMENT_MANDATORY);
        return 1;
    }

    int file_descriptor = open(argv[1], O_RDONLY);
    if (file_descriptor < 0) return 1;
    
    int len = lseek(file_descriptor, 0, SEEK_END);
    char* data = mmap(0, len, PROT_READ, MAP_PRIVATE, file_descriptor, 0);

    if((tokens = tokenization(data)) == NULL) return -1;
    close(file_descriptor);
    
    for (size_t i = 0; i < tokens->length; i++)
    {
        operator_t* operator = (operator_t*) get_from_list(tokens, i)->data;
        printf("%s(%s)\n", operator->name, operator->characters);
    }
    
    destroy_list(tokens);    
    return 0;
}
