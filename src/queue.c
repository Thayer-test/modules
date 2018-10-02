/* 
 * queue.c -- implements a generic queue
 * Hides the internal representation of a queue.
 *
 */
#include <stdlib.h>
#include <stdbool.h>
#include <queue.h>

/* general definitions */
#define DEFAULT_MAX_FREE 50	/* at most 50 free links allowed */


/* BEGINNING OF PRIVATE SECTION */
/* 
 * A hidden queue (hqueue_t) is a linked list of hidden links
 * (hlink_t) accessed through front and back pointers -- these
 * structures are not visible outside the queue module.
 */
typedef struct link_struct {
  struct link_struct *linknextp;           /* next Link */
  struct link_struct *linkprevp;           /* previous Link */
  void *linkelementp;                      /* ptr to queue element */
} hlink_t;				   /* a hidden link */

/* link accessor macros */
#define next(l) (((hlink_t*)l)->linknextp)
#define prev(l) (((hlink_t*)l)->linkprevp)
#define element(l) (((hlink_t*)l)->linkelementp)

typedef struct {
  hlink_t *queuefrontp;		/* pointer to front of queue */
  hlink_t *queuebackp;		/* pointer to end of queue */
  hlink_t *queuefreep;		/* free list of links */
  int freespaces;		/* number of spaces for free links */
} hqueue_t;			/* a hidden queue */

/* queue accessor macros */
#define back(q) (((hqueue_t*)q)->queuebackp)
#define front(q) (((hqueue_t*)q)->queuefrontp)
#define qfree(q) (((hqueue_t*)q)->queuefreep)
#define spaces(q) (((hqueue_t*)q)->freespaces)

/* 
 * hidden helper functions 
 */
static void free_link(hqueue_t *qp,hlink_t *lp) {		       
  if(spaces(qp)>0) {		/* room on the freelist */
    next(lp) = qfree(qp);	/* add link to freelist */
    qfree(qp) = lp;				
    spaces(qp)--;		/* one less space for a free link */
  }						
  else				/* otherwise, just free it */
    free((void*)lp);		
}

static hlink_t* get_link(hqueue_t *qp) {
  hlink_t* lp;
  
  if(qfree(qp) != NULL) {	/* any links on freelist */
    lp=qfree(qp);		/* grab one */
    qfree(qp) = next(lp);
    spaces(qp)++;		/* one more space for a free link */
  }
  else 				/* otherwise, malloc one */
    lp = (hlink_t*)malloc(sizeof(hlink_t));
  if (lp) {			/* then initialize it */
    next(lp) = NULL; 
    prev(lp) = NULL;
    element(lp) = NULL;
  }
  return lp;                               
}
/* END OF PRIVATE SECTION */



/* BEGINNING OF PUBLIC SECTION */

/* 
 * qopen -- opens a queue 
 */
queue_t *qopen(void) {
  hqueue_t* qp;
  void *rp;
  
  /* create a lock for the queue */
  qp = (hqueue_t*)malloc(sizeof(hqueue_t));
  if(qp) {			/* intialize the queue to empty */
    front(qp) = NULL;
    back(qp) = NULL;  
    qfree(qp) = NULL;
    spaces(qp) = DEFAULT_MAX_FREE; /* number of free links allowed */
  }
  rp=(void*)qp;
  return (queue_t*)rp;
}

/* 
 * qclose -- close a queue by free'ing each link in the queue, its
 * associated elements, and the free list.
 */
void qclose(queue_t *qp) {
  hlink_t *p, *holdp;
  
  for(p=front(qp); p!=NULL; ) { /* p points to links */
    holdp=p;			/* save the current link */
    if(element(p)!=NULL)	/* if the element exists */
      free(element(p));		/* free it */
    p=next(p);			/* move p on to next link */
    free(holdp);		/* free the current link */
  }
  for(p=qfree(qp); p!=NULL; ) {	/* p points to an empty link */
    holdp=p;			/* hold the current link */
    p=next(p);			/* move p on to next link */
    free(holdp);		/* free the current link */
  }
  free((void*)qp);		/* free the queue structure */
}

/*
 * qadd -- adds an value ep to the back of queue qp
 */
int32_t qput(queue_t *qp, void *ep) {
  hlink_t *newp,*bp;
  int32_t rc;
  
  newp=get_link(qp);                       /* get a new link */
  if(newp) {                               /* next already null */
    element(newp) = ep;		/* add queue element to link */
    bp=back(qp);
    if(bp) {			    /* list is not empty */
      next(bp)=newp;                /* change last entry */
      prev(newp)=bp;                /* previous is old back */
    }
    else
      front(qp) = newp;             /* front of the queue */
    back(qp) = newp;		    /* new is now queue back */
  } 
  if (newp == NULL)
    rc = -1;
  else
    rc = 0;
  return rc;
}


void* qget(queue_t *qp) {
  hlink_t *fp;
  void* ep;
  
  fp=front(qp);                        /* find front of queue */
  if(fp) {                             /* queue not empty */
    ep = element(fp);                  /* take the first value */
    front(qp) = next(fp);	/* new front is next in queue */
    if(front(qp)==NULL)		/* if list now empty */
      back(qp)=NULL;		/* back also null */
    else
      prev(front(qp)) = NULL;	     /* no prev for new front */
    free_link(qp,fp);                /* add link to free list */
  }
  else				/* nothing in queue */
    ep = NULL;			/* nothing to return */
  return ep;
}

/*
 * qapply -- applies a function to every element of the queue 
 */
void qapply(queue_t *qp, void (*fn)(void* ep)) {
  hlink_t *p;
  
  for(p=front(qp); p!=NULL ; p=next(p)) {
    (*fn)(element(p));                     /* apply fn to all  */
  }
}

/* 
 * qsearch -- applies a search function to every element of queue
 * and returns a pointer to the element if its is found
 */
void* qsearch(queue_t *qp, 
	      bool (*searchfn)(void* elementp, const void* searchkey),
	      const void* keyp) {
  hlink_t *p;
  void *result;
  bool found;
  
  result=NULL;
  for(found=false, p=front(qp) ; 
      p!=NULL && !(found=(*searchfn)(element(p),keyp)) ;
      p=next(p))
    ;                                    /* apply fn to all  */
  if(found) 
    result=element(p);
  return result;
}

/*
 * qremove -- applies a search function to every element of the queue;
 * removes and returns the sought for element if it is found
 */
void* qremove(queue_t *qp, 
	      bool (*searchfn)(void* elementp,const void* searchkey),
	      const void* keyp) {
  hlink_t *p;
  void *result;
	bool found;
  
  result=NULL;			/* no result by default */
  for(found=false, p=front(qp) ; 
      p!=NULL && !(found=(*searchfn)(element(p),keyp)) ;
      p=next(p))
    ;				/* apply fn to all  */
  if(found) {
    result=element(p);
    if(prev(p))			       /* something before p? */
      next(prev(p)) = next(p);         /* prev points past p */
    else			       /* p is front of q */
      front(qp) = next(p);             /* set new front */
    if(next(p))			       /* something after p? */
      prev(next(p)) = prev(p);         /* next points past p */
    else			       /* p is back of q */
      back(qp) = prev(p);              /* set new back */
    free_link(qp,p);		       /* add link to free list */
  }
  return result;
}

/*
 * qconcat -- concatenate q2 into q1 -- q2 is no longer valid after
 * this operation 
 */
void qconcat(queue_t *q1p, queue_t *q2p) {
  hlink_t *p,*holdp;

  if(front(q2p)!=NULL) {
    if(front(q1p)==NULL) {	/* q1 empty, q2 is result */
      front(q1p) = front(q2p);
      back(q1p) = back(q2p);
    } 
    else {	
      /* put q2 at back of q1 */
      prev(front(q2p)) = back(q1p);        /* q2 front pts back at q1 */
      next(back(q1p)) = front(q2p);
      back(q1p) = back(q2p);
      /* leave the free list and spaces alone */
    }
  }
  /* deallocate the free links in q2 */
  for(p=qfree(q2p); p!=NULL; ) { /* p points to an empty link */
    holdp=p;			 /* hold the current link */
    p=next(p);			 /* move p on to next link */
    free_link(q1p,holdp);	 /* free the current link */
  }
  free(q2p);                              /* deallocate q2 */
}

#ifdef FREESPACES
/* manipulation of the number of free links kept */
void qsetfreespaces(queue_t *qp, int cnt) {
  spaces(qp) = cnt;
}

int qgetfreespaces(queue_t *qp) {
  int rv;
  rv = spaces(qp);
  return rv;
}
#endif /* FREESPACES */

