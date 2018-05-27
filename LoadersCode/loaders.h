//
// Created by ororor012 on 14/03/18.
//

#ifndef POCKETPACKER_LOADERS_H
#define POCKETPACKER_LOADERS_H

#define LOADER_FUNC_TERMINATE __asm__("nop\nnop\nnop\nnop\n")
#define LOADER_FUNC_TERMINATOR_OPCODE 0x90909090

/* For packer loader */
typedef struct TNode {
    unsigned char representedChar;
    unsigned long frequency;
    struct TNode *leftSon, *rightSon;
    struct TNode *prev, *next; // For unmounted nodes' list
} TNode;
#define MAX_TREE_NODES 512
#define FREQ_ENTRY_SIZE (sizeof(char) + sizeof(unsigned long))



#endif //POCKETPACKER_LOADERS_H
