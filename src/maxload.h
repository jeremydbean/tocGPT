/*
 * Values mirrored from db.c and used across the maxload implementation.
 */
#define MAXLOAD_KEY_HASH 127
#define MAXLOADFILE "../area/maxload.txt"

/* Global hash table and counter defined in maxload.c. */
extern ITEM_MAX_LOAD *maxload_index_hash[MAXLOAD_KEY_HASH];
extern int top_maxload;

/* Hook used after loading the maxload file. */
int maxload_chain args((void));
