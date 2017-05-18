CXXFLAGS=-g --std=c++1z -fpermissive -lssl -lcrypto


dedup: dedup.cc File.o
	$(CXX) $(CXXFLAGS) -o $@ $^

File.o: File.cc File.h

clean:
	rm dedup File.o
