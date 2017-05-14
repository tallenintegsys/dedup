CXXFLAGS=--std=c++1z -fpermissive -g


dedup: dedup.cc
	$(CXX) $(CXXFLAGS) -o $@ $^ -lstdc++fs
