#!/bin/bash -x
# Remember the (present) current working directory...
THISDIR=$(pwd)

# Tell BASH to stop running this script once their is an error...
set -e

# If using a cluster, remember to:
#   module reset
#   module load gcc/13 tbb flexiblas

# Install everything into ./local...
mkdir "${THISDIR}/local"
cd "${THISDIR}/local"

#
# The following builds are intentionally done using C++20...
#
git clone https://github.com/google/googletest.git
mkdir gtest.build
cd gtest.build
CC=gcc CXX=g++ cmake ../googletest \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=../installed \
  -DCMAKE_CXX_STANDARD=20 \
  -DCMAKE_CXX_EXTENSIONS=OFF
make
make install
cd ..

git clone https://github.com/kokkos/mdspan.git
mkdir mdspan.build
cd mdspan.build
CC=gcc CXX=g++ cmake ../mdspan \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=../installed \
  -DCMAKE_CXX_EXTENSIONS=OFF \
  -DCMAKE_CXX_STANDARD=20 \
  -DMDSPAN_CXX_STANDARD=20 \
  -DMDSPAN_ENABLE_TESTS=OFF \
  -DMDSPAN_ENABLE_BENCHMARKS=OFF \
  -DMDSPAN_ENABLE_TESTS=OFF \
  -DMDSPAN_ENABLE_EXAMPLES=OFF \
  -DMDSPAN_ENABLE_BENCHMARKS=OFF \
  -DMDSPAN_ENABLE_COMP_BENCH=OFF \
  -DMDSPAN_ENABLE_CUDA=OFF \
  -DMDSPAN_ENABLE_SYCL=OFF \
  -DMDSPAN_ENABLE_HIP=OFF \
  -DMDSPAN_ENABLE_OPENMP=OFF \
  -DMDSPAN_USE_SYSTEM_GTEST=OFF \
  -DMDSPAN_USE_SYSTEM_BENCHMARK=OFF \
  -DMDSPAN_INSTALL_STDMODE_HEADERS=ON \
  -DMDSPAN_USE_BRACKET_OPERATOR=0 \
  -DMDSPAN_USE_PAREN_OPERATOR=1
make
make install
cd ..

git clone https://github.com/kokkos/stdBLAS.git
mkdir stdblas.build
cd stdblas.build
CC=gcc CXX=g++ cmake ../stdBLAS \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=../installed \
  -DCMAKE_CXX_STANDARD=20 \
  -DCMAKE_CXX_EXTENSIONS=OFF \
  -DLINALG_ENABLE_KOKKOS=OFF \
  -DLINALG_ENABLE_TESTS=OFF \
  -DLINALG_ENABLE_BLAS=OFF \
  -DLINALG_ENABLE_EXAMPLES=OFF
# Notice the above command has ENABLE_BLAS turned off.
# If you've an installed BLAS it can be turned on.
make
make install
cd ..

# mdspan is missing an mdspan file that #includes mdspan.hpp so create a
# symlink that enables this...
#cd "${THISDIR}/local/installed/include/experimental/"
#ln -s ../mdspan/mdspan.hpp mdspan

# Done!
cd "${THISDIR}"
