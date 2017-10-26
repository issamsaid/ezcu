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
/// @file test/cuda/knl_test.cu
/// @author Issam SAID
/// @brief the CUDA kernels used for ezcu kernel utilities tesing.
/// 

#define G(m, z, y, x) m[(2*sx+dimx)*((2*sy+dimy)*(z+sz) + y+sy) + x+sx]
#define       W(z, x) u[(2*lx+dimx)*(z+lz) + x+lx]

extern "C" __global__ void test_knl_1(float *input, 
                                      float *output,
                                      int dimx) {
  int gid = threadIdx.x + blockIdx.x*blockDim.x;
  output[gid] = input[gid];
}

extern "C" __global__ void test_knl_2(float *input, 
                                      float *output,
                                      float *coefx,
                                      float *coefy,
                                      float *coefz,
                                      int dimx, int dimy, int dimz) {
  int gid = threadIdx.x + blockIdx.x*blockDim.x;
  output[gid] = input[gid];
}

extern "C" __global__ void test_knl_3(float *input, 
                                      float *output,
                                      int dimx, int dimy, int dimz) {
  int gid = threadIdx.x + blockIdx.x*blockDim.x;
  output[gid] = input[gid];
}

extern "C" __global__ void stencil_v_3d(float  *input,
                                        float *output,
                                        float  *coefx,
                                        float  *coefy,
                                        float  *coefz,
                                        int      dimx, 
                                        int      dimy, 
                                        int      dimz,
                                        int        sx,   
                                        int        sy, 
                                        int        sz) {
    int xgid  = threadIdx.x + blockIdx.x*blockDim.x;
    int ygid  = threadIdx.y + blockIdx.y*blockDim.y;
    int zgid;
    
    /// laplacian operator
    float  laplacian;
    float  current;	
    float b00;
    float b01;
    float b02;
    float b03;
    float f00;
    float f01;
    float f02;
    float f03;
    float  coef = coefx[0] + coefy[0] + coefz[0];
       
    if( (xgid<dimx) && (ygid<dimy) ) { 
        
        b03 = 0.;
        b02 = G(input, -4, ygid, xgid);
        b01 = G(input, -3, ygid, xgid);
        b00 = G(input, -2, ygid, xgid);
        current  = G(input, -1, ygid, xgid);
        f00  = G(input,  0, ygid, xgid);
        f01  = G(input,  1, ygid, xgid);
        f02  = G(input,  2, ygid, xgid);
        f03  = G(input,  3, ygid, xgid);
     
	//#pragma unroll 4
        for(zgid = 0; zgid < dimz; zgid++) {
            b03 = b02;
            b02 = b01;
            b01 = b00;
            b00 = current;
            current  = f00;
            f00  = f01;
            f01  = f02;
            f02  = f03;
            f03  = G(input, (zgid+sz), ygid, xgid);

            laplacian  = coef * current
                + coefx[1] * ( G(input, zgid, ygid, xgid+1) +
                               G(input, zgid, ygid, xgid-1) )
                + coefx[2] * ( G(input, zgid, ygid, xgid+2) +
                               G(input, zgid, ygid, xgid-2) )
                + coefx[3] * ( G(input, zgid, ygid, xgid+3) +
                               G(input, zgid, ygid, xgid-3) )
                + coefx[4] * ( G(input, zgid, ygid, xgid+4) +
                               G(input, zgid, ygid, xgid-4) )

                + coefy[1] * ( G(input, zgid, ygid+1, xgid) +
                               G(input, zgid, ygid-1, xgid) )
                + coefy[2] * ( G(input, zgid, ygid+2, xgid) +
                               G(input, zgid, ygid-2, xgid) )
                + coefy[3] * ( G(input, zgid, ygid+3, xgid) +
                               G(input, zgid, ygid-3, xgid) )
                + coefy[4] * ( G(input, zgid, ygid+4, xgid) +
                               G(input, zgid, ygid-4, xgid) )

                + coefz[1] * ( f00 + b00 )
                + coefz[2] * ( f01 + b01 )
                + coefz[3] * ( f02 + b02 )
                + coefz[4] * ( f03 + b03 );
            G(output, zgid, ygid, xgid) = current + laplacian;
        }
    }
}

extern "C" __global__ void add_source_2d(int ix, int iz,
                                         int ix_p, int iz_p, 
                                         float rxt, float rzt, 
                                         int dimx,                         
                                         int lx, int lz, 
                                         float* source, 
                                         int it, float* u) {
  float src = source[it];
  W(iz, ix)     +=  src*(1.f-rxt)*(1.f-rzt);
  W(iz_p, ix)   +=  src*rzt*(1.f-rxt);
  W(iz,   ix_p) +=  src*(1.f-rzt)*rxt;
  W(iz_p, ix_p) +=  src*rzt*rxt;
}
