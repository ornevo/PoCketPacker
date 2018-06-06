/*
 * Created by Or Nevo Michrowski
 * Description:
 *  This file variables that are used across several classes
 */

#include "./Globals.h"


using namespace std;


shared_ptr<ELFIO::elfio> Globals::elf((ELFIO::elfio*) new ELFIO::elfio());