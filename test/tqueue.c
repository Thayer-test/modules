/*
 * queue testing program
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdbool.h>
#include <queue.h>
#include <tutils.h>					

#define NUMELEMENTS 100	       /* number of elements in a big queue */

static void single_queue(int test);
static void multi_queue(int test);
static int cnt;
static void cntelements(void *ep);

int main(int argc, char *argv[]) {
  int test;
  if(argc!=2) {
    printf("Usage: %s <testnumber> -- testnumber=1-20\n",argv[0]);
    exit(EXIT_FAILURE);
  }
  test=atoi(argv[1]);
  if(test<=0 || test>20)
    exit(EXIT_FAILURE);
  if (test>0 && test<7) 
    single_queue(test);
  else
    multi_queue(test);
  exit(EXIT_SUCCESS);
}

static void single_queue(int test) {
  queue_t *qp;
  void *p1,*p2,*p3,*ep;
  int i;

  /* open a queue */
  qp=qopen();
  switch(test) {
  case 1:
    /* check the queue exists */
    if(qp==NULL)
      exit(EXIT_FAILURE);
    break;
  case 2:
    /* try to get on an empty queue */
    if((ep=qget(qp)) != NULL) 
      exit(EXIT_FAILURE);
    break;
  case 3:
    /* make some generic people to be in the queue */
    p1 = make_person("steve",STEVE_AGE,SALARY);
    p2 = make_person("bill",BILL_AGE,SALARY);
    p3 = make_person("john",JOHN_AGE,SALARY);
    /* put to an empty queue */
    if(qput(qp,p1)!=0)
			exit(EXIT_FAILURE);
    /* put to a non-empty queue */
    if(qput(qp,p2)!=0)
			exit(EXIT_FAILURE);
    /* put to a non-empty queue */
    if(qput(qp,p3)!=0)
			exit(EXIT_FAILURE);

    /* get them, check them, and free them in order*/
    ep=qget(qp);
    check_person(ep,"steve",STEVE_AGE);
    free_person(ep);

    ep=qget(qp);
    check_person(ep,"bill",BILL_AGE);
    free_person(ep);

    ep=qget(qp);
    check_person(ep,"john",JOHN_AGE);
    free_person(ep);

    /* make sure the queue is empty */
    if((ep=qget(qp)) != NULL) 
      exit(EXIT_FAILURE);
    break;
  case 4:
    /* fill the queue with NUMELEMENTS */
    for(i=0;i<NUMELEMENTS; i++) {
      p1 = make_person("steve",i,SALARY);
      if(qput(qp,p1)!=0)
				exit(EXIT_FAILURE);
    }
    /* empty the queue */
    for(i=0;i<NUMELEMENTS; i++) 
      get_n_check(qp,"steve",i);

    /* make sure the queue is empty */
    if((ep=qget(qp)) != NULL) 
      exit(EXIT_FAILURE);
    break;
  case 5:
    /* use apply on empty list */
    cnt=0;
    qapply(qp,cntelements);
    if(cnt!=0)
      exit(EXIT_FAILURE);
    break;
  case 6:
    /* fill the queue with NUMELEMENTS */
    for(i=0;i<NUMELEMENTS; i++) {
      p1 = make_person("steve",STEVE_AGE,SALARY);
      if(qput(qp,p1)!=0)
				exit(EXIT_FAILURE);
    }
    /* count them with apply */
    cnt=0;
    qapply(qp,cntelements);
    if(cnt!=NUMELEMENTS)
      exit(EXIT_FAILURE);
    break;
  default:
    exit(EXIT_FAILURE);
  }
  /* try closing a queue */
  qclose(qp);
}

static void cntelements(void *ep) {
  if(ep!=NULL) {
    cnt++;
    printf("cnt=%d\n",cnt);
  }
}


static void multi_queue(int test) {
  void *ep;			/* element pointer */
  int age;
  queue_t *q1,*q2,*q3,*q4;

  /* make some generic people to be in the queues */
  void *p1 = make_person("steve", STEVE_AGE, SALARY);
  void *p2 = make_person("bill", BILL_AGE, SALARY);
  void *p3 = make_person("john", JOHN_AGE, SALARY);
  void *p4 = make_person("fred", FRED_AGE, SALARY);
  void *p5 = make_person("george", GEORGE_AGE, SALARY);
  void *p6 = make_person("nigel", NIGEL_AGE, SALARY);
  /* make a generic person not in a queue */
  void *p7 = make_person("cory", CORY_AGE, SALARY);
  
  q1=qopen();			/* initialized non empty */
  q2=qopen();			/* initialized non empty */
  q3=qopen();			/* initialized empty */
  q4=qopen();			/* initialized empty */

  /* open a queue, and put 3 people in it */
  if(qput(q1,p1)!=0)
		exit(EXIT_FAILURE);
  if(qput(q1,p2)!=0)
		exit(EXIT_FAILURE);
  if(qput(q1,p3)!=0)
		exit(EXIT_FAILURE);

  /* open a second queue, and put 3 people in it */
  if(qput(q2,p4)!=0)
		exit(EXIT_FAILURE);
  if(qput(q2,p5)!=0)
		exit(EXIT_FAILURE);
  if(qput(q2,p6)!=0)
		exit(EXIT_FAILURE);


  switch(test) {
  case 7:
    /* search empty list */
    age=STEVE_AGE;
    if(qsearch(q3,is_age,(void*)&age)!=NULL) {
      exit(EXIT_FAILURE);
		}
		qclose(q1);
		qclose(q2);
		qclose(q3);
		qclose(q4);
		free_person(p7);
    break;
  case 8:
    /* remove from empty list */
    age=STEVE_AGE;
    if(qremove(q3,is_age,(void*)&age)!=NULL) {
      exit(EXIT_FAILURE);
		}
		qclose(q1);
		qclose(q2);
		qclose(q3);
		qclose(q4);
		free_person(p7);
    break;
  case 9:
    /* qconcat 2 empty */
    qconcat(q3,q4);
    check_empty(q3);
		/* check still functions */
    if(qput(q3,p7)!=0)
			exit(EXIT_FAILURE);
    get_n_check(q3,"cory",CORY_AGE);
    check_empty(q3);
		qclose(q1);
		qclose(q2);
		qclose(q3);
		/* q4 is closed */
    break;
  case 10:
    /* nonempty-empty */
    qconcat(q1,q3);
		/* check still functions */
    if(qput(q1,p7)!=0)
			exit(EXIT_FAILURE);
    get_n_check(q1,"steve",STEVE_AGE);
    get_n_check(q1,"bill",BILL_AGE);
    get_n_check(q1,"john",JOHN_AGE);
    get_n_check(q1,"cory",CORY_AGE);
    check_empty(q1);
		qclose(q1);
		qclose(q2);
		/* q3 is closed */
		qclose(q4);
    break;
  case 11:
    /* empty-nonempty */
    qconcat(q3,q1);
    if(qput(q3,p7)!=0)
			exit(EXIT_FAILURE);
    get_n_check(q3,"steve",STEVE_AGE);
    get_n_check(q3,"bill",BILL_AGE);
    get_n_check(q3,"john",JOHN_AGE);
    get_n_check(q3,"cory",CORY_AGE);
    check_empty(q3);
		/* q1 is closed */
		qclose(q2);
		qclose(q3);
		qclose(q4);
    break;
  case 12:
    /* nonempty-nonempty */
    qconcat(q1,q2);
    if(qput(q1,p7)!=0)
			exit(EXIT_FAILURE);
    get_n_check(q1,"steve",STEVE_AGE);
    get_n_check(q1,"bill",BILL_AGE);
    get_n_check(q1,"john",JOHN_AGE);
    get_n_check(q1,"fred",FRED_AGE);
    get_n_check(q1,"george",GEORGE_AGE);
    get_n_check(q1,"nigel",NIGEL_AGE);
    get_n_check(q1,"cory",CORY_AGE);
    check_empty(q1);
		qclose(q1);
		/* q2 is closed */
		qclose(q3);
		qclose(q4);
    break;
  case 13:
    /* search for beginning */
    age=STEVE_AGE;
    ep=qsearch(q1,is_age,(void*)&age);
    check_person(ep,"steve",STEVE_AGE);
		qclose(q1);
		qclose(q2);
		qclose(q3);
		qclose(q4);
		free_person(p7);
    break;
  case 14:
    /* search for end */
    age=JOHN_AGE;
    ep=qsearch(q1,is_age,(void*)&age);
    check_person(ep,"john",JOHN_AGE);
		qclose(q1);
		qclose(q2);
		qclose(q3);
		qclose(q4);
		free_person(p7);
    break;
  case 15:
    /* search for middle */
    age=BILL_AGE;
    ep=qsearch(q1,is_age,(void*)&age);
    check_person(ep,"bill",BILL_AGE);
		qclose(q1);
		qclose(q2);
		qclose(q3);
		qclose(q4);
		free_person(p7);
    break;
  case 16:
    /* search not present */
    age=99;
    ep=qsearch(q1,is_age,(void*)&age);
    if(ep!=NULL) {
      exit(EXIT_FAILURE);
		}
		qclose(q1);
		qclose(q2);
		qclose(q3);
		qclose(q4);
		free_person(p7);
    break;
  case 17:
    /* remove for beginning */
    age=STEVE_AGE;
    ep=qremove(q1,is_age,(void*)&age);
    check_person(ep,"steve",STEVE_AGE);
		free_person(ep);
    get_n_check(q1,"bill",BILL_AGE);
    get_n_check(q1,"john",JOHN_AGE);
    check_empty(q1);
		qclose(q1);
		qclose(q2);
		qclose(q3);
		qclose(q4);
		free_person(p7);
    break;
  case 18:
    /* remove for end */
    age=JOHN_AGE;
    ep=qremove(q1,is_age,(void*)&age);
    check_person(ep,"john",JOHN_AGE);
		free_person(ep);
    get_n_check(q1,"steve",STEVE_AGE);
    get_n_check(q1,"bill",BILL_AGE);
    check_empty(q1);
		qclose(q1);
		qclose(q2);
		qclose(q3);
		qclose(q4);
		free_person(p7);
    break;
  case 19:
    /* remove for middle */
    age=BILL_AGE;
    ep=qremove(q1,is_age,(void*)&age);
    check_person(ep,"bill",BILL_AGE);
		free_person(ep);
    get_n_check(q1,"steve",STEVE_AGE);
    get_n_check(q1,"john",JOHN_AGE);
    check_empty(q1);
		qclose(q1);
		qclose(q2);
		qclose(q3);
		qclose(q4);
		free_person(p7);
    break;
  case 20:
    /* remove not present */
    age=99;
    ep=qremove(q1,is_age,(void*)&age);
    if(ep!=NULL)
      exit(EXIT_FAILURE);
    get_n_check(q1,"steve",STEVE_AGE);
    get_n_check(q1,"bill",BILL_AGE);
    get_n_check(q1,"john",JOHN_AGE);
    check_empty(q1);
		qclose(q1);
		qclose(q2);
		qclose(q3);
		qclose(q4);
		free_person(p7);
    break;
  default:
    exit(EXIT_FAILURE);
    break;
  }
}
