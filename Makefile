CXXFLAGS=--std=c++1z -fpermissive -lssl -lcrypto


dedup: dedup.cc
	$(CXX) $(CXXFLAGS) -o $@ $^ 

clean:
	rm dedup
