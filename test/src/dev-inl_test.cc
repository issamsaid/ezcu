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
///    contributors may be used to endorse or promote products derived from this
///    software without specific prior written permission.
///
/// THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
/// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
/// AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
/// THE copyright holder OR ITS CONTRIBUTERS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
/// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
/// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
/// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
/// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
/// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
/// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
/// 
/// @file dev-inl_test.cc
/// @author Issam SAID
/// @brief Unit testing file for the devices manipulation private functions.
///
#include <ezCU/flags.h>
#include <__ezcu/dev-inl.h>
#include <gtest/gtest.h>

extern ezcu_env_t ezcu;

namespace {

    class DevInlTest : public ::testing::Test {
    protected:
        int nb_devices;
        virtual void SetUp() { 
            ezcu        = (ezcu_env_t) malloc(sizeof(struct __ezcu_env_t));
            ezcu->devs  = &urb_sentinel;
            ezcu->knls  = &urb_sentinel;
            ezcu->mems  = &urb_sentinel;
            ezcu->fdout = stdout;
            ezcu->fderr = stderr;
            cuInit(0);
            cuDeviceGetCount(&nb_devices);
        }
        virtual void TearDown() { 
            if (ezcu) {
                free(ezcu); 
                ezcu = NULL;
            }
        }
    };

    TEST_F(DevInlTest, __ezcu_dev_query) {
        ASSERT_EQ(__ezcu_dev_query(), nb_devices);
    }

    TEST_F(DevInlTest, __ezcu_dev_get_cc_hex) {
        ASSERT_EQ(__ezcu_dev_get_cc_hex(2, 6), 0x62);
    }

    TEST_F(DevInlTest, __ezcu_dev_select_by_vendor) {
        int i, n = __ezcu_dev_query();
        CUdevice ids[n];
        for (i=0; i < n; ++i) cuDeviceGet(&ids[i], i);
        ASSERT_EQ(n, nb_devices);
        ASSERT_TRUE(__ezcu_dev_select_by_vendor(ids, n, NVIDIA) > 0);
        ASSERT_TRUE(__ezcu_dev_select_by_vendor(ids, n, AMD) == 0);
        ASSERT_TRUE(__ezcu_dev_select_by_vendor(ids, n, INTEL) == 0);
        ASSERT_TRUE(__ezcu_dev_select_by_vendor(ids, n, APPLE) == 0);
    }

    TEST_F(DevInlTest, __ezcu_dev_select_by_dev_type) {
        int i, n = __ezcu_dev_query();
        CUdevice ids[n], t_ids[n];
        for (i=0; i < n; ++i) cuDeviceGet(&ids[i], i);
        ASSERT_EQ(n, nb_devices);
        ASSERT_GE(__ezcu_dev_select_by_dev_type(ids, n, t_ids, GPU), 1);
        ASSERT_GE(__ezcu_dev_select_by_dev_type(ids, n, t_ids, ALL), 1);
        ASSERT_EQ(__ezcu_dev_select_by_dev_type(ids, n, t_ids, CPU), 0);
        ASSERT_GE(__ezcu_dev_select_by_dev_type(ids, n, t_ids, ACCELERATOR), 0);
    }

    TEST_F(DevInlTest, __ezcu_dev_select_by_dev_prop) {
        size_t j, i, p_n, s_n, n = __ezcu_dev_query();
        int minor, major;
        CUdevice d, ids[n], t_ids[n], p_ids[n];
        for (j=0; j < n; ++j) cuDeviceGet(&ids[j], j);
        ASSERT_EQ(n, nb_devices);
        s_n = __ezcu_dev_select_by_dev_prop(ids, n, t_ids, CC20);
        ASSERT_GE(s_n, 1);
        ASSERT_LE(s_n, n);
        ezcu_dev_prop_flags_t nDevProps[] = {CC20, CC30, CC35, CC50, CC60};
        for (j = 0; j < s_n; ++j) {
            d = t_ids[j];
            CUcontext ctx;
            cuCtxCreate(&ctx, CU_CTX_MAP_HOST | CU_CTX_SCHED_YIELD, d);
            cuCtxSetCurrent(ctx);
            __EZCU_DEV_GET(d, 
                           CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MAJOR, major);      
            __EZCU_DEV_GET(d, 
                           CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MINOR, minor);

            for (i = 0; i < sizeof(nDevProps)/sizeof(*nDevProps); ++i) {
                p_n = __ezcu_dev_select_by_dev_prop(ids, n, p_ids, nDevProps[i]);
                if (__ezcu_dev_get_cc_hex(minor, major) >= 
                    ezcu_flags_to_dev_prop(nDevProps[i])) {
                    ASSERT_GE(p_n, 1);
                } else {
                    ASSERT_EQ(p_n, 0);
                }
            }
            cuCtxDestroy(ctx);
        }
    }

    TEST_F(DevInlTest, __ezcu_dev_select_by_dev_index) {
        int        n = __ezcu_dev_query();
        int i, ids[n], t_ids[n];
        for(i=0; i<n; i++) ids[i] = i;
        ASSERT_GT(n, 0);
        ASSERT_EQ(__ezcu_dev_select_by_dev_index(ids, n, t_ids, FIRST), 1);
        ezcu_dev_index_flags_t nDevIdxs[] = {FIRST, SECOND, THIRD, FOURTH,
                                             FIFTH, SIXTH, SEVENTH, EIGHTH};
        for (i=0; i<8; i++) {
            if (n > i) {
                ASSERT_EQ(__ezcu_dev_select_by_dev_index(ids, n, t_ids,
                                                         nDevIdxs[i]), 1);
            } else {
                ASSERT_EQ(__ezcu_dev_select_by_dev_index(ids, n, t_ids,
                                                         nDevIdxs[i]), 0);
            }
        }
    }

}  // namespace