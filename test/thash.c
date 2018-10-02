/*
 * thash.c -- regression test for the hash module
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hash.h>
#include <tutils.h>

#define THASH_DEBUG 1

#define MULTIPLE 100		/* #entries = 100*tablesize */

int main(int argc, char *argv[]) {
  void *pp;
  int key,tablesize;
  hashtable_t *ht;
  char nm[NAMESIZE];

  if(argc!=2 || ((tablesize=atoi(argv[1]))<=0)) {
    printf("[Usage: thash <tablesize>]\n");
    exit(EXIT_FAILURE);
  }

  /* open a hash table and put MULTIPLE entries in it */
  ht=hopen((uint32_t)tablesize);
  for(key=0;key<(MULTIPLE*tablesize);key++) {
    snprintf(nm,sizeof(nm),"%s%d","nm",key);
    pp = make_person(nm,key,SALARY);
    if(hput(ht,(void*)pp,(char*)&key,sizeof(key))!=0)
			exit(EXIT_FAILURE);
  }

#ifdef THASH_DEBUG
  printf("\nInitial RANDOM hashtable:\n");
  happly(ht,print_person);
#endif
  /* search for and check every value that is known to be present, in
   * the opposite order they were inserted
   */
  for(key=(MULTIPLE*tablesize)-1; key>=0; key--) {
    snprintf(nm,sizeof(nm),"%s%d","nm",key);
    pp=(person_t*)hsearch(ht,is_age,(char*)&key,sizeof(key));
    check_person(pp,nm,key);
  }
#ifdef THASH_DEBUG
  printf("[hashtable search succeeded]\n");
#endif

  /* search for something thats not there */
  key=(MULTIPLE*tablesize);	/* one more than biggest key used */
  if(hsearch(ht,is_age,(char*)&key,sizeof(key))!=NULL) {
#ifdef THASH_DEBUG
    printf("[error: absent entry found]\n");
#endif
    exit(EXIT_FAILURE);
  }
#ifdef THASH_DEBUG
  printf("[search for non-exitant entry succeeded]\n");
#endif


  /* remove each entry and make sure whats removed is correct */
  for(key=(MULTIPLE*tablesize)-1; key>=0; key--) {
    snprintf(nm,sizeof(nm),"%s%d","nm",key);
    pp=(person_t*)hremove(ht,is_age,(char*)&key,sizeof(key));
    check_person(pp,nm,key);
    free_person(pp);
  }
#ifdef THASH_DEBUG
  printf("[removing all entries succeeded]\n");
#endif

#ifdef THASH_DEBUG
  printf("Final Hashtable:\n");
  happly(ht,print_person);
#endif

  /* close the hash table and terminate */
  hclose(ht);
  return(EXIT_SUCCESS);
}

