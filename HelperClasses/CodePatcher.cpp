//
// Created by ororor012 on 04/05/18.
//

#include "CodePatcher.h"


using namespace std;


CodePatcher::CodePatcher() : code(new char[LOADER_CODE_SIZE]),
                             localbuff(new char[LOCAL_BUFF_SIZE]),
                             offset(0) {}

void CodePatcher::writeLoader(ELFIO::section *loaderCodeSection, ELFIO::section *packedCodeSection, ELFIO::Elf64_Addr textSecAddr) {
    /* The loader's/unpacker's code structure is:
        1. Save the initial environment
            I. Push all registers
            II. Backup rsp in r10, which doesn't change in the loader's code
        2. Set predefined registers to the needed input addresses for the loader's work:
            I. the destination address in rdi
            II. the compressed code address in rsi
        3. The loader code
        4. Restore the initial environment
            I. Restore rsp from r10, where we backed it up in step 1.II.
            II. Pop all registers
        5. Code jumping to the new extracted code
    */

    /* 1. I. Push all registers */
    writeData(STACK_BUCKUP_ARRS_LEN, (const char *)PUSHA_CODE);

    /* 1. II. Buckup current rsp in r10.
     * mov r10, rsp is 3 bytes, but there is no 3 bytes pointer type.
     * So we copy 4 bytes and then write 3
     */
    *(uint32_t *)localbuff.get() = MOV_RSP_TO_R10;
    writeData(MOV_RSP_R10_INST_SIZE);

    /* 2. I. Write RDI */
    writeMovInstruction(MOV_TO_RDI_INSTRUCTION, textSecAddr);

    /* 2. II. Write RSI */
    writeMovInstruction(MOV_TO_RSI_INSTRUCTION, packedCodeSection->get_address());

    /* 3. Write the loader's code itself */
    writeUnpackerLoader();

    /* 4. I. Restore initial environment by re-assigning rsp to its inital value we saved in r10 */
    *(uint32_t *)localbuff.get() = MOV_R10_TO_RSP;
    writeData(MOV_RSP_R10_INST_SIZE);

    /* 4. II. Pop all registers */
    writeData(STACK_BUCKUP_ARRS_LEN, (const char *)POPA_CODE);

    /* 5. Write code jumping to the new, unpacked code */
    // Move the jump address to rsi, since rsi's value gets overridden in the entrypoint
    writeMovInstruction(MOV_TO_RSI_INSTRUCTION, textSecAddr);

    // jmp rsi
    *(uint_least16_t*)localbuff.get() = JMP_TO_RSI_INSTRUCTION;
    writeData(sizeof(uint_least16_t));

    // Make sure no extra uneeded buffer was allocated. Just for sake good programming practices.
    if(offset != LOADER_CODE_SIZE)
        cout << "Warning: The specified loader code size defined in the program is bigger than"
                " the code actually is.\nIf you see this message, letting developers know would be much appreciated." << endl;

    // After writing all the required code, write the code buffer to the ELF's memory
    loaderCodeSection->set_data(code.get(), (ELFIO::Elf_Word)offset);

    // And finally, redirect the entrypoint
    Globals::elf->set_entry(loaderCodeSection->get_address());
}

void CodePatcher::writeData(int datalen) {
    this->writeData(datalen, this->localbuff.get());
}

void CodePatcher::writeData(int datalen, const char *origin) {
    // Check for buffer overflow, probably caused by not updating the loader code's size
    //  constant after changing the code's size.
    if(offset + datalen > LOADER_CODE_SIZE)
        emitBufferOverflowError();

    char *curr = (char *)code.get() + offset;
    for (int i = 0; i < datalen; ++i)
        curr[i] = origin[i];
    offset += datalen;
}

void CodePatcher::writeMovInstruction(unsigned short movInstPrefix, unsigned long valueOperand) {
    *(uint_least16_t*)localbuff.get() = movInstPrefix;
    writeData(sizeof(uint_least16_t));

    *(uint64_t*)localbuff.get() = valueOperand;
    writeData(sizeof(uint_least64_t));

}

void CodePatcher::writeUnpackerLoader() {
    ifstream unpackerCodeFile("./LoadersCode/packerLoader.bin", ios::binary | ios::ate);

    int unpackerCodeSize = (int)unpackerCodeFile.tellg();
    if(unpackerCodeSize != UNPACKER_CODE_SIZE){
        cout << "ERROR: The loader's code size found in packerLoader.bin does not match the one "
             "defined in the program.\nPlease contact the developers for further support." << endl;
        exit(1);
    } else if(offset + unpackerCodeSize > LOADER_CODE_SIZE)
        emitBufferOverflowError();

    unpackerCodeFile.seekg(0, ios::beg);
    unpackerCodeFile.read(code.get() + offset, unpackerCodeSize);
    unpackerCodeFile.close();
    offset += unpackerCodeSize;
}

void CodePatcher::emitBufferOverflowError() {
    cout << "ERROR: Stack overflow detected while writing loader.\n"
            "Please contact the developers for further support." << endl;
    exit(1);
}
