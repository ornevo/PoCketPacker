//
// Created by ororor012 on 04/05/18.
//

#ifndef FINALPACKERTRY_GLOBALS_H
#define FINALPACKERTRY_GLOBALS_H

#define TMP_FILE_PATH "/tmp/pocketpacker.tmp"


#include <memory>
#include "../include/elfio/elfio.hpp"


class Globals {
public:
    static std::shared_ptr<ELFIO::elfio> elf;

    // param trunc indicates whether to truncate the file's
    //  previous content
    static std::fstream& getTmpFile(bool trunc=true);
};


#endif //FINALPACKERTRY_GLOBALS_H
