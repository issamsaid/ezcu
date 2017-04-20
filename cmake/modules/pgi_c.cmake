set(CMAKE_C_FLAGS_DEBUG   "${CMAKE_C_FLAGS_DEBUG}  -Mpreprocess -w -Mextend  -D__EZCU_DEBUG -O0 -Mbounds -Mchkptr -Mchkstk -DLOG_DEBUG")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Mpreprocess -Mipa=fast -Minfo=opt -O3")
find_package(OpenMP)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mp=numa")
else (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Mnoopenmp")
endif (OPENMP_FOUND)
