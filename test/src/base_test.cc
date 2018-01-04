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
/// @brief The unit testing file of the ezcu setup main routines.
///
#include <ezcu/flags.h>
#include <ezcu/base.h>
#include <ezcu/util.h>
#include <ezcu/dev.h>
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
    ezcu_init();
    ASSERT_TRUE(ezcu != NULL);
    ezcu_release();
    ASSERT_TRUE(ezcu == NULL);
  }

  TEST_F(BaseTest, init_default_singleton) {
    ASSERT_TRUE(ezcu == NULL);
    ezcu_init();
    ASSERT_TRUE(ezcu != NULL);
    ezcu_init();
    ezcu_init();
    ezcu_init();
    ezcu_init();
    ezcu_init();
    ASSERT_TRUE(ezcu != NULL);
    ezcu_release();
    ASSERT_TRUE(ezcu == NULL);
  }

  TEST_F(BaseTest, release_default_singleton) {
    ASSERT_TRUE(ezcu == NULL);
    ezcu_init();
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

  TEST_F(BaseTest, load) {
    ASSERT_TRUE(ezcu == NULL);
    ezcu_init();
    ezcu_load(PREFIX"/knl_test.cu", NULL);
    ezcu_release();
    ASSERT_TRUE(ezcu == NULL);
  }

  TEST_F(BaseTest, info) {
    ezcu_init();
    ezcu_info();
    ezcu_release();
  }

  TEST_F(BaseTest, stress_init_release) {
    int i;        
    for (i=0; i<N; i++) {
      ezcu_init();
      ezcu_info();
      ezcu_release();
    }
  }

}  // namespace
