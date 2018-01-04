# -DCMAKE_LIBRARY_PATH=${CUDA_HOME}/lib64/stubs
cmake  -Dezcu_debug=off -DNVRTC_LIBRARY=${CUDA_HOME}/lib64/stubs/libnvrtc.so -DCUDA_CUDA_LIBRARY=${CUDA_HOME}/lib64/stubs/libcuda.so -DCMAKE_C_COMPILER=icc  -DCMAKE_CXX_COMPILER=icpc -DCMAKE_Fortran_COMPILER=ifort -G "Unix Makefiles" ../

