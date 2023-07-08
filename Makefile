CXXFLAGS= -g -std=gnu++23


dedup: dedup.o FileDB.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -lssl -lcrypto

dedup.o: dedup.cc

FileDB.o: FileDB.cc FileDB.h


clean:
	rm -f dedup *.o

valgrind: dedup
	valgrind --leak-check=full ./dedup test*

.PHONY: tags beautify

beautify:
	clang-format -style=file -i *.cc
	clang-format -style=file -i *.h

tags:
	ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .
