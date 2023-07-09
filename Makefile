CXXFLAGS= -g -std=gnu++23


dedup: dedup.o FileDB2.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -lssl -lcrypto

dedup.o: dedup.cc

FileDB2.o: FileDB2.cc FileDB2.h Sha512.h

valgrind: dedup
	valgrind --leak-check=full ./dedup test*

.PHONY: clean tags beautify

clean:
	rm -f dedup *.o

beautify:
	clang-format -style=file -i *.cc
	clang-format -style=file -i *.h

tags:
	ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .
