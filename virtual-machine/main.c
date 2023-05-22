#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MEMORY_SIZE 256
#define FILE_ARGUMENT_MANDATORY "File argument is mandatory!"

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

bool load_file(const char *file, uint8_t *memory){
    FILE *file_pointer = fopen(file, "rb");
    if (file_pointer == NULL){
        return 1;
    }

    fseek(file_pointer, 0, SEEK_END);
    long file_length = ftell(file_pointer);
    if (file_length > MEMORY_SIZE){
        return 1;
    }

    int8_t byte = 0;
    int index = 0;
    while ((byte = fgetc(file_pointer)) != EOF){
        memory[index] = byte;
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

    bool state_register = 0;

    uint8_t memory[MEMORY_SIZE] = {0};

    load_file(argv[1], memory);

    for (uint8_t program_counter = 0; program_counter < MEMORY_SIZE; program_counter++){
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

            break;
        case OR:

            break;
        case AND:

            break;
        case NOT:

            break;
        case JMP:

            break;
        case JN:

            break;
        case JZ:

            break;
        case OUT:
            printf("")
            break;    
        case HLZ:

            break;
        default:
            break;
        }
    }

    return 0;
}
