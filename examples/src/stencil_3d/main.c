///
/// @copyright Copyright (c) 2016-, Issam SAID <said.issam@gmail.com>
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without
/// modification, are permitted provided that the following conditions
/// are met:
///
/// 1. Redistributions of source code must retain the above copyright
///    notice, this list of conditions and the following disclaimer.
/// 2. Redistributions in binary form must reproduce the above copyright
///    notice, this list of conditions and the following disclaimer in the
///    documentation and/or other materials provided with the distribution.
/// 3. Neither the name of the copyright holder nor the names of its 
///    contributors may be used to endorse or promote products derived from 
///    this software without specific prior written permission.
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
/// @file stencil_3d/main.c
/// @author Issam SAID
/// @brief An example of 3D finite difference stencil code based on 
/// the ezcu C/C++ interface.
///
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ezcu/ezcu.h>

#define N 320

///
/// @brief The main program of the ezcu based sgemm C/C++ example.
///
/// This is the main routine that shows how to use the ezcu C/C++ interface 
/// to implement a simple matrix to matrix multiplication.
/// Note that the CUDA kernel is implemented in a seperate file (sgemm.cl).
/// @return Error code if any.
///
int main(void) {
    unsigned int i;
    float *u;
    float *v;
    float *coeffs;
    
    unsigned int grid[3]  = {N/16, N/16, 1};
    unsigned int block[3] = {16, 16, 1};
    
    ezcu_dev_t device;

    fprintf(stdout, "... start of the ezcu 3D stencil C/C++ example\n");

    ///
    ///< Initialize ezcu with selecting the default GPU.
    ///
    ezcu_init(GPU);

    ///
    ///< Load the CUDA kernel that runs the stencil computations.
    ///
    ezcu_load(PREFIX"/stencil_3d.cu", NULL);

    ///
    ///< Get a pointer to the desired device (in this case the default GPU).
    ///
    device = ezcu_dev_find(DEFAULT);

    u      = (float*)malloc((N+8)*(N+8)*(N+8)*sizeof(float));
    v      = (float*)malloc((N+8)*(N+8)*(N+8)*sizeof(float));
    coeffs = (float*)malloc(5*sizeof(float));

    memset(u,   0, (N+8)*(N+8)*(N+8)*sizeof(float));
    srand (time(NULL));
#pragma omp parallel for private(i)
    for (i = 0; i< (N+8)*(N+8)*(N+8); ++i) v[i] = (float)(i%100);

    coeffs[0] = -2.6;
    coeffs[1] =  1.6;
    coeffs[2] = -0.2;
    coeffs[3] =  0.02;
    coeffs[4] = -0.001;

    /// 
    ///< Wrap the matrices into ezcu memory objects.
    ///
    ezcu_mem_wrap(device, u, (N+8)*(N+8)*(N+8), FLOAT | READ_WRITE  | HWA);
    ezcu_mem_wrap(device, v, (N+8)*(N+8)*(N+8), FLOAT | READ_WRITE  | HWA);
    ezcu_mem_wrap(device, coeffs, 5, FLOAT | READ_ONLY | HWA);

    ///
    ///< Set the work size an dimension of the kernel.
    ///
    ezcu_knl_set_wrk("stencil_3d", 2, grid, block);

    /// 
    ///< Run the kernel on the default GPU.
    ///
    ezcu_knl_run("stencil_3d", device, v, u, coeffs, N, N, N);

    ///
    ///< Update the u buffer on the CPU side so that the results can be seen
    ///< on the host side.
    ///
    ezcu_mem_update(u, READ_ONLY);

    free(u);
    free(v);
    free(coeffs);

    ///
    ///< Release ezcu resources.
    ///
    ezcu_release();
    fprintf(stdout, "... end   of the ezcu 3D stencil C/C++ example\n");
    return EXIT_SUCCESS;
}
