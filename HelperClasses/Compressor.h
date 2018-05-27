//
// Created by ororor012 on 04/05/18.
//

#ifndef FINALPACKERTRY_COMPRESSOR_H
#define FINALPACKERTRY_COMPRESSOR_H

#define FREQ_ENTRY_SIZE (sizeof(char) + sizeof(unsigned long))


#include <vector>
#include <memory>

using namespace std;

typedef struct CompressionRetVal {
    const char* compressedCode;
    unsigned long compressedDataSize;
} CompressionRetVal;

// For huffman tree
class TNode{
public:
    char ch;
    unsigned long freq;
    std::string encoding;
    bool isSet;
    std::shared_ptr<TNode> right, left;

    TNode() :  ch('\x00'),
               freq(0),
               isSet(false) {}

    TNode(const char representedChar) : ch(representedChar),
                                        freq(1),
                                        encoding(""),
                                        isSet(true) {}


    TNode(unsigned long chFreq,
          std::shared_ptr<TNode> leftNode,
          std::shared_ptr<TNode> rightNode) : freq(chFreq),
                                         right(rightNode),
                                         left(leftNode),
                                         isSet(false) {}

    TNode operator = (const TNode& other) {
        this -> ch = other.ch;
        this -> freq = other.freq;
        this -> isSet = other.isSet;
        this -> right = other.right;
        this -> left = other.left;
        this -> encoding = other.encoding;
        return *this;
    }
};


class Compressor {
public:
    /*
     * Returns a vector of the packed code.
     * This function simply calls the compress function. All it does is getting the code from the
     *  elfio object, and transforming the return char* value into the vector0
     */
    static vector<char> compressCode();

private:
    // Returns a raw pointer to the compressed data. First 8 bytes are the
    //  returned data size, then byte 9 and on is the compressed data
    /* Compressed data header structure:
     * --------------------
     * | lastByteUsedBits | - a byte which holds how many bits from the last byte are used to represent data
     * | ---------------- |
     * | originalDataSize | - unsigned long, indicating the original data size before compression.
     * | ---------------- |
     * | compressDataSize | - unsigned long, indicating the original data size after compression (headers aren't included).
     * | ---------------- |
     * |  numberOfFreqs   | - a byte which holds how many frequency pairs are held below
     * | ---------------- |
     * | Array of chars   | - In the format of ------------------------------------------------------------
     * |  frequencies     |                    | (char) represented character | (unsigned l ong) frequency |
     * --------------------                    ------------------------------------------------------------
     */
    static CompressionRetVal compress(const char* data, unsigned long dataLen);

    static shared_ptr<TNode> buildTreeFromFreqMap(vector<shared_ptr<TNode>> arr);
    static vector<shared_ptr<TNode>> fileToFreqMap(const char *f, unsigned long size);

    // Helpers
    static void setEncodingCodes(shared_ptr<TNode> huffTree);
    static void setEncodingCodes(string pathSoFar, shared_ptr<TNode> huffTree);
};


#endif //FINALPACKERTRY_COMPRESSOR_H
