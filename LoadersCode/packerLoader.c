/*
 * Created by Or Nevo Michrowski
 * Description:
 *  This file contains the definition and implementation of the unpacker loader
 */

#include "./loaders.h"

/**
 * This is the unpacker loader.
 * @param dest is the address to which the unpacker should write the unpacked code.
 *              should be passed in RDI register.
 * @param compressedCode is the address of the compressed code, as was compressed by the packer.
 *              should be passed in RSI register.
 */
void packerLoader();

void packerLoader() {
    /*
     * We assume that the destination address comes in rdi, and the compressed code address (input)
     *  comes in rsi.
     * */
    // First move the addresses into local variables
    unsigned char *dest = NULL, *compressedCode = NULL;

    __asm__ volatile (
    "movq %%rdi, %0\n"
            "movq %%rsi, %1\n"
    : "=r" (dest),
    "=r" (compressedCode)
    );

    // Since we may not have malloc at hand, we will allocate all space
    //  needed for tree in the stack.
    // The tree is a binary tree with max 256 leafs, and no nodes with only one child.
    // This means that the total max number of nodes is 256 + 128 + 64 + 32 + 16 + 8 + 4 + 2 + 1 = 511
    TNode treeStorage[MAX_TREE_NODES];

    // for now assume we have the compressed code
    unsigned char lastBitFill = *(compressedCode++);
    unsigned long originalSize = *(unsigned long *)compressedCode;
    compressedCode += sizeof(unsigned long);
    unsigned long compressedDataSize = *(unsigned long *)compressedCode;
    compressedCode += sizeof(unsigned long);
    unsigned short numOfFreqs = *(unsigned char*)(compressedCode++);

    // Copy frequencies from compressed data to stack nodes
    //  in a decending order
    for (int j = 0; j < numOfFreqs; ++j, compressedCode += FREQ_ENTRY_SIZE) {
        treeStorage[j].representedChar = *compressedCode;
        treeStorage[j].frequency = *((unsigned long*) (compressedCode + 1));
        treeStorage[j].leftSon = treeStorage[j].rightSon = NULL;
        treeStorage[j].prev = (j == 0 ? NULL : &(treeStorage[j - 1]));
        treeStorage[j].next = (j == numOfFreqs - 1 ? NULL : &(treeStorage[j + 1]));
    }

    /*
     * Logic explanation: Since we don't have access to the heap
     *  allocation functions in the loader, we'll need to store all
     *  the tree in the stack.
     * But for the tree construction, we need to iterate over the unmounted
     *  nodes. That's why we use a linked list, which will allow us
     *  to keep record of the unmounted nodes while keeping the memory
     *  management simple.
     * */
    TNode *unmountedNodes = treeStorage;

    // build tree
    while(unmountedNodes && unmountedNodes->next) { // while we have more than one unmounted node
        // find the min two nodes
        TNode *min = unmountedNodes, *i = NULL;
        for (i = unmountedNodes; i -> next; i = i -> next)
            if(i->frequency + i->next->frequency <
               min->frequency + min->next->frequency)
                min = i;

        // We want to add the father in the same list index as min. so copy min's next to
        //  the end of the list and put the father in his place
        // Copy min's next to the end of the list
        TNode *newMinsNextPlace = &(treeStorage[numOfFreqs++]);
        newMinsNextPlace -> leftSon = min->next->leftSon;
        newMinsNextPlace -> rightSon = min->next->rightSon;
        newMinsNextPlace -> frequency = min->next->frequency;
        newMinsNextPlace -> next = min->next->next;
        newMinsNextPlace -> prev = min->next->prev;
        newMinsNextPlace -> representedChar = min->next->representedChar;

        // Add the new father
        TNode *father = min->next;
        father->leftSon = min;
        father->rightSon = newMinsNextPlace;
        father->frequency = min->frequency + newMinsNextPlace->frequency;
        father->next = father->prev = NULL;

        // Alter the list's pointers to add the father in the min's place
        // now i is the last node in the list
        // Back pointer
        if(min != unmountedNodes) {
            min->prev->next = father;
            father->prev = min->prev;
        } else
            unmountedNodes = father;
        // Front pointer. The next one is already pointing here.
        father->next = newMinsNextPlace->next;
    }

    // Iterate over data
    TNode* root = unmountedNodes; // For readability
    TNode* currentNode = root;
    unsigned char currentChar, *destPtr = dest;

    for (unsigned long i = 0; i < compressedDataSize; ++i) {
        currentChar = compressedCode[i];

        // Iterate on char's bits
        for (int j = 0; j < (i+1==compressedDataSize ? lastBitFill : 8); ++j){

            // Move down the tree properly
            if(currentChar & (1<<(7-j)))
                currentNode = currentNode -> rightSon;
            else
                currentNode = currentNode -> leftSon;

            if(!currentNode->leftSon && !currentNode->rightSon) { // if a leaf
                *(destPtr++) = currentNode->representedChar;
                currentNode = root;
            }
        }
    }

    *destPtr = '\x00';

    LOADER_FUNC_TERMINATE;
}