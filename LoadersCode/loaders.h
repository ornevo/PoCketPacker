/*
 * Created by Or Nevo Michrowski
 * Description:
 *  This file defines constants and types to be used by the loaders
 */

#ifndef POCKETPACKER_LOADERS_H
#define POCKETPACKER_LOADERS_H


// To recognize the end of the loader function in memory, terminate
//  it with a bunch of NOPs
#define LOADER_FUNC_TERMINATE __asm__("nop\nnop\nnop\nnop\n")
#define LOADER_FUNC_TERMINATOR_OPCODE 0x90909090

/* For packer loader */
// TNode represents a node in the huffman tree
typedef struct TNode {
    unsigned char representedChar;
    unsigned long frequency;
    struct TNode *leftSon, *rightSon;
    struct TNode *prev, *next; // For unmounted nodes' list
} TNode;

// The tree is a binary tree with max 256 leafs, and no nodes with only one child.
// This means that the total max number of nodes is 256 + 128 + 64 + 32 + 16 + 8 + 4 + 2 + 1 = 511
#define MAX_TREE_NODES 512
#define FREQ_ENTRY_SIZE (sizeof(char) + sizeof(unsigned long))


#endif //POCKETPACKER_LOADERS_H
