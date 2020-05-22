#include <stdio.h>
#include "complex.h"


int check(adjmat mat, int u, int v, enum bool expected)
{
    int result = path(mat, u, v);
    if(result != expected) {
        printf("--- TEST FAILED WITH u: \"%d\", v: \"%d\" ---- returned %d instead of %d\n", u, v, result, expected);
        return 1;
    }
    return 0;
}

main() {
    int failedTests = 0;

    adjmat x = {0};

    x[0][4] = 1;
    x[0][5] = 1;
    x[0][7] = 1;
    x[0][9] = 1;
    x[0][10] = 1;
    x[2][1] = 1;
    x[6][0] = 1;
    x[6][2] = 1;
    x[6][8] = 1;
    x[8][3] = 1;

    failedTests += check (x, 6, 0, 1);
    failedTests += check (x, 6, 2, 1);
    failedTests += check (x, 6, 1, 1);
    failedTests += check (x, 12, 0, 0);
    failedTests += check (x, -1, 0, 0);
    failedTests += check (x, 9, 6, 0);
    failedTests += check (x, 8, 3, 1);
    failedTests += check (x, 8, 0, 0);
    failedTests += check (x, 4, 9, 0);
    failedTests += check (x, 8, 8, 1);

    if(failedTests == 0)
        printf("ALL TESTS PASSED!\n");
}

