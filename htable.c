/* htable.c - 9/16/19.
 * COSC242 Group Assignment.
 * 
 * File containing functions for creating and using a hash table.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "htable.h"
#include "mylib.h"

struct htablerec {
    int capacity;       /*The capacity of the hash table.*/
    int num_keys;       /*The number of keys currently in the hash table.*/ 
    int* frequencies;   /*An array containing the frequency of each key.*/
    char** keys;        /*The keys contained in the hash table.*/
    int* stats;         /*An array containing stats about the state of the hash table.*/
    hashing_t method;   /*The type of collision resolution to use in the 
                         *filling and search of the hash table.*/
};

/*
 * Converts a string into an unsigned int so that it
 * can be used in hashing calculations.
 * 
 * @param word - the string to convert to an unsigned int.
 * 
 * @return - the string as an unsigned int.
 */
static unsigned int htable_word_to_int(char *word) {
    unsigned int result = 0;
    
    while (*word != '\0') {
        result = (*word++ + 31 * result);   
    }

    return result;
}

/*
 * Calculates a step to use in double hashing collision resolution.
 * This function is used by htable_insert and htable_search if
 * DOUBLE_H is passed to htable_new.
 * 
 * @param h - the hash table that the key is to be inserted into.
 * @param i_key - the integer representation of the key to be inserted.
 * 
 * @return - an unsigned int that is the step to be used in the double hashing
 *           collision resolution calculations. 
 */
static unsigned int htable_step(htable h, unsigned int i_key) {
    unsigned int result = 1 + (i_key % (h->capacity - 1));
    return result;
}

/*
 * Frees memory allocated during the creation of a new hash table.
 * 
 * @param h the htable to free.
 */
void htable_free(htable h) {
    int i;
    for(i = 0; i < h->capacity; i++) {
        if(h->keys[i] != NULL) { 
            free(h->keys[i]);
        }
    } 

    free(h->frequencies);
    free(h->keys);
    free(h->stats);
    free(h);
}

/*
 * Insert a given string into a hash table, using linear probing or 
 * double hashing as the collision resolution method. This function also 
 * increments the strings matching frequency in h->freqs, and sets 
 * h->stats[h->num_keys] to the number of collisions that occured while
 * inserting the string. 
 * 
 * Returns 1 if the string is inserted and is the first instance 
 * of that string in the hash table, the frequency of the matching string 
 * is returned if it is not the first instance. 0 is returned if 
 * the string is not inserted due to the hash table being full.
 * 
 * @param h - the hash table to insert the string into.
 * @param str - the string to insert.
 * 
 * @return - 1 if the string is the first instance of
 *             the string in the hash table, the frequency of the 
 *             matching string in the hash table if is not. 0 is returned
 *             if the hash table is full and the string was not inserted.
 */
int htable_insert(htable h, char *str) {
    int collisions = 0;
    unsigned int key = htable_word_to_int(str);
    unsigned int index = key % h->capacity;
    unsigned int step = (h->method == LINEAR_P) ? 1 : htable_step(h, key);

    while(h->keys[index] != NULL && strcmp(str, h->keys[index]) != 0) {
        index = (index + step) % h->capacity;
        

        if(index == (key % h->capacity)) {
            return 0;
        }

        collisions++;
    }

    if(h->keys[index] == NULL) {
        h->keys[index] = emalloc((strlen(str) + 1) * sizeof(h->keys[0][0]));
        strcpy(h->keys[index], str);
        h->frequencies[index] = 1; 
        h->stats[h->num_keys] = collisions;
        h->num_keys++;
        return 1;    
    } else {
        h->frequencies[index]++;
        return h->frequencies[index];
    }

    if(collisions == h->capacity) {
        return 0;
    }
}

/*
 * Create a new empty hash table, using either linear probing or double hashing 
 * collision resolution. 
 * Note: The hashing type provided will dictate the collision resolution method 
 * used in htable_search & htable_insert.
 * 
 * @param capacity - the size of the hash table to create.
 * @param h_type - the collision resolution method to use.
 */
htable htable_new(int capacity, hashing_t h_type) {
    int i;
    htable result = emalloc(sizeof *result);
    result->capacity = capacity;
    result->num_keys = 0;
    result->method = h_type;
    result->frequencies = emalloc(result->capacity * sizeof(result->
                                                            frequencies[0]));
    result->keys = emalloc(result->capacity * sizeof(result->keys[0]));
    result->stats = emalloc(result->capacity * sizeof(result->stats[0]));
    
    for(i = 0; i < result->capacity; i++) {
        result->frequencies[i] = 0;
        result->keys[i] = NULL;
        result->stats[i] = 0;
    }

    return result;
}

/*
 * Prints a hash table to a given stream.
 * 
 * @param h - the hash table.
 * @param stream - the stream to print to.
 */
void htable_print(htable h, FILE *stream) {
    int i;
    for(i = 0; i < h->capacity; i++) {
        if(h->keys[i] != NULL) {
            fprintf(stream, "%d %s\n", h->frequencies[i], h->keys[i]);
        }
    }
}

/*
 * Prints out the entire contents of a hash table
 * to stderr, including each key's position in the hash table,
 * its frequency, and its stats.
 * Note: Empty keys will not be printed.
 * 
 * @param h - the hash table to print.
 */
void htable_print_entire_table(htable h) {
    int count = 0;

    fprintf(stderr, "  Pos  Freq  Stats  Word\n");
    fprintf(stderr, "----------------------------------------\n");
    for(count = 0; count < h-> capacity; count ++){
        if(h->keys [count] != NULL){
            fprintf(stderr, "%5d %5d %5d   %s\n",
                    count,
                    h->frequencies[count],
                    h->stats[count],
                    h->keys[count]);
        } else {
            fprintf(stderr,"%5d %5d %5d   \n",
                    count,
                    h->frequencies[count],
                    h->stats[count]);
        }
    }
}

/*
 * Searches a given hash table for a string using either linear probing
 * or double hashing.
 * 
 * @param h - the hash table.
 * @param str - the string to search for.
 */
int htable_search(htable h, char *str) {
    int collisions = 0;
    unsigned int key = htable_word_to_int(str);
    unsigned int index = key % h->capacity;
    unsigned int step = (h->method == LINEAR_P) ? 1 : htable_step(h, key);
    while(h->keys[index] != NULL && strcmp(str, h->keys[index]) != 0) {
        index = (index + step) % h->capacity;
        collisions++;

        
        if(index == (key % h->capacity)) {
            break;
        }
    }   

    if(collisions == h->capacity) {
        return 0;
    }

    return h->frequencies[index];
}

/**
 * Prints out a line of data from the hash table to reflect the state
 * the table was in when it was a certain percentage full.
 * Note: If the hashtable is less full than percent_full then no data
 * will be printed.
 *
 * @param h - the hash table.
 * @param stream - a stream to print the data to.
 * @param percent_full - the point at which to show the data from.
 */
static void print_stats_line(htable h, FILE *stream, int percent_full) {
    int current_entries = h->capacity * percent_full / 100;
    double average_collisions = 0.0;
    int at_home = 0;
    int max_collisions = 0;
    int i = 0;

    if (current_entries > 0 && current_entries <= h->num_keys) {
        for (i = 0; i < current_entries; i++) {
            if (h->stats[i] == 0) {
                at_home++;
            } 
            if (h->stats[i] > max_collisions) {
                max_collisions = h->stats[i];
            }
            average_collisions += h->stats[i];
        }
    
        fprintf(stream, "%4d %10d %11.1f %10.2f %11d\n", percent_full, 
                current_entries, at_home * 100.0 / current_entries,
                average_collisions / current_entries, max_collisions);
    }
}


/**
 * Prints out a table showing what the following attributes were like
 * at regular intervals (as determined by num_stats) while the
 * hashtable was being built.
 *
 * @li Percent At Home - how many keys were placed without a collision
 * occurring.
 * @li Average Collisions - how many collisions have occurred on
 *  average while placing all of the keys so far.
 * @li Maximum Collisions - the most collisions that have occurred
 * while placing a key.
 *
 * @param h the hashtable to print statistics summary from.
 * @param stream the stream to send output to.
 * @param num_stats the maximum number of statistical snapshots to print.
 */
void htable_print_stats(htable h, FILE *stream, int num_stats) {
    int i;

    fprintf(stream, "\n%s\n\n", 
            h->method == LINEAR_P ? "Linear Probing" : "Double Hashing"); 
    fprintf(stream, "Percent   Current    Percent    Average      Maximum\n");
    fprintf(stream, " Full     Entries    At Home   Collisions   Collisions\n");
    fprintf(stream, "------------------------------------------------------\n");
    for (i = 1; i <= num_stats; i++) {
        print_stats_line(h, stream, 100 * i / num_stats);
    }
    fprintf(stream, "------------------------------------------------------\n\n");
}
