CXXFLAGS= -g --std=c++17


dedup: dedup.o File.o DirectoryTree.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -lssl -lcrypto

dedup.o: dedup.cc

File.o: File.cc File.h

DirectoryTree.o: DirectoryTree.cc DirectoryTree.h

clean:
	rm -f dedup *.o

valgrind: dedup
	valgrind --leak-check=full ./dedup testfiles/

.PHONY: tags beautify

beautify:
	clang-format -style=file -i *.cc
	clang-format -style=file -i *.h

tags:
	ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .
