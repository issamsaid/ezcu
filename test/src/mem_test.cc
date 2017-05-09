///
/// @copyright Copyright (c) 2016-, Issam SAID <said.issam@gmail.com>
/// All rights reserved.
/// 
/// Redistribution and use in source and binary forms, with or without 
/// modification,
/// are permitted provided that the following conditions are met:
/// 
/// 1. Redistributions of source code must retain the above copyright notice, 
///    this list of conditions and the following disclaimer.
/// 2. Redistributions in binary form must reproduce the above copyright notice,
///    this list of conditions and the following disclaimer in the documentation
///    and/or other materials provided with the distribution.
/// 3. Neither the name of the copyright holder nor the names of its 
///    contributors may be used to endorse or promote products derived from 
///    this software without specific prior written permission.
/// 
/// THIS SOFTWARE IS PROVIDED ''AS IS'' AND ANY
/// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
/// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
/// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR ITS CONTRIBUTORS BE 
/// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
/// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
/// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
/// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
/// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
/// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
/// THE POSSIBILITY OF SUCH DAMAGE.
/// 
/// @file mem_test.cc
/// @author Issam SAID
/// @brief Unit testing file for the ezCU memory objects manipulation routines.
///
#include <ezCU/base.h>
#include <ezCU/flags.h>
#include <ezCU/knl.h>
#include <ezCU/mem.h>
#include <ezCU/timer.h>
#include <ezCU/dev.h>
#include <gtest/gtest.h>
#include <__ezcu/mem-inl.h>
#include <__ezcu/knl-inl.h>
#include <__ezcu/util-inl.h>

#define MEM_ALIGN 4096

namespace {

    class MemTest : public ::testing::Test {
    protected:
        ezcu_dev_t d;
        size_t N;
        unsigned int grid[3], block[3];
        virtual void SetUp() {
            ezcu_init(DEFAULT);
            d = ezcu_dev_find(DEFAULT);
            N = 128;
            grid[0]  = N/16;
            grid[1]  = N/16;
            grid[2]  = 1;
            block[0] = 16;
            block[1] = 16;
            block[2] = 1;
            ezcu_load(PREFIX"/mem_test.cu", NULL);
        }
        virtual void TearDown() { ezcu_release(); }
    };

    TEST_F(MemTest, wrap_default_buffer) {
        float *h;
        int ret_val; 
        ezcu_mem_t m;
        ret_val = posix_memalign((void**)(&h), MEM_ALIGN, N*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        m       = ezcu_mem_wrap(d, h, N, DEFAULT);
        ASSERT_EQ(h, m->h);
        ASSERT_EQ(m,  __ezcu_mem_find(h));
        ASSERT_EQ(m->size*m->unit_size, N*sizeof(float));
        ASSERT_TRUE(EZCU_FLAGS_HAVE(m->flags, HWA));
        ASSERT_TRUE(EZCU_FLAGS_HAVE(m->flags, FLOAT));
        __ezcu_mem_release(m);
        free(h);
    }

    TEST_F(MemTest, wrap_char_hwa_buffer) {
        char *h;
        int ret_val;
        ezcu_mem_t m;
        ret_val = posix_memalign((void**)(&h), MEM_ALIGN, 
                                  N*sizeof(char));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        m = ezcu_mem_wrap(d, h, N, CHAR | HWA);
        ASSERT_EQ(h, m->h);
        ASSERT_EQ(m,  __ezcu_mem_find(h));
        ASSERT_EQ(m->size*m->unit_size, N*sizeof(char));
        ASSERT_TRUE(EZCU_FLAGS_HAVE(m->flags, HWA));
        ASSERT_TRUE(EZCU_FLAGS_HAVE(m->flags, CHAR));
        free(h);
    }

    TEST_F(MemTest, wrap_uchar_hwa_buffer) {
        unsigned char *h;
        int ret_val;
        ezcu_mem_t m;
        ret_val = posix_memalign((void**)(&h), MEM_ALIGN, 
                                  N*sizeof(unsigned char));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        m = ezcu_mem_wrap(d, h, N, CHAR | HWA);
        ASSERT_EQ(h, m->h);
        ASSERT_EQ(m,  __ezcu_mem_find(h));
        ASSERT_EQ(m->size*m->unit_size, N*sizeof(unsigned char));
        ASSERT_TRUE(EZCU_FLAGS_HAVE(m->flags, HWA));
        ASSERT_TRUE(EZCU_FLAGS_HAVE(m->flags, CHAR));
        free(h);
    }

    TEST_F(MemTest, wrap_short_hwa_buffer) {
        signed short *h;
        int ret_val;
        ezcu_mem_t m;
        ret_val = posix_memalign((void**)(&h), MEM_ALIGN, 
                                  N*sizeof(signed short));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        m = ezcu_mem_wrap(d, h, N, SHORT | HWA);
        ASSERT_EQ(h, m->h);
        ASSERT_EQ(m,  __ezcu_mem_find(h));
        ASSERT_EQ(m->size*m->unit_size, N*sizeof(signed short));
        ASSERT_TRUE(EZCU_FLAGS_HAVE(m->flags, HWA));
        ASSERT_TRUE(EZCU_FLAGS_HAVE(m->flags, SHORT));
        free(h);
    }
    
    TEST_F(MemTest, wrap_long_hwa_buffer) {
        long *h;
        int ret_val;
        ezcu_mem_t m;
        ret_val = posix_memalign((void**)(&h), MEM_ALIGN, 
                                  N*sizeof(long));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        m = ezcu_mem_wrap(d, h, N, LONG | HWA);
        ASSERT_EQ(h, m->h);
        ASSERT_EQ(m,  __ezcu_mem_find(h));
        ASSERT_EQ(m->size*m->unit_size, N*sizeof(long));
        ASSERT_TRUE(EZCU_FLAGS_HAVE(m->flags, HWA));
        ASSERT_TRUE(EZCU_FLAGS_HAVE(m->flags, LONG));
        free(h);
    }

    TEST_F(MemTest, wrap_float_hwa_buffer) {
        float *h;
        int ret_val;
        ezcu_mem_t m;
        ret_val = posix_memalign((void**)(&h), MEM_ALIGN, N*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        m = ezcu_mem_wrap(d, h, N, HWA);
        ASSERT_EQ(h, m->h);
        ASSERT_EQ(m,  __ezcu_mem_find(h));
        ASSERT_EQ(m->size*m->unit_size, N*sizeof(float));
        ASSERT_TRUE(EZCU_FLAGS_HAVE(m->flags, HWA));
        ASSERT_TRUE(EZCU_FLAGS_HAVE(m->flags, FLOAT));
        free(h);
    }

    TEST_F(MemTest, wrap_double_hwa_buffer) {
        double *h;
        int ret_val;
        ezcu_mem_t m;
        ret_val = posix_memalign((void**)(&h), MEM_ALIGN, 
                                  N*sizeof(double));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        m = ezcu_mem_wrap(d, h, N, DOUBLE | HWA);
        ASSERT_EQ(h, m->h);
        ASSERT_EQ(m,  __ezcu_mem_find(h));
        ASSERT_EQ(m->size*m->unit_size, N*sizeof(double));
        ASSERT_TRUE(EZCU_FLAGS_HAVE(m->flags, HWA));
        ASSERT_TRUE(EZCU_FLAGS_HAVE(m->flags, DOUBLE));
        free(h);
    }

    TEST_F(MemTest, wrap_int_pinned_buffer) {
        int *h ;
        int ret_val;
        ezcu_mem_t m;
        ret_val = posix_memalign((void**)(&h), MEM_ALIGN, N*sizeof(int));    
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        m   = ezcu_mem_wrap(d, h, N, HWA | PINNED | INT);
        ASSERT_EQ(m, __ezcu_mem_find(h));
        ASSERT_EQ(m->size*m->unit_size, N*sizeof(int));
        ASSERT_TRUE(EZCU_FLAGS_HAVE(m->flags, HWA));
        ASSERT_TRUE(EZCU_FLAGS_HAVE(m->flags, PINNED));
        ASSERT_TRUE(EZCU_FLAGS_HAVE(m->flags, INT));
        free(h);
    }

    TEST_F(MemTest, wrap_float_read_only_buffer) {
        float *h ; 
        int ret_val;
        ezcu_mem_t m;
        ret_val = posix_memalign((void**)(&h), MEM_ALIGN, N*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        m    = ezcu_mem_wrap(d, h, N, READ_ONLY);
        ASSERT_TRUE(EZCU_FLAGS_HAVE(m->flags, READ_ONLY));
        ASSERT_EQ(m, __ezcu_mem_find(h));
        free(h);
    }

    TEST_F(MemTest, wrap_float_write_only_buffer) {
        float *h ;
        int ret_val;
        ezcu_mem_t m;
        ret_val = posix_memalign((void**)(&h), MEM_ALIGN, N*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        m = ezcu_mem_wrap(d, h, N, WRITE_ONLY);
        ASSERT_EQ(m, __ezcu_mem_find(h));
        ASSERT_TRUE(EZCU_FLAGS_HAVE(m->flags, WRITE_ONLY));
        free(h);
    }

    TEST_F(MemTest, wrap_float_host_zero_copy_buffer) {
        float *h;
        int ret_val;
        ezcu_mem_t m;
        ret_val = posix_memalign((void**)(&h), MEM_ALIGN, N*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        m = ezcu_mem_wrap(d, h, N, HOST | ZERO_COPY);
        ASSERT_EQ(m, __ezcu_mem_find(h));
        ASSERT_TRUE(EZCU_FLAGS_HAVE(m->flags, HOST));
        ASSERT_TRUE(EZCU_FLAGS_HAVE(m->flags, ZERO_COPY));
        free(h);
    }

    TEST_F(MemTest, default_buffer_access_operator) {
        unsigned int i;
        float *h;
        int ret_val;
        ezcu_mem_t m;
        ret_val = posix_memalign((void**)(&h), MEM_ALIGN, N*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        m = ezcu_mem_wrap(d, h, N, READ_WRITE);
        ASSERT_EQ(m->size, N);
        ASSERT_EQ(m->h, h);
        ezcu_mem_update(h, WRITE_ONLY);
        for (i = 0; i < N; ++i) h[i] = i;
        ezcu_mem_update(h, READ_ONLY);
        for (i = 0; i < N; ++i) ASSERT_FLOAT_EQ(((float*)m->h)[i], i*1.f);
        free(h);
    }

    // 
    // This test fails on both AMD and NVIDIA
    // It has to be checked before uncommenting it
    //
    /*
    TEST_F(MemTest, pack_unpack_halos_using_rect) {
        unsigned int x, y, z, nw, nh, nd;
        float *buf;
        float *cpy;
        int ret_val;
        size_t g[2], l[2];

        nw = 128;
        nh = 128;
        nd = 128;

        l[0]=16;
        l[1]=16;
        g[0]=nw;
        g[1]=nh;
        ret_val = posix_memalign((void**)(&buf), 
                       MEM_ALIGN, (nw+8)*(nh+8)*(nd+8)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&cpy), 
                       MEM_ALIGN, (nw+8)*(nh+8)*(nd+8)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        
        ezcu_mem_t m = ezcu_mem_wrap(d, buf, (nw+8)*(nh+8)*(nd+8), READ_WRITE);
        ezcu_mem_wrap(d, cpy, (nw+8)*(nh+8)*(nd+8), READ_WRITE);

        ASSERT_EQ(m->size, (nw+8)*(nh+8)*(nd+8));
        ASSERT_EQ(m->h, buf);
        ezcu_load(PREFIX"/data/select.cl", NULL);
        ezcu_knl_set_wrk("kselect", 2, g, l);
        ezcu_mem_update(buf, WRITE_ONLY);
        for (z=0; z < nd+8; ++z)
            for (y=0; y < nh+8; ++y)
                for (x=0; x < nw+8; ++x)
                    buf[(nw+8)*(z*(nh+8) + y) + x] = -1;                         
        for (z=4; z < 8; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=4; x < nw+4; ++x) {
                    ASSERT_FLOAT_EQ(buf[(nw+8)*(z*(nh+8) + y) + x],-1*1.f);
                }
            }
        }
        ezcu_knl_sync_run("kselect", d, buf, nw, nh, nd);

        // pack
        ezcu_timer_tick();
        ezcu_mem_pop(buf,  4, nw+3,  4, nh+3,  4,    7, nw+8, nh+8, CL_FALSE);
        ezcu_mem_pop(buf,  4, nw+3,  4, nh+3, nd, nd+3, nw+8, nh+8, CL_FALSE);
        ezcu_mem_pop(buf,  4,    7,  4, nh+3,  4, nd+3, nw+8, nh+8, CL_FALSE);
        ezcu_mem_pop(buf, nw, nw+3,  4, nh+3,  4, nd+3, nw+8, nh+8, CL_FALSE);
        ezcu_mem_pop(buf,  4, nw+3,  4,    7,  4, nd+3, nw+8, nh+8, CL_FALSE);
        ezcu_mem_pop(buf,  4, nw+3, nh, nh+3,  4, nd+3, nw+8, nh+8, CL_TRUE);
        fprintf(stdout, "... time pack       : %f %s\n", 
                ezcu_timer_read(), ezcu_timer_uget());
        // Z
        for (z=4; z < 8; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=4; x < nw+4; ++x) {
                    ASSERT_FLOAT_EQ(buf[(nw+8)*(z*(nh+8) + y) + x], 
                                  ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        for (z=nd; z < nd+4; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=4; x < nw+4; ++x) {
                    ASSERT_FLOAT_EQ(buf[(nw+8)*(z*(nh+8) + y) + x], 
                                   ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        // X
        for (z=4; z < nd+4; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=4; x < 7; ++x) {
                    ASSERT_FLOAT_EQ(buf[(nw+8)*(z*(nh+8) + y) + x], 
                                  ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        for (z=4; z < nd+4; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=nw; x < nw+4; ++x) {
                    //printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(buf[(nw+8)*(z*(nh+8) + y) + x], 
                                   ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        // Y
        for (z=4; z < nd+4; ++z) {
            for (y=4; y < 8; ++y) {
                for (x=4; x < nw+4; ++x) {
                    //printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(buf[(nw+8)*(z*(nh+8) + y) + x], 
                                  ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        for (z=4; z < nd+4; ++z) {
            for (y=nh; y < nh+4; ++y) {
                for (x=4; x < nw+4; ++x) {
                    //printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(buf[(nw+8)*(z*(nh+8) + y) + x], 
                                   ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        for (z=8; z < nd; ++z) {
            for (y=8; y < nh; ++y) {
                for (x=8; x < nw; ++x) {
                    ASSERT_FLOAT_EQ(buf[(nw+8)*(z*(nh+8) + y) + x], -1*1.f);
                }
            }
        }
        ezcu_knl_sync_exec("kselect", d, buf, nw, nh, nd);
        ezcu_timer_tick();
        ezcu_mem_update(buf, READ_ONLY);
        fprintf(stdout, "... time full update: %f %s\n", 
                ezcu_timer_read(), ezcu_timer_uget());

        for (z=0; z < nd+8; ++z) {
            for (y=0; y < nh+8; ++y) {
                for (x=0; x < nw+8; ++x) {
                    cpy[(nw+8)*(z*(nh+8) + y) + x] = 
                        buf[(nw+8)*(z*(nh+8) + y) + x];
                }
            }
        }
        
        // unpack
        ezcu_timer_tick();
        ezcu_mem_push(cpy,  4, nw+3,  4, nh+3,  4,    7, nw+8, nh+8, CL_FALSE);
        ezcu_mem_push(cpy,  4, nw+3,  4, nh+3, nd, nd+3, nw+8, nh+8, CL_FALSE);
        ezcu_mem_push(cpy,  4,    7,  4, nh+3,  4, nd+3, nw+8, nh+8, CL_FALSE);
        ezcu_mem_push(cpy, nw, nw+3,  4, nh+3,  4, nd+3, nw+8, nh+8, CL_FALSE);
        ezcu_mem_push(cpy,  4, nw+3,  4,    7,  4, nd+3, nw+8, nh+8, CL_FALSE);
        ezcu_mem_push(cpy,  4, nw+3, nh, nh+3,  4, nd+3, nw+8, nh+8, CL_TRUE);
        fprintf(stdout, "... time unpack     : %f %s\n", 
                ezcu_timer_read(), ezcu_timer_uget());

        for (z=0; z < nd+8; ++z) {
            for (y=0; y < nh+8; ++y) {
                for (x=0; x < nw+8; ++x) {
                    cpy[(nw+8)*(z*(nh+8) + y) + x] = -1;
                }
            }
        }

        ezcu_mem_update(cpy, READ_ONLY);
        
        // Z
        for (z=4; z < 8; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=4; x < nw+4; ++x) {
                    printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(cpy[(nw+8)*(z*(nh+8) + y) + x], 
                                  ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        for (z=nd; z < nd+4; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=4; x < nw+4; ++x) {
                    ASSERT_FLOAT_EQ(cpy[(nw+8)*(z*(nh+8) + y) + x], 
                                   ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        // X
        for (z=4; z < nd+4; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=4; x < 7; ++x) {
                    ASSERT_FLOAT_EQ(cpy[(nw+8)*(z*(nh+8) + y) + x], 
                                  ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        for (z=4; z < nd+4; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=nw; x < nw+4; ++x) {
                    //printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(cpy[(nw+8)*(z*(nh+8) + y) + x], 
                                   ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        // Y
        for (z=4; z < nd+4; ++z) {
            for (y=4; y < 8; ++y) {
                for (x=4; x < nw+4; ++x) {
                    //printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(cpy[(nw+8)*(z*(nh+8) + y) + x], 
                                  ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        for (z=4; z < nd+4; ++z) {
            for (y=nh; y < nh+4; ++y) {
                for (x=4; x < nw+4; ++x) {
                    //printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(cpy[(nw+8)*(z*(nh+8) + y) + x], 
                                   ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }

        free(buf);
        free(cpy);
    }
    */
    TEST_F(MemTest, pack_unpack_halos_using_kernels) {
        int x, y, z, nw, nh, nd;
        float *buf, *cpy;
        float *tab_north;
        float *tab_south;
        float *tab_west;
        float *tab_east;
        float *tab_front;
        float *tab_back;
        int ret_val;
        nw = N;
        nh = N;
        nd = N;

        ret_val = posix_memalign((void**)(&buf), 
                       MEM_ALIGN, (nw+8)*(nh+8)*(nd+8)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&cpy), 
                       MEM_ALIGN, (nw+8)*(nh+8)*(nd+8)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&tab_north), 
                       MEM_ALIGN, (nw)*(nh)*(4)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&tab_south), 
                       MEM_ALIGN, (nw)*(nh)*(4)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&tab_west), 
                       MEM_ALIGN, (nd)*(nh)*(4)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&tab_east), 
                       MEM_ALIGN, (nd)*(nh)*(4)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&tab_front), 
                       MEM_ALIGN, (nd)*(nw)*(4)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&tab_back), 
                       MEM_ALIGN, (nd)*(nw)*(4)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}

        ezcu_mem_wrap(d, buf, (nw+8)*(nh+8)*(nd+8), READ_WRITE);
        ezcu_mem_wrap(d, cpy, (nw+8)*(nh+8)*(nd+8), READ_WRITE);
        ezcu_mem_wrap(d, tab_north, (nw)*(nh)*(4),  READ_WRITE);
        ezcu_mem_wrap(d, tab_south, (nw)*(nh)*(4),  READ_WRITE);
        ezcu_mem_wrap(d, tab_west,  (nd)*(nh)*(4),  READ_WRITE);
        ezcu_mem_wrap(d, tab_east,  (nd)*(nh)*(4),  READ_WRITE);
        ezcu_mem_wrap(d, tab_front, (nd)*(nw)*(4),  READ_WRITE);
        ezcu_mem_wrap(d, tab_back,  (nd)*(nw)*(4),  READ_WRITE);
    
        ezcu_mem_update(buf, WRITE_ONLY);
        for (z=0; z < nd+8; ++z)
            for (y=0; y < nh+8; ++y)
                for (x=0; x < nw+8; ++x)
                    buf[(nw+8)*(z*(nh+8) + y) + x] = -1;                         
        for (z=4; z < 8; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=4; x < nw+4; ++x) {
                    ASSERT_FLOAT_EQ(buf[(nw+8)*(z*(nh+8) + y) + x],-1*1.f);
                }
            }
        }
        grid[0]  = N/16;
        grid[1]  = N/16;
        block[0] = 16;
        block[1] = 16;
        ezcu_knl_set_wrk("kselect", 2, grid, block);
        ezcu_knl_set_mem("kselect", 0, buf);
        ezcu_knl_set_int32("kselect", 1, nw);
        ezcu_knl_set_int32("kselect", 2, nh);
        ezcu_knl_set_int32("kselect", 3, nd);       
        ezcu_knl_sync_exec("kselect", d);

        // pack
        grid[0]  = N/16;
        grid[1]  = N/16;
        block[0] = 16;
        block[1] = 16;
        ezcu_knl_set_wrk("pack_north", 2, grid, block);
        ezcu_knl_set_int32("pack_north", 0, nw);
        ezcu_knl_set_int32("pack_north", 1, nh);
        ezcu_knl_set_int32("pack_north", 2, nd); 
        ezcu_knl_set_mem("pack_north", 3, buf);      
        ezcu_knl_set_mem("pack_north", 4, tab_north);      

        ezcu_knl_set_wrk("pack_south", 2, grid, block);
        ezcu_knl_set_int32("pack_south", 0, nw);
        ezcu_knl_set_int32("pack_south", 1, nh);
        ezcu_knl_set_int32("pack_south", 2, nd); 
        ezcu_knl_set_mem("pack_south", 3, buf);      
        ezcu_knl_set_mem("pack_south", 4, tab_south);      
        
        grid[0]  = 1;
        grid[1]  = N/16;
        block[0] = 4;
        block[1] = 16;
        ezcu_knl_set_wrk("pack_east", 2, grid, block);
        ezcu_knl_set_int32("pack_east", 0, nw);
        ezcu_knl_set_int32("pack_east", 1, nh);
        ezcu_knl_set_int32("pack_east", 2, nd); 
        ezcu_knl_set_mem("pack_east",   3, buf);      
        ezcu_knl_set_mem("pack_east",   4, tab_east);      

        ezcu_knl_set_wrk("pack_west", 2, grid, block);
        ezcu_knl_set_int32("pack_west", 0, nw);
        ezcu_knl_set_int32("pack_west", 1, nh);
        ezcu_knl_set_int32("pack_west", 2, nd); 
        ezcu_knl_set_mem("pack_west",   3, buf);      
        ezcu_knl_set_mem("pack_west",   4, tab_west);      

        grid[0]  = N/16;
        grid[1]  = 1;
        block[0] = 16;
        block[1] = 4;
        ezcu_knl_set_wrk("pack_front", 2, grid, block);
        ezcu_knl_set_int32("pack_front", 0, nw);
        ezcu_knl_set_int32("pack_front", 1, nh);
        ezcu_knl_set_int32("pack_front", 2, nd); 
        ezcu_knl_set_mem("pack_front",   3, buf);      
        ezcu_knl_set_mem("pack_front",   4, tab_front);  

        ezcu_knl_set_wrk("pack_back", 2, grid, block);
        ezcu_knl_set_int32("pack_back", 0, nw);
        ezcu_knl_set_int32("pack_back", 1, nh);
        ezcu_knl_set_int32("pack_back", 2, nd); 
        ezcu_knl_set_mem("pack_back",   3, buf);      
        ezcu_knl_set_mem("pack_back",   4, tab_back);      
    
        ezcu_timer_tick();
        ezcu_knl_exec("pack_north", d);
        ezcu_knl_exec("pack_south", d);
        ezcu_knl_exec("pack_east",  d);
        ezcu_knl_exec("pack_west",  d);
        ezcu_knl_exec("pack_front", d);
        ezcu_knl_exec("pack_back",  d);
        ezcu_dev_wait(d);
        ezcu_mem_update(tab_north, READ_ONLY);
        ezcu_mem_update(tab_south, READ_ONLY);
        ezcu_mem_update(tab_west,  READ_ONLY);
        ezcu_mem_update(tab_east,  READ_ONLY);
        ezcu_mem_update(tab_front, READ_ONLY);
        ezcu_mem_update(tab_back,  READ_ONLY);
        fprintf(stdout, "... time pack       : %f %s\n", 
        ezcu_timer_read(), ezcu_timer_uget());
        // Z
        for (z=4; z < 8; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=4; x < nw+4; ++x) {
                    //printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(tab_south[(nw)*((z-4)*(nh) + (y-4)) + x-4], 
                                  ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }       
        for (z=nd; z < nd+4; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=4; x < nw+4; ++x) {
                    //printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(tab_north[(nw)*((z-nd)*(nh) + (y-4)) + x-4], 
                                   ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        // X
        for (z=4; z < nd+4; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=4; x < 8; ++x) {
                    //printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(tab_west[(4)*((z-4)*(nh) + (y-4)) + (x-4)], 
                                  ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        for (z=4; z < nd+4; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=nw; x < nw+4; ++x) {
                    //printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(tab_east[4*((z-4)*(nh) + (y-4)) + (x-nw)],  
                                   ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }

        // Y
        for (z=4; z < nd+4; ++z) {
            for (y=4; y < 8; ++y) {
                for (x=4; x < nw+4; ++x) {
                    //printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(tab_back[nw*((z-4)*(4) + (y-4)) + x-4],  
                                    ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        for (z=4; z < nd+4; ++z) {
            for (y=nh; y < nh+4; ++y) {
                for (x=4; x < nw+4; ++x) {
                    //printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(tab_front[nw*((z-4)*(4) + (y-nh)) + x-4],  
                                    ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        ezcu_knl_sync_exec("kselect", d);
        ezcu_timer_tick();
        ezcu_mem_update(buf, READ_ONLY);
        fprintf(stdout, "... time full update: %f %s\n", 
                ezcu_timer_read(), ezcu_timer_uget());
        
        // Unpack
        grid[0]  = N/16;
        grid[1]  = N/16;
        block[0] = 16;
        block[1] = 16;
        ezcu_knl_set_wrk("unpack_north", 2, grid, block);
        ezcu_knl_set_int32("unpack_north", 0, nw);
        ezcu_knl_set_int32("unpack_north", 1, nh);
        ezcu_knl_set_int32("unpack_north", 2, nd); 
        ezcu_knl_set_mem("unpack_north",   3, cpy);      
        ezcu_knl_set_mem("unpack_north",   4, tab_north);      

        ezcu_knl_set_wrk("unpack_south", 2, grid, block);
        ezcu_knl_set_int32("unpack_south", 0, nw);
        ezcu_knl_set_int32("unpack_south", 1, nh);
        ezcu_knl_set_int32("unpack_south", 2, nd); 
        ezcu_knl_set_mem("unpack_south",   3, cpy);      
        ezcu_knl_set_mem("unpack_south",   4, tab_south);      
        
        grid[0]  = 1;
        grid[1]  = N/16;
        block[0] = 4;
        block[1] = 16;
        ezcu_knl_set_wrk("unpack_east", 2, grid, block);
        ezcu_knl_set_int32("unpack_east", 0, nw);
        ezcu_knl_set_int32("unpack_east", 1, nh);
        ezcu_knl_set_int32("unpack_east", 2, nd); 
        ezcu_knl_set_mem("unpack_east",   3, cpy);      
        ezcu_knl_set_mem("unpack_east",   4, tab_east);      

        ezcu_knl_set_wrk("unpack_west", 2, grid, block);
        ezcu_knl_set_int32("unpack_west", 0, nw);
        ezcu_knl_set_int32("unpack_west", 1, nh);
        ezcu_knl_set_int32("unpack_west", 2, nd); 
        ezcu_knl_set_mem("unpack_west",   3, cpy);      
        ezcu_knl_set_mem("unpack_west",   4, tab_west);      

        grid[0]  = N/16;
        grid[1]  = 1;
        block[0] = 16;
        block[1] = 4;
        ezcu_knl_set_wrk("unpack_front", 2, grid, block);
        ezcu_knl_set_int32("unpack_front", 0, nw);
        ezcu_knl_set_int32("unpack_front", 1, nh);
        ezcu_knl_set_int32("unpack_front", 2, nd); 
        ezcu_knl_set_mem("unpack_front",   3, cpy);      
        ezcu_knl_set_mem("unpack_front",   4, tab_front);  

        ezcu_knl_set_wrk("unpack_back", 2, grid, block);
        ezcu_knl_set_int32("unpack_back", 0, nw);
        ezcu_knl_set_int32("unpack_back", 1, nh);
        ezcu_knl_set_int32("unpack_back", 2, nd); 
        ezcu_knl_set_mem("unpack_back",   3, cpy);      
        ezcu_knl_set_mem("unpack_back",   4, tab_back);      

        ezcu_timer_tick();
        ezcu_knl_exec("unpack_north", d);
        ezcu_knl_exec("unpack_south", d);
        ezcu_knl_exec("unpack_east",  d);
        ezcu_knl_exec("unpack_west",  d);
        ezcu_knl_exec("unpack_front", d);
        ezcu_knl_exec("unpack_back",  d);
        ezcu_dev_wait(d);
        fprintf(stdout, "... time unpack     : %f %s\n", 
                ezcu_timer_read(), ezcu_timer_uget());
        ezcu_mem_update(cpy, READ_ONLY);
        /// Z
        for (z=4; z < 8; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=4; x < nw+4; ++x) {
                    ASSERT_FLOAT_EQ(cpy[(nw+8)*(z*(nh+8) + y) + x], 
                                  ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        for (z=nd; z < nd+4; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=4; x < nw+4; ++x) {
                    ASSERT_FLOAT_EQ(cpy[(nw+8)*(z*(nh+8) + y) + x], 
                                   ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        // X
        for (z=4; z < nd+4; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=4; x < 7; ++x) {
                    ASSERT_FLOAT_EQ(cpy[(nw+8)*(z*(nh+8) + y) + x], 
                                  ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        for (z=4; z < nd+4; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=nw; x < nw+4; ++x) {
                    //printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(cpy[(nw+8)*(z*(nh+8) + y) + x], 
                                   ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        // Y
        for (z=4; z < nd+4; ++z) {
            for (y=4; y < 8; ++y) {
                for (x=4; x < nw+4; ++x) {
                    //printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(cpy[(nw+8)*(z*(nh+8) + y) + x], 
                                  ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        for (z=4; z < nd+4; ++z) {
            for (y=nh; y < nh+4; ++y) {
                for (x=4; x < nw+4; ++x) {
                    //printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(cpy[(nw+8)*(z*(nh+8) + y) + x], 
                                   ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        
        free(buf);
        free(cpy);
        free(tab_north);
        free(tab_south);
        free(tab_east);
        free(tab_west);
        free(tab_front);
        free(tab_back);
    }

    TEST_F(MemTest, pack_unpack_halos_using_kernels_zc) {
        int x, y, z, nw, nh, nd;
        float *buf, *cpy;
        float *tab_north;
        float *tab_south;
        float *tab_west;
        float *tab_east;
        float *tab_front;
        float *tab_back;
        int ret_val;
        nw = N;
        nh = N;
        nd = N;

        ret_val = posix_memalign((void**)(&buf), 
                       MEM_ALIGN, (nw+8)*(nh+8)*(nd+8)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&cpy), 
                       MEM_ALIGN, (nw+8)*(nh+8)*(nd+8)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&tab_north), 
                       MEM_ALIGN, (nw)*(nh)*(4)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&tab_south), 
                       MEM_ALIGN, (nw)*(nh)*(4)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&tab_west), 
                       MEM_ALIGN, (nd)*(nh)*(4)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&tab_east), 
                       MEM_ALIGN, (nd)*(nh)*(4)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&tab_front), 
                       MEM_ALIGN, (nd)*(nw)*(4)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&tab_back), 
                       MEM_ALIGN, (nd)*(nw)*(4)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}

        ezcu_mem_wrap(d, buf, (nw+8)*(nh+8)*(nd+8), 
                HOST | ZERO_COPY | READ_WRITE);
        ezcu_mem_wrap(d, cpy, (nw+8)*(nh+8)*(nd+8), 
                HOST | ZERO_COPY | READ_WRITE);
        ezcu_mem_wrap(d, tab_north, (nw)*(nh)*(4),  
                HOST | ZERO_COPY | READ_WRITE);
        ezcu_mem_wrap(d, tab_south, (nw)*(nh)*(4),  
                HOST | ZERO_COPY | READ_WRITE);
        ezcu_mem_wrap(d, tab_west,  (nd)*(nh)*(4),  
                HOST | ZERO_COPY | READ_WRITE);
        ezcu_mem_wrap(d, tab_east,  (nd)*(nh)*(4),  
                HOST | ZERO_COPY | READ_WRITE);
        ezcu_mem_wrap(d, tab_front, (nd)*(nw)*(4),  
                HOST | ZERO_COPY | READ_WRITE);
        ezcu_mem_wrap(d, tab_back,  (nd)*(nw)*(4),  
                HOST | ZERO_COPY | READ_WRITE);
    
        ezcu_mem_update(buf, WRITE_ONLY);
        for (z=0; z < nd+8; ++z)
            for (y=0; y < nh+8; ++y)
                for (x=0; x < nw+8; ++x)
                    buf[(nw+8)*(z*(nh+8) + y) + x] = -1;                         
        for (z=4; z < 8; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=4; x < nw+4; ++x) {
                    ASSERT_FLOAT_EQ(buf[(nw+8)*(z*(nh+8) + y) + x],-1*1.f);
                }
            }
        }
        grid[0]  = N/16;
        grid[1]  = N/16;
        block[0] = 16;
        block[1] = 16;
        ezcu_knl_set_wrk("kselect", 2, grid, block);
        ezcu_knl_sync_run("kselect", d, buf, nw, nh, nd);

        // pack
        grid[0]  = N/16;
        grid[1]  = N/16;
        block[0] = 16;
        block[1] = 16;
        ezcu_knl_set_wrk("pack_north", 2, grid, block);
        ezcu_knl_set_int32("pack_north", 0, nw);
        ezcu_knl_set_int32("pack_north", 1, nh);
        ezcu_knl_set_int32("pack_north", 2, nd); 
        ezcu_knl_set_mem("pack_north", 3, buf);      
        ezcu_knl_set_mem("pack_north", 4, tab_north);      

        ezcu_knl_set_wrk("pack_south", 2, grid, block);
        ezcu_knl_set_int32("pack_south", 0, nw);
        ezcu_knl_set_int32("pack_south", 1, nh);
        ezcu_knl_set_int32("pack_south", 2, nd); 
        ezcu_knl_set_mem("pack_south", 3, buf);      
        ezcu_knl_set_mem("pack_south", 4, tab_south);      
        
        grid[0]  = 1;
        grid[1]  = N/16;
        block[0] = 4;
        block[1] = 16;
        ezcu_knl_set_wrk("pack_east", 2, grid, block);
        ezcu_knl_set_int32("pack_east", 0, nw);
        ezcu_knl_set_int32("pack_east", 1, nh);
        ezcu_knl_set_int32("pack_east", 2, nd); 
        ezcu_knl_set_mem("pack_east",   3, buf);      
        ezcu_knl_set_mem("pack_east",   4, tab_east);      

        ezcu_knl_set_wrk("pack_west", 2, grid, block);
        ezcu_knl_set_int32("pack_west", 0, nw);
        ezcu_knl_set_int32("pack_west", 1, nh);
        ezcu_knl_set_int32("pack_west", 2, nd); 
        ezcu_knl_set_mem("pack_west",   3, buf);      
        ezcu_knl_set_mem("pack_west",   4, tab_west);      

        grid[0]  = N/16;
        grid[1]  = 1;
        block[0] = 16;
        block[1] = 4;
        ezcu_knl_set_wrk("pack_front", 2, grid, block);
        ezcu_knl_set_int32("pack_front", 0, nw);
        ezcu_knl_set_int32("pack_front", 1, nh);
        ezcu_knl_set_int32("pack_front", 2, nd); 
        ezcu_knl_set_mem("pack_front",   3, buf);      
        ezcu_knl_set_mem("pack_front",   4, tab_front);  

        ezcu_knl_set_wrk("pack_back", 2, grid, block);
        ezcu_knl_set_int32("pack_back", 0, nw);
        ezcu_knl_set_int32("pack_back", 1, nh);
        ezcu_knl_set_int32("pack_back", 2, nd); 
        ezcu_knl_set_mem("pack_back",   3, buf);      
        ezcu_knl_set_mem("pack_back",   4, tab_back);      
    
        ezcu_timer_tick();
        ezcu_knl_exec("pack_north", d);
        ezcu_knl_exec("pack_south", d);
        ezcu_knl_exec("pack_east",  d);
        ezcu_knl_exec("pack_west",  d);
        ezcu_knl_exec("pack_front", d);
        ezcu_knl_exec("pack_back",  d);
        ezcu_dev_wait(d);
        ezcu_mem_update(tab_north, READ_ONLY);
        ezcu_mem_update(tab_south, READ_ONLY);
        ezcu_mem_update(tab_west,  READ_ONLY);
        ezcu_mem_update(tab_east,  READ_ONLY);
        ezcu_mem_update(tab_front, READ_ONLY);
        ezcu_mem_update(tab_back,  READ_ONLY);
        fprintf(stdout, "... time pack       : %f %s\n", 
        ezcu_timer_read(), ezcu_timer_uget());
        // Z
        for (z=4; z < 8; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=4; x < nw+4; ++x) {
                    ASSERT_FLOAT_EQ(tab_south[(nw)*((z-4)*(nh) + (y-4)) + x-4], 
                                  ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }       
        for (z=nd; z < nd+4; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=4; x < nw+4; ++x) {
                    ASSERT_FLOAT_EQ(tab_north[(nw)*((z-nd)*(nh) + (y-4)) + x-4], 
                                   ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        // X
        for (z=4; z < nd+4; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=4; x < 8; ++x) {
                    //printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(tab_west[(4)*((z-4)*(nh) + (y-4)) + (x-4)], 
                                  ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        for (z=4; z < nd+4; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=nw; x < nw+4; ++x) {
                    //printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(tab_east[4*((z-4)*(nh) + (y-4)) + (x-nw)],  
                                   ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }

        // Y
        for (z=4; z < nd+4; ++z) {
            for (y=4; y < 8; ++y) {
                for (x=4; x < nw+4; ++x) {
                    //printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(tab_back[nw*((z-4)*(4) + (y-4)) + x-4],  
                                    ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        for (z=4; z < nd+4; ++z) {
            for (y=nh; y < nh+4; ++y) {
                for (x=4; x < nw+4; ++x) {
                    //printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(tab_front[nw*((z-4)*(4) + (y-nh)) + x-4],  
                                    ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        ezcu_knl_sync_run("kselect", d, buf, nw, nh, nd);
        ezcu_timer_tick();
        ezcu_mem_update(buf, READ_ONLY);
        fprintf(stdout, "... time full update: %f %s\n", 
                ezcu_timer_read(), ezcu_timer_uget());
        
        // Unpack
        grid[0]  = N/16;
        grid[1]  = N/16;
        block[0] = 16;
        block[1] = 16;
        ezcu_knl_set_wrk("unpack_north", 2, grid, block);
        ezcu_knl_set_int32("unpack_north", 0, nw);
        ezcu_knl_set_int32("unpack_north", 1, nh);
        ezcu_knl_set_int32("unpack_north", 2, nd); 
        ezcu_knl_set_mem("unpack_north",   3, cpy);      
        ezcu_knl_set_mem("unpack_north",   4, tab_north);      

        ezcu_knl_set_wrk("unpack_south", 2, grid, block);
        ezcu_knl_set_int32("unpack_south", 0, nw);
        ezcu_knl_set_int32("unpack_south", 1, nh);
        ezcu_knl_set_int32("unpack_south", 2, nd); 
        ezcu_knl_set_mem("unpack_south",   3, cpy);      
        ezcu_knl_set_mem("unpack_south",   4, tab_south);      
        
        grid[0]  = 1;
        grid[1]  = N/16;
        block[0] = 4;
        block[1] = 16;
        ezcu_knl_set_wrk("unpack_east", 2, grid, block);
        ezcu_knl_set_int32("unpack_east", 0, nw);
        ezcu_knl_set_int32("unpack_east", 1, nh);
        ezcu_knl_set_int32("unpack_east", 2, nd); 
        ezcu_knl_set_mem("unpack_east",   3, cpy);      
        ezcu_knl_set_mem("unpack_east",   4, tab_east);      

        ezcu_knl_set_wrk("unpack_west", 2, grid, block);
        ezcu_knl_set_int32("unpack_west", 0, nw);
        ezcu_knl_set_int32("unpack_west", 1, nh);
        ezcu_knl_set_int32("unpack_west", 2, nd); 
        ezcu_knl_set_mem("unpack_west",   3, cpy);      
        ezcu_knl_set_mem("unpack_west",   4, tab_west);      

        grid[0]  = N/16;
        grid[1]  = 1;
        block[0] = 16;
        block[1] = 4;
        ezcu_knl_set_wrk("unpack_front", 2, grid, block);
        ezcu_knl_set_int32("unpack_front", 0, nw);
        ezcu_knl_set_int32("unpack_front", 1, nh);
        ezcu_knl_set_int32("unpack_front", 2, nd); 
        ezcu_knl_set_mem("unpack_front",   3, cpy);      
        ezcu_knl_set_mem("unpack_front",   4, tab_front);  

        ezcu_knl_set_wrk("unpack_back", 2, grid, block);
        ezcu_knl_set_int32("unpack_back", 0, nw);
        ezcu_knl_set_int32("unpack_back", 1, nh);
        ezcu_knl_set_int32("unpack_back", 2, nd); 
        ezcu_knl_set_mem("unpack_back",   3, cpy);      
        ezcu_knl_set_mem("unpack_back",   4, tab_back);      

        ezcu_timer_tick();
        ezcu_knl_exec("unpack_north", d);
        ezcu_knl_exec("unpack_south", d);
        ezcu_knl_exec("unpack_east",  d);
        ezcu_knl_exec("unpack_west",  d);
        ezcu_knl_exec("unpack_front", d);
        ezcu_knl_exec("unpack_back",  d);
        ezcu_dev_wait(d);
        fprintf(stdout, "... time unpack     : %f %s\n", 
                ezcu_timer_read(), ezcu_timer_uget());
        ezcu_mem_update(cpy, READ_ONLY);
        /// Z
        for (z=4; z < 8; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=4; x < nw+4; ++x) {
                    ASSERT_FLOAT_EQ(cpy[(nw+8)*(z*(nh+8) + y) + x], 
                                  ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        for (z=nd; z < nd+4; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=4; x < nw+4; ++x) {
                    ASSERT_FLOAT_EQ(cpy[(nw+8)*(z*(nh+8) + y) + x], 
                                   ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        // X
        for (z=4; z < nd+4; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=4; x < 7; ++x) {
                    ASSERT_FLOAT_EQ(cpy[(nw+8)*(z*(nh+8) + y) + x], 
                                  ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        for (z=4; z < nd+4; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=nw; x < nw+4; ++x) {
                    //printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(cpy[(nw+8)*(z*(nh+8) + y) + x], 
                                   ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        // Y
        for (z=4; z < nd+4; ++z) {
            for (y=4; y < 8; ++y) {
                for (x=4; x < nw+4; ++x) {
                    //printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(cpy[(nw+8)*(z*(nh+8) + y) + x], 
                                  ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        for (z=4; z < nd+4; ++z) {
            for (y=nh; y < nh+4; ++y) {
                for (x=4; x < nw+4; ++x) {
                    //printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(cpy[(nw+8)*(z*(nh+8) + y) + x], 
                                   ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        
        free(buf);
        free(cpy);
        free(tab_north);
        free(tab_south);
        free(tab_east);
        free(tab_west);
        free(tab_front);
        free(tab_back);
    }

    TEST_F(MemTest, pack_unpack_halos_using_kernels_pinned) {
        int x, y, z, nw, nh, nd;
        float *buf, *cpy;
        float *tab_north;
        float *tab_south;
        float *tab_west;
        float *tab_east;
        float *tab_front;
        float *tab_back;
        int ret_val;
        nw = N;
        nh = N;
        nd = N;

        ret_val = posix_memalign((void**)(&buf), 
                       MEM_ALIGN, (nw+8)*(nh+8)*(nd+8)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&cpy), 
                       MEM_ALIGN, (nw+8)*(nh+8)*(nd+8)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&tab_north), 
                       MEM_ALIGN, (nw)*(nh)*(4)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&tab_south), 
                       MEM_ALIGN, (nw)*(nh)*(4)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&tab_west), 
                       MEM_ALIGN, (nd)*(nh)*(4)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&tab_east), 
                       MEM_ALIGN, (nd)*(nh)*(4)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&tab_front), 
                       MEM_ALIGN, (nd)*(nw)*(4)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}
        ret_val = posix_memalign((void**)(&tab_back), 
                       MEM_ALIGN, (nd)*(nw)*(4)*sizeof(float));
        if (ret_val) {fprintf(stderr, "Failed to allocate memory\n"); FAIL();}

        ezcu_mem_wrap(d, buf, (nw+8)*(nh+8)*(nd+8), 
                HWA | PINNED | READ_WRITE);
        ezcu_mem_wrap(d, cpy, (nw+8)*(nh+8)*(nd+8), 
                HWA | PINNED | READ_WRITE);
        ezcu_mem_wrap(d, tab_north, (nw)*(nh)*(4),  
                HWA | PINNED | READ_WRITE);
        ezcu_mem_wrap(d, tab_south, (nw)*(nh)*(4),  
                HWA | PINNED | READ_WRITE);
        ezcu_mem_wrap(d, tab_west,  (nd)*(nh)*(4),  
                HWA | PINNED | READ_WRITE);
        ezcu_mem_wrap(d, tab_east,  (nd)*(nh)*(4),  
                HWA | PINNED | READ_WRITE);
        ezcu_mem_wrap(d, tab_front, (nd)*(nw)*(4),  
                HWA | PINNED | READ_WRITE);
        ezcu_mem_wrap(d, tab_back,  (nd)*(nw)*(4),  
                HWA | PINNED | READ_WRITE);
    
        ezcu_mem_update(buf, WRITE_ONLY);
        for (z=0; z < nd+8; ++z)
            for (y=0; y < nh+8; ++y)
                for (x=0; x < nw+8; ++x)
                    buf[(nw+8)*(z*(nh+8) + y) + x] = -1;                         
        for (z=4; z < 8; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=4; x < nw+4; ++x) {
                    ASSERT_FLOAT_EQ(buf[(nw+8)*(z*(nh+8) + y) + x],-1*1.f);
                }
            }
        }
        grid[0]  = N/16;
        grid[1]  = N/16;
        block[0] = 16;
        block[1] = 16;
        ezcu_knl_set_wrk("kselect", 2, grid, block);
        ezcu_knl_set_mem("kselect", 0, buf);
        ezcu_knl_set_int32("kselect", 1, nw);
        ezcu_knl_set_int32("kselect", 2, nh);
        ezcu_knl_set_int32("kselect", 3, nd);       
        ezcu_knl_sync_exec("kselect", d);

        // pack
        grid[0]  = N/16;
        grid[1]  = N/16;
        block[0] = 16;
        block[1] = 16;
        ezcu_knl_set_wrk("pack_north", 2, grid, block);
        ezcu_knl_set_int32("pack_north", 0, nw);
        ezcu_knl_set_int32("pack_north", 1, nh);
        ezcu_knl_set_int32("pack_north", 2, nd); 
        ezcu_knl_set_mem("pack_north", 3, buf);      
        ezcu_knl_set_mem("pack_north", 4, tab_north);      

        ezcu_knl_set_wrk("pack_south", 2, grid, block);
        ezcu_knl_set_int32("pack_south", 0, nw);
        ezcu_knl_set_int32("pack_south", 1, nh);
        ezcu_knl_set_int32("pack_south", 2, nd); 
        ezcu_knl_set_mem("pack_south", 3, buf);      
        ezcu_knl_set_mem("pack_south", 4, tab_south);      
        
        grid[0]  = 1;
        grid[1]  = N/16;
        block[0] = 4;
        block[1] = 16;
        ezcu_knl_set_wrk("pack_east", 2, grid, block);
        ezcu_knl_set_int32("pack_east", 0, nw);
        ezcu_knl_set_int32("pack_east", 1, nh);
        ezcu_knl_set_int32("pack_east", 2, nd); 
        ezcu_knl_set_mem("pack_east",   3, buf);      
        ezcu_knl_set_mem("pack_east",   4, tab_east);      

        ezcu_knl_set_wrk("pack_west", 2, grid, block);
        ezcu_knl_set_int32("pack_west", 0, nw);
        ezcu_knl_set_int32("pack_west", 1, nh);
        ezcu_knl_set_int32("pack_west", 2, nd); 
        ezcu_knl_set_mem("pack_west",   3, buf);      
        ezcu_knl_set_mem("pack_west",   4, tab_west);      

        grid[0]  = N/16;
        grid[1]  = 1;
        block[0] = 16;
        block[1] = 4;
        ezcu_knl_set_wrk("pack_front", 2, grid, block);
        ezcu_knl_set_int32("pack_front", 0, nw);
        ezcu_knl_set_int32("pack_front", 1, nh);
        ezcu_knl_set_int32("pack_front", 2, nd); 
        ezcu_knl_set_mem("pack_front",   3, buf);      
        ezcu_knl_set_mem("pack_front",   4, tab_front);  

        ezcu_knl_set_wrk("pack_back", 2, grid, block);
        ezcu_knl_set_int32("pack_back", 0, nw);
        ezcu_knl_set_int32("pack_back", 1, nh);
        ezcu_knl_set_int32("pack_back", 2, nd); 
        ezcu_knl_set_mem("pack_back",   3, buf);      
        ezcu_knl_set_mem("pack_back",   4, tab_back);      
    
        ezcu_timer_tick();
        ezcu_knl_exec("pack_north", d);
        ezcu_knl_exec("pack_south", d);
        ezcu_knl_exec("pack_east",  d);
        ezcu_knl_exec("pack_west",  d);
        ezcu_knl_exec("pack_front", d);
        ezcu_knl_exec("pack_back",  d);
        ezcu_dev_wait(d);
        ezcu_mem_update(tab_north, READ_ONLY);
        ezcu_mem_update(tab_south, READ_ONLY);
        ezcu_mem_update(tab_west,  READ_ONLY);
        ezcu_mem_update(tab_east,  READ_ONLY);
        ezcu_mem_update(tab_front, READ_ONLY);
        ezcu_mem_update(tab_back,  READ_ONLY);
        fprintf(stdout, "... time pack       : %f %s\n", 
        ezcu_timer_read(), ezcu_timer_uget());
        // Z
        for (z=4; z < 8; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=4; x < nw+4; ++x) {
                    ASSERT_FLOAT_EQ(tab_south[(nw)*((z-4)*(nh) + (y-4)) + x-4], 
                                  ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }       
        for (z=nd; z < nd+4; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=4; x < nw+4; ++x) {
                    ASSERT_FLOAT_EQ(tab_north[(nw)*((z-nd)*(nh) + (y-4)) + x-4], 
                                   ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        // X
        for (z=4; z < nd+4; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=4; x < 8; ++x) {
                    //printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(tab_west[(4)*((z-4)*(nh) + (y-4)) + (x-4)], 
                                  ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        for (z=4; z < nd+4; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=nw; x < nw+4; ++x) {
                    //printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(tab_east[4*((z-4)*(nh) + (y-4)) + (x-nw)],  
                                   ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }

        // Y
        for (z=4; z < nd+4; ++z) {
            for (y=4; y < 8; ++y) {
                for (x=4; x < nw+4; ++x) {
                    //printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(tab_back[nw*((z-4)*(4) + (y-4)) + x-4],  
                                    ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        for (z=4; z < nd+4; ++z) {
            for (y=nh; y < nh+4; ++y) {
                for (x=4; x < nw+4; ++x) {
                    //printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(tab_front[nw*((z-4)*(4) + (y-nh)) + x-4],  
                                    ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        ezcu_knl_sync_run("kselect", d, buf, nw, nh, nd);
        ezcu_timer_tick();
        ezcu_mem_update(buf, READ_ONLY);
        fprintf(stdout, "... time full update: %f %s\n", 
                ezcu_timer_read(), ezcu_timer_uget());
        
        // Unpack
        grid[0]  = N/16;
        grid[1]  = N/16;
        block[0] = 16;
        block[1] = 16;
        ezcu_knl_set_wrk("unpack_north", 2, grid, block);
        ezcu_knl_set_int32("unpack_north", 0, nw);
        ezcu_knl_set_int32("unpack_north", 1, nh);
        ezcu_knl_set_int32("unpack_north", 2, nd); 
        ezcu_knl_set_mem("unpack_north",   3, cpy);      
        ezcu_knl_set_mem("unpack_north",   4, tab_north);      

        ezcu_knl_set_wrk("unpack_south", 2, grid, block);
        ezcu_knl_set_int32("unpack_south", 0, nw);
        ezcu_knl_set_int32("unpack_south", 1, nh);
        ezcu_knl_set_int32("unpack_south", 2, nd); 
        ezcu_knl_set_mem("unpack_south",   3, cpy);      
        ezcu_knl_set_mem("unpack_south",   4, tab_south);      
        
        grid[0]  = 1;
        grid[1]  = N/16;
        block[0] = 4;
        block[1] = 16;
        ezcu_knl_set_wrk("unpack_east", 2, grid, block);
        ezcu_knl_set_int32("unpack_east", 0, nw);
        ezcu_knl_set_int32("unpack_east", 1, nh);
        ezcu_knl_set_int32("unpack_east", 2, nd); 
        ezcu_knl_set_mem("unpack_east",   3, cpy);      
        ezcu_knl_set_mem("unpack_east",   4, tab_east);      

        ezcu_knl_set_wrk("unpack_west", 2, grid, block);
        ezcu_knl_set_int32("unpack_west", 0, nw);
        ezcu_knl_set_int32("unpack_west", 1, nh);
        ezcu_knl_set_int32("unpack_west", 2, nd); 
        ezcu_knl_set_mem("unpack_west",   3, cpy);      
        ezcu_knl_set_mem("unpack_west",   4, tab_west);      

        grid[0]  = N/16;
        grid[1]  = 1;
        block[0] = 16;
        block[1] = 4;
        ezcu_knl_set_wrk("unpack_front", 2, grid, block);
        ezcu_knl_set_int32("unpack_front", 0, nw);
        ezcu_knl_set_int32("unpack_front", 1, nh);
        ezcu_knl_set_int32("unpack_front", 2, nd); 
        ezcu_knl_set_mem("unpack_front",   3, cpy);      
        ezcu_knl_set_mem("unpack_front",   4, tab_front);  

        ezcu_knl_set_wrk("unpack_back", 2, grid, block);
        ezcu_knl_set_int32("unpack_back", 0, nw);
        ezcu_knl_set_int32("unpack_back", 1, nh);
        ezcu_knl_set_int32("unpack_back", 2, nd); 
        ezcu_knl_set_mem("unpack_back",   3, cpy);      
        ezcu_knl_set_mem("unpack_back",   4, tab_back);      

        ezcu_timer_tick();
        ezcu_knl_exec("unpack_north", d);
        ezcu_knl_exec("unpack_south", d);
        ezcu_knl_exec("unpack_east",  d);
        ezcu_knl_exec("unpack_west",  d);
        ezcu_knl_exec("unpack_front", d);
        ezcu_knl_exec("unpack_back",  d);
        ezcu_dev_wait(d);
        fprintf(stdout, "... time unpack     : %f %s\n", 
                ezcu_timer_read(), ezcu_timer_uget());
        ezcu_mem_update(cpy, READ_ONLY);
        /// Z
        for (z=4; z < 8; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=4; x < nw+4; ++x) {
                    ASSERT_FLOAT_EQ(cpy[(nw+8)*(z*(nh+8) + y) + x], 
                                  ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        for (z=nd; z < nd+4; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=4; x < nw+4; ++x) {
                    ASSERT_FLOAT_EQ(cpy[(nw+8)*(z*(nh+8) + y) + x], 
                                   ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        // X
        for (z=4; z < nd+4; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=4; x < 7; ++x) {
                    ASSERT_FLOAT_EQ(cpy[(nw+8)*(z*(nh+8) + y) + x], 
                                  ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        for (z=4; z < nd+4; ++z) {
            for (y=4; y < nh+4; ++y) {
                for (x=nw; x < nw+4; ++x) {
                    //printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(cpy[(nw+8)*(z*(nh+8) + y) + x], 
                                   ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        // Y
        for (z=4; z < nd+4; ++z) {
            for (y=4; y < 8; ++y) {
                for (x=4; x < nw+4; ++x) {
                    //printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(cpy[(nw+8)*(z*(nh+8) + y) + x], 
                                  ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        for (z=4; z < nd+4; ++z) {
            for (y=nh; y < nh+4; ++y) {
                for (x=4; x < nw+4; ++x) {
                    //printf("%d %d %d\n", z,y,x);
                    ASSERT_FLOAT_EQ(cpy[(nw+8)*(z*(nh+8) + y) + x], 
                                   ((nw+8)*(z*(nh+8) + y) + x)*1.f);
                }
            }
        }
        
        free(buf);
        free(cpy);
        free(tab_north);
        free(tab_south);
        free(tab_east);
        free(tab_west);
        free(tab_front);
        free(tab_back);
    }

}  // namespace