#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "cgraph/list/list.h"

#define FILE_ARGUMENT_MANDATORY "File argument is mandatory!"

#define SUM "SUM\0"
#define SUBTRACTION "SUBTRACTION\0"
#define MULTIPLICATION "MULTIPLICATION\0"
#define DIVISION "DIVISION\0"
#define INDETERMINATE "INDETERMINATE\0"
#define POTENTIATION "POTENTIATION\0"
#define INTEGER_NUMBER "REAL_NUMBER\0"
#define REAL_NUMBER "REAL_NUMBER\0"

typedef struct token_s
{
    char* name;
    char* characters;
} token_t;

bool is_number(const char* character){
    return character > 47 && character < 58; 
}

list_t* tokenization(char* string){
    list_t* tokens = create_list();
    for (int i = 0; string[i] != '\0'; i++){
        token_t token;
        
        if(is_number(string[i])){
            bool has_dot = false;
            size_t size = 0;

            while (is_number(string[i + size]) || (string[i + size] == '.' && !has_dot)){
                if (string[i + size] == '.') has_dot = true;
                size++;
            }

            token.name = (has_dot)? REAL_NUMBER : INTEGER_NUMBER;
            token.characters = (char*) calloc(size, sizeof(char));
            if(token.characters == NULL){
                destroy_list(tokens);
                return NULL;
            }
            strncpy(token.characters, &string[i], size);
            i += size;
        } else{
            bool great_operator = false;

            switch (string[i]) {
            case '+': 
                token.name = SUM;
                break;
            case '-':
                token.name = SUBTRACTION;
                break;
            case '*':
                if(string[i + 1] == '*'){
                    great_operator = true;
                    token.name = POTENTIATION;
                } else token.name = MULTIPLICATION;
                break;
            case '/':
                token.name = DIVISION;
                break;
            default: 
                token.name = INDETERMINATE;
                break;
            }

            token.characters = (char*) calloc(2 + great_operator, sizeof(char));
            if(token.characters == NULL){
                destroy_list(tokens);
                return NULL;
            }

            strncpy(token.characters, &string[i], 1 + great_operator);
            if(great_operator) i++;
        }

        if(add_to_list(tokens, "operator_t", sizeof(token_t), &token)){
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
        token_t* token = (token_t*) get_from_list(tokens, i)->data;
        printf("%s(%s)\n", token->name, token->characters);
        free(token->characters);
    }
    
    destroy_list(tokens);    
    return 0;
}
