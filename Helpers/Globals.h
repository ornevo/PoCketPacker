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


using namespace std;


class Globals {
public:
    static shared_ptr<ELFIO::elfio> elf;
};


#endif //FINALPACKERTRY_GLOBALS_H
