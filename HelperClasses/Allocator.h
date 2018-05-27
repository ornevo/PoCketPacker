//
// Created by ororor012 on 04/05/18.
//

#ifndef FINALPACKERTRY_ALLOCATOR_H
#define FINALPACKERTRY_ALLOCATOR_H


#include "../include/elfio/elfio.hpp"
#include "../include/elfio/elfio_section.hpp"
#include "../include/elfio/elfio_segment.hpp"
#include <memory>

using namespace std;


class Allocator {
private:
    static ELFIO::Elf64_Addr getValidSegmentAddr(ELFIO::Elf_Xword alignment);
    static ELFIO::Elf64_Addr getNextAvailableSecAddress(ELFIO::segment *containingSeg);

    //Adds a general use segment, with specified size
    static shared_ptr<ELFIO::segment>
    addSegment(unsigned long segSizeInFile, unsigned long segSizeInMem, ELFIO::Elf_Word permissions,
               ELFIO::Elf_Xword align);

    // Adds a general section
    // Tries to find a segment with matching permissions, and if there is none creates one.
    // "Matching permissions" means "segPermissions" >= "requestedPermissions". That the segment
    //      has AT LEAST the requested permissions
    static ELFIO::section *
    addSection(string sectionName,
               unsigned long secSizeInFile, unsigned long secSizeInMem,
               ELFIO::Elf_Word permissions, ELFIO::Elf_Xword align);

public:
    /*
     * Adds a section to the data segment, which will contain the packed code.
     */
    static ELFIO::section * addPackedSec(unsigned long packedCodeSize);

    /*
     * Adds a section to the text segment, which will contain the unpacking code.
     * */
    static ELFIO::section * addLoaderSec(unsigned long size);

};


#endif //FINALPACKERTRY_ALLOCATOR_H
