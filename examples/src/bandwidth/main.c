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
/// 3. Neither the name of the UPMC nor the names of its contributors
///    may be used to endorse or promote products derived from this software
///    without specific prior written permission.
///
/// THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
/// INCLUDING, BUT NOT LIMITED TO, WARRANTIES OF MERCHANTABILITY AND FITNESS
/// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE UPMC OR
/// ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
/// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
/// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
/// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
/// LIABILITY, WETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
/// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
/// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///
/// @file bandwidth/main.c
/// @author Issam SAID
/// @brief An example of bandwidth benchmark code based on the ezcu C/C++
///        interface.
///
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ezcu/ezcu.h>
#include <uparser/uparser.h>

///
/// @brief The main program of the ezcu based bandwidth C/C++ example.
///
/// This is the main routine that shows how to use the ezcu C/C++ interface 
/// to implement a simple bandwidth test.
/// Note that the CUDA kernel is implemented in a separate file (bandwidth.cu).
/// @return Error code if any.
///
int main(int argc, char** argv) {
    double elapsed_time = 0.0;
    unsigned int i, n=100; 
    float  *a, *b;
    int N = 128, ilp = 1;
    unsigned int  grid[3] = {128,1,1};
    unsigned int block[3] = {32,1,1}; 
   
    ezcu_dev_t device;

    ///
    ///< Parse command line arguments.
    ///
    uparser_init(argc, argv);
    uparser_opt(0, "size",     "128", "size of buffers in MBytes");
    uparser_opt(0, "block", "32,1,1", "the CUDA block size");
    uparser_opt(0, "ilp",        "1", "number of grid points per thread");
    uparser_parse();

    uparser_get_int32("size",     &N);
    uparser_get_int32("block", block);
    uparser_get_int32("ilp",    &ilp);
    N       *= (1024*1024/4);
    grid[0]  = N/block[0]/ilp;
    
    fprintf(stdout, "... start of the ezcu bandwidth C/C++ example\n");
    
    ///
    ///< Initialize ezcu with selecting the default GPU.
    ///
    ezcu_init();

    ///
    ///< Load the CUDA kernel that runs the memory copy.
    ///
    ezcu_load(PREFIX"/bandwidth.cu", NULL);

    ///
    ///< Get a pointer to the desired device (in this case the default GPU).
    ///
    device = ezcu_dev_find(0);

    a = (float*)malloc(N*sizeof(float));
    b = (float*)malloc(N*sizeof(float));

    srand (time(NULL));
#pragma omp parallel for private(i)
    for (i = 0; i< N; ++i) a[i]  = i%2 == 0 ? -rand()%10 : rand()%10;
#pragma omp parallel for private(i)
    for (i = 0; i< N; ++i) b[i]  = 0;

    /// 
    ///< Wrap the buffers into ezcu memory objects.
    ///
    ezcu_mem_wrap(device, a, N, FLOAT | READ_ONLY  | HWA);
    ezcu_mem_wrap(device, b, N, FLOAT | WRITE_ONLY | HWA);

    ///
    ///< Set the work size and the dimensions of the kernel.
    ///
    ezcu_knl_set_wrk("bandwidth", 1, grid, block);

    /// 
    ///< Run the kernel on the default GPU.
    ///
    for (i = 0; i< n; ++i)
        elapsed_time += ezcu_knl_timed_run("bandwidth", device, a, b, ilp, N);

    ///
    ///< Update the b buffer on the CPU.
    ///
    ezcu_mem_update(b, READ_ONLY);

    ///
    ///< Make sure the results are valid.
    ///
    for (i = 0; i< N; ++i) {
        if (a[i] != b[i]) {
            fprintf(stdout, "... error in the element number %d %f != %f\n", 
                    i, b[i], a[i]);
            break;
        }
    }

    ///
    ///< Print report.
    ///
    fprintf(stdout, "... elapsed time: %20.8f %s (%8.2f GBytes/s)\n", 
            elapsed_time, ezcu_timer_uget(), 
            (n*(N*2)*sizeof(float)*1e-9)/(elapsed_time*ezcu_timer_coef()));
    
    free(a);
    free(b);

    ///
    ///< Release ezcu resources.
    ///
    ezcu_release();

    ///
    ///< Release the parser.
    ///
    uparser_release();
    fprintf(stdout, "... end   of the ezcu bandwidth C/C++ example\n");
    return EXIT_SUCCESS;
}
