CXX=g++
CXX_FLAGS=-Iinclude -Wall -O3 -std=c++23

@all: ./builddir ./builddir/libmummy.o ./builddir/scale.exe ./builddir/main.exe ./builddir/compare.exe ./builddir/gen_benchmark.exe ./builddir/use_benchmark.exe

./builddir/compare.exe: ./src/compare.cc ./builddir/libmummy.o
	${CXX} ${CXX_FLAGS} ./src/compare.cc ./builddir/libmummy.o -o ./builddir/compare.exe

./builddir/gen_benchmark.exe: ./src/gen_benchmark.cc ./builddir/libmummy.o
	${CXX} ${CXX_FLAGS} ./src/gen_benchmark.cc ./builddir/libmummy.o -o ./builddir/gen_benchmark.exe

./builddir/use_benchmark.exe: ./src/use_benchmark.cc ./builddir/libmummy.o
	${CXX} ${CXX_FLAGS} ./src/use_benchmark.cc ./builddir/libmummy.o -o ./builddir/use_benchmark.exe

./builddir/scale.exe: ./src/scale.cc ./builddir/libmummy.o
	${CXX} ${CXX_FLAGS} ./src/scale.cc ./builddir/libmummy.o -o ./builddir/scale.exe

./builddir/main.exe: ./src/main.cc ./builddir/libmummy.o
	${CXX} ${CXX_FLAGS} ./src/main.cc ./builddir/libmummy.o -o ./builddir/main.exe

./builddir/libmummy.o: ./src/library.cc
	${CXX} ${CXX_FLAGS} ./src/library.cc -c -o ./builddir/libmummy.o

./builddir:
	mkdir -p ./builddir

clean:
	rm -rf builddir
