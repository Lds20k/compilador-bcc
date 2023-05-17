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

bool operation_erro = false;

typedef struct token_s{
    char* name;
    char* characters;
} token_t;

typedef union number_u{
    int integer_value;
    float real_value;
} number_t;

typedef struct value_s{
    bool real;
    number_t number;
} value_t;

bool is_operation(const char* character){
    return !strcmp(character, SUM) ||
           !strcmp(character, SUBTRACTION) ||
           !strcmp(character, MULTIPLICATION) ||
           !strcmp(character, DIVISION);
}

bool is_number(const char* character){
    return *character > 47 && *character < 58;
}

value_t number_sum(value_t a, value_t b){
    value_t value = {0};

    if(a.real && b.real){
        value.real = true;
        value.number.real_value = a.number.real_value + b.number.real_value;
    }else if (a.real || b.real){
        value.real = true;
        if(a.real)
            value.number.real_value = a.number.real_value + (float) b.number.integer_value;
        else
            value.number.real_value = (float) a.number.integer_value + b.number.real_value;
    }else{
        value.real = false;
        value.number.integer_value = a.number.integer_value + b.number.integer_value;
    }

    return value;    
}

value_t number_subtraction(value_t a, value_t b){
    value_t value = {0};

    if(a.real && b.real){
        value.real = true;
        value.number.real_value = a.number.real_value - b.number.real_value;
    }else if (a.real || b.real){
        value.real = true;
        if(a.real)
            value.number.real_value = a.number.real_value - (float) b.number.integer_value;
        else
            value.number.real_value = (float) a.number.integer_value - b.number.real_value;
    }else{
        value.real = false;
        value.number.integer_value = a.number.integer_value - b.number.integer_value;
    }

    return value;    
}

value_t number_multiplication(value_t a, value_t b){
    value_t value = {0};

    if(a.real && b.real){
        value.real = true;
        value.number.real_value = a.number.real_value * b.number.real_value;
    }else if (a.real || b.real){
        value.real = true;
        if(a.real)
            value.number.real_value = a.number.real_value * (float) b.number.integer_value;
        else
            value.number.real_value = (float) a.number.integer_value * b.number.real_value;
    }else{
        value.real = false;
        value.number.integer_value = a.number.integer_value * b.number.integer_value;
    }

    return value;    
}

value_t number_division(value_t a, value_t b){
    value_t value = {0};

    if(a.real && b.real){
        value.real = true;
        value.number.real_value = a.number.real_value / b.number.real_value;
    }else if (a.real || b.real){
        value.real = true;
        if(a.real)
            value.number.real_value = a.number.real_value / (float) b.number.integer_value;
        else
            value.number.real_value = (float) a.number.integer_value / b.number.real_value;
    }else{
        value.real = true;
        value.number.real_value = (float) a.number.integer_value / (float) b.number.integer_value;
    }

    return value;    
}

value_t parse_number(token_t* token){
    value_t value = {0};
    
    if(strcmp(token->name, INTEGER_NUMBER) == 0){
        value.number.integer_value = strtol(token->characters, NULL, 10);
        value.real = false;
    }

    if(strcmp(token->name, REAL_NUMBER) == 0){
        value.number.real_value = strtof(token->characters, NULL);
        value.real = true;
    }

    return value;
}

value_t parse_term(list_t* tokens, size_t index){
    size_t current_index = index;
    token_t* token = (token_t*) get_from_list(tokens, current_index)->data;
    while (!strcmp(token->name, SPACE)){
        current_index++;
        token = (token_t*) get_from_list(tokens, current_index)->data;
    }

    if (strcmp(token->name, INTEGER_NUMBER) && strcmp(token->name, REAL_NUMBER)){
        printf("Unexpected token %s of value \"%s\" in column %d, number token is expected\n", token->name, token->characters, current_index + 1);
        operation_erro = true;
        return (value_t){0};
    }

    current_index++;
    data_t* data = get_from_list(tokens, current_index);
    if (data == NULL) return parse_number(token);

    token_t* next_token = (token_t*) data->data;    
    while (!strcmp(next_token->name, SPACE)){
        current_index++;
        next_token = (token_t*) get_from_list(tokens, current_index)->data;
    }

    value_t number = parse_number(token);
    value_t next_number = parse_term(tokens, current_index + 1);

    

    if(!strcmp(next_token->name, SUM))
        return number_sum(number, next_number);
    else if(!strcmp(next_token->name, SUBTRACTION))
        return number_subtraction(number, next_number);
    else if(!strcmp(next_token->name, MULTIPLICATION))
        return number_multiplication(number, next_number);
    else if(!strcmp(next_token->name, DIVISION)){
        if (!next_number.number.integer_value || !next_number.number.real_value){
            printf("Division by zero is not a valid operation\n");
            operation_erro = true;
            return (value_t){0};
        }
        return number_division(number, next_number);
    }
    else {
        printf("Unexpected token %s of value \"%s\" in column %d, operation token is expected\n", next_token->name, next_token->characters, current_index + 1);
        operation_erro = true;
        return (value_t){0};
    }
}

value_t parse_expression(list_t* list){
    value_t value = {0};
    if (list->length < 1) return value;

    return parse_term(list, 0);
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
        printf(">> ");
        fgets(data, 256, stdin);
        data[strcspn(data, "\n")] = 0;
        if(!strcmp(data, "quit")){
            quit = true;
            continue;
        }
        
        list_t* tokens = NULL;

        if((tokens = tokenization(data)) == NULL) return -1;
        
        value_t value = parse_expression(tokens);

        if(!operation_erro)
            if (value.real)
                printf("%f\n", value.number.real_value);
            else
                printf("%d\n", value.number.integer_value);
        else
            operation_erro = false;

        for(size_t i = 0; i < tokens->length; i++){
            token_t* token = (token_t*) get_from_list(tokens, i)->data;
            free(token->characters);
        }

        destroy_list(tokens);    
    }
    return 0;
}
