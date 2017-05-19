CXXFLAGS=-g --std=c++1z -fpermissive


dedup: dedup.o File.o
	$(CXX) $(CXXFLAGS) -lssl -lcrypto -o $@ $^

dedup.o: dedup.cc

File.o: File.cc File.h

clean:
	rm -f dedup *.o
