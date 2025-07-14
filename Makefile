
# If using GCC on with mdspan and stdBLAS already installed...
#   * STDBLAS_INCL assumes it is installed in ./local/installed/
#STDBLAS_INCL=-I./local/installed/include -I./local/installed/include/experimental
#CXX=g++
#CXXFLAGS=-std=c++20 $(STDBLAS_INCL) -O2 -march=native
#LDFLAGS=-lblas -ltbb

# If using NVIDIA HPC SDK on a CPU...
CXX=nvc++
CXXFLAGS=--c++23 -O2 -march=native -stdpar=multicore
LDFLAGS=-lblas #-static-libgcc -static-libstdc++

# If using NVIDIA HPC SDK to run parallel algorithms on a GPU
# (this needs NVIDIA GPU hardware to run the executables)...
#CXX=nvc++
#CXXFLAGS=--c++23 -O2 -march=native -stdpar=gpu
#LDFLAGS=-cudalib=cublas

#all: generate_points.exe basic.exe task.exe
all: generate_points.exe task.exe

clean:
	rm -f *.exe

run: generate_points.exe task.exe
	./generate_points.exe 5 | ./task.exe 90

generate_points.exe : generate_points.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

%.exe : %.cpp coss2025_cpp_course.hpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

