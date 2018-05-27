//
// Created by ororor012 on 04/05/18.
//

#include "CodePatcher.h"


using namespace std;


void CodePatcher::writeLoader(ELFIO::section *loaderCodeSection, ELFIO::section *packedCodeSection, ELFIO::Elf64_Addr textSecAddr) {
    // First write the
    unique_ptr<char[]> code(new char[LOADER_CODE_SIZE]);
    unsigned long offset = 0;

    /* The code structure is:
        1. Set predefined registers to the needed input addresses for the loader's work:
            I. the destination address in rdi
            II. the compressed code address in rsi
        2. The loader code
        3. Code jumping to the new extracted code
    */
    // 1. I. Write RDI
    *(uint_least16_t*)(code.get() + offset) = MOV_TO_RDI_INSTRUCTION;
    offset += sizeof(uint_least16_t);
    *(uint64_t*)(code.get() + offset) = textSecAddr;
    offset += sizeof(uint64_t);

    // 1. II. Write RSI
    *(uint_least16_t*)(code.get() + offset) = MOV_TO_RSI_INSTRUCTION;
    offset += sizeof(uint_least16_t);
    *(uint64_t*)(code.get() + offset) = packedCodeSection->get_address();
    offset += sizeof(uint64_t);

    // 2. Write the loader's code itself
    ifstream unpackerCodeFile("./LoadersCode/packerLoader.bin", ios::binary | ios::ate);

    int unpackerCodeSize = (int)unpackerCodeFile.tellg();
    unpackerCodeFile.seekg(0, ios::beg);
    unpackerCodeFile.read(code.get() + offset, unpackerCodeSize);
    unpackerCodeFile.close();
    offset += unpackerCodeSize;

    // 3. Write code jumping to the new, unpacked code
    // Move the jump address to rax
    *(uint_least16_t*)(code.get() + offset) = MOV_TO_RAX_INSTRUCTION;
    offset += sizeof(uint_least16_t);
    *(uint64_t*)(code.get() + offset) = textSecAddr;
    offset += sizeof(uint64_t);
    // jmp rax
    *(uint_least16_t*)(code.get() + offset) = JMP_TO_RAX_INSTRUCTION;
    offset += sizeof(uint_least16_t);

    // Write to the section
    loaderCodeSection->set_data(code.get(), (ELFIO::Elf_Word)offset);

    // And finally, redirect the entrypoint
    Globals::elf->set_entry(loaderCodeSection->get_address());
}
