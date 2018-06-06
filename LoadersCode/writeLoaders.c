/*
 * Created by Or Nevo Michrowski
 * Description:
 *  This script writes the binary loader functions to individual files. Writes only the machine code of the functions,
 *      which will be injected to the code memory space.
 */

#include <stdio.h>
#include <stdlib.h>
#include "loaders.h"

#include "./packerLoader.c"

/**
 * Dumps functions into files, to be later patched into the ELF.
 * The functions should be terminated with LOADER_FUNC_TERMINATE (which is "nop nop nop nop")
 * @param filename is the destination filename, to which the function will be dumped.
 * @param func is a pointer to the function to dump.
 */
void dumpFunctionMachineCode(const char *filename, void (*func)()){
    char* readPtr = (char*) func;
    FILE * output = fopen(filename, "w");
    if (!output) {
        fprintf(stderr, "Error opening file \"%s\" for writing.\nBye!\n", filename);
        exit(1);
    }

    // While end delimeter not found
    while (*((unsigned int *)readPtr) != LOADER_FUNC_TERMINATOR_OPCODE)
        if(fputc(*(readPtr++), output) == EOF){
            fprintf(stderr, "Error writing data to file \"%s\".\nBye!\n", filename);
            exit(1);
        }

    fclose(output);
}

void main(void) {
    dumpFunctionMachineCode("packerLoader.bin", packerLoader);
}