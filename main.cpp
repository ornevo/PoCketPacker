//#include <boost/filesystem.hpp>
#include <iostream>
#include "./HelperClasses/Globals.h"
#include "./HelperClasses/Compressor.h"
#include "./HelperClasses/Allocator.h"
#include "./HelperClasses/CodePatcher.h"

//namespace fs = boost::filesystem;
using namespace std;

/*
 * Initializes the passed file in the arguments.
 * Returns true if file is valid.
 */
bool initElfio(int argc, char** argv);

/*
 * Get .text segment
 * */
ELFIO::segment *getExecutableSegment();

/*
 * Get .text section
 */
ELFIO::section *getTextSection();


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
    getExecutableSegment()->set_flags(SHF_ALLOC | SHF_WRITE | SHF_EXECINSTR);

    // Create the loader code section
    ELFIO::section *loaderCodeSection = Allocator::addLoaderSec(LOADER_CODE_SIZE);

    ELFIO::section *textSection = getTextSection();
    // Override old code from text section
    // Since the elfio library is stupid, we need to allocate a hell lot of 0s for it
    //  to copy into section memory
    char *tmpZeros = new char[textSection->get_size()]();
    textSection->set_data(tmpZeros, textSection->get_size());
    delete[] tmpZeros;

    // Write the new entry point, decompression code.
    CodePatcher::writeLoader(loaderCodeSection, packedCodeSec, textSection->get_address());

    // Save the elf
    Globals::elf->save(string(argv[argc - 1]) + ".pck");

    // TODO: Remove in production
    cout << strerror(errno) << endl;

    return 0;
}

bool initElfio(int argc, char** argv) {
    // Check for the file's validity
    if(argc < 2) {
        cout << "Not enough arguments supplied (" + to_string(argc) +
                "). Please specify a filename to compress" << endl;
        return true;
    }
    //    else if(!fs::exists(argv[argc - 1])) // If the specified file does not exist
    //        cout << "File \"" << argv[argc - 1] << "\" does not exist. Please specify a valid file." << endl;

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

ELFIO::segment *getExecutableSegment() {
    ELFIO::segment *ret = nullptr;

    for_each(Globals::elf->segments.begin(), Globals::elf->segments.end(), [&](ELFIO::segment *current) {
        if(!ret && current->get_type() == PT_LOAD && current->get_flags() & SHF_EXECINSTR) ret = current;
    });

    if(!ret) throw "ERROR: no executable segment was found";

    return ret;
}

ELFIO::section *getTextSection() {
    ELFIO::section *ret = nullptr;

    for_each(Globals::elf->sections.begin(), Globals::elf->sections.end(), [&](ELFIO::section *current) {
        if(!ret && current->get_name() == ".text") ret = current;
    });

    if(!ret) throw "ERROR: no executable segment was found";

    return ret;
}