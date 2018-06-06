/*
 * Created by Or Nevo Michrowski
 * Description:
 *  This file variables that are used across several classes
 */

#ifndef FINALPACKERTRY_GLOBALS_H
#define FINALPACKERTRY_GLOBALS_H

#include <memory>
#include "../include/elfio/elfio.hpp"

#define TMP_FILE_PATH "/tmp/pocketpacker.tmp"


class Globals {
public:
    static std::shared_ptr<ELFIO::elfio> elf;
};


#endif //FINALPACKERTRY_GLOBALS_H
