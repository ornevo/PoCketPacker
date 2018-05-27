//
// Created by ororor012 on 14/03/18.
// This script writes the binary loader functions to individual files. Writes only the machine code of the functions,
//     which will be injected to the code memory space.
//
#include <stdio.h>
#include <stdlib.h>
#include "loaders.h"

#include "./packerLoader.c"

// Prints function which are terminated with nop nop nop nop
void saveFunctionMachineCode(const char* filename, void (*func)()){
    char* readPtr = (char*) func;
    FILE * output = fopen(filename, "w");
    if (!output) {
        fprintf(stderr, "Error opening file \"%s\" for writing.\nBye!\n", filename);
        exit(1);
    }

    while (*((unsigned int *)readPtr) != LOADER_FUNC_TERMINATOR_OPCODE) // While end delimeter not found
        if(fputc(*(readPtr++), output) == EOF){
            fprintf(stderr, "Error writing data to file \"%s\".\nBye!\n", filename);
            exit(1);
        }

    fclose(output);
}

void main(void) {
    saveFunctionMachineCode("packerLoader.bin", packerLoader);
}