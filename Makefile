CXXFLAGS= -g --std=c++17


dedup: dedup.o File.o RootDirectory.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -lssl -lcrypto

dedup.o: dedup.cc

File.o: File.cc File.h

RootDirectory.o: RootDirectory.cc RootDirectory.h

clean:
	rm -f dedup *.o

valgrind: dedup
	valgrind --leak-check=full ./dedup testfiles/

.PHONY: tags
tags:
	ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .
