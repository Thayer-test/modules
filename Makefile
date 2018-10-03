# default make file

all:
			(cd ./build ; make)

tests:
			(cd ./build ; make tests)

grind:
			(cd ./build ; make grind)

gcov:
			(cd ./build ; make gcov)

# use special flags for profiling
gprof:
			(cd ./build ; make clean ; make gprof XFLAGS='-pg -O' ; make clean)

clean:
			(cd ./build ; make clean)
