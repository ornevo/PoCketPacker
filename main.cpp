/*
 * Created by Or Nevo Michrowski
 * Description:
 *  This file contains the main function, which contains the top-level flow of the packing
 *      process and calls top-level functions.
 */

#include <iostream>
#include "Helpers/Globals.h"
#include "Helpers/Logger.h"
#include "ELFHandlers/Utility.h"
#include "Processors/Compressor.h"
#include "ELFHandlers/Allocator.h"
#include "ELFHandlers/CodePatcher.h"

#define PACKED_FILE_EXTENSION ".pck"

#define DEBUG

using namespace std;


/**
 * Initializes the passed file in the arguments and validates it.
 * @param argc the argc which was passed to main(...)
 * @param argv the argv which was passed to main(...)
 */
void initElfio(int argc, char** argv);


int main(int argc, char** argv) {
    // Init log mask
    Logger::logLevelMask = LOG_LEVEL_INFO | LOG_LEVEL_ERROR;
#ifdef DEBUG
    Logger::logLevelMask |= LOG_LEVEL_DEBUG;
#endif

    Logger::log("initializing...");
    initElfio(argc, argv);
    Logger::log("done initializing.");

    // Compress the code in the .text section
    Logger::log("compressing code...");
    vector<char> compressedCode = Compressor::compressCode();
    unsigned long originalCodeSize = *(unsigned long*)(compressedCode.data() + 1);  // According to the header
    Logger::log("compression done.");

    // Create a read only section for the packed code bytes
    Logger::log("adding a section to store the packed code...");
    ELFIO::section *packedCodeSec = Allocator::addPackedSec(compressedCode.size());

    // Write the compressed code to its designated section
    packedCodeSec->set_data(compressedCode.data(), (unsigned int)compressedCode.size());

    // Make the text segment writable. Since the packed code is not necessarily location independent,
    //  we need to unpack it back to it's original address. So we must make the .text segment writable.
    Logger::log("making executable section writable...");
    Utility::getExecutableSegment()->set_flags(SHF_ALLOC | SHF_WRITE | SHF_EXECINSTR);

    // Create the loader code section
    Logger::log("adding code section for the loader's code...");
    ELFIO::section *loaderCodeSection = Allocator::addLoaderSec(LOADER_CODE_SIZE);

    ELFIO::section *textSection = Utility::getTextSection();
    // Override old code from text section
    // Since the elfio library is stupid, we need to allocate a hell lot of 0s for it
    //  to copy into section memory
    char *tmpZeros = new char[textSection->get_size()]();
    textSection->set_data(tmpZeros, textSection->get_size());
    delete[] tmpZeros;

    Logger::log("writing loader code to the entrypoint...");
    // Write the new entry point, decompression code.
    CodePatcher patcher = CodePatcher();
    patcher.writeLoader(loaderCodeSection, packedCodeSec, textSection->get_address());

    // Save the elf
    Logger::log("saving result...");
    Globals::elf->save(string(argv[argc - 1]) + PACKED_FILE_EXTENSION);
    // If caused a file saving error
    if(errno)
        Logger::throwErr(strerror(errno));

    return 0;
}

void initElfio(int argc, char** argv) {
    // Check for the file's validity
    if(argc < 2)
        Logger::throwErr("Not enough arguments supplied (" + to_string(argc) +
                         "). Please specify a filename to compress");

    string filename = argv[argc - 1];

    // Validate elf properties
    // Try opening and parsing
    if( !Globals::elf->load(filename) )
        Logger::throwErr("Opening \"" + filename + "\" or parsing its content.");
    // File properties checks
    // Check if x64 intel
    else if( Globals::elf->get_machine() != EM_X86_64 )
        Logger::throwErr("ELF's designated machine must be x86_64.");
    // Executable, not relocatable
    else if( Globals::elf->get_type() != ET_EXEC )
        Logger::throwErr("ELF file must be executable (e.g. not relocatable or shared).");
    // 64 bit
    else if( Globals::elf->get_class() != ELFCLASS64 )
        Logger::throwErr("ELF file must be 64 bit.");
    // Little endian
    else if( Globals::elf->get_encoding() != ELFDATA2LSB )
        Logger::throwErr("ELF file must be encoded as little endian.");
}