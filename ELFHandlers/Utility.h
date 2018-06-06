/*
 * Created by Or Nevo Michrowski
 * Description:
 *  This file is the definition of the Utility class, which contains some general functions
 *      that are used by a few different classes
 */

#ifndef FINALPACKERTRY_HELPERS_H
#define FINALPACKERTRY_HELPERS_H

#include "../include/elfio/elfio.hpp"
#include "../Helpers/Globals.h"
#include "../Helpers/Constants.h"


class Utility {
public:
    /**
     * Tries to find a segment with matching permissions.
     * "Matching permissions" means "segPermissions" >= "requestedPermissions". It means that
     *       the segment has AT LEAST the requested permissions
     * @param perms are the requested permissions for the segment, ORed together
     * @return the first segment matching the requested permissions, or nullptr if not found.
     */
    static ELFIO::segment *getSegmentByPermissions(ELFIO::Elf_Word perms);

    /**
     * @return the first executable segment found in the provided ELF.
     */
    static ELFIO::segment *getExecutableSegment();

    /**
     * @return the ELF's section called '.text', where the code should be
     */
    static ELFIO::section *getTextSection();

    /**
     * @param val an unsigned numeric value
     * @return a string representing the hex of val
     */
    static string intToHex(unsigned long val);
};


#endif //FINALPACKERTRY_HELPERS_H
