/*
 * Created by Or Nevo Michrowski
 * Description:
 *  This file is the definition of the CodePatcher class, which writes the loader's code
 *      to the ELF's new entrypoint.
 *  It also defines as constants the assembly opcodes of different instructions which gets
 *      written to the loader's code dynamically by the CodePatcher.
 */

#ifndef FINALPACKERTRY_CODEALLOCATOR_H
#define FINALPACKERTRY_CODEALLOCATOR_H


#include "../include/elfio/elfio.hpp"
#include "../include/elfio/elfio_section.hpp"
#include "../include/elfio/elfio_segment.hpp"
#include "../Helpers/Globals.h"
#include <memory>
#include <cstdint>

/* Code patching constants */
// The size of the unpacker code present in packerLoader.bin, without the initialization or trailing added code
#define UNPACKER_CODE_SIZE 1566
// The size of the code added before the unpacker and after it
#define HEADER_AND_TRAILER_CODE_SIZE 86
// The size of the whole patched loader code.
#define LOADER_CODE_SIZE UNPACKER_CODE_SIZE + HEADER_AND_TRAILER_CODE_SIZE

// Instruction to move value into RDI register. The value should be a 64 bit value concatenated to this instruction.
// For example, 'mov rdi, 0x90901515deadbeef' will be 0x48 0xbf 0xef 0xbe 0xad 0xde 0x15 0x15 0x90 0x90
#define MOV_TO_RDI_INSTRUCTION 0xbf48

// Same goes here as with RDI
#define MOV_TO_RSI_INSTRUCTION 0xbe48

// Just to backup RSP before loader execution
#define MOV_RSP_TO_R10 0xe28949
#define MOV_R10_TO_RSP 0xd4894c
#define MOV_RSP_R10_INST_SIZE 3

// Needs no appending.
#define JMP_TO_RSI_INSTRUCTION 0xe6ff

// The size of the local buffer from which the data is being copied to the unpacking code.
#define LOCAL_BUFF_SIZE 100

#define UNPACKER_CODE_FILE_PATH "./LoadersCode/packerLoader.bin"

// Push and pop instructions for all registers
#define STACK_BUCKUP_ARRS_LEN 24


const unsigned char PUSHA_CODE[] = {0x54, 0x50, 0x53, 0x51, 0x52, 0x56, 0x57, 0x55, 0x41, 0x50, 0x41, 0x51, 0x41, 0x52, 0x41, 0x53, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57};
const unsigned char POPA_CODE[]  = {0x41, 0x5f, 0x41, 0x5e, 0x41, 0x5d, 0x41, 0x5c, 0x41, 0x5b, 0x41, 0x5a, 0x41, 0x59, 0x41, 0x58, 0x5d, 0x5f, 0x5e, 0x5a, 0x59, 0x5b, 0x58, 0x5c};


using namespace std;


class CodePatcher {
private:
    // The buffer to which the code will be stored before writing to the ELF's memory
    unique_ptr<char[]> code;
    // The current write offset into the code buffer
    unsigned long offset;
    // A buffer from which data will be copied to the code buffer. Needed for write helper methods
    // It's a 'transfer station' before copying to the code buffer.
    unique_ptr<char[]> localbuff;

    /**
     * This method opens the loader .bin file, and writes its code to the code buffer
     */
    void writeUnpackerLoader();

    /**
     * Writes datalen bytes from the localbuffer to the code buffer. Updates offset
     * @param datalen is the number of bytes to copy
     */
    void writeData(int datalen);

    /**
     * Writes datalen bytes from the origin to the code buffer. Updates offset
     * @param datalen is the number of bytes to copy
     * @param origin is the buffer from which the coping is done
     */
    void writeData(int datalen, const char *origin);

    /**
     * Writes a mov instruction to the code
     * @param movInstPrefix is the constant of the relevent MOV instruction. e.g. MOV_TO_RSI_INSTRUCTION
     * @param valueOperand is the value to move into the register
     */
    void writeMovInstruction(unsigned short movInstPrefix, unsigned long valueOperand);

    /**
     * Simply logs an error message and exits.
     */
    void emitBufferOverflowError();

public:
    CodePatcher();

    /**
     * Writes the loader to the entry-point.
     * @param loaderCodeSection is the section to which we should write the code.
     * @param packedCodeSection is the section that contains the packed code.
     * @param textSecAddr is the virtual address of the .text section
     */
    void
    writeLoader(ELFIO::section *loaderCodeSection, ELFIO::section *packedCodeSection, ELFIO::Elf64_Addr textSecAddr);

};


#endif //FINALPACKERTRY_CODEALLOCATOR_H
