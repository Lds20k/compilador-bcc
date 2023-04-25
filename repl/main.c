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
#define SPACE "SPACE\0"
#define INDETERMINATE "INDETERMINATE\0"
#define POTENTIATION "POTENTIATION\0"
#define INTEGER_NUMBER "INTEGER_NUMBER\0"
#define REAL_NUMBER "REAL_NUMBER\0"

typedef struct token_s
{
    char* name;
    char* characters;
} token_t;

typedef struct value_s
{
    int integer_value;
    float real_value;
} value_t;

bool is_number(const char* character){
    return *character > 47 && *character < 58;
}

value_t parse_number(list_t* tokens, int index){
    value_t value = {0};
    if(tokens->length >= 1){
        token_t* token = (token_t*) get_from_list(tokens, index)->data;

        if (strcmp(token->name, INTEGER_NUMBER) != 0 && strcmp(token->name, REAL_NUMBER) != 0){
            printf("Unexpected token %s of value %s\n", token->name, token->characters);
            value_t value = {0};
            return value;
        }
        
        if(strcmp(token->name, INTEGER_NUMBER) == 0)
            value.integer_value = strtol(token->characters, NULL, 10);

        if(strcmp(token->name, REAL_NUMBER) == 0)
            value.real_value = strtof(token->characters, NULL);
    }

    return value;
}

value_t parse_term(list_t* tokens, size_t index, bool operation_flag, bool number_flag){
    if(index >= tokens->length){
        if(number_flag){
            printf("Invalid operation token syntax\n");
            value_t value = {0};
            return value;
        }
        
        value_t value = {0};
        return value;
    }

    token_t* token = (token_t*) get_from_list(tokens, index)->data;

    if(strcmp(token->name, SPACE) == 0) return parse_term(tokens, index + 1, operation_flag, number_flag);
    else {
        if(!strcmp(token->name, SUM) && operation_flag)
            return parse_term(tokens, index + 1, false, true);
        if(!strcmp(token->name, SUBTRACTION) && operation_flag)
            return parse_term(tokens, index + 1, false, true);
        else if(operation_flag){
            printf("Unexpected token %s of value %s\n", token->name, token->characters);
            value_t value = {0};
            return value;
        }

        value_t return_value = {0};
        value_t value = parse_number(tokens, index);
        value_t next_value = parse_term(tokens, index + 1, true, false);

        bool value_real = value.real_value != 0 && value.integer_value == 0;
        bool next_value_real = next_value.real_value != 0 && next_value.integer_value == 0;

        if (value_real || next_value_real){
            return_value.real_value = (value_real) ? value.real_value : (float)value.integer_value;
            return_value.real_value += (next_value_real) ? next_value.real_value : (float)next_value.integer_value;

            return return_value;
        }
        
        return_value.integer_value = value.integer_value;
        return_value.integer_value += next_value.integer_value;

        return return_value;
    }
}

value_t parse_expression(list_t* list){
    value_t value = {0};
    if (list->length < 1) return value;

    return parse_term(list, 0, false, false);
}

list_t* tokenization(char* string){
    list_t* tokens = create_list();
    for (int i = 0; string[i] != '\0'; i++){
        token_t token;
        
        if(is_number(&string[i])){
            bool has_dot = false;
            size_t size = 0;

            while (is_number(&string[i + size]) || (string[i + size] == '.' && !has_dot)){
                if (string[i + size] == '.') has_dot = true;
                size++;
            }

            token.name = (has_dot)? REAL_NUMBER : INTEGER_NUMBER;
            token.characters = (char*) calloc(size + 1, sizeof(char));
            if(token.characters == NULL){
                destroy_list(tokens);
                return NULL;
            }
            strncpy(token.characters, &string[i], size);
            i += size - 1;
        }else{
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
            case ' ': 
                token.name = SPACE;
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
    char data[256] = "\0";
    bool quit = false;

    while(!quit){
        printf(">>");
        fgets(data, 256, stdin);
        data[strcspn(data, "\n")] = 0;
        if(!strcmp(data, "quit")){
            quit = true;
            continue;
        }
        
        list_t* tokens = NULL;

        if((tokens = tokenization(data)) == NULL) return -1;
        
        value_t value = parse_expression(tokens);

        if (value.real_value != 0 && value.integer_value == 0)
            printf("%f\n", value.real_value);
        else
            printf("%d\n", value.integer_value);

        for(size_t i = 0; i < tokens->length; i++){
            token_t* token = (token_t*) get_from_list(tokens, i)->data;
            free(token->characters);
        }

        destroy_list(tokens);    
    }
    return 0;
}
