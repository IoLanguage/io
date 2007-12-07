// The Computer Language Shootout
// http://shootout.alioth.debian.org/
// Precedent C entry modified by bearophile for speed and size, 31 Jan 2006
// Compile with:  -O3 -s -std=c99 -fomit-frame-pointer

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chrono.h"

typedef unsigned char boolean;


static void nsieve(int m) {
    unsigned int count = 0, i, j;
    boolean * flags = (boolean *) malloc(m * sizeof(boolean));
    memset(flags, 1, m);

    for (i = 2; i < m; ++i)
        if (flags[i]) {
            ++count;
            for (j = i << 1; j < m; j += i)
//                if (flags[j])
                   flags[j] = 0;
    }

    free(flags);
    printf("Primes up to %8u %8u\n", m, count);
}

int main(int argc, char * argv[]) {
//    int m = atoi(argv[1]);
    int m = 9;
	int i;
ui64 c  = read_counter();
for (int k=0; k<10; k++) {
    for (i = 0; i < 3; i++)
        nsieve(10000 << (m-i));
}

	render(read_counter()-c,10);
    return 0;
}
