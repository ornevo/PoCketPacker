/*
 * Created by Or Nevo Michrowski
 * Description:
 *  This file contains the main function, which contains the top-level flow of the packing
 *      process and calls top-level functions.
 */

#include <iostream>
#include "Helpers/Globals.h"
#include "ELFHandlers/Utility.h"
#include "Processors/Compressor.h"
#include "ELFHandlers/Allocator.h"
#include "ELFHandlers/CodePatcher.h"

#define PACKED_FILE_EXTENSION ".pck"


using namespace std;


/**
 * Initializes the passed file in the arguments.
 * @param argc the argc which was passed to main(...)
 * @param argv the argv which was passed to main(...)
 * @return true if file is valid.
 */
bool initElfio(int argc, char** argv);


int main(int argc, char** argv) {
    if(!initElfio(argc, argv))
        exit(1);

    // Compress the code in the .text section
    vector<char> compressedCode = Compressor::compressCode();
    unsigned long originalCodeSize = *(unsigned long*)(compressedCode.data() + 1);  // According to the header

    // Create a read only section for the packed code bytes
    ELFIO::section *packedCodeSec = Allocator::addPackedSec(compressedCode.size());

    // Write the compressed code to its designated section
    packedCodeSec->set_data(compressedCode.data(), (unsigned int)compressedCode.size());

    // Make the text segment writable. Since the packed code is not necessarily location independent,
    //  we need to unpack it back to it's original address. So we must make the .text segment writable.
    Utility::getExecutableSegment()->set_flags(SHF_ALLOC | SHF_WRITE | SHF_EXECINSTR);

    // Create the loader code section
    ELFIO::section *loaderCodeSection = Allocator::addLoaderSec(LOADER_CODE_SIZE);

    ELFIO::section *textSection = Utility::getTextSection();
    // Override old code from text section
    // Since the elfio library is stupid, we need to allocate a hell lot of 0s for it
    //  to copy into section memory
    char *tmpZeros = new char[textSection->get_size()]();
    textSection->set_data(tmpZeros, textSection->get_size());
    delete[] tmpZeros;

    // Write the new entry point, decompression code.
    CodePatcher patcher = CodePatcher();
    patcher.writeLoader(loaderCodeSection, packedCodeSec, textSection->get_address());

    // Save the elf
    Globals::elf->save(string(argv[argc - 1]) + PACKED_FILE_EXTENSION);

    // TODO: Remove in production
    cout << strerror(errno) << endl;

    return 0;
}

bool initElfio(int argc, char** argv) {
    // TODO: error
    // Check for the file's validity
    if(argc < 2) {
        cout << "Not enough arguments supplied (" + to_string(argc) +
                "). Please specify a filename to compress" << endl;
        return true;
    }

    string filename = argv[argc - 1];

    // Validate elf properties
    // Try opening and parsing
    if( !Globals::elf->load(filename) )
        cout << "Opening \"" << filename << "\" or parsing its content." << endl;
    // File properties checks
    // Check if x64 intel
    else if( Globals::elf->get_machine() != EM_X86_64 )
        cout << "ELF's designated machine must be x86_64." << endl;
    // Executable, not relocatable
    else if( Globals::elf->get_type() != ET_EXEC )
        cout << "ELF file must be executable (e.g. not relocatable or shared)." << endl;
    // 64 bit
    else if( Globals::elf->get_class() != ELFCLASS64 )
        cout << "ELF file must be 64 bit." << endl;
    // Little endian
    else if( Globals::elf->get_encoding() != ELFDATA2LSB )
        cout << "ELF file must be encoded as little endian." << endl;
    else
        return true;
    return false;
}