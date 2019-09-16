/* asgn.c - 9/16/19.
 * COSC242 Group Assignment.
 * 
 * A program which fills a hash table with words read from STDIN, 
 * using either linear probing or double hashing. During the filling 
 * of the hash table statistics on the number of collisions that occured are
 * recorded for each state of the hash table. If the -c parameter is passed
 * it is able to be used as a spell checker for a given document. Information 
 * on the number of unknown words in the document is printed, as well as 
 * the unknown words themselves. The time taken to fill the hash table and 
 * search it are also printed.
 * 
 * The program accepts a number of command line arguments:
 * -c 'filename' - Checks the spelling of words in 'filename' using words read
 *                 from stdin as the dictionary. -p is ignored. 
 * -d            - Use double hashing as the collision resolution strategy
 *                 (linear probing is the default).
 * -e            - Write the entire contents of the hash table to stderr.
 * -p            - Print stats information collected during the filling of
 *                 the hash table.
 * -s            - Display up to a given number of stats snapshots when given
 *                 -p as an argument.
 * -t tablesize  - Use the first prime >= tablesize as the size of 
 *                 the hash table.
 * -h            - Print a help message.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <time.h>
#include "htable.h"
#include "mylib.h"

int main(int argc, char* argv[]) {
    htable dictionary;
    char word[256];

    clock_t start, end;
    double fillTime = 0.0;
    double searchTime = 0.0;
    int unknownWords = 0;
    char *filename = NULL;
    FILE* filePointer = NULL;
    hashing_t hashing_method = LINEAR_P;
    int table_size = 113;

    bool c_flag = false;
    bool e_flag = false;
    bool p_flag = false;
    bool s_flag = false;
    bool h_flag = false;

    int snapshots = 0;
    int length = 0;
    char option;
    while((option = getopt(argc, argv, "c:deps:t:h")) != EOF) {
        switch(option) {
            case 'c':
                c_flag = true;
                filename = optarg;
                break;
            case 'd':
                hashing_method = DOUBLE_H;
                break;
            case 'e':
                e_flag = true;
                break;
            case 'p':
                p_flag = true;
                break;
            case 's':
                s_flag = true;
                snapshots = atoi(optarg);
                break;
            case 't':
                table_size = next_prime(atoi(optarg));
                break;
            case 'h':
                h_flag = true;
                break;
            default: 
                h_flag = true;
                break;
        }
    }

    if(h_flag) {
        fprintf(stderr,
                "Usage: ./sample-asgn [OPTION]... <STDIN>\n\n"
                "Perform various operations using a hash table.  "
                "By default, words are\n"
                "read from stdin and added to the hash table, before"
                "being printed out\n"
                "alongside their frequencies to stdout.\n\n"

                " -c FILENAME  Check spelling of words in FILENAME using"
                " words\n"
                "              from stdin as dictionary.  Print unknown"
                "words to\n"
                );
        fprintf(stderr, 
                "              stdout, timing info & count to stderr "
                "(ignore -p)\n"
                " -d           Use double hashing (linear probing is the "
                "default)\n"
                " -e           Display entire contents of hash table on"
                "stderr\n"
                " -p           Print stats info instead of frequencies & "
                "words\n" 
                " -s SNAPSHOTS Show SNAPSHOTS stats snapshots (if -p is "
                "used)\n" 
                " -t TABLESIZE Use the first prime >= TABLESIZE as htable"
                "size\n\n" 

                " -h           Display this message\n\n");
        return EXIT_SUCCESS;
    }

    dictionary = htable_new(table_size, hashing_method);
    
    start = clock();
    while(getword(word, sizeof(word), stdin) != EOF) {
        htable_insert(dictionary, word);
        length++;
    }
    end = clock();
    fillTime  = ((double) (end - start)) / CLOCKS_PER_SEC;

    if(e_flag) {
        htable_print_entire_table(dictionary);
    }

    if(p_flag && s_flag && !c_flag) {
        /*stderr?*/ 
        htable_print_stats(dictionary, stdout, snapshots);
    } else if(p_flag && !c_flag) {
        htable_print_stats(dictionary, stdout, 10);
    }
   
    if(c_flag) {
        filePointer = fopen(filename, "r");

        start = clock();
        while(getword(word, sizeof(word), filePointer) != EOF) {
            int result = htable_search(dictionary, word);
            if(result == 0) {
                fprintf(stdout, "%s\n", word);
                unknownWords++;
            }
        }
        
        end = clock();
        searchTime = ((double) (end - start)) / CLOCKS_PER_SEC;

        
        fprintf(stderr, "Fill time     : %f\n", fillTime);
        fprintf(stderr, "Search time   : %f\n", searchTime);
        fprintf(stderr, "Unknown words = %d\n", unknownWords);
        
        fclose(filePointer);
       
    }

    htable_free(dictionary);
   
    return EXIT_SUCCESS;
}
