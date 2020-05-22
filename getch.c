#include <stdio.h>
#include "getch.h"

static int buffer[BUFFER_SIZE];
static int pos = 0;

int getch(void){
    if(pos > 0)
        return buffer[--pos];
    return getchar();
}

void ungetch(int c){
    if(pos < BUFFER_SIZE)
        buffer[pos++] = c;
}
