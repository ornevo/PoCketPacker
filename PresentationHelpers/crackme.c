#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned int crc32(unsigned char *input, int len) {
    unsigned int r = 0xffffffff;
    for(int i = 0; i < len; i++)
        for(int j = 0; j < 8; j++)
            r = r>>1 ^ ((r^input[i]>>j)&1)*0xedb88320;
    return r;
}

int main(void) {
    char input[100];
    printf("Please supply the key:\n");
    scanf("%99s", input);
    if(crc32(input, strlen(input)) % 0x6dc1)
        puts("Wrong key.");
    else
        puts("Nice! You just won.");
}
