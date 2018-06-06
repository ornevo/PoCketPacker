/*
 * Created by Or Nevo Michrowski
 * Description:
 *  This file is the implementation of the Allocator class, which handles adding sections and
 *      segments to the ELF file.
 */

#include "Allocator.h"
#include "../Helpers/Globals.h"


using namespace std;


/* PUBLIC FUNCTIONS */

ELFIO::section * Allocator::addPackedSec(unsigned long packedCodeSize) {
    ELFIO::section *ret = addSection(STORED_PACKED_CODE_SECTION_NAME, packedCodeSize, packedCodeSize,
                                     SHF_WRITE | SHF_ALLOC, 0x4);
    return ret;
}

ELFIO::section * Allocator::addLoaderSec(unsigned long size) {
    ELFIO::section *ret = addSection(LOADER_CODE_SECTION_NAME, size, size,
                                     SHF_ALLOC | SHF_EXECINSTR, 0x1000);
    return ret;
}


/* PRIVATE FUNCTIONS */

ELFIO::section *
Allocator::addSection(string sectionName, unsigned long secSizeInFile, unsigned long secSizeInMem,
                      ELFIO::Elf_Word permissions, ELFIO::Elf_Xword align) {
    ELFIO::section *ret = Globals::elf->sections.add(sectionName);


    // This means that the section holds information defined by the program, with no
    //  specific meaning.
    ret->set_type(SHT_PROGBITS);

    // Permissions
    ret->set_flags(permissions);

    // Align
    ret->set_addr_align(align);

    // Find a segment with the proper permissions for the section
    ELFIO::segment *seg = Utility::getSegmentByPermissions(permissions);

    // If there is no existing segment with the requested permissions, create one
    if (seg == nullptr)
        seg = addSegment(secSizeInFile, secSizeInMem, permissions, align).get();

    ret->set_address(getNextAvailableSecAddress(seg));

    // Add the section to the segment
    seg->add_section_index(ret->get_index(), ret->get_addr_align());
    seg->set_file_size(seg->get_file_size() + secSizeInFile);
    seg->set_memory_size(seg->get_memory_size() + secSizeInMem);

    return ret;
}

ELFIO::Elf64_Addr Allocator::getNextAvailableSecAddress(ELFIO::segment *containingSeg) {
    // Finds the last section in that segment
    ELFIO::Elf64_Addr ret = containingSeg->get_virtual_address();
    ELFIO::Elf64_Addr segEndAddr = containingSeg->get_virtual_address() + containingSeg->get_memory_size();

    for_each(Globals::elf->sections.begin(), Globals::elf->sections.end(), [&](ELFIO::section *curr) {
        if(curr->get_address() < segEndAddr && curr->get_address() >= ret)
            ret = curr->get_address() + curr->get_size();
    });

    return ret;
}

shared_ptr<ELFIO::segment> Allocator::addSegment(unsigned long segSizeInFile, unsigned long segSizeInMem,
                                                 ELFIO::Elf_Word permissions, ELFIO::Elf_Xword align) {
    shared_ptr<ELFIO::segment> ret(Globals::elf->segments.add());

    // Segment type is loadable. It will actually get loaded to process memory in runtime
    ret->set_type( PT_LOAD );

    // We write in runtime the code, and we execute it. So it needs all flags on.
    ret->set_flags( permissions );

    ret->set_align( align );

    // Set the address to the next available address
    ret->set_virtual_address(getValidSegmentAddr(ret->get_align()) );
    ret->set_physical_address(ret->get_virtual_address());

    // It holds no data in the file
    ret->set_file_size( segSizeInFile );

    // In the memory it will occupy enough memory for the extracted code
    ret->set_memory_size( segSizeInMem );

    return ret;
}

ELFIO::Elf64_Addr Allocator::getValidSegmentAddr(ELFIO::Elf_Xword alignment) {
    // We need to specify an address for the new segment,
    // so that it won't overlap with other loadable segments.
    ELFIO::Elf64_Addr nextFreeVAddr = 0;

    // For each segment, if the currently free address is taken by it, increase the free address to be after it.
    for_each(Globals::elf->segments.begin(), Globals::elf->segments.end(), [&](ELFIO:: segment *current) {
        if(current->get_type() == PT_LOAD && current->get_virtual_address() >= nextFreeVAddr)
            nextFreeVAddr = current->get_virtual_address() + current->get_memory_size();
    });

    // If not aligned properly, increase to the next aligned address
    if(nextFreeVAddr % alignment)
        nextFreeVAddr += alignment - (nextFreeVAddr % alignment);

    return nextFreeVAddr;
}
