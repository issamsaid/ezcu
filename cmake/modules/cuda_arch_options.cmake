## supported CUDA architectures.
set (SM30 -gencode=arch=compute_30,code=\\\"sm_30,compute_30\\\") 
set (SM32 -gencode=arch=compute_32,code=\\\"sm_32,compute_32\\\")
set (SM35 -gencode=arch=compute_35,code=\\\"sm_35,compute_35\\\") 
set (SM50 -gencode=arch=compute_50,code=\\\"sm_50,compute_50\\\")
set (SM52 -gencode=arch=compute_52,code=\\\"sm_52,compute_52\\\")
set (SM53 -gencode=arch=compute_53,code=\\\"sm_53,compute_53\\\")
set (SM60 -gencode=arch=compute_60,code=\\\"sm_60,compute_60\\\")
set (SM61 -gencode=arch=compute_61,code=\\\"sm_61,compute_61\\\")
set (SM62 -gencode=arch=compute_62,code=\\\"sm_62,compute_62\\\")
set (SM70 -gencode=arch=compute_70,code=\\\"sm_70,compute_70\\\")

## if compiling against CUDA Toolkit 6.x
if(CUDA_VERSION_MAJOR MATCHES 6)
  set(CUDA_ARCH "SM30 SM32 SM50 SM52 SM53 SM60" 
      CACHE STRING
      "target arch: SM30 SM32 SM50 SM52 SM53 SM60")
endif(CUDA_VERSION_MAJOR MATCHES 6)   

## if compiling against CUDA Toolkit 7.x
if(CUDA_VERSION_MAJOR MATCHES 7)
  if(CUDA_VERSION_MINOR MATCHES 0)
    set(CUDA_ARCH "SM30 SM32 SM50 SM52 SM53 SM60" 
        CACHE STRING
        "target arch: SM30 SM32 SM50 SM52 SM53 SM60")
  else(CUDA_VERSION_MINOR MATCHES 0)  
    set(CUDA_ARCH "SM30 SM32 SM50 SM52 SM53 SM60 SM61 SM62" 
        CACHE STRING
        "target arch: SM30 SM32 SM50 SM52 SM53 SM60 SM61 SM62")
  endif(CUDA_VERSION_MINOR MATCHES 0)  
endif(CUDA_VERSION_MAJOR MATCHES 7)

## if compiling against CUDA Toolkit 8.x
if(CUDA_VERSION_MAJOR MATCHES 8)
  if(CUDA_VERSION_MINOR MATCHES 0)
    set(CUDA_ARCH "SM30 SM32 SM50 SM52 SM53 SM60 SM61 SM62" 
        CACHE STRING
        "target arch: SM30 SM32 SM50 SM52 SM53 SM60 SM61 SM62")
  else(CUDA_VERSION_MINOR MATCHES 0)  
    set(CUDA_ARCH "SM30 SM32 SM50 SM52 SM53 SM60 SM61 SM62" 
        CACHE STRING
        "target arch: SM30 SM32 SM50 SM52 SM53 SM60 SM61 SM62")
  endif(CUDA_VERSION_MINOR MATCHES 0)  
endif(CUDA_VERSION_MAJOR MATCHES 8)

## if compiling against CUDA Toolkit 9.x
if(CUDA_VERSION_MAJOR MATCHES 9)
  if(CUDA_VERSION_MINOR MATCHES 0)
    set(CUDA_ARCH "SM30 SM32 SM50 SM52 SM53 SM60 SM61 SM62 SM70" 
        CACHE STRING
        "target arch: SM30 SM32 SM50 SM52 SM53 SM60 SM61 SM62 SM70")
  else(CUDA_VERSION_MINOR MATCHES 0)  
    set(CUDA_ARCH "SM30 SM32 SM50 SM52 SM53 SM60 SM61 SM62 SM70" 
        CACHE STRING
        "target arch: SM30 SM32 SM50 SM52 SM53 SM60 SM61 SM62 SM70")
  endif(CUDA_VERSION_MINOR MATCHES 0)  
endif(CUDA_VERSION_MAJOR MATCHES 9)

## replace ' ' with ; to match the proper cmake format
string(REGEX REPLACE " " ";" CUDA_ARCH ${CUDA_ARCH})

## set the compiler flags for each NV target
foreach(target ${CUDA_ARCH})
	set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} ${${target}}")
endforeach(target ${CUDA_ARCH})