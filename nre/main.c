#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MEMORY_SIZE 256
#define FILE_ARGUMENT_MANDATORY "File argument is mandatory!\n"
#define FILE_INVALID "File is invalid!\n"
#define FILE_HEADER 42

enum instruction{
    NOP = 0,
    STA = 16,
    LDA = 32,
    ADD = 48,
    OR = 64,
    AND = 80,
    NOT = 96,
    JMP = 128,
    JN = 144,
    JZ = 160,
    OUT = 172,
    HLZ = 240
};

enum state{
    ZERO = 0,
    NEGATIVE = 1,
    NO_STATE = 3
};

int set_state_register(uint8_t accumulator_register){
    if(accumulator_register == 0) return ZERO;
    if(accumulator_register > 127) return NEGATIVE;
    return NO_STATE;
}

bool load_file(const char *file, uint8_t* accumulator_register, uint8_t* program_counter, uint8_t* state_register, uint8_t *memory){
    FILE *file_pointer = fopen(file, "rb");
    if (file_pointer == NULL){
        return 1;
    }

    fseek(file_pointer, 0, SEEK_END);
    long file_length = ftell(file_pointer);
    if (file_length > MEMORY_SIZE + 3){
        return 1;
    }
    fseek(file_pointer, 0, SEEK_SET);
    if (file_length < 4){
        return 1;
    }
    

    int8_t byte = 0;
    int index = 0;
    while ((byte = fgetc(file_pointer)) != EOF){
        if (index < 3){
            if(index == 0 && byte != FILE_HEADER) return 1;
            if(index == 1){
                *accumulator_register = byte;
                *state_register = set_state_register(*accumulator_register);
            }
            if(index == 2) *program_counter = byte;
        }else{
            memory[index - 3] = byte;
        }
        index++;
    }

    fclose(file_pointer);
    return 0;
}

int main(int argc, char const *argv[])
{
    if (argc < 2){
        printf(FILE_ARGUMENT_MANDATORY);
        return 1;
    }

    uint8_t accumulator_register = 0;
    uint8_t program_counter = 0;
    uint8_t state_register = 0;
    bool quit = 0;
    uint8_t memory[MEMORY_SIZE] = {0};

    if(load_file(argv[1], &accumulator_register, &program_counter, &state_register, memory)){
        printf(FILE_INVALID);
        return 1;
    }

    for (;!quit; program_counter++){
        switch (memory[program_counter]){
        case STA:
            program_counter++;
            memory[program_counter] = accumulator_register;
            break;
        case LDA:
            program_counter++;
            accumulator_register = memory[program_counter];
            break;
        case ADD:
            program_counter++;
            accumulator_register += memory[program_counter];
            break;
        case OR:
            program_counter++;
            accumulator_register |= memory[program_counter];
            break;
        case AND:
            program_counter++;
            accumulator_register &= memory[program_counter];
            break;
        case NOT:
            program_counter++;
            accumulator_register = ~accumulator_register;
            break;
        case JMP:
            program_counter++;
            program_counter = memory[program_counter];
            break;
        case JN:
            if(state_register == NEGATIVE){
                program_counter++;
                program_counter = memory[program_counter];
            }
            break;
        case JZ:
            if(state_register == ZERO){
                program_counter++;
                program_counter = memory[program_counter];
            }
            break;
        case OUT:
            printf("%c", accumulator_register);
            break;    
        case HLZ:
            quit = true;
            break;
        default:
            break;
        }

        state_register = set_state_register(accumulator_register);
        quit = program_counter + 1 > MEMORY_SIZE || quit;
    }
    printf("\n");
    printf("----------------------------report-----------------------------\n");
    printf("accumulator register: %d\n", accumulator_register);
    printf("program counter: %d\n", program_counter);
    printf("flag N: %d\n", state_register == NEGATIVE);
    printf("flag Z: %d\n", state_register == ZERO);
    printf("----------------------------memory-----------------------------\n");
    for (int i = 1; i <= MEMORY_SIZE; i++){
        printf("%03d ", memory[i - 1]);
        if (i != 0 && i % 16 == 0) printf("\n");
    }
    printf("---------------------------------------------------------------\n");

    return 0;
}
