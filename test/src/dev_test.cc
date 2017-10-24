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
///    contributors may be used to endorse or promote products derived from this software
///    without specific prior written permission.
///
/// THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
/// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
/// AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
/// THE COPYRIGHT HOLDER OR ITS CONTRIBUTERS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
/// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
/// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
/// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
/// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
/// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
/// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
/// 
/// @file dev_test.cc
/// @author Issam SAID
/// @brief Unit testing file for the devices manipulation private functions.
///
#include <ezcu/flags.h>
#include <ezcu/base.h>
#include <ezcu/util.h>
#include <ezcu/dev.h>
#include <__ezcu/dev-inl.h>
#include <gtest/gtest.h>

namespace {

    class DevTest : public ::testing::Test {
    protected:
        virtual void SetUp() { ezcu_init(FIRST); }

        virtual void TearDown() { ezcu_release(); }
    };

    TEST_F(DevTest, find_default) {
        size_t n       = ezcu_count(ALL);
        ezcu_dev_t d0  = ezcu_dev_find(DEFAULT);
        ezcu_dev_t d2  = ezcu_dev_find(FIRST);
        ASSERT_EQ(d0, d2);
        ASSERT_GE(n, (size_t)1);
    }
      
    TEST_F(DevTest, find_all) {
        ezcu_dev_t d1  = ezcu_dev_find(ALL | FIRST);
        ezcu_dev_t d2  = ezcu_dev_find(FIRST);
        ASSERT_EQ(d1, d2);
    }

    TEST_F(DevTest, find_cpu) {
        //ASSERT_DEATH(ezcu_dev_find(CPU), ".*");
    }

    TEST_F(DevTest, find_gpu) {
        ezcu_dev_t gpu1 = ezcu_dev_find(GPU | FIRST);
        ezcu_dev_t gpu2 = ezcu_dev_find(GPU | FIRST);
        ASSERT_EQ(gpu1, gpu2);
    }

    TEST_F(DevTest, find_accelerator) {
        ezcu_dev_t gpu1 = ezcu_dev_find(ACCELERATOR | FIRST);
        ezcu_dev_t gpu2 = ezcu_dev_find(GPU | FIRST);
        ASSERT_EQ(gpu1, gpu2);
    }

    TEST_F(DevTest, find_by_index) {
        ezcu_dev_t first  = ezcu_dev_find(FIRST);
        ezcu_dev_t def    = ezcu_dev_find(DEFAULT);
        ASSERT_EQ(first, def);
    }

    TEST_F(DevTest, info) {
        ezcu_dev_t first = ezcu_dev_find(FIRST);
        __ezcu_dev_info(first);
    }

    TEST_F(DevTest, wait) {
        ezcu_dev_t first = ezcu_dev_find(FIRST);
        ezcu_dev_wait(first);
    }

}  // namespace
