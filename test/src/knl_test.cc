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
/// @file knl_test.cc
/// @author Issam SAID
/// @brief Unit testing file for the ezcu kernels manipulation routines.
///
#include <math.h>
#include <ezcu/knl.h>
#include <ezcu/dev.h>
#include <ezcu/mem.h>
#include <ezcu/timer.h>
#include <ezcu/base.h>
#include <__ezcu/config/cuda.h>
#include <__ezcu/config/mem.h>
#include <__ezcu/util-inl.h>
#include <__ezcu/mem-inl.h>
#include <__ezcu/knl-inl.h>
#include <gtest/gtest.h>

#include "knl_performance.h"

extern ezcu_env_t ezcu;

namespace {

  const static int DIM = 256, ITER = 10;
  const static int N = 4*1024*1024/4;

  class KnlTest : public ::testing::Test {
  protected:
    ezcu_dev_t d;
    virtual void SetUp() {
      ezcu_init(DEFAULT);
      ezcu_timer_uset(MILLI_SECONDS);
      ezcu_load(PREFIX"/knl_test.cu", NULL);
      d = ezcu_dev_find(0);
    }
    virtual void TearDown() { ezcu_release(); }
  };

  void populate(float* buffer, size_t size) {
    unsigned int i;
    for (i = 0; i < size; ++i) buffer[i] = i;
  }

    // TEST_F(KnlTest, copy_kernel_hzc) {
    //     unsigned int i;
    //     float *hsrc;
    //     int ret_val;
    //     unsigned int grid[3] = {N/64, 1, 1}, block[3] = {64, 1, 1};

    //     ret_val = posix_memalign((void**)(&hsrc), 
    //             __EZCU_MEM_ALIGN_SIZE, N*sizeof(float));
    //     if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    //     float *hdst;
    //     ret_val = posix_memalign((void**)(&hdst), 
    //             __EZCU_MEM_ALIGN_SIZE, N*sizeof(float));
    //     if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}

    //     ezcu_mem_wrap(d, hsrc, N, HOST | ZERO_COPY | READ_WRITE);
    //     ezcu_mem_t dst = ezcu_mem_wrap(d, hdst, N, 
    //                                    HOST | ZERO_COPY | WRITE_ONLY);
    //     ezcu_mem_update(hsrc, WRITE_ONLY);
    //     populate(hsrc, N);

    //     ezcu_knl_set_wrk("test_knl_1", 1, grid, block);
    //     ezcu_knl_sync_run("test_knl_1", d, hsrc, hdst, N);

    //     ezcu_mem_update(hdst, READ_ONLY);

    //     for (i = 0; i < dst->size; ++i) ASSERT_FLOAT_EQ(hdst[i], i);

    //     free(hsrc);
    //     free(hdst);
    // }

  TEST_F(KnlTest, copy_kernel) {
    unsigned int i;
    float *hsrc;
    int ret_val;
    unsigned int grid[3] = {N/64, 1, 1}, block[3] = {64, 1, 1};

    ret_val = posix_memalign((void**)(&hsrc), 
                              __EZCU_MEM_ALIGN_SIZE, N*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    float *hdst;
    ret_val = posix_memalign((void**)(&hdst), 
                              __EZCU_MEM_ALIGN_SIZE, N*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}

    ezcu_mem_wrap(d, hsrc, N, HWA | READ_WRITE);
    ezcu_mem_t dst = ezcu_mem_wrap(d, hdst, N, HWA | WRITE_ONLY);
    ezcu_mem_update(hsrc, WRITE_ONLY);
    populate(hsrc, N);

    ezcu_knl_set_wrk("test_knl_1", 1, grid, block);
    ezcu_knl_sync_run("test_knl_1", d, hsrc, hdst, N);

    ezcu_mem_update(hdst, READ_ONLY);

    for (i = 0; i < dst->size; ++i) ASSERT_FLOAT_EQ(hdst[i], i);
    free(hsrc);
    free(hdst);
  }

  TEST_F(KnlTest, shared_mem_objects) {
    unsigned int i;
    ezcu_knl_t k1;
    ezcu_knl_t k2;
    float *hsrc;
    int ret_val;
    unsigned int grid[3] = {N/64, 1, 1}, block[3] = {64, 1, 1};
    ret_val = posix_memalign((void**)(&hsrc), 
      __EZCU_MEM_ALIGN_SIZE, N*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    float *hdst1;
    ret_val = posix_memalign((void**)(&hdst1), 
      __EZCU_MEM_ALIGN_SIZE, N*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    float *hdst2;
    ret_val = posix_memalign((void**)(&hdst2), 
      __EZCU_MEM_ALIGN_SIZE, N*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    float *ha;
    ret_val = posix_memalign((void**)(&ha), 
      __EZCU_MEM_ALIGN_SIZE, 4*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    float *hb;
    ret_val = posix_memalign((void**)(&hb), 
      __EZCU_MEM_ALIGN_SIZE, 4*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    float *hc;
    ret_val = posix_memalign((void**)(&hc), 
      __EZCU_MEM_ALIGN_SIZE, 4*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}

    ezcu_mem_wrap(d, hsrc, N, HWA | READ_WRITE);
    ezcu_mem_wrap(d, ha, 4, HWA | READ_WRITE);
    ezcu_mem_wrap(d, hb, 4, HWA | READ_WRITE);
    ezcu_mem_wrap(d, hc, 4, HWA | READ_WRITE);
    ezcu_mem_t dst1 = ezcu_mem_wrap(d, hdst1, N, HWA | WRITE_ONLY);
    ezcu_mem_t dst2 = ezcu_mem_wrap(d, hdst2, N, HWA | WRITE_ONLY);

    ezcu_mem_update(hsrc, WRITE_ONLY);
    populate(hsrc, N);

    ezcu_knl_set_wrk("test_knl_1", 1, grid, block);
    ezcu_knl_set_wrk("test_knl_2", 1, grid, block);
    ezcu_knl_set_wrk("test_knl_3", 1, grid, block);

    ezcu_knl_set_mem("test_knl_1", 0, hsrc);
    ezcu_knl_set_mem("test_knl_1", 1, hdst1);
    ezcu_knl_set_int32("test_knl_1", 2, N);

    ezcu_knl_set_mem("test_knl_2", 0, hsrc);
    ezcu_knl_set_mem("test_knl_2", 1, hdst2);
    ezcu_knl_set_mem("test_knl_2", 2, ha);
    ezcu_knl_set_mem("test_knl_2", 3, hb);
    ezcu_knl_set_mem("test_knl_2", 4, hc);
    ezcu_knl_set_int32("test_knl_2", 5, N);
    ezcu_knl_set_int32("test_knl_2", 6, N);
    ezcu_knl_set_int32("test_knl_2", 7, N);

    ezcu_knl_sync_exec("test_knl_1", d);
    ezcu_knl_sync_exec("test_knl_2", d);
    ezcu_mem_update(hdst1, READ_ONLY);
    ezcu_mem_update(hdst2, READ_ONLY);
    for (i = 0; i < dst1->size; ++i) ASSERT_FLOAT_EQ(hdst1[i], i);
    for (i = 0; i < dst2->size; ++i) ASSERT_FLOAT_EQ(hdst2[i], i);

    k1 = __ezcu_knl_find("test_knl_1");
    k2 = __ezcu_knl_find("test_knl_2");

    ASSERT_EQ(2, urb_tree_size(&k1->mems));
    ASSERT_EQ(5, urb_tree_size(&k2->mems));
    ASSERT_EQ(6, urb_tree_size(&ezcu->mems));

    free(hsrc);
    free(hdst1);
    free(hdst2);
    free(ha);
    free(hb);
    free(hc);
  }

  TEST_F(KnlTest, swap_mem_objects) {
    int i;
    ezcu_knl_t k1;
    float *hsrc;
    int ret_val;
    unsigned int grid[3] = {N/64, 1, 1}, block[3] = {64, 1, 1};
    ret_val = posix_memalign((void**)(&hsrc), 
      __EZCU_MEM_ALIGN_SIZE, N*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    float *hdst1;
    ret_val = posix_memalign((void**)(&hdst1), 
      __EZCU_MEM_ALIGN_SIZE, N*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    float *hdst2;
    ret_val = posix_memalign((void**)(&hdst2), 
      __EZCU_MEM_ALIGN_SIZE, N*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    float *ha;
    ret_val = posix_memalign((void**)(&ha), 
      __EZCU_MEM_ALIGN_SIZE, 4*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    float *hb;
    ret_val = posix_memalign((void**)(&hb), 
      __EZCU_MEM_ALIGN_SIZE, 4*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    float *hc;
    ret_val = posix_memalign((void**)(&hc), 
      __EZCU_MEM_ALIGN_SIZE, 4*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}

    ezcu_mem_wrap(d, hsrc,  N, HWA | READ_WRITE);
    ezcu_mem_wrap(d, hdst1, N, HWA | READ_WRITE);
    ezcu_mem_wrap(d, hdst2, N, HWA | READ_WRITE);
    ezcu_mem_update(hsrc, WRITE_ONLY);

    populate(hsrc, N);

    ezcu_knl_set_wrk("test_knl_1", 2, grid, block);
    ezcu_knl_set_mem("test_knl_1", 0, hsrc);
    ezcu_knl_set_mem("test_knl_1", 1, hdst1);
    ezcu_knl_set_int32("test_knl_1", 2, N);

    k1 = __ezcu_knl_find("test_knl_1");

    ezcu_knl_sync_exec("test_knl_1", d);
    ezcu_mem_update(hdst1, READ_ONLY);
    for (i = 0; i < N; ++i) ASSERT_FLOAT_EQ(hdst1[i], i);
      ASSERT_EQ(urb_tree_size(&k1->mems), 2);
    ASSERT_EQ(urb_tree_size(&ezcu->mems), 3);
    ezcu_knl_set_mem("test_knl_1", 0, hsrc);
    ezcu_knl_set_mem("test_knl_1", 1, hdst2);
    ezcu_knl_set_int32("test_knl_1", 2, N);
    ezcu_knl_sync_exec("test_knl_1", d);
    ezcu_mem_update(hdst2, READ_ONLY);
    for (i = 0; i <N; ++i) ASSERT_FLOAT_EQ(hdst2[i], i);
      ASSERT_EQ(urb_tree_size(&k1->mems), 2);
    ASSERT_EQ(urb_tree_size(&ezcu->mems), 3);

    ezcu_knl_sync_exec("test_knl_1", d);
    ezcu_mem_update(hsrc, READ_ONLY);
    for (i = 0; i <N; ++i) ASSERT_FLOAT_EQ(hsrc[i], i);
      ASSERT_EQ(urb_tree_size(&k1->mems), 2);
    ASSERT_EQ(urb_tree_size(&ezcu->mems), 3);

    free(hsrc);
    free(hdst1);
    free(hdst2);
    free(ha);
    free(hb);
    free(hc);
  }

  TEST_F(KnlTest, sync_exec_by_name) {
    int i;
    float *hsrc;
    int ret_val;
    unsigned int grid[3] = {N/64, 1, 1}, block[3] = {64, 1, 1};
    ret_val = posix_memalign((void**)(&hsrc), 
      __EZCU_MEM_ALIGN_SIZE, N*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    float *hdst;
    ret_val = posix_memalign((void**)(&hdst), 
      __EZCU_MEM_ALIGN_SIZE, N*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    ezcu_mem_wrap(d, hsrc, N, HWA | READ_ONLY);
    ezcu_mem_wrap(d, hdst, N, HWA | WRITE_ONLY);
    ezcu_mem_update(hsrc, WRITE_ONLY);
    populate(hsrc, N);
    ezcu_knl_set_wrk("test_knl_1", 1, grid, block);
    ezcu_knl_set_mem("test_knl_1", 0, hsrc);
    ezcu_knl_set_mem("test_knl_1", 1, hdst);
    ezcu_knl_set_int32("test_knl_1", 2, N);

    ezcu_timer_tick();
    for (i=0; i<ITER; ++i)
      ezcu_knl_sync_exec("test_knl_1", d);
    fprintf(stdout, 
      "... time: %f %s\n", ezcu_timer_read()/ITER, ezcu_timer_uget());
    ezcu_mem_update(hdst, READ_ONLY);
    for (i = 0; i < N; ++i) ASSERT_FLOAT_EQ(hdst[i], i);
      free(hsrc);
    free(hdst);
  }

  TEST_F(KnlTest, exec_by_name) {
    int i;
    float *hsrc;
    int ret_val;
    unsigned int grid[3] = {N/64, 1, 1}, block[3] = {64, 1, 1};
    ret_val = posix_memalign((void**)(&hsrc), 
              __EZCU_MEM_ALIGN_SIZE, N*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    float *hdst;
    ret_val = posix_memalign((void**)(&hdst), 
              __EZCU_MEM_ALIGN_SIZE, N*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    ezcu_mem_wrap(d, hsrc, N, HWA | READ_ONLY);
    ezcu_mem_wrap(d, hdst, N, HWA | WRITE_ONLY);
    ezcu_mem_update(hsrc, WRITE_ONLY);
    populate(hsrc, N);
    ezcu_knl_set_wrk("test_knl_1", 1, grid, block);
    ezcu_knl_set_mem("test_knl_1", 0, hsrc);
    ezcu_knl_set_mem("test_knl_1", 1, hdst);
    ezcu_knl_set_int32("test_knl_1", 2, N);
    ezcu_timer_tick();
    for (i=0; i<ITER; ++i) ezcu_knl_exec("test_knl_1", d);
      ezcu_dev_wait(d);
    fprintf(stdout, 
      "... time: %f %s\n", ezcu_timer_read()/ITER, ezcu_timer_uget());
    ezcu_mem_update(hdst, READ_ONLY);
    for (i = 0; i < N; ++i) ASSERT_FLOAT_EQ(hdst[i], i);
    free(hsrc);
    free(hdst);
  }

  ///
  /// Stencil 3D
  /// 
  void randomize_buffer(float* buffer, size_t size) {
    unsigned int i;
    for (i=0; i<size; ++i) {
      buffer[i] = (float)(i % 100);
    }
  }

  void setup_stencil_coeffs(float *t) {
    t[0] = -2.6f;
    t[1] =  1.6f;
    t[2] = -0.2f;
    t[3] =  0.02f;
    t[4] = -0.001f;
  }

#define O(u,z,y,x) (u[(x+s[0]) + (2*s[0] + dim[0]) *            \
                      ((2*s[1] + dim[1]) * (z+s[2]) + (y+s[1]))])

  void check_stencil_3d(int *dim, int *s,
                        float *coefx, float *coefy, float *coefz,
                        float *i, float *res, float e) {
    int x, y, z;
    float laplacian, coef0;
    size_t size = (2*s[0] + dim[0])*(2*s[1] + 
      dim[1])*(2*s[2] + dim[2])*sizeof(float);
    float *o = (float*)(malloc(size));
    coef0 = coefx[0] + coefy[0] + coefz[0];
    memset(o, 0, size);

#pragma omp parallel for private(laplacian, z, y, x)
    for(z = 0; z < dim[2] ; ++z) {
      for(y = 0; y < dim[1]; ++y) {
        for(x = 0; x < dim[0]; ++x) {
          laplacian = coef0 * O(i, z, y, x)                
                    + coefx[1]*( O(i, z,   y,   x+1) + O(i, z,   y,   x-1))
                    + coefy[1]*( O(i, z,   y+1, x  ) + O(i, z,   y-1, x  ))
                    + coefz[1]*( O(i, z+1, y,   x  ) + O(i, z-1, y,   x  ))
                        
                    + coefx[2]*( O(i, z,   y,   x+2) + O(i, z,   y,   x-2))
                    + coefy[2]*( O(i, z,   y+2, x  ) + O(i, z,   y-2, x  ))
                    + coefz[2]*( O(i, z+2, y,   x  ) + O(i, z-2, y,   x  ))
                        
                    + coefx[3]*( O(i, z,   y,   x+3) + O(i, z,   y,   x-3))
                    + coefy[3]*( O(i, z,   y+3, x  ) + O(i, z,   y-3, x  ))
                    + coefz[3]*( O(i, z+3, y,   x  ) + O(i, z-3, y,   x  ))
                    
                    + coefx[4]*( O(i, z,   y,   x+4) + O(i, z,   y,   x-4))
                    + coefy[4]*( O(i, z,   y+4, x  ) + O(i, z,   y-4, x  ))
                    + coefz[4]*( O(i, z+4, y,   x  ) + O(i, z-4, y,   x  ));
          O(o, z,y,x) = O(i, z,y,x) + laplacian;
        }
      }
    }
        
    for (z = 0; z < dim[2]; ++z)
      for (y = 0; y < dim[1]; ++y)
        for (x = 0; x < dim[0]; ++x)
          ASSERT_NEAR(O(o, z,y,x), O(res, z,y,x), e);
    free(o);
  }
    
  TEST_F(KnlTest, stencil_3d_wrapped_duplicated) {
    double comm, time = 0., flops;
    int i;
    float epsilon = 1.e-1;
    size_t size;
    int dim[3] = {DIM, DIM, DIM}, s[3] = {4, 4, 4};
    unsigned int grid[3] = {DIM/16, DIM/16, 1}, block[3] = {16, 16, 1};

    size   = (2*s[0] + dim[0])*(2*s[1] + dim[1])*(2*s[2] + dim[2]);
    flops  = 2+(3*s[0]+1)+(3*s[1]+1)+(3*s[2]+1);

    float *ui;
    float *uo;
    float *coefx;
    float *coefy;
    float *coefz;
    int ret_val;
    ret_val = posix_memalign((void**)(&ui),    
      __EZCU_MEM_ALIGN_SIZE,     size*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    ret_val = posix_memalign((void**)(&uo),    
      __EZCU_MEM_ALIGN_SIZE,     size*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    ret_val = posix_memalign((void**)(&coefx), 
      __EZCU_MEM_ALIGN_SIZE, (s[0]+1)*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    ret_val = posix_memalign((void**)(&coefy), 
      __EZCU_MEM_ALIGN_SIZE, (s[1]+1)*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    ret_val = posix_memalign((void**)(&coefz), 
      __EZCU_MEM_ALIGN_SIZE, (s[2]+1)*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}

    memset(uo, 0, size*sizeof(float));
    randomize_buffer(ui, size);
    setup_stencil_coeffs(coefx);
    setup_stencil_coeffs(coefy);
    setup_stencil_coeffs(coefz);

    ezcu_mem_wrap(d,    ui,   size, READ_ONLY);
    ezcu_mem_wrap(d,    uo,   size, WRITE_ONLY);
    ezcu_mem_wrap(d, coefx, s[0]+1, READ_ONLY);
    ezcu_mem_wrap(d, coefy, s[1]+1, READ_ONLY);
    ezcu_mem_wrap(d, coefz, s[2]+1, READ_ONLY);

    ezcu_knl_set_wrk("stencil_v_3d", 2, grid, block);
    ezcu_knl_set_mem("stencil_v_3d",    0, ui);
    ezcu_knl_set_mem("stencil_v_3d",    1, uo);
    ezcu_knl_set_mem("stencil_v_3d",    2, coefx);
    ezcu_knl_set_mem("stencil_v_3d",    3, coefy);
    ezcu_knl_set_mem("stencil_v_3d",    4, coefz);
    ezcu_knl_set_int32("stencil_v_3d",  5, dim[0]);
    ezcu_knl_set_int32("stencil_v_3d",  6, dim[1]);
    ezcu_knl_set_int32("stencil_v_3d",  7, dim[2]);
    ezcu_knl_set_int32("stencil_v_3d",  8,  s[0]);
    ezcu_knl_set_int32("stencil_v_3d",  9,  s[1]);
    ezcu_knl_set_int32("stencil_v_3d", 10, s[2]);

    for (i = 0; i < 5; ++i)
      ezcu_knl_sync_exec("stencil_v_3d", d);
    ezcu_knl_sync_run("stencil_v_3d", d, ui, uo, coefx, coefy, coefz, 
                       DIM, DIM, DIM, 4, 4, 4);
    ezcu_timer_tick();
    for (i=0; i<ITER; ++i) ezcu_knl_exec("stencil_v_3d", d);
      ezcu_dev_wait(d);

    time = ezcu_timer_read();
    ezcu_timer_tick();
    ezcu_mem_update(uo, READ_ONLY);
    comm = ezcu_timer_read();
    fprintf(stdout, 
      "... avertage time: %8.2f (%8.2f) %s, %8.2f (%8.2f) Gflop/s\n",
      time/ITER, comm + time/ITER, ezcu_timer_uget(),
      flops*1.e-9*ITER*dim[0]*dim[1]*dim[2]/(time*ezcu_timer_coef()),
      flops*1.e-9*dim[0]*dim[1]*dim[2]/
      ((comm+(time/ITER))*ezcu_timer_coef()));
    check_stencil_3d(dim, s, coefx, coefy, coefz, ui, uo, epsilon);
    free(ui);
    free(uo);
    free(coefx);
    free(coefy);
    free(coefz);
  }

  TEST_F(KnlTest, stencil_3d_wrapped_pinned) {
    double comm, time = 0., flops;
    int i;
    float epsilon = 1.e-1;
    size_t size;
    int dim[3] = {DIM, DIM, DIM}, s[3] = {4, 4, 4};
    unsigned int grid[3] = {DIM/16, DIM/16, 1}, block[3] = {16, 16, 1};

    size   = (2*s[0] + dim[0])*(2*s[1] + dim[1])*(2*s[2] + dim[2]);
    flops  = 2+(3*s[0]+1)+(3*s[1]+1)+(3*s[2]+1);

    float *ui;
    float *uo;
    float *coefx;
    float *coefy;
    float *coefz;
    int ret_val;
    ret_val = posix_memalign((void**)(&ui),    
      __EZCU_MEM_ALIGN_SIZE,     size*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    ret_val = posix_memalign((void**)(&uo),    
      __EZCU_MEM_ALIGN_SIZE,     size*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    ret_val = posix_memalign((void**)(&coefx), 
      __EZCU_MEM_ALIGN_SIZE, (s[0]+1)*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    ret_val = posix_memalign((void**)(&coefy), 
      __EZCU_MEM_ALIGN_SIZE, (s[1]+1)*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    ret_val = posix_memalign((void**)(&coefz), 
      __EZCU_MEM_ALIGN_SIZE, (s[2]+1)*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}

    memset(uo, 0, size*sizeof(float));
    randomize_buffer(ui, size);
    setup_stencil_coeffs(coefx);
    setup_stencil_coeffs(coefy);
    setup_stencil_coeffs(coefz);

    ezcu_mem_wrap(d,    ui,   size, READ_ONLY  | PINNED);
    ezcu_mem_wrap(d,    uo,   size, WRITE_ONLY | PINNED);
    ezcu_mem_wrap(d, coefx, s[0]+1, READ_ONLY  | PINNED);
    ezcu_mem_wrap(d, coefy, s[1]+1, READ_ONLY  | PINNED);
    ezcu_mem_wrap(d, coefz, s[2]+1, READ_ONLY  | PINNED);

    ezcu_knl_set_wrk("stencil_v_3d", 2, grid, block);
    ezcu_knl_set_mem("stencil_v_3d",    0, ui);
    ezcu_knl_set_mem("stencil_v_3d",    1, uo);
    ezcu_knl_set_mem("stencil_v_3d",    2, coefx);
    ezcu_knl_set_mem("stencil_v_3d",    3, coefy);
    ezcu_knl_set_mem("stencil_v_3d",    4, coefz);
    ezcu_knl_set_int32("stencil_v_3d",  5, dim[0]);
    ezcu_knl_set_int32("stencil_v_3d",  6, dim[1]);
    ezcu_knl_set_int32("stencil_v_3d",  7, dim[2]);
    ezcu_knl_set_int32("stencil_v_3d",  8,  s[0]);
    ezcu_knl_set_int32("stencil_v_3d",  9,  s[1]);
    ezcu_knl_set_int32("stencil_v_3d", 10, s[2]);

    for (i = 0; i < 5; ++i) ezcu_knl_sync_exec("stencil_v_3d", d);
    ezcu_knl_sync_run("stencil_v_3d", d, ui, uo, coefx, coefy, coefz, 
                       DIM, DIM, DIM, 4, 4, 4);
    ezcu_timer_tick();
    for (i=0; i<ITER; ++i) ezcu_knl_exec("stencil_v_3d", d);
    ezcu_dev_wait(d);
        
    time = ezcu_timer_read();
    ezcu_timer_tick();
    ezcu_mem_update(uo, READ_ONLY);
    comm = ezcu_timer_read();
    fprintf(stdout, 
            "... avertage time: %8.2f (%8.2f) %s, %8.2f (%8.2f) Gflop/s\n",
            time/ITER, comm + time/ITER, ezcu_timer_uget(),
            flops*1.e-9*ITER*dim[0]*dim[1]*dim[2]/(time*ezcu_timer_coef()),
            flops*1.e-9*dim[0]*dim[1]*dim[2]/
            ((comm+(time/ITER))*ezcu_timer_coef()));
    check_stencil_3d(dim, s, coefx, coefy, coefz, ui, uo, epsilon);
    free(ui);
    free(uo);
    free(coefx);
    free(coefy);
    free(coefz);
  }

  TEST_F(KnlTest, stencil_3d_wrapped_zc) {
    double comm, time = 0., flops;
    int i;
    float epsilon = 1.e-1;
    size_t size;
    int dim[3] = {DIM, DIM, DIM}, s[3] = {4, 4, 4};
    unsigned int grid[3] = {DIM/16, DIM/16, 1}, block[3] = {16, 16, 1};

    size   = (2*s[0] + dim[0])*(2*s[1] + dim[1])*(2*s[2] + dim[2]);
    flops  = 2+(3*s[0]+1)+(3*s[1]+1)+(3*s[2]+1);

    float *ui;
    float *uo;
    float *coefx;
    float *coefy;
    float *coefz;
    int ret_val;
    ret_val = posix_memalign((void**)(&ui),    
              __EZCU_MEM_ALIGN_SIZE,     size*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    ret_val = posix_memalign((void**)(&uo),    
              __EZCU_MEM_ALIGN_SIZE,     size*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    ret_val = posix_memalign((void**)(&coefx), 
              __EZCU_MEM_ALIGN_SIZE, (s[0]+1)*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    ret_val = posix_memalign((void**)(&coefy), 
              __EZCU_MEM_ALIGN_SIZE, (s[1]+1)*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    ret_val = posix_memalign((void**)(&coefz), 
              __EZCU_MEM_ALIGN_SIZE, (s[2]+1)*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}

    memset(uo, 0, size*sizeof(float));
    randomize_buffer(ui, size);
    setup_stencil_coeffs(coefx);
    setup_stencil_coeffs(coefy);
    setup_stencil_coeffs(coefz);

    ezcu_mem_wrap(d,    ui,   size, READ_ONLY  | HOST | ZERO_COPY);
    ezcu_mem_wrap(d,    uo,   size, WRITE_ONLY | HOST | ZERO_COPY);
    ezcu_mem_wrap(d, coefx, s[0]+1, READ_ONLY  | HOST | ZERO_COPY);
    ezcu_mem_wrap(d, coefy, s[1]+1, READ_ONLY  | HOST | ZERO_COPY);
    ezcu_mem_wrap(d, coefz, s[2]+1, READ_ONLY  | HOST | ZERO_COPY);

    ezcu_knl_set_wrk("stencil_v_3d", 2, grid, block);
    ezcu_knl_set_mem("stencil_v_3d",    0, ui);
    ezcu_knl_set_mem("stencil_v_3d",    1, uo);
    ezcu_knl_set_mem("stencil_v_3d",    2, coefx);
    ezcu_knl_set_mem("stencil_v_3d",    3, coefy);
    ezcu_knl_set_mem("stencil_v_3d",    4, coefz);
    ezcu_knl_set_int32("stencil_v_3d",  5, dim[0]);
    ezcu_knl_set_int32("stencil_v_3d",  6, dim[1]);
    ezcu_knl_set_int32("stencil_v_3d",  7, dim[2]);
    ezcu_knl_set_int32("stencil_v_3d",  8,  s[0]);
    ezcu_knl_set_int32("stencil_v_3d",  9,  s[1]);
    ezcu_knl_set_int32("stencil_v_3d", 10, s[2]);

    for (i = 0; i < 5; ++i)
      ezcu_knl_sync_exec("stencil_v_3d", d);
    ezcu_knl_sync_run("stencil_v_3d", d, ui, uo, coefx, coefy, coefz, 
                       DIM, DIM, DIM, 4, 4, 4);
    ezcu_timer_tick();
    for (i=0; i<ITER; ++i) ezcu_knl_exec("stencil_v_3d", d);
      ezcu_dev_wait(d);

    time = ezcu_timer_read();
    ezcu_timer_tick();
    ezcu_mem_update(uo, READ_ONLY);
    comm = ezcu_timer_read();
    fprintf(stdout, 
            "... avertage time: %8.2f (%8.2f) %s, %8.2f (%8.2f) Gflop/s\n",
            time/ITER, comm + time/ITER, ezcu_timer_uget(),
            flops*1.e-9*ITER*dim[0]*dim[1]*dim[2]/(time*ezcu_timer_coef()),
            flops*1.e-9*dim[0]*dim[1]*dim[2]/
            ((comm+(time/ITER))*ezcu_timer_coef()));
    check_stencil_3d(dim, s, coefx, coefy, coefz, ui, uo, epsilon);
    free(ui);
    free(uo);
    free(coefx);
    free(coefy);
    free(coefz);
  }

  TEST_F(KnlTest, stencil_3d_rw_wrapped_duplicated) {
    double comm, time = 0., flops;
    int i;
    float epsilon = 1.e-1;
    size_t size;
    int dim[3] = {DIM, DIM, DIM}, s[3] = {4, 4, 4};
    unsigned int grid[3] = {DIM/16, DIM/16, 1}, block[3] = {16, 16, 1};

    size   = (2*s[0] + dim[0])*(2*s[1] + dim[1])*(2*s[2] + dim[2]);
    flops  = 2+(3*s[0]+1)+(3*s[1]+1)+(3*s[2]+1);

    float *ui;
    float *uo;
    float *coefx;
    float *coefy;
    float *coefz;
    int ret_val;
    ret_val = posix_memalign((void**)(&ui),    
                             __EZCU_MEM_ALIGN_SIZE, size*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    ret_val = posix_memalign((void**)(&uo),    
                             __EZCU_MEM_ALIGN_SIZE, size*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    ret_val = posix_memalign((void**)(&coefx), 
                             __EZCU_MEM_ALIGN_SIZE, (s[0]+1)*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
    ret_val = posix_memalign((void**)(&coefy), 
                              __EZCU_MEM_ALIGN_SIZE, (s[1]+1)*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
      ret_val = posix_memalign((void**)(&coefz), 
                        __EZCU_MEM_ALIGN_SIZE, (s[2]+1)*sizeof(float));
    if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}

    memset(uo, 0, size*sizeof(float));
    randomize_buffer(ui, size);
    setup_stencil_coeffs(coefx);
    setup_stencil_coeffs(coefy);
    setup_stencil_coeffs(coefz);

    ezcu_mem_wrap(d,    ui,   size, DEFAULT);
    ezcu_mem_wrap(d,    uo,   size, DEFAULT);
    ezcu_mem_wrap(d, coefx, s[0]+1, READ_ONLY);
    ezcu_mem_wrap(d, coefy, s[1]+1, READ_ONLY);
    ezcu_mem_wrap(d, coefz, s[2]+1, READ_ONLY);

    ezcu_knl_set_wrk("stencil_v_3d", 2, grid, block);
    ezcu_knl_set_mem("stencil_v_3d",    0, ui);
    ezcu_knl_set_mem("stencil_v_3d",    1, uo);
    ezcu_knl_set_mem("stencil_v_3d",    2, coefx);
    ezcu_knl_set_mem("stencil_v_3d",    3, coefy);
    ezcu_knl_set_mem("stencil_v_3d",    4, coefz);
    ezcu_knl_set_int32("stencil_v_3d",  5, dim[0]);
    ezcu_knl_set_int32("stencil_v_3d",  6, dim[1]);
    ezcu_knl_set_int32("stencil_v_3d",  7, dim[2]);
    ezcu_knl_set_int32("stencil_v_3d",  8,  s[0]);
    ezcu_knl_set_int32("stencil_v_3d",  9,  s[1]);
    ezcu_knl_set_int32("stencil_v_3d", 10, s[2]);

    for (i = 0; i < 5; ++i)
      ezcu_knl_sync_exec("stencil_v_3d", d);
    ezcu_knl_sync_run("stencil_v_3d", d, ui, uo, coefx, coefy, coefz, 
                      DIM, DIM, DIM, 4, 4, 4);
    ezcu_timer_tick();
    for (i=0; i<ITER; ++i) ezcu_knl_exec("stencil_v_3d", d);
    ezcu_dev_wait(d);
        
    time = ezcu_timer_read();
    ezcu_timer_tick();
    ezcu_mem_update(uo, READ_ONLY);
    comm = ezcu_timer_read();
    fprintf(stdout, 
            "... avertage time: %8.2f (%8.2f) %s, %8.2f (%8.2f) Gflop/s\n",
            time/ITER, comm + time/ITER, ezcu_timer_uget(),
            flops*1.e-9*ITER*dim[0]*dim[1]*dim[2]/(time*ezcu_timer_coef()),
            flops*1.e-9*dim[0]*dim[1]*dim[2]/
                ((comm+(time/ITER))*ezcu_timer_coef()));
    check_stencil_3d(dim, s, coefx, coefy, coefz, ui, uo, epsilon);
    free(ui);
    free(uo);
    free(coefx);
    free(coefy);
    free(coefz);
  }

    TEST_F(KnlTest, stencil_3d_rw_wrapped_pinned) {
        double comm, time = 0., flops;
        int i;
        float epsilon = 1.e-1;
        size_t size;
        int dim[3] = {DIM, DIM, DIM}, s[3] = {4, 4, 4};
        unsigned int grid[3] = {DIM/16, DIM/16, 1}, block[3] = {16, 16, 1};

        size   = (2*s[0] + dim[0])*(2*s[1] + dim[1])*(2*s[2] + dim[2]);
        flops  = 2+(3*s[0]+1)+(3*s[1]+1)+(3*s[2]+1);
  
        float *ui;
        float *uo;
        float *coefx;
        float *coefy;
        float *coefz;
        int ret_val;
        ret_val = posix_memalign((void**)(&ui),    
                        __EZCU_MEM_ALIGN_SIZE,     size*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&uo),    
                        __EZCU_MEM_ALIGN_SIZE,     size*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&coefx), 
                        __EZCU_MEM_ALIGN_SIZE, (s[0]+1)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&coefy), 
                        __EZCU_MEM_ALIGN_SIZE, (s[1]+1)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&coefz), 
                        __EZCU_MEM_ALIGN_SIZE, (s[2]+1)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}

        memset(uo, 0, size*sizeof(float));
        randomize_buffer(ui, size);
        setup_stencil_coeffs(coefx);
        setup_stencil_coeffs(coefy);
        setup_stencil_coeffs(coefz);

        ezcu_mem_wrap(d,    ui,   size, PINNED);
        ezcu_mem_wrap(d,    uo,   size, PINNED);
        ezcu_mem_wrap(d, coefx, s[0]+1, READ_ONLY  | PINNED);
        ezcu_mem_wrap(d, coefy, s[1]+1, READ_ONLY  | PINNED);
        ezcu_mem_wrap(d, coefz, s[2]+1, READ_ONLY  | PINNED);

        ezcu_knl_set_wrk("stencil_v_3d", 2, grid, block);
        ezcu_knl_set_mem("stencil_v_3d",    0, ui);
        ezcu_knl_set_mem("stencil_v_3d",    1, uo);
        ezcu_knl_set_mem("stencil_v_3d",    2, coefx);
        ezcu_knl_set_mem("stencil_v_3d",    3, coefy);
        ezcu_knl_set_mem("stencil_v_3d",    4, coefz);
        ezcu_knl_set_int32("stencil_v_3d",  5, dim[0]);
        ezcu_knl_set_int32("stencil_v_3d",  6, dim[1]);
        ezcu_knl_set_int32("stencil_v_3d",  7, dim[2]);
        ezcu_knl_set_int32("stencil_v_3d",  8,  s[0]);
        ezcu_knl_set_int32("stencil_v_3d",  9,  s[1]);
        ezcu_knl_set_int32("stencil_v_3d", 10, s[2]);
                
        for (i = 0; i < 5; ++i)
           ezcu_knl_sync_exec("stencil_v_3d", d);
        ezcu_knl_sync_run("stencil_v_3d", d, ui, uo, coefx, coefy, coefz, 
                           DIM, DIM, DIM, 4, 4, 4);
        ezcu_timer_tick();
        for (i=0; i<ITER; ++i) ezcu_knl_exec("stencil_v_3d", d);
        ezcu_dev_wait(d);
        
        time = ezcu_timer_read();
        ezcu_timer_tick();
        ezcu_mem_update(uo, READ_ONLY);
        comm = ezcu_timer_read();
        fprintf(stdout, 
                "... avertage time: %8.2f (%8.2f) %s, %8.2f (%8.2f) Gflop/s\n",
                time/ITER, comm + time/ITER, ezcu_timer_uget(),
                flops*1.e-9*ITER*dim[0]*dim[1]*dim[2]/(time*ezcu_timer_coef()),
                flops*1.e-9*dim[0]*dim[1]*dim[2]/
                ((comm+(time/ITER))*ezcu_timer_coef()));
        check_stencil_3d(dim, s, coefx, coefy, coefz, ui, uo, epsilon);
        free(ui);
        free(uo);
        free(coefx);
        free(coefy);
        free(coefz);
    }

    TEST_F(KnlTest, stencil_3d_rw_wrapped_zc) {
        double comm, time = 0., flops;
        int i;
        float epsilon = 1.e-1;
        size_t size;
        int dim[3] = {DIM, DIM, DIM}, s[3] = {4, 4, 4};
        unsigned int grid[3] = {DIM/16, DIM/16, 1}, block[3] = {16, 16, 1};

        size   = (2*s[0] + dim[0])*(2*s[1] + dim[1])*(2*s[2] + dim[2]);
        flops  = 2+(3*s[0]+1)+(3*s[1]+1)+(3*s[2]+1);
  
        float *ui;
        float *uo;
        float *coefx;
        float *coefy;
        float *coefz;
        int ret_val;
        ret_val = posix_memalign((void**)(&ui),    
                        __EZCU_MEM_ALIGN_SIZE,     size*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&uo),    
                        __EZCU_MEM_ALIGN_SIZE,     size*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&coefx), 
                        __EZCU_MEM_ALIGN_SIZE, (s[0]+1)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&coefy), 
                        __EZCU_MEM_ALIGN_SIZE, (s[1]+1)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&coefz), 
                        __EZCU_MEM_ALIGN_SIZE, (s[2]+1)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}

        memset(uo, 0, size*sizeof(float));
        randomize_buffer(ui, size);
        setup_stencil_coeffs(coefx);
        setup_stencil_coeffs(coefy);
        setup_stencil_coeffs(coefz);

        ezcu_mem_wrap(d,    ui,   size,              HOST | ZERO_COPY);
        ezcu_mem_wrap(d,    uo,   size,              HOST | ZERO_COPY);
        ezcu_mem_wrap(d, coefx, s[0]+1, READ_ONLY  | HOST | ZERO_COPY);
        ezcu_mem_wrap(d, coefy, s[1]+1, READ_ONLY  | HOST | ZERO_COPY);
        ezcu_mem_wrap(d, coefz, s[2]+1, READ_ONLY  | HOST | ZERO_COPY);

        ezcu_knl_set_wrk("stencil_v_3d", 2, grid, block);
        ezcu_knl_set_mem("stencil_v_3d",    0, ui);
        ezcu_knl_set_mem("stencil_v_3d",    1, uo);
        ezcu_knl_set_mem("stencil_v_3d",    2, coefx);
        ezcu_knl_set_mem("stencil_v_3d",    3, coefy);
        ezcu_knl_set_mem("stencil_v_3d",    4, coefz);
        ezcu_knl_set_int32("stencil_v_3d",  5, dim[0]);
        ezcu_knl_set_int32("stencil_v_3d",  6, dim[1]);
        ezcu_knl_set_int32("stencil_v_3d",  7, dim[2]);
        ezcu_knl_set_int32("stencil_v_3d",  8,  s[0]);
        ezcu_knl_set_int32("stencil_v_3d",  9,  s[1]);
        ezcu_knl_set_int32("stencil_v_3d", 10, s[2]);
                
        for (i = 0; i < 5; ++i)
            ezcu_knl_sync_exec("stencil_v_3d", d);
        ezcu_knl_sync_run("stencil_v_3d", d, ui, uo, coefx, coefy, coefz, 
                           DIM, DIM, DIM, 4, 4, 4);
        ezcu_timer_tick();
        for (i=0; i<ITER; ++i) ezcu_knl_exec("stencil_v_3d", d);
        ezcu_dev_wait(d);
        
        time = ezcu_timer_read();
        ezcu_timer_tick();
        ezcu_mem_update(uo, READ_ONLY);
        comm = ezcu_timer_read();
        fprintf(stdout, 
                "... avertage time: %8.2f (%8.2f) %s, %8.2f (%8.2f) Gflop/s\n",
                time/ITER, comm + time/ITER, ezcu_timer_uget(),
                flops*1.e-9*ITER*dim[0]*dim[1]*dim[2]/(time*ezcu_timer_coef()),
                flops*1.e-9*dim[0]*dim[1]*dim[2]/
                ((comm+(time/ITER))*ezcu_timer_coef()));
        check_stencil_3d(dim, s, coefx, coefy, coefz, ui, uo, epsilon);
        free(ui);
        free(uo);
        free(coefx);
        free(coefy);
        free(coefz);
    }

    TEST_F(KnlTest, find_performance) {
        double ltime, dtime = 0.;
        int i, n=1000;
        ezcu_knl_t k;
        size_t size;
        int dim[3] = {DIM, DIM, DIM}, s[3] = {4, 4, 4};
        unsigned int grid[3] = {DIM/16, DIM/16, 1}, block[3] = {16, 16, 1};

        size   = (2*s[0] + dim[0])*(2*s[1] + dim[1])*(2*s[2] + dim[2]);
  
        float *ui;
        float *uo;
        float *coefx;
        float *coefy;
        float *coefz;
        int ret_val;
        ret_val = posix_memalign((void**)(&ui),    
                        __EZCU_MEM_ALIGN_SIZE,     size*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&uo),    
                        __EZCU_MEM_ALIGN_SIZE,     size*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&coefx), 
                        __EZCU_MEM_ALIGN_SIZE, (s[0]+1)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&coefy), 
                        __EZCU_MEM_ALIGN_SIZE, (s[1]+1)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&coefz), 
                        __EZCU_MEM_ALIGN_SIZE, (s[2]+1)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}

        memset(uo, 0, size*sizeof(float));
        randomize_buffer(ui, size);
        setup_stencil_coeffs(coefx);
        setup_stencil_coeffs(coefy);
        setup_stencil_coeffs(coefz);

        ezcu_mem_wrap(d,    ui,   size, READ_ONLY);
        ezcu_mem_wrap(d,    uo,   size, WRITE_ONLY);
        ezcu_mem_wrap(d, coefx, s[0]+1, READ_ONLY);
        ezcu_mem_wrap(d, coefy, s[1]+1, READ_ONLY);
        ezcu_mem_wrap(d, coefz, s[2]+1, READ_ONLY);

        ezcu_knl_set_wrk("stencil_v_3d", 2, grid, block);
        ezcu_knl_set_mem("stencil_v_3d",    0, ui);
        ezcu_knl_set_mem("stencil_v_3d",    1, uo);
        ezcu_knl_set_mem("stencil_v_3d",    2, coefx);
        ezcu_knl_set_mem("stencil_v_3d",    3, coefy);
        ezcu_knl_set_mem("stencil_v_3d",    4, coefz);
        ezcu_knl_set_int32("stencil_v_3d",  5, dim[0]);
        ezcu_knl_set_int32("stencil_v_3d",  6, dim[1]);
        ezcu_knl_set_int32("stencil_v_3d",  7, dim[2]);
        ezcu_knl_set_int32("stencil_v_3d",  8,  s[0]);
        ezcu_knl_set_int32("stencil_v_3d",  9,  s[1]);
        ezcu_knl_set_int32("stencil_v_3d", 10, s[2]);
        
        ezcu_timer_tick();
        k = __ezcu_knl_find("stencil_v_3d");      
        for (i = 0; i < 5; ++i)
            ezcu_fknl_sync_run(k, d, ui, uo, coefx, coefy, coefz,
                               dim[0], dim[1], dim[2], s[0], s[1], s[2]);
        for (i=0; i<n; ++i) ezcu_fknl_exec(k, d);
        ezcu_dev_wait(d);
        dtime = ezcu_timer_read();
        
        ezcu_timer_tick();     
        for (i = 0; i < 5; ++i)
            ezcu_knl_sync_run("stencil_v_3d", d, ui, uo, coefx, coefy, coefz,
                              dim[0], dim[1], dim[2], s[0], s[1], s[2]);
        for (i=0; i<n; ++i) ezcu_knl_exec("stencil_v_3d", d);
        ezcu_dev_wait(d);
        ltime = ezcu_timer_read();
        
        fprintf(stdout, "... direct  time: %8.2f %s\n", 
                dtime, ezcu_timer_uget());
        fprintf(stdout, "... loockup time: %8.2f %s\n", 
                ltime, ezcu_timer_uget());
        fprintf(stdout, "... overhead    : %8.2f %s (%4.1f %%)\n", 
                ltime-dtime, ezcu_timer_uget(), 100.0*(ltime-dtime)/dtime);
        free(ui);
        free(uo);
        free(coefx);
        free(coefy);
        free(coefz);
    }    

}  // namespace