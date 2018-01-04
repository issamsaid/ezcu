///
/// @copyright Copyright (c) 2016-, Issam SAID <said.issam@gmail.com>
/// DEFAULT rights reserved.
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
/// 3. Neither the name of the copyright holder nor the names of its contributors
///    may be used to endorse or promote products derived from this software
///    without specific prior written permission.
///
/// THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
/// INCLUDING, BUT NOT LIMITED TO, WARRANTIES OF MERCHANTABILITY AND FITNESS
/// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHDEFAULT THE copyright holder OR
/// ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
/// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
/// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
/// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
/// LIABILITY, WETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
/// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
/// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///
/// @file util_test.cc
/// @author Issam SAID
/// @brief Unit testing file for the ezcu utilities.
///
#include <ezcu/flags.h>
#include <ezcu/base.h>
#include <ezcu/util.h>
#include <__ezcu/dev-inl.h>
#include <gtest/gtest.h>

namespace {

  class UtilTest : public ::testing::Test {
  protected:
    virtual void SetUp() {
    	ezcu_init();
		}
    virtual void TearDown() {
    	ezcu_release();
		}
  };

  TEST_F(UtilTest, ezcu_str2flags) {
		ezcu_flags_t flags[] = {HWA, HOST, PINNED, ZERO_COPY,
																		 READ_ONLY, READ_WRITE, WRITE_ONLY,
																		 CHAR, INT, LONG, FLOAT, DOUBLE, SIZET,
																		 UNSIGNED_INT, UNSIGNED_LONG, SHORT, 
																		 POINTER};
		const char * strs[] = {"HWA", "HOST", "PINNED", "ZERO_COPY",              
                     "READ_ONLY", "READ_WRITE", "WRITE_ONLY",         
                     "CHAR", "INT", "LONG", "FLOAT", "DOUBLE", "SIZET",     
                     "UNSIGNED_INT", "UNSIGNED_LONG", "SHORT", 
									   "POINTER", "STOP"};
    int i=0;
		while(strcmp(strs[i], "STOP")) {
			ASSERT_EQ(
				static_cast<ezcu_flags_t>(ezcu_str2flags(strs[i])), flags[i]);
			i++;
		}
  }

  TEST_F(UtilTest, ezcu_count) {
    size_t n, s;
    n = __ezcu_dev_query();
    s = ezcu_count();
    ASSERT_EQ(n, s);
  }
}  // namespace
