/*
 * Created by Or Nevo Michrowski
 * Description:
 *  This file is the implementation of the Compressor class, which implements the
 *      huffman tree compression algorithm and uses it to compress the code of the ELF
 *      for runtime extraction.
 */

#include "Compressor.h"
#include "../Helpers/Globals.h"
#include <climits>

#define DEBUG


using namespace std;


/* PUBLIC FUNCTIONS */

vector<char> Compressor::compressCode() {
    // This function simply calls the compress function. All it does is getting the code from the
    //  elfio object, and transforming the return char* value into the vector

    // Get the code
    ELFIO::section *codeSec = Utility::getTextSection();
    const char* code = codeSec->get_data();
    unsigned long codeSize = codeSec->get_size();

    CompressionRetVal compressedCodeStruct = Compressor::compress(code, codeSize);

    return vector<char>(compressedCodeStruct.compressedCode,
                             compressedCodeStruct.compressedCode + compressedCodeStruct.compressedDataSize);
}


/* PRIVATE FUNCTIONS */

CompressionRetVal Compressor::compress(const char* data, unsigned long dataLen) {
    vector<shared_ptr<TNode>> freqs = dataToFreqMap(data, dataLen); // generate frequencies map from the source file.

    // Since buildTreeFromArr modifies the array and we need it for later header
    //  construction, clone it
    vector<shared_ptr<TNode>> freqsCopyForTree;
    // Copy only used pointers from freqs.
    // freqs hold unused pointers to allow a direct O(1) access to a given character,
    //  but freqsCopy is used to construct the tree, so this property isn't valuable
    for(auto i = freqs.begin(); i < freqs.end(); i++)
        if(i -> use_count())
            freqsCopyForTree.push_back(*i);

    // Sort it, as we need a sorted by frequency list for the tree construction
    sort(freqsCopyForTree.begin(), freqsCopyForTree.end(),
         [](const shared_ptr<TNode> & a, const shared_ptr<TNode> & b) { return (a -> freq) > (b -> freq); });

    // buildTreeFromFreqMap corrupts the vector, but we still need it for later header
    //  construction. Hence we save a copy of it.
    vector<shared_ptr<TNode>> freqsCopyForHeader(freqsCopyForTree);

    shared_ptr<TNode> huffTree = buildTreeFromFreqMap( freqsCopyForTree ); // build a Huffman tree from the map
    setEncodingCodes(huffTree); // Give each leaf an encoding

    /* Start writing of the compressed data */
    fstream &output = getTmpFile();

    // Write frequencies to header
    for(auto i = freqsCopyForHeader.begin(); i < freqsCopyForHeader.end(); i++) {
        output.write(&(i -> get() -> ch), sizeof(char)); // Which character
        output.write((char *)&(i -> get() -> freq), sizeof(unsigned long));
    }

    // Now actually write the compressed data
    unique_ptr<unsigned char> writeBuffer(new unsigned char(0)), writeBufferCount(new unsigned char(0));
    for (int i = 0; i < dataLen; i++) {
        unsigned char currentChar = data[i];
        string charEnc = freqs[currentChar] -> encoding;
        // Add code, as bits
        for (char j : charEnc) {
            *writeBuffer <<= 1;
            *writeBuffer += j - '0';

            // check if filled the buffer
            if(++(*writeBufferCount) == 8){
                output.write((char *)writeBuffer.get(), sizeof(unsigned char));
                *writeBufferCount = 0;
            }
        }
    }
    // write remaining buffer
    *writeBuffer <<= 8 - *writeBufferCount; // Move the bits to the most sigiificant bits
    output.write((char*)writeBuffer.get(), sizeof(unsigned char));

    // Allocate space for returned compressed data.
    // We need to return its size as well, and we will return
    //  it in the first 8 bytes of the returned data
    output.seekg(0, output.end);
    int freqsListSize = freqsCopyForHeader.size()*FREQ_ENTRY_SIZE;
    unsigned long compressedDataSize = output.tellg() - (streamoff)freqsListSize;
    int extraHeaderSize = 2*sizeof(char) + 2*sizeof(unsigned long);
    auto compressedData = new char[extraHeaderSize  + compressedDataSize + freqsListSize];
    auto writePtr = compressedData;

    /* Write header fields */
    *((unsigned char*) writePtr) = *writeBufferCount; // lastByteUsedBits
    writePtr += sizeof(char);
    *((unsigned long*) writePtr) = dataLen; // originalDataSize
    writePtr += sizeof(unsigned long);
    *((unsigned long *) writePtr) = compressedDataSize; // compressDataSize
    writePtr += sizeof(unsigned long);
    *((unsigned char*) writePtr) = freqsCopyForHeader.size(); // numberOfFreqs
    writePtr += sizeof(char);

    // Copy data to allocated space
    output.seekg(0, output.beg);
    output.read(writePtr, compressedDataSize + freqsListSize);

    #ifdef DEBUG

        ofstream tmpdebugfile("./tests/compressedData", ios_base::out | ios_base::binary);
        tmpdebugfile.write(compressedData, (writePtr-compressedData) + compressedDataSize + freqsListSize);
        tmpdebugfile.close();
        ofstream tmpdebugfile2("./tests/decompressedData.orig", ios_base::out | ios_base::binary);
        tmpdebugfile2.write(data, dataLen);
        tmpdebugfile2.close();


    #endif

    return {compressedData, (writePtr-compressedData) + compressedDataSize + freqsListSize};
}

shared_ptr<TNode> Compressor::buildTreeFromFreqMap(vector<shared_ptr<TNode>> vec){
    if(vec.size() == 1) return vec[0]; // If no items, nothing to do

    // find the min two nodes
    auto min = vec.begin();
    for (auto i = vec.begin(); i+1 < vec.end(); i++)
        if((*i)->freq + (*(i+1))->freq < (*min)->freq + (*(min+1))->freq)
            min = i;

    // Create a tree node to be their father
    shared_ptr<TNode> ret(new TNode(
            (*min) -> freq + (*(min + 1)) -> freq,
            *min,
            *(min + 1)
    ));

    // copy the new node to the array, and erase his two sons
    vec.erase(min); // first
    vec.erase(min); // second ( since it moved one place back in the last erase )
    vec.insert(min, ret); // insert their father

    return buildTreeFromFreqMap(vec);
}

vector<shared_ptr<TNode>> Compressor::dataToFreqMap(const char *data, unsigned long size){
    vector<shared_ptr<TNode>> ret(UCHAR_MAX + 1);
    unsigned char curr;

    for (unsigned long i = 0; i < size; ++i) {
        curr = data[i];
        // Increase this char's frequency by 1
        if(!ret[curr].use_count()) // create frequency entry for this char
            ret[curr] = shared_ptr<TNode>(new TNode(curr));
        else
            ret[curr] -> freq++;
    }

    return ret;
}

void Compressor::setEncodingCodes(shared_ptr<TNode> huffTree) { setEncodingCodes("", move(huffTree)); }

void Compressor::setEncodingCodes(string pathSoFar, shared_ptr<TNode> huffTree) {
    if(huffTree -> isSet) {
        huffTree->encoding = pathSoFar;
        cout << +((unsigned char)huffTree->ch) << " " << pathSoFar << endl;
    } else {
        setEncodingCodes(pathSoFar + "0", huffTree -> left);
        setEncodingCodes(pathSoFar + "1", huffTree -> right);
    }
}

fstream& Compressor::getTmpFile(bool trunc) {
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
