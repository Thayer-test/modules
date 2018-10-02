/* 
 * hash.c -- implements a generic hash table as an indexed set of queues.
 *
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <queue.h>
#include <hash.h>



/* PRIVATE SECTION */

/* the hidden structre of a hash table */
typedef struct {
  uint32_t table_size;		/* the size of the table */
  void **table;			/* pointer to a table of void* */
} hhash_t;

/* accessor macros */
#define hsize(htp) (((hhash_t*)htp)->table_size)
#define htable(htp) (((hhash_t*)htp)->table)
#define hqueue(htp,qindex) (*(htable(htp)+qindex))

/* The following (rather complicated) code, between the dashed line
 * marks, has been taken from Paul Hsieh's website. It is under the
 * terms of the BSD license. It's a really good hash function used all
 * over the place nowadays, including Google Sparse Hash. 
*/
/*----------------------------------------------------------------*/
#define get16bits(d) (*((const uint16_t *) (d)))

static uint32_t SuperFastHash (const char *data, int len, uint32_t tablesize) {
  uint32_t hash = len, tmp;
  int rem;
  
  if (len <= 0 || data == NULL) return 0;
  rem = len & 3;
  len >>= 2;
  /* Main loop */
  for (;len > 0; len--) {
    hash  += get16bits (data);
    tmp    = (get16bits (data+2) << 11) ^ hash;
    hash   = (hash << 16) ^ tmp;
    data  += 2*sizeof (uint16_t);
    hash  += hash >> 11;
  }
  /* Handle end cases */
  switch (rem) {
  case 3: hash += get16bits (data);
    hash ^= hash << 16;
    hash ^= data[sizeof (uint16_t)] << 18;
    hash += hash >> 11;
    break;
  case 2: hash += get16bits (data);
    hash ^= hash << 11;
    hash += hash >> 17;
    break;
  case 1: hash += *data;
    hash ^= hash << 10;
    hash += hash >> 1;
  }
  /* Force "avalanching" of final 127 bits */
  hash ^= hash << 3;
  hash += hash >> 5;
  hash ^= hash << 4;
  hash += hash >> 17;
  hash ^= hash << 25;
  hash += hash >> 6;
  return hash % tablesize;
}
/*-----------------------------------------------------------------*/

#define hashfn(table,key,keylen)\
	SuperFastHash(key, keylen, hsize(table))

/*
 static uint32_t hashfn(hashtable_t *ctx, const char *key, int keylen) {
  return SuperFastHash(key, keylen, hsize(ctx));
}

*/
/* END OF PRIVATE SECTION */



/* PUBLIC SECTION */

hashtable_t *hopen(uint32_t hsize) {
  hhash_t *htp;
  void **tp, **p, **endp;
  
  htp = malloc(sizeof(hhash_t));	  /* the hash table */
  tp = malloc(sizeof(void*)*hsize);       /* indexed set of queues */
  hsize(htp) = hsize;
  htable(htp) = tp;
  for(p=tp, endp=tp+hsize; p<endp; p++)
    *p=qopen();			/* each entry is a queue */
  return (hashtable_t*)htp;
}

void hclose(hashtable_t *htp) {
  void **tp,**p,**endp;
  
  tp = htable(htp);
  for(p=tp, endp=tp+hsize(htp); p<endp; p++)
    qclose(*p);				  /* close each queue */
  free(tp);                               /* free the index */
  free(htp);                              /* free the hash table */
}

/*
 * hput -- adds an value to a hash table under a specific key
 */
int32_t hput(hashtable_t *htp, void *ep, const char *key, int keylen) {
  uint32_t index;
  void *qp;
  
  index=hashfn(htp, key, keylen);	   /* get queue index */
  qp=hqueue(htp, index);						 /* find queue */
  return qput(qp, ep);							 /* put in queue */
}

/*
 * happly -- apply a function to every entry in the table
 */
void happly(hashtable_t *htp, void (*fn)(void *ep)) {
  uint32_t i,hsize;
  void *qp;
  
  hsize = hsize(htp);
  for(i=0; i<hsize; i++) {	/* for each entry in table */
    qp=hqueue(htp, i);		/* take queue i */
    if(qp != NULL)		/* (may be empty) */
      qapply(qp, fn);		/* apply in queue */
  }
}

/* 
 * hsearch -- find an entry matching key. We don't need to include the
 *            keylen in the searchfn call because that function has
 *            knowledge of what a key actually is.
 */ 
void* hsearch(hashtable_t *htp, 
              bool (*searchfn)(void *elementp, const void *searchkeyp),
              const char *key, int keylen) {
  uint32_t index;
  void *qp,*ep;
  
  index=hashfn(htp, key, keylen); /* get queue index */
  qp=hqueue(htp, index);
  ep=qsearch(qp, searchfn, key);
  return ep;
}


/* 
 * hremove -- find an entry matching key. We don't need to include the
 *            keylen in the searchfn call because that function has
 *            knowledge of what a key actually is.
 */ 
void* hremove(hashtable_t *htp, 
              bool (*searchfn)(void* elementp, const void* searchkeyp),
              const char *key, int keylen) {
  uint32_t index;
  void *qp,*ep;
  
  index=hashfn(htp, key, keylen); /* get queue index */
  qp=hqueue(htp, index);
  ep=qremove(qp, searchfn, key);
  return ep;
}

/* END OF PUBLIC SECTION */
