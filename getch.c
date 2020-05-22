#include <stdio.h>
#include "getch.h"

static int buffer[BUFFER_SIZE];
static int stored[MAX_STORED_LENGTH];
static int bufferPos = 0;
static int storedPos = 0;

int getch(void){
    int c;
    if(bufferPos > 0)
        c = buffer[--bufferPos];
    else
        c = getchar();

    if(storedPos < MAX_STORED_LENGTH) {
        stored[storedPos] = c;
        storedPos++;
    }
    return c;
}

void ungetch(int c){
    storedPos--;
    if(bufferPos < BUFFER_SIZE)
        buffer[bufferPos++] = c;
}

int *getStoredAndClear(void){
    stored[storedPos] = '\0';
    storedPos = 0;
    return stored;
}
