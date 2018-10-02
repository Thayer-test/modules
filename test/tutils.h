#pragma once

#include <stdbool.h>
#define NAMESIZE 256
#define SALARY ((double)50.00) /* default salary */
#define STEVE_AGE 10
#define BILL_AGE 11
#define JOHN_AGE 12
#define FRED_AGE 13
#define GEORGE_AGE 14
#define NIGEL_AGE 15
#define CORY_AGE 16


/* structures that are to be placed in
 * the queue are defined OUTSIDE the queue -- allowing
 * the queue to contain any type of element
 */
typedef void queue_t;

typedef struct person_struct {
  char name[NAMESIZE];
  int age;
  double salary;
} person_t;

/* public functions provided by qtest_utils.c */
void* make_person(char* namep, int age, double salary);
void check_person(void *ep,char *name,int age);
void free_person(void *ep);
void print_person(void *ep);
void get_n_check(queue_t *qp,char *s,int a);
void check_empty(queue_t *qp);
bool is_age(void *ep,const void *keyp);
