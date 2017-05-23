CXXFLAGS= -g --std=c++14


dedup: dedup.o File.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -lssl -lcrypto

dedup.o: dedup.cc

File.o: File.cc File.h

clean:
	rm -f dedup *.o

valgrind: dedup
	valgrind --leak-check=full ./dedup testfiles/

