CXXFLAGS= -g -std=gnu++23


dedup: dedup.o FileDB.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -lssl -lcrypto

dedup.o: dedup.cc

FileDB.o: FileDB.cc FileDB.h Sha512.h

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
