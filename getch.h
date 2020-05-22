#define BUFFER_SIZE 2
#define MAX_STORED_LENGTH 100
int getch(void);
void ungetch(int);
int *getStoredAndClear(void);
