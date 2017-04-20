///
/// @copyright Copyright (c) 2016-, Issam SAID <said.issam@gmail.com>
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without
/// modification, are permetted provided that the following conditions
/// are met:
///
/// 1. Redistributions of source code must retain the above copyright
///    notice, this list of conditions and the following disclaimer.
/// 2. Redistributions in binary form must reproduce the above copyright
///    notice, this list of conditions and the following disclaimer in the
///    documentation and/or other materials provided with the distribution.
/// 3. Neither the name of the COPYRIGHT HOLDER nor the names of its contributors
///    may be used to endorse or promote products derived from this software
///    without specific prior written permission.
///
/// THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
/// INCLUDING, BUT NOT LIMITED TO, WARRANTIES OF MERCHANTABILITY AND FITNESS
/// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
/// HOLDER OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
/// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
/// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
/// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
/// LIABILITY, WETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
/// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
/// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///
/// @file test/src/mem_test.cu
/// @author Issam SAID
/// @brief the CUDA kernels used for ezCU memory utilities tesing.
///
#define S(m, z, y, x) m[(8+dimx)*((8+dimy)*(z+4) + y+4) + x+4]
#define DS      4
#define ZRO     4
#define ONE     5
#define TWO     6
#define TRE     7
#define FOR     8
#define   U0(z, y, x)    u0[(2*DS+dimx)*((2*DS+dimy)*(z+DS) + (y+DS)) + x+DS]
#define   U1(z, y, x)    u1[(2*DS+dimx)*((2*DS+dimy)*(z+DS) + y+DS) + x+DS]
#define ROC2(z, y, x)  roc2[dimx*(dimy*(z) + y ) + x] 
#define  ETA(z, y, x)   eta[(dimx+2)*((dimy+2)*(z+1) + (y+1) ) + (x+1)]
#define  PHI(z, y, x)   phi[dimx*(dimy*(z) + y ) + x]
#define    L(y, x)        l[y+DS][x+DS]

extern "C" __global__ void kselect(float *input, 
                                   int     dimx, 
                                   int     dimy, 
                                   int     dimz) {
    int xgid = threadIdx.x + blockIdx.x*blockDim.x;
    int ygid = threadIdx.y + blockIdx.y*blockDim.y;
    int zgid;
    // Z
    if( (xgid<dimx) && (ygid<dimy) ) { 
        for(zgid = 0; zgid < 4; ++zgid) {
            S(input, zgid, ygid, xgid) = 
                (8+dimx)*((8+dimy)*(zgid+4) + (ygid+4)) + xgid+4;
        }
        for(zgid = dimz-4; zgid < dimz; ++zgid) {
            S(input, zgid, ygid, xgid) = 
                (8+dimx)*((8+dimy)*(zgid+4) + (ygid+4)) + xgid+4;
        }
    }
    // X
    if(( (xgid >= 0) && (xgid<4) ) || ( (xgid >= (dimx-4)) && (xgid<dimx) )) { 
        for(zgid = 0; zgid < dimz; ++zgid) {
            S(input, zgid, ygid, xgid) = 
                (8+dimx)*((8+dimy)*(zgid+4) + (ygid+4)) + xgid+4;
        }
    }
    // Y
    if(( (ygid >= 0) && (ygid<4) ) || ( (ygid >= (dimy-4)) && (ygid<dimy) )) { 
        for(zgid = 0; zgid < dimz; ++zgid) {
            S(input, zgid, ygid, xgid) = 
                (8+dimx)*((8+dimy)*(zgid+4) + (ygid+4)) + xgid+4;
        }
    }

}

///
/// size should be dimx x dimy
/// pack dimx * dimy * lz entries
/// tab[DS][dimy][dimx]
///
extern "C" __global__ void pack_south(int   dimx, 
                                      int   dimy, 
                                      int   dimz,
                                      float  *u0,
                                      float *tab) {
    int xgid = threadIdx.x + blockIdx.x*blockDim.x;
    int ygid = threadIdx.y + blockIdx.y*blockDim.y;
    int zgid;
    
    if ((xgid<dimx) && (ygid<dimy)) { 
        for(zgid = 0; zgid < DS; ++zgid) {
            tab[xgid + dimx*(ygid+(zgid*dimy))] = U0(zgid, ygid, xgid);
        }
    }
}

///
/// size should be dimx x dimy
/// unpack dimx * dimy * lz entries
/// tab[DS][dimy][dimx]
///
extern "C" __global__ void unpack_south(int  dimx, 
                                        int  dimy, 
                                        int  dimz,
                                        float *u0,
                                        float *tab) {
    int xgid = threadIdx.x + blockIdx.x*blockDim.x;
    int ygid = threadIdx.y + blockIdx.y*blockDim.y;
    int zgid;
    
    if ((xgid<dimx) && (ygid<dimy)) { 
        for(zgid = 0; zgid < DS; ++zgid) {
            U0(zgid, ygid, xgid) = tab[xgid + dimx*(ygid+(zgid*dimy))];
        }
    }
}

///
/// size should be dimx x dimy
/// pack dimx * dimy * lz entries
/// tab[DS][dimy][dimx]
///
extern "C" __global__ void pack_north(int  dimx, 
                                      int  dimy, 
                                      int  dimz,
                                      float *u0,
                                      float *tab) {
    int xgid = threadIdx.x + blockIdx.x*blockDim.x;
    int ygid = threadIdx.y + blockIdx.y*blockDim.y;
    int zgid;
    
    if ((xgid<dimx) && (ygid<dimy)) { 
        for(zgid = dimz-DS; zgid < dimz; ++zgid) {
            tab[xgid + dimx*(ygid+((zgid-dimz+DS)*dimy))] = 
                U0(zgid, ygid, xgid);
        }
    }
}

///
/// size should be dimx x dimy
/// unpack dimx * dimy * lz entries
/// tab[DS][dimy][dimx]
///
extern "C" __global__ void unpack_north(int  dimx, 
                                        int  dimy, 
                                        int  dimz,
                                        float *u0,
                                        float *tab) {
    int xgid = threadIdx.x + blockIdx.x*blockDim.x;
    int ygid = threadIdx.y + blockIdx.y*blockDim.y;
    int zgid;
    
    if ((xgid<dimx) && (ygid<dimy)) { 
        for(zgid = dimz-DS; zgid < dimz; ++zgid) {
            U0(zgid, ygid, xgid) = tab[xgid + 
                    dimx*(ygid+((zgid-dimz+DS)*dimy))];
        }
    }
}

///
/// size should be DS x dimy
/// pack DS * dimy * dimz entries
/// tab[dimz][dimy][DS]
extern "C" __global__ void pack_west(int  dimx, 
                                     int  dimy, 
                                     int  dimz,
                                     float *u0,
                                     float *tab) {
    int xgid = threadIdx.x + blockIdx.x*blockDim.x;
    int ygid = threadIdx.y + blockIdx.y*blockDim.y;
    int zgid;
    if((xgid >= 0) && (xgid<DS)) { 
        for(zgid = 0; zgid < dimz; ++zgid) {
            tab[xgid + DS*(ygid+(zgid*dimy))] = U0(zgid, ygid, xgid);
        }
    }
}

///
/// size should be DS x dimy
/// unpack DS * dimy * dimz entries
/// tab[dimz][dimy][DS]
///
extern "C" __global__ void unpack_west(int  dimx, 
                                       int  dimy, 
                                       int  dimz,
                                       float *u0,
                                       float *tab) {
    int xgid = threadIdx.x + blockIdx.x*blockDim.x;
    int ygid = threadIdx.y + blockIdx.y*blockDim.y;
    int zgid;
    if((xgid >= 0) && (xgid<DS)) { 
        for(zgid = 0; zgid < dimz; ++zgid) {
            U0(zgid, ygid, xgid) = tab[xgid + DS*(ygid+(zgid*dimy))];
        }
    }
}

///
/// size should be DS x dimy
/// pack DS * dimy * dimz entries
/// tab[dimz][dimy][DS]
///
extern "C" __global__ void pack_east(int  dimx, 
                                     int  dimy, 
                                     int  dimz,
                                     float *u0,
                                     float *tab) {
    int xgid = threadIdx.x + blockIdx.x*blockDim.x;
    int ygid = threadIdx.y + blockIdx.y*blockDim.y;
    int zgid;
    if((xgid>=0) && (xgid<4)) { 
        for(zgid = 0; zgid < dimz; ++zgid) {
            tab[xgid + DS*(ygid+(zgid*dimy))] = U0(zgid, ygid, xgid+dimx-DS);
        }
    }
}

///
/// size should be DS x dimy
/// unpack DS * dimy * dimz entries
/// tab[dimz][dimy][DS]
///
extern "C" __global__ void unpack_east(int  dimx, 
                                       int  dimy, 
                                       int  dimz,
                                       float *u0,
                                       float *tab) {
    int xgid = threadIdx.x + blockIdx.x*blockDim.x;
    int ygid = threadIdx.y + blockIdx.y*blockDim.y;
    int zgid;
    if((xgid>=0) && (xgid<4)) { 
        for(zgid = 0; zgid < dimz; ++zgid) {
            U0(zgid, ygid, xgid+dimx-DS) = tab[xgid + DS*(ygid+(zgid*dimy))];
        }
    }
}

///
/// size should be dimx x DS
/// pack dimx * ly * dimz entries
/// tab[dimz][DS][dimx]
///
extern "C" __global__ void pack_front(int  dimx, 
                                      int  dimy, 
                                      int  dimz,
                                      float *u0,
                                      float *tab) {
    int xgid = threadIdx.x + blockIdx.x*blockDim.x;
    int ygid = threadIdx.y + blockIdx.y*blockDim.y;
    int zgid;
    if((ygid >= 0) && (ygid<DS)) { 
        for(zgid = 0; zgid < dimz; ++zgid) {
            tab[xgid + dimx*(ygid+(zgid*DS))] = U0(zgid, ygid+dimy-DS, xgid);
        }
    }
}

///
/// size should be dimx x DS
/// unpack dimx * ly * dimz entries
/// tab[dimz][DS][dimx]
///
extern "C" __global__ void unpack_front(int  dimx, 
                                        int  dimy, 
                                        int  dimz,
                                        float *u0,
                                        float *tab) {
    int xgid = threadIdx.x + blockIdx.x*blockDim.x;
    int ygid = threadIdx.y + blockIdx.y*blockDim.y;
    int zgid;
    if((ygid >= 0) && (ygid<DS)) { 
        for(zgid = 0; zgid < dimz; ++zgid) {
            U0(zgid, ygid+dimy-DS, xgid) = tab[xgid + dimx*(ygid+(zgid*DS))];
        }
    }
}

///
/// size should be dimx x DS
/// pack dimx * ly * dimz entries
/// tab[dimz][DS][dimx]
///
extern "C" __global__ void pack_back(int  dimx, 
                                     int  dimy, 
                                     int  dimz,
                                     float *u0,
                                     float *tab) {
    int xgid = threadIdx.x + blockIdx.x*blockDim.x;
    int ygid = threadIdx.y + blockIdx.y*blockDim.y;
    int zgid;
    if((ygid >= 0) && (ygid<DS)) { 
        for(zgid = 0; zgid < dimz; ++zgid) {
            tab[xgid + dimx*(ygid+(zgid*DS))] = U0(zgid, ygid, xgid);
        }
    }
}

///
/// size should be dimx x DS
/// unpack dimx * ly * dimz entries
/// tab[dimz][DS][dimx]
///
extern "C" __global__ void unpack_back(int   dimx, 
                                       int   dimy, 
                                       int   dimz,
                                       float  *u0,
                                       float *tab) {
    int xgid = threadIdx.x + blockIdx.x*blockDim.x;
    int ygid = threadIdx.y + blockIdx.y*blockDim.y;
    int zgid;
    if((ygid >= 0) && (ygid<DS)) { 
        for(zgid = 0; zgid < dimz; ++zgid) {
            U0(zgid, ygid, xgid) = tab[xgid + dimx*(ygid+(zgid*DS))];
        }
    }
}
