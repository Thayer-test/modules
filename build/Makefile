# make [ tests | grind | gcov | gprof XFLAGS=-pg | clean ]
CC=gcc
SRC=../src
TEST=../test
CFLAGS=-Wall -pedantic -std=c11 -I$(SRC) -I$(TEST)
# extra flags used for debugging or overwritten for profiling 
XFLAGS=-g --coverage

all:			tqueue thash

# build the modules
%.o:			$(SRC)/%.c $(SRC)/%.h
					$(CC) $(CFLAGS) $(XFLAGS)  -c $<

# build the tests
tutils.o:	$(TEST)/tutils.c $(TEST)/tutils.h
					$(CC) $(CFLAGS) $(XFLAGS)  -c $<

tqueue.o:	$(TEST)/tqueue.c
					$(CC) $(CFLAGS) $(XFLAGS)  -c $<

thash.o:	$(TEST)/thash.c
					$(CC) $(CFLAGS) $(XFLAGS)  -c $<

tqueue:		queue.o tutils.o tqueue.o
					$(CC) $(CFLAGS) $(XFLAGS)  queue.o tutils.o tqueue.o -o $@

thash:		hash.o queue.o tutils.o thash.o
					$(CC) $(CFLAGS) $(XFLAGS)  queue.o hash.o tutils.o thash.o -o $@

# testing target
tests:		tqueue thash
					all.test

# valgrind target
grind:		tqueue thash
					grind.test

# coverage target
gcov:			tqueue thash
					all.test
					gcov hash.c
					gcov queue.c

gprof:		tqueue thash
					runtest.sh "thash 10000"
					gprof --brief thash gmon.out > gprof.analysis

clean:
					rm -f *.o thash tqueue *.gcda *.gcno *.gcov gmon.out gprof.analysis


