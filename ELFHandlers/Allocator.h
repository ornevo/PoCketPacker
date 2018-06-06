/*
 * Created by Or Nevo Michrowski
 * Description:
 *  This file is the definition of the Allocator class, which handles adding sections and
 *      segments to the ELF file.
 */

#ifndef FINALPACKERTRY_ALLOCATOR_H
#define FINALPACKERTRY_ALLOCATOR_H


#include "../include/elfio/elfio.hpp"
#include "../include/elfio/elfio_section.hpp"
#include "../include/elfio/elfio_segment.hpp"
#include <memory>
#include "./Utility.h"

#define STORED_PACKED_CODE_SECTION_NAME ".pckdtext"
#define LOADER_CODE_SECTION_NAME ".unpacktext"


using namespace std;


class Allocator {
private:
    /**
     * @param alignment is the requested alignment of the address
     * @return the address of the first free address in which we could add a new segment
     */
    static ELFIO::Elf64_Addr getValidSegmentAddr(ELFIO::Elf_Xword alignment);

    /**
     * Finds the address in which the last section in containingSeg ends.
     * @param containingSeg is the segment in which we find the address
     * @return the address in which the last section in containingSeg ends.
     */
    static ELFIO::Elf64_Addr getNextAvailableSecAddress(ELFIO::segment *containingSeg);

    /**
     * Adds a general-use segment to the elf
     * @param segSizeInFile is the amount of bytes the segment's data will take in the ELF object file
     * @param segSizeInMem is the amount of bytes the segment will require in memory, in runtime
     * @param permissions are the permissions (Read/Write/Execute) the segment should have, ORed together
     * @param align is an address align for the segment's virtual address
     * @return the newly created segment
     */
    static shared_ptr<ELFIO::segment>
    addSegment(unsigned long segSizeInFile, unsigned long segSizeInMem, ELFIO::Elf_Word permissions,
               ELFIO::Elf_Xword align);

    /**
     * Adds a general-use section
     * Tries to find a segment with matching permissions, and if there isn't one creates one.
     * "Matching permissions" means "segPermissions" >= "requestedPermissions". It means that
     *       the segment has AT LEAST the requested permissions
     * @param sectionName is the name for the newly created section
     * @param secSizeInFile is the amount of bytes the segment's data will take in the ELF object file
     * @param secSizeInMem is the amount of bytes the segment will require in memory, in runtime
     * @param permissions are the permissions (Read/Write/Execute) the segment should have, ORed together
     * @param align is an address align for the segment's virtual address
     * @return the newly created section
     */
    static ELFIO::section *
    addSection(string sectionName,
               unsigned long secSizeInFile, unsigned long secSizeInMem,
               ELFIO::Elf_Word permissions, ELFIO::Elf_Xword align);

public:
    /**
     * Adds a section which will contain the packed code to the data segment.
     * @param packedCodeSize is the size of the packed code
     * @return the added section
     */
    static ELFIO::section * addPackedSec(unsigned long packedCodeSize);

    /**
     * Adds a section to the text segment, which will contain the unpacking code.
     * @param size is the size of the loader code
     * @return the added section
     */
    static ELFIO::section * addLoaderSec(unsigned long size);

};


#endif //FINALPACKERTRY_ALLOCATOR_H
