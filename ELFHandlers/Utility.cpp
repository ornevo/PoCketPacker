/*
 * Created by Or Nevo Michrowski
 * Description:
 *  This file is the implementation of the Utility class, which contains some general functions
 *      that are used by a few different classes
 */

#include <sstream>
#include "Utility.h"


using namespace std;


/* PUBLIC FUNCTIONS */

ELFIO::segment *Utility::getExecutableSegment() {
    ELFIO::segment *ret = getSegmentByPermissions(SHF_EXECINSTR);

    if(!ret) throw "ERROR: no executable segment was found";

    return ret;
}

ELFIO::section *Utility::getTextSection() {
    ELFIO::section *ret = nullptr;

    for_each(Globals::elf->sections.begin(), Globals::elf->sections.end(), [&](ELFIO::section *current) {
        if(!ret && current->get_name() == CODE_SECTION_NAME) ret = current;
    });

    if(!ret) throw "ERROR: no executable section was found";

    return ret;
}

ELFIO::segment *Utility::getSegmentByPermissions(ELFIO::Elf_Word perms) {
    ELFIO::segment *ret = nullptr;

    for_each(Globals::elf->segments.begin(), Globals::elf->segments.end(), [&](ELFIO::segment *current) {
        if(!ret && current->get_type() == PT_LOAD && current->get_flags() & perms == perms)
            ret = current;
    });

    return ret;
}

string Utility::intToHex(unsigned long val) {
    stringstream stream;
    stream << hex << val;
    string result( stream.str() );
    return result;
}
