CC=gcc
SRC=../src
TEST=../test
CFLAGS=-Wall -pedantic -std=c11 -I$(SRC) -I$(TEST) -g -pg --coverage

all:			tqueue thash

tests:		tqueue thash
					all.test
					grind.test

gcov:			tqueue thash
# run all the tests
					all.test
# see how much was covered
					gcov hash.c
					gcov queue.c

gprof:		tqueue thash
# run the code
					runtest.sh "thash 10000"
					# run gprof
					gprof thash gmon.out > gprof.analysis

tutils.o:	$(TEST)/tutils.c $(TEST)/tutils.h
					$(CC) $(CFLAGS) -c $<

tqueue.o:	$(TEST)/tqueue.c
					$(CC) $(CFLAGS) -c $<

thash.o:	$(TEST)/thash.c
					$(CC) $(CFLAGS) -c $<

%.o:			$(SRC)/%.c $(SRC)/%.h
					$(CC) $(CFLAGS) -c $<

tqueue:		queue.o tutils.o tqueue.o
					$(CC) $(CFLAGS) queue.o tutils.o tqueue.o -o $@

thash:		hash.o queue.o tutils.o thash.o
					$(CC) $(CFLAGS) queue.o hash.o tutils.o thash.o -o $@

clean:
					rm -f *.o thash tqueue *.gcda *.gcno *.gcov gmon.out gprof.analysis


