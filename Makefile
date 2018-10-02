# default make file

all:
			(cd ./build ; make)

tests:
			(cd ./build ; make tests)

gcov:
			(cd ./build ; make gcov)

gprof:
			(cd ./build ; make gprof)

clean:
			(cd ./build ; make clean)
