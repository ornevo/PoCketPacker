//
// Created by ororor012 on 11/05/18.
//

#include "./Globals.h"


using namespace std;


std::shared_ptr<ELFIO::elfio> Globals::elf((ELFIO::elfio*) new ELFIO::elfio());

fstream& Globals::getTmpFile(bool trunc) {
    static fstream tmpFile;

    if(tmpFile.is_open()){
        // If the file is already open and no truncation is required, just return it.
        // If we want to truncate, close it for reopening.
        if(trunc)
            tmpFile.close(); // for reopen, to truncate
        else {
            tmpFile.seekg(0, ios_base::beg);
            return tmpFile;
        }
    }

    // If not open yet, open it
    tmpFile.open(TMP_FILE_PATH,
                 fstream::in | fstream::out | (trunc ? fstream::trunc : fstream::app) | fstream::binary);

    if (!tmpFile.good()) {
        cout << "ERROR opening tmp file for read/write." << endl;
        exit(1);
    }

    tmpFile.seekg(0, ios_base::beg);
    return tmpFile;
}
