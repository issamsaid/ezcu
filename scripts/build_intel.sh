#!/bin/bash
cmake -DCMAKE_C_COMPILER=icc                                       \
      -DCMAKE_CXX_COMPILER=icpc                                    \
			-DCMAKE_Fortran_COMPILER=ifort                               \
	    -DCMAKE_CUDA_HOST_COMPILER=${INTEL_CC_HOME}/bin/intel64/icpc \
	    -DCMAKE_CUDA_FLAGS="-std=c++11"                              \
			-G "Unix Makefiles" ../

