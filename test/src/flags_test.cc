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
/// @file ezcu_flags_test.cc
/// @author Issam SAID
/// @brief Unit testing file for the ezcu bitwise flags manipulation.
///
#include <ezcu/flags.h>
#include <ezcu/types.h>
#include <__ezcu/types-inl.h>
#include <gtest/gtest.h>

extern ezcu_env_t ezcu;

namespace {

  class FlagsTest : public ::testing::Test {
    protected:
      virtual void SetUp() { 
        ezcu        = (ezcu_env_t) malloc(sizeof(struct __ezcu_env_t));
        ezcu->devs  = &urb_sentinel;
        ezcu->knls  = &urb_sentinel;
        ezcu->mems  = &urb_sentinel;
        ezcu->fdout = stdout;
        ezcu->fderr = stderr;
				::testing::FLAGS_gtest_death_test_style = "threadsafe";
				testing::internal::CaptureStdout();
      }
      virtual void TearDown() { 
        if (ezcu) {
				  std::string output = testing::internal::GetCapturedStdout();
          free(ezcu); 
          ezcu = NULL;
        }
      }
  };

  TEST_F(FlagsTest, flags_have) {
    int i=0;
    ezcu_flags_t flags_tab[] = 
    { DEFAULT, 
      HWA, HOST, ZERO_COPY, PINNED,
      READ_ONLY, WRITE_ONLY, READ_WRITE,
      CHAR, INT, UNSIGNED_INT, UNSIGNED_LONG,
      LONG, SIZET, FLOAT, DOUBLE, SHORT, 0xFFFFFFFFFFFFFFFF };
    while(flags_tab[i] != 0xFFFFFFFFFFFFFFFF) {
      ASSERT_TRUE(EZCU_FLAGS_HAVE(flags_tab[i], flags_tab[i]));
      i++;    
    }
  }

  TEST_F(FlagsTest, check_default) {
    ezcu_flags_t flags = DEFAULT;
    ezcu_flags_check_default(flags);
    flags = ZERO_COPY | DEFAULT;
    ASSERT_DEATH(ezcu_flags_check_default(flags), ".*");
  }
 
  TEST_F(FlagsTest, check_mem_locations) {
    int i=0;
    ezcu_flags_t flags=DEFAULT, 
    tab[] = { HOST, HWA, 0xFFFFFFFFFFFFFFFF };
    ezcu_flags_check_mem_locations(flags);
    while(tab[i] != 0xFFFFFFFFFFFFFFFF) {
      ASSERT_DEATH(ezcu_flags_check_mem_locations(tab[i]|DEFAULT), ".*");
      i++;    
    }
    flags = HWA;
    ezcu_flags_check_mem_locations(flags);
    flags = HOST;
    ezcu_flags_check_mem_locations(flags);
    flags = HOST | HWA;
    ASSERT_DEATH(ezcu_flags_check_mem_locations(flags), ".*");
  }
   
  TEST_F(FlagsTest, check_mem_props) {
    int i=0;
    ezcu_flags_t flags=DEFAULT, 
    tab[] = { PINNED, ZERO_COPY, 0xFFFFFFFFFFFFFFFF };
    ezcu_flags_check_mem_props(flags);
    while(tab[i] != 0xFFFFFFFFFFFFFFFF) {
      ASSERT_DEATH(ezcu_flags_check_mem_props(tab[i]|DEFAULT), ".*");
      i++;    
    }
    flags = PINNED;
    ezcu_flags_check_mem_props(flags);
    flags = ZERO_COPY;
    ezcu_flags_check_mem_props(flags);
    flags = PINNED | ZERO_COPY;
    ASSERT_DEATH(ezcu_flags_check_mem_props(flags), ".*");
  }

  TEST_F(FlagsTest, check_mem_access_modes) {
    int i=0;
    ezcu_flags_t flags=DEFAULT, 
    tab[] = { READ_ONLY, WRITE_ONLY, READ_WRITE, 0xFFFFFFFFFFFFFFFF };
    ezcu_flags_check_mem_access_modes(flags);
    while(tab[i] != 0xFFFFFFFFFFFFFFFF) {
      ASSERT_DEATH(ezcu_flags_check_mem_access_modes(tab[i]|DEFAULT), ".*");
      i++;    
    }
    flags = READ_ONLY;
    ezcu_flags_check_mem_access_modes(flags);
    flags = READ_WRITE;
    ezcu_flags_check_mem_access_modes(flags);
    flags = WRITE_ONLY;
    ezcu_flags_check_mem_access_modes(flags);
    flags = WRITE_ONLY | READ_ONLY;
    ASSERT_DEATH(ezcu_flags_check_mem_access_modes(flags), ".*");
    flags = WRITE_ONLY | READ_WRITE;
    ASSERT_DEATH(ezcu_flags_check_mem_access_modes(flags), ".*");
    flags = READ_ONLY  | READ_WRITE;
    ASSERT_DEATH(ezcu_flags_check_mem_access_modes(flags), ".*");
  }

  TEST_F(FlagsTest, check_mem_types) {
    int i, j;
    ezcu_flags_t flags = DEFAULT;
    ezcu_flags_t mem_types[] = 
    { CHAR, INT, UNSIGNED_INT, UNSIGNED_LONG,
      LONG, SIZET, FLOAT, SHORT, DOUBLE, (0xFFFFFFFFFFFFFFFF) };
    ezcu_flags_check_mem_types(flags);
    i=0, j=0;        
    while(mem_types[i] != (0xFFFFFFFFFFFFFFFF)) {
      ASSERT_DEATH(
        ezcu_flags_check_mem_types((mem_types[i] | DEFAULT)), ".*");
      i++;
    }
    flags = CHAR;
    ASSERT_TRUE((flags & EZCU_FLAGS_MEM_TYPES_MASK)!=0);
    ezcu_flags_check_mem_types(flags);
    flags = INT;
    ASSERT_TRUE((flags & EZCU_FLAGS_MEM_TYPES_MASK)!=0);
    ezcu_flags_check_mem_types(flags);
    flags = UNSIGNED_INT;
    ASSERT_TRUE((flags & EZCU_FLAGS_MEM_TYPES_MASK)!=0);
    ezcu_flags_check_mem_types(flags);
    flags = UNSIGNED_LONG;
    ASSERT_TRUE((flags & EZCU_FLAGS_MEM_TYPES_MASK)!=0);
    ezcu_flags_check_mem_types(flags);
    flags = LONG;
    ASSERT_TRUE((flags & EZCU_FLAGS_MEM_TYPES_MASK)!=0);
    ezcu_flags_check_mem_types(flags);
    flags = SIZET;
    ASSERT_TRUE((flags & EZCU_FLAGS_MEM_TYPES_MASK)!=0);
    ezcu_flags_check_mem_types(flags);
    flags = FLOAT;
    ASSERT_TRUE((flags & EZCU_FLAGS_MEM_TYPES_MASK)!=0);
    ezcu_flags_check_mem_types(flags);
    flags = DOUBLE;
    ASSERT_TRUE((flags & EZCU_FLAGS_MEM_TYPES_MASK)!=0);
    ezcu_flags_check_mem_types(flags);
    flags = SHORT;
    ASSERT_TRUE((flags & EZCU_FLAGS_MEM_TYPES_MASK)!=0);
    ezcu_flags_check_mem_types(flags);
    i=0, j=0;
    while(mem_types[i] != (0xFFFFFFFFFFFFFFFF)) {
      while(mem_types[j] != (0xFFFFFFFFFFFFFFFF)) {
        if (mem_types[i] != mem_types[j]) {
          ASSERT_DEATH(
            ezcu_flags_check_mem_types((mem_types[i] |
              mem_types[j])), ".*");
        }
        j++;
      }
      i++;
    }
  }

  TEST_F(FlagsTest, flags_to_mem_unit_size) {
    int i=0, N=8;
    ezcu_mem_types_flags_t 
    tab[] = { CHAR, SHORT, INT, UNSIGNED_INT, LONG, 
              UNSIGNED_LONG, FLOAT, DOUBLE, SIZET};
    size_t val[] = { sizeof(char), 
                     sizeof(short), 
                     sizeof(int), 
                     sizeof(unsigned int),
                     sizeof(long),
                     sizeof(unsigned long),
                     sizeof(float),
                     sizeof(double),
                     sizeof(size_t)};
    while(i < N) {
      ASSERT_EQ(ezcu_flags_to_mem_unit_size(tab[i]), val[i]);
      i++;    
    }        
  }

  TEST_F(FlagsTest, flags_dev_to_str) {
    char tmp_0[__EZCU_STR_SIZE];
    ezcu_flags_mem_to_str(HWA, tmp_0);
    ASSERT_STREQ(tmp_0, "  HWA");
    ezcu_flags_mem_to_str(HWA | ZERO_COPY, tmp_0);
    ASSERT_STREQ(tmp_0, "  HWA | ZERO_COPY");
    ezcu_flags_mem_to_str(HWA | PINNED, tmp_0);
    ASSERT_STREQ(tmp_0, "  HWA | PINNED");
    ezcu_flags_mem_to_str(HWA | PINNED | READ_WRITE, tmp_0);
    ASSERT_STREQ(tmp_0, "  HWA | PINNED | READ_WRITE");
  }
}  // namespace
