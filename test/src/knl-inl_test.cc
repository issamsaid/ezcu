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
/// @file knl-inl_test.cc
/// @author Issam SAID
/// @brief Unit testing file for the kernels manipulation private functions.
///
#include <ezcu/flags.h>
#include <__ezcu/knl-inl.h>
#include <gtest/gtest.h>

extern ezcu_env_t ezcu;

namespace {

  class KnlInlTest : public ::testing::Test {
    protected:
      virtual void SetUp() { 
        ezcu        = (ezcu_env_t) malloc(sizeof(struct __ezcu_env_t));
        ezcu->devs  = &urb_sentinel;
        ezcu->knls  = &urb_sentinel;
        ezcu->mems  = &urb_sentinel;
        ezcu->fdout = stdout;
        ezcu->fderr = stderr;
        cuInit(0);
      }
      virtual void TearDown() { 
        if (ezcu) {
          free(ezcu); 
          ezcu = NULL;
        }
      }
  };

  TEST_F(KnlInlTest, __ezcu_knl_extract) {
  }
}  // namespace