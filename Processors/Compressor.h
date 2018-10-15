/*
 * Created by Or Nevo Michrowski
 * Description:
 *  This file is the definition of the Compressor class, which implements the
 *      huffman tree compression algorithm and uses it to compress the code of the ELF
 *      for runtime extraction.
 */

#ifndef FINALPACKERTRY_COMPRESSOR_H
#define FINALPACKERTRY_COMPRESSOR_H

#include <vector>
#include <memory>
#include "../Helpers/Constants.h"
#include "../ELFHandlers/Utility.h"

#define FREQ_ENTRY_SIZE (sizeof(char) + sizeof(unsigned long))


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
    string encoding;
    bool isSet;
    shared_ptr<TNode> right, left;

    TNode() :  ch('\x00'),
               freq(0),
               isSet(false) {}

    TNode(const char representedChar) : ch(representedChar),
                                        freq(1),
                                        encoding(""),
                                        isSet(true) {}


    TNode(unsigned long chFreq,
          shared_ptr<TNode> leftNode,
          shared_ptr<TNode> rightNode) : freq(chFreq),
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
    /**
     * Compresses and returns the code in the .text section.
     * @return a vector of the packed code.
     */
    static vector<char> compressCode();

private:
    /**
     * Compresses the data pointed by the data pointer with huffman's algorithm, and returns the compressed code.
     * Compressed data header structure:
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
     * @param data is a pointer to the data to compress
     * @param dataLen is the amount of bytes to read from data
     * @return a raw pointer to the compressed data.
     */
    static CompressionRetVal compress(const char* data, unsigned long dataLen);

    /**
     * Builds a huffman tree of bytes based on their frequency in the data, according to
     *  huffman's algorithm
     * @param arr is an array of bytes frequencies in the data
     * @return a pointer to the root of the huffman tree
     */
    static shared_ptr<TNode> buildTreeFromFreqMap(vector<shared_ptr<TNode>> arr);

    /**
     * Reads data and counts an appearance frequency for each byte in the data.
     * @param data is pointer to the data to compress
     * @param size is the amount of bytes to read from data - the size of the data to compress
     * @return a vector of TNodes, each representing a byte frequency.
     */
    static vector<shared_ptr<TNode>> dataToFreqMap(const char *data, unsigned long size);

    /* Helpers functions */

    /**
     * Walks the tree, and assigns the leafs (which represent bytes) with their
     *  compression code according to the path from the root to them.
     * This function simply calls the recursive setEncodingCodes(string, shared_ptr<TNode>)
     * @param huffTree is a pointer to the root of the huffman tree
     */
    static void setEncodingCodes(shared_ptr<TNode> huffTree);

    /**
     * Recursively Walks the tree, and assigns the leafs (which represent bytes) with their
     *  compression code according to the path from the root to them.
     * @param pathSoFar is a string representing the path from the tree's original root
     *  to the current recursion root, where going to the left son is represented as a '0'
     *  and going to the right son is represented as a '1'.
     * @param huffTree is the current recursive root of the tree
     */
    static void setEncodingCodes(string pathSoFar, shared_ptr<TNode> huffTree);

    /**
     * Supplies a temporary file for processing
     * @param trunc indicates whether to truncate the file's previous content
     * @return the temporary file's handle.
     */
    static fstream& getTmpFile(bool trunc=true);
};


#endif //FINALPACKERTRY_COMPRESSOR_H
