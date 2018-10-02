/*
 * tutils.c -- utilities used by multiple tests
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <queue.h>
#include <tutils.h>

void* make_person(char* namep,int age,double salary) {
  person_t* pp;

  if(!(pp = (person_t*)malloc(sizeof(person_t)))) {
    printf("[Error: malloc failed allocating person]\n");
    exit(EXIT_FAILURE);
  }
  strcpy(pp->name,namep);
  pp->age=age;
  pp->salary=salary;
  return (void*)pp;
}

void free_person(void *ep) {
  person_t* pp=(person_t*)ep;
	if(pp!=NULL)
		free(pp);
}

void check_person(void *ep,char *namep,int age) {
  person_t* pp=(person_t*)ep;

  if(ep==NULL ||
     (strcmp(pp->name,namep)!=0) ||  /* name bad */
     (pp->age!=age) ||		     /* age bad */
     (pp->salary<0))                 /* salary bad */
    exit(EXIT_FAILURE);
}

void print_person(void *ep) {
	person_t* pp=(person_t*)ep;
	printf("%s: age=%d, salary=%lf\n",pp->name,pp->age,pp->salary);
}


void check_empty(queue_t *qp) {
  void *ep;
  ep=qget(qp);
  if(ep!=NULL)
		exit(EXIT_FAILURE);
}

void get_n_check(queue_t *qp,char *s,int a) {
  void *ep;

  ep=qget(qp);
	check_person(ep,s,a);
	free_person(ep);
}

/* NOTE: To search a queue you need a boolean function
 * and a key. The boolean function is applied to every element 
 * of a queue until an element that matches the key is found.
 * In order for this to work on arbitrary structures, all arguments
 * are passed as void*. 
 */
bool is_age(void *ep,const void *keyp) {
  /* NOTE: First coerce the arguments to be correct types */
  person_t* pp=(person_t*)ep;
  int* ap=(int*)keyp;
  
  /* Then do the actual comparison and return a result */
  if(pp->age == *ap)
    return true;
  return false;
}
