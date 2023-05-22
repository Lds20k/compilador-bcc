#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MEMORY_SIZE 256
#define FILE_ARGUMENT_MANDATORY "File argument is mandatory!"
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

bool load_file(const char *file, uint8_t* accumulator_register, uint8_t* program_counter, uint8_t *memory){
    FILE *file_pointer = fopen(file, "rb");
    if (file_pointer == NULL){
        return 1;
    }

    fseek(file_pointer, 0, SEEK_END);
    long file_length = ftell(file_pointer);
    if (file_length > MEMORY_SIZE){
        return 1;
    }
    fseek(file_pointer, 0, SEEK_SET);

    int8_t byte = 0;
    int index = 0;
    while ((byte = fgetc(file_pointer)) != EOF){
        if (index < 3){
            if(index == 0 && byte != FILE_HEADER) return 1;
            if(index == 1) *accumulator_register = byte;
            if(index == 2) *program_counter = byte;
        }else{
            memory[index - 3] = byte;
        }
        index++;
    }

    fclose(file_pointer);
}

int main(int argc, char const *argv[])
{
    if (argc < 2){
        printf(FILE_ARGUMENT_MANDATORY);
        return 1;
    }

    uint8_t accumulator_register = 0;
    uint8_t program_counter = 0;
    bool state_register = 0;
    bool quit = 0;
    uint8_t memory[MEMORY_SIZE] = {0};

    load_file(argv[1], &accumulator_register, &program_counter, memory);

    for (;program_counter < MEMORY_SIZE && !quit; program_counter++){
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
            if(state_register){
                program_counter++;
                program_counter = memory[program_counter];
            }
            break;
        case JZ:
            if(!state_register){
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
    }
    printf("\n");
    printf("accumulator register: %d\n", accumulator_register);
    printf("program counter: %d\n", program_counter);
    printf("flag N: %d\n", state_register);
    printf("flag Z: %d\n", !state_register);
    printf("----------------------------memory-----------------------------\n");
    for (int i = 1; i <= MEMORY_SIZE; i++){
        printf("%03d ", memory[i - 1]);
        if (i != 0 && i % 16 == 0) printf("\n");
    }
    printf("---------------------------------------------------------------\n");

    return 0;
}
