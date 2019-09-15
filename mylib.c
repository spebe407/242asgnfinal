/* mylib.c - 9/16/19.
 * COSC242 Group Assignment.
 * 
 * File containing useful functions used by htable.c and asgn.c.
 */

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include "mylib.h"

/*
 * Allocate a given amount of memory, exiting the program if
 * the memory allocation fails, else returning the result of the
 * memory allocation (from malloc(s)).
 * 
 * @param s - the amount of memory to allocate.
 * 
 * @return the result from malloc(s).
 */
void *emalloc(size_t s) {
    void *result = malloc(s);
    if(NULL == result) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }

    return result;
}

/*
 * Read a word from a stream, words are converted to lower case.
 * 
 * @param s - a char array to read the string into.
 * @param limit - the maximum size of the string that can be read.
 * @param stream - the steam from which to read the next string.
 * 
 * @return - the word read from the given stream, represented as an int.
 */
int getword(char *s, int limit, FILE *stream) {
    int c;
    char *w = s;
    assert(limit > 0 && s != NULL && stream != NULL);
    /* skip to the start of the word */
    while (!isalnum(c = getc(stream)) && EOF != c);
    
    if (EOF == c) {
        return EOF;
    } else if (--limit > 0) { /* reduce limit by 1 to allow for the \0 */
        *w++ = tolower(c);
    }

    while (--limit > 0) {
        if (isalnum(c = getc(stream))) {
            *w++ = tolower(c);
        } else if ('\'' == c) {
            limit++;
        } else {
            break;
        }
    }
    *w = '\0';
    
    return w - s;
}

/*
 * Returns 1 if a given int 'x' is a prime number.
 * 
 * @param x - the number to check.
 * 
 * @return - 1 if the number is a prime, 0 if it is not. 
 */
int is_prime(int x){
    int i = 2;
    for(i = 2;  i < x; i++){
        if(x % i == 0){
            return 0;
        }
    }
    return 1;
}

/*
 * Calcuates the next prime number following a given int.
 * For example if the provided number is 8, the returned value will be
 * 11, the next closest prime number.
 * 
 * @param n - the number from which to find the next prime number.
 * 
 * @return - the prime number closest to the provided number.
 */
int next_prime(int n) {
    int x = n;
    if(is_prime(x)) {
        return x;
    }
    
    return next_prime(x + 1);
}
