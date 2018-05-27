//
// Created by ororor012 on 04/05/18.
//

#ifndef FINALPACKERTRY_CODEALLOCATOR_H
#define FINALPACKERTRY_CODEALLOCATOR_H


#include "../include/elfio/elfio.hpp"
#include "../include/elfio/elfio_section.hpp"
#include "../include/elfio/elfio_segment.hpp"
#include "./Globals.h"
#include <memory>
#include <cstdint>

/* Code patching constants */
// 1566 is the loader code size, 32 is the extra manually added code
#define LOADER_CODE_SIZE 1566 + 32

// Instruction to move value into RDI register. The value should be a 64 bit value concatenated to this instruction.
// For example, 'mov rdi, 0x90901515deadbeef' will be 0x48 0xbf 0xef 0xbe 0xad 0xde 0x15 0x15 0x90 0x90
#define MOV_TO_RDI_INSTRUCTION 0xbf48

// Same goes here as with RDI
#define MOV_TO_RSI_INSTRUCTION 0xbe48

// Same format. Just append the 64 bit value to move.
#define MOV_TO_RAX_INSTRUCTION 0xb848

// Needs no appending.
#define JMP_TO_RAX_INSTRUCTION 0xe0ff


using namespace std;


class CodePatcher {
public:
    /*
     * Writes the loader to the entrypoint.
     * parameter loaderCodeSection: the section to which we should write the code
     * parameter packedCodeSection: the section that contains the packed code.
     */
    static void
    writeLoader(ELFIO::section *loaderCodeSection, ELFIO::section *packedCodeSection, ELFIO::Elf64_Addr textSecAddr);

};


#endif //FINALPACKERTRY_CODEALLOCATOR_H
