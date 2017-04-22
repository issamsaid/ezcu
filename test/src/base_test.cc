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
/// DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
/// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
/// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
/// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
/// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
/// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
/// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
/// 
/// @file base_test.cc
/// @author Issam SAID
/// @brief The unit testing file of the ezCU setup main routines.
///
#include <ezCU/flags.h>
#include <ezCU/base.h>
#include <ezCU/dev.h>
#include <__ezcu/dev-inl.h>
#include <gtest/gtest.h>

extern ezcu_env_t ezcu;

namespace {

    class BaseTest : public ::testing::Test {
    protected:
        const static int N=100;
        virtual void SetUp() {}
        virtual void TearDown() {}
    };

    TEST_F(BaseTest, init_default) {
        ASSERT_TRUE(ezcu == NULL);
        ezcu_init(DEFAULT);
        ASSERT_TRUE(ezcu != NULL);
        ezcu_release();
        ASSERT_TRUE(ezcu == NULL);
    }

    TEST_F(BaseTest, init_default_singleton) {
        ASSERT_TRUE(ezcu == NULL);
        ezcu_init(DEFAULT);
        ASSERT_TRUE(ezcu != NULL);
        ezcu_init(DEFAULT);
        ezcu_init(DEFAULT);
        ezcu_init(DEFAULT);
        ezcu_init(DEFAULT);
        ezcu_init(DEFAULT);
        ASSERT_TRUE(ezcu != NULL);
        ezcu_release();
        ASSERT_TRUE(ezcu == NULL);
    }

    TEST_F(BaseTest, release_singleton) {
        ASSERT_TRUE(ezcu == NULL);
        ezcu_init(DEFAULT);
        ASSERT_TRUE(ezcu != NULL);
        ezcu_release();
        ezcu_release();
        ezcu_release();
        ezcu_release();
        ezcu_release();
        ezcu_release();
        ezcu_release();
        ASSERT_TRUE(ezcu == NULL);
    }

    TEST_F(BaseTest, init_vendor) {
        unsigned int n;
        ASSERT_TRUE(ezcu == NULL);
        ezcu_init(NVIDIA);
        n = __ezcu_dev_query();
        ASSERT_TRUE(ezcu != NULL);
        ASSERT_TRUE(ezcu->initialized[NVIDIA & EZCU_LKP_MASK]);
        ASSERT_FALSE(ezcu->initialized[AMD & EZCU_LKP_MASK]);
        ASSERT_FALSE(ezcu->initialized[APPLE & EZCU_LKP_MASK]);
        ASSERT_FALSE(ezcu->initialized[INTEL & EZCU_LKP_MASK]);
        ezcu_release();
        ASSERT_GE(n, 0);
    }

    TEST_F(BaseTest, init_wrong_vendor) {
        ASSERT_DEATH(ezcu_init(AMD), ".*");
        ASSERT_DEATH(ezcu_init(INTEL), ".*");
        ASSERT_DEATH(ezcu_init(APPLE), ".*");
    }

    TEST_F(BaseTest, init_all) {
        unsigned int n;
        ASSERT_TRUE(ezcu == NULL);
        ezcu_init(ALL);
        n = __ezcu_dev_query();
        ASSERT_TRUE(ezcu != NULL);
        ASSERT_TRUE(ezcu->initialized[NVIDIA & EZCU_LKP_MASK]);
        ASSERT_FALSE(ezcu->initialized[AMD & EZCU_LKP_MASK]);
        ASSERT_FALSE(ezcu->initialized[APPLE & EZCU_LKP_MASK]);
        ASSERT_FALSE(ezcu->initialized[INTEL & EZCU_LKP_MASK]);
        ezcu_release();
        ASSERT_GE(n, 0);
    }

    TEST_F(BaseTest, init_gpu) {
        unsigned int n;
        ASSERT_TRUE(ezcu == NULL);
        ezcu_init(GPU);
        n = __ezcu_dev_query();
        ASSERT_TRUE(ezcu != NULL);
        ASSERT_TRUE(ezcu->initialized[NVIDIA & EZCU_LKP_MASK]);
        ASSERT_FALSE(ezcu->initialized[AMD & EZCU_LKP_MASK]);
        ASSERT_FALSE(ezcu->initialized[APPLE & EZCU_LKP_MASK]);
        ASSERT_FALSE(ezcu->initialized[INTEL & EZCU_LKP_MASK]);
        ezcu_release();
        ASSERT_GE(n, 0);
    }

    TEST_F(BaseTest, init_first_gpu) {
        unsigned int n;
        ASSERT_TRUE(ezcu == NULL);
        ezcu_init(GPU | FIRST);
        n = urb_tree_size(&ezcu->devs);
        ASSERT_TRUE(ezcu != NULL);
        ASSERT_TRUE(ezcu->initialized[NVIDIA & EZCU_LKP_MASK]);
        ASSERT_FALSE(ezcu->initialized[AMD & EZCU_LKP_MASK]);
        ASSERT_FALSE(ezcu->initialized[APPLE & EZCU_LKP_MASK]);
        ASSERT_FALSE(ezcu->initialized[INTEL & EZCU_LKP_MASK]);
        ezcu_release();
        ASSERT_EQ(n, 1);
    }

    TEST_F(BaseTest, init_nvidia_first_gpu) {
        unsigned int n;
        ASSERT_TRUE(ezcu == NULL);
        ezcu_init(NVIDIA | GPU | FIRST);
        n = urb_tree_size(&ezcu->devs);
        ASSERT_TRUE(ezcu != NULL);
        ASSERT_TRUE(ezcu->initialized[NVIDIA & EZCU_LKP_MASK]);
        ASSERT_FALSE(ezcu->initialized[AMD & EZCU_LKP_MASK]);
        ASSERT_FALSE(ezcu->initialized[APPLE & EZCU_LKP_MASK]);
        ezcu_release();
        ASSERT_EQ(n, 1);
    }

    ///
    /// TODO: fix cudaSetDevice within another thread.
    TEST_F(BaseTest, init_with_cc) {
        size_t i;
        ASSERT_TRUE(ezcu == NULL);
        ezcu_init(GPU | CC20 | CC35);
        ASSERT_GE(urb_tree_size(&ezcu->devs), 1);
        ezcu_dev_prop_flags_t nDevProps[] = {CC20, CC30, CC35, CC50, CC60};
        ezcu_dev_t d = ezcu_dev_find(DEFAULT);
        int minor = d->minor;
        int major = d->major;
        ezcu_release();        
        for (i=0; i<sizeof(nDevProps)/sizeof(*nDevProps); i++) {
            if (__ezcu_dev_get_cc_hex(minor, major) >=
                ezcu_flags_to_dev_prop(nDevProps[i])) {
                ezcu_init(nDevProps[i]);
                ASSERT_GE(urb_tree_size(&ezcu->devs), 1);
                ezcu_release();        
            }
        }   
    }

    TEST_F(BaseTest, load) {
        ASSERT_TRUE(ezcu == NULL);
        ezcu_init(ALL);
        ezcu_load(PREFIX"/knl_test.cu", NULL);
        ezcu_release();
        ASSERT_TRUE(ezcu == NULL);
    }

    TEST_F(BaseTest, info) {
        ezcu_init(DEFAULT);
        ezcu_info();
        ezcu_release();
    }

    TEST_F(BaseTest, stress_init_release) {
        int i;        
        for (i=0; i<N; i++) {
            ezcu_init(ALL);
            ezcu_info();
            ezcu_release();
        }
    }

}  // namespace
