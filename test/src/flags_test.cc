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
      }
      virtual void TearDown() { 
        if (ezcu) {
          free(ezcu); 
          ezcu = NULL;
        }
      }
  };

  TEST_F(FlagsTest, flags_have) {
    int i=0;
    ezcu_flags_t flags, flags_tab[] = 
    { DEFAULT, AMD, APPLE, NVIDIA, INTEL,   
      ALL, GPU, CPU, ACCELERATOR,
      HWA, HOST, ZERO_COPY, PINNED,
      READ_ONLY, WRITE_ONLY, READ_WRITE,
      FIRST, SECOND, THIRD, FOURTH, 
      FIFTH, SIXTH, SEVENTH, EIGHTH,
      CC20, CC30, CC35, CC50, CC60,
      CHAR, INT, UNSIGNED_INT, UNSIGNED_LONG,
      LONG, SIZET, FLOAT, DOUBLE, SHORT, 0xFFFFFFFFFFFFFFFF };
    while(flags_tab[i] != 0xFFFFFFFFFFFFFFFF) {
      ASSERT_TRUE(EZCU_FLAGS_HAVE(flags_tab[i], flags_tab[i]));
      i++;    
    }

    flags = EZCU_FLAGS_DEV_INDEXES_MASK;
    ASSERT_TRUE(EZCU_FLAGS_HAVE(flags, FIRST & 0x000FF00000000000));
    ASSERT_TRUE(EZCU_FLAGS_HAVE(flags, SECOND & 0x000FF00000000000));
    ASSERT_TRUE(EZCU_FLAGS_HAVE(flags, THIRD & 0x000FF00000000000));
    ASSERT_TRUE(EZCU_FLAGS_HAVE(flags, FOURTH & 0x000FF00000000000));
    ASSERT_TRUE(EZCU_FLAGS_HAVE(flags, FIFTH & 0x000FF00000000000));
    ASSERT_TRUE(EZCU_FLAGS_HAVE(flags, SIXTH & 0x000FF00000000000));
    ASSERT_TRUE(EZCU_FLAGS_HAVE(flags, SEVENTH & 0x000FF00000000000));
    ASSERT_TRUE(EZCU_FLAGS_HAVE(flags, EIGHTH & 0x000FF00000000000));

    flags = EZCU_FLAGS_MEM_TYPES_MASK;
    ASSERT_TRUE(EZCU_FLAGS_HAVE(flags, 
      (CHAR & 0x0000FFF00000000)));
    ASSERT_TRUE(EZCU_FLAGS_HAVE(flags, 
      (INT & 0x0000FFF00000000)));
    ASSERT_TRUE(EZCU_FLAGS_HAVE(flags, 
      (UNSIGNED_INT & 0x0000FFF00000000)));
    ASSERT_TRUE(EZCU_FLAGS_HAVE(flags, 
      (UNSIGNED_LONG & 0x0000FFF00000000)));
    ASSERT_TRUE(EZCU_FLAGS_HAVE(flags, 
      (LONG & 0x0000FFF00000000)));
    ASSERT_TRUE(EZCU_FLAGS_HAVE(flags, 
      (SIZET & 0x0000FFF00000000)));
    ASSERT_TRUE(EZCU_FLAGS_HAVE(flags, 
      (FLOAT & 0x0000FFF00000000)));
    ASSERT_TRUE(EZCU_FLAGS_HAVE(flags, 
      (DOUBLE & 0x0000FFF00000000)));
    ASSERT_TRUE(EZCU_FLAGS_HAVE(flags, 
      (SHORT & 0x0000FFF00000000)));

    flags = EZCU_FLAGS_DEV_PROPS_MASK;
    ASSERT_TRUE(EZCU_FLAGS_HAVE(flags, CC20 & 0x00F00FF000000000));
    ASSERT_TRUE(EZCU_FLAGS_HAVE(flags, CC30 & 0x00F00FF000000000));
    ASSERT_TRUE(EZCU_FLAGS_HAVE(flags, CC35 & 0x00F00FF000000000));
    ASSERT_TRUE(EZCU_FLAGS_HAVE(flags, CC50 & 0x00F00FF000000000));
    ASSERT_TRUE(EZCU_FLAGS_HAVE(flags, CC60 & 0x00F00FF000000000));
  }

  TEST_F(FlagsTest, check_default) {
    ezcu_flags_t flags = DEFAULT;
    ezcu_flags_check_default(flags);
    flags = AMD | DEFAULT;
    ASSERT_DEATH(ezcu_flags_check_default(flags), ".*");
  }

  TEST_F(FlagsTest, check_vendors) {
    int i=0;
    ezcu_flags_t flags=DEFAULT, 
    vendors[] = { INTEL, AMD, APPLE, NVIDIA, 0xFFFFFFFFFFFFFFFF };
    ezcu_flags_check_vendors(flags);
    while(vendors[i] != 0xFFFFFFFFFFFFFFFF) {
      ASSERT_DEATH(ezcu_flags_check_vendors(vendors[i]|DEFAULT), ".*");
      i++;    
    }
    flags = NVIDIA;
    ezcu_flags_check_vendors(flags);
    flags = AMD | NVIDIA;
    ASSERT_DEATH(ezcu_flags_check_vendors(flags), ".*");
  }

  TEST_F(FlagsTest, check_dev_types) {
    int i=0;
    ezcu_flags_t flags=DEFAULT, 
    tab[] = { ALL, CPU, ACCELERATOR, GPU, 0xFFFFFFFFFFFFFFFF };
    ezcu_flags_check_dev_types(flags);
    while(tab[i] != 0xFFFFFFFFFFFFFFFF) {
      ASSERT_DEATH(ezcu_flags_check_dev_types(tab[i]|DEFAULT), ".*");
      i++;    
    }
    flags = ALL;
    ezcu_flags_check_dev_types(flags);
    ASSERT_TRUE(EZCU_FLAGS_HAVE(flags, ALL));
    ASSERT_TRUE(EZCU_FLAGS_HAVE(flags, CPU));
    ASSERT_TRUE(EZCU_FLAGS_HAVE(flags, GPU));
    ASSERT_TRUE(EZCU_FLAGS_HAVE(flags, ACCELERATOR));
    flags = CPU;
    ezcu_flags_check_dev_types(flags);
    flags = GPU;
    ezcu_flags_check_dev_types(flags);
    ASSERT_TRUE(EZCU_FLAGS_HAVE(flags, GPU));
        //ASSERT_TRUE(EZCU_FLAGS_HAVE(flags, ACCELERATOR));

    flags = ACCELERATOR;
    ezcu_flags_check_dev_types(flags);
    flags = CPU | GPU;
    ezcu_flags_check_dev_types(flags);
    flags = ALL | ACCELERATOR;
    ezcu_flags_check_dev_types(flags);
    flags = ALL | CPU;
    ezcu_flags_check_dev_types(flags);
  }

  TEST_F(FlagsTest, check_dev_indexes) {
    int i=0;
    ezcu_flags_t flags=DEFAULT, 
    tab[] = { FIRST, SECOND, THIRD, FOURTH, 
              FIFTH, SIXTH,  SEVENTH, EIGHTH, 0xFFFFFFFFFFFFFFFF };
    ezcu_flags_check_dev_indexes(flags);
    while(tab[i] != 0xFFFFFFFFFFFFFFFF) {
      ASSERT_DEATH(ezcu_flags_check_dev_indexes(tab[i]|DEFAULT), ".*");
      i++;    
    }        
    flags = FIRST;
    ezcu_flags_check_dev_indexes(flags);
    flags = FIRST | SECOND;
    ezcu_flags_check_dev_indexes(flags);
    flags = FIRST | SECOND | THIRD;
    ezcu_flags_check_dev_indexes(flags);
    flags = FIRST | SECOND | THIRD | FOURTH;
    ezcu_flags_check_dev_indexes(flags);
    flags = FIRST | SECOND | THIRD | FOURTH | FIFTH;
    ezcu_flags_check_dev_indexes(flags);
    flags = FIRST | SECOND | THIRD | FOURTH | FIFTH | SIXTH;
    ezcu_flags_check_dev_indexes(flags);
    flags = FIRST | SECOND | THIRD | FOURTH | FIFTH | SIXTH | SEVENTH;
    ezcu_flags_check_dev_indexes(flags);
    flags = FIRST | SECOND | THIRD | FOURTH \
    | FIFTH | SIXTH | SEVENTH | EIGHTH;
    ezcu_flags_check_dev_indexes(flags);
  }

  TEST_F(FlagsTest, check_dev_props) {
    int i=0;
    ezcu_flags_t flags=DEFAULT, 
    tab[] = {CC20, CC30, CC35, CC50, CC60, 0xFFFFFFFFFFFFFFFF };
    ezcu_flags_check_dev_props(flags);
    while(tab[i] != 0xFFFFFFFFFFFFFFFF) {
      ASSERT_DEATH(ezcu_flags_check_dev_props(tab[i]|DEFAULT), ".*");
      i++;    
    }
    flags = CC20;
    ezcu_flags_check_dev_props(flags);
    flags = CC20 | CC30;
    ezcu_flags_check_dev_props(flags);
    flags = CC35 | CC30 | CC20;
    ezcu_flags_check_dev_props(flags);
    flags = CC50 | CC20 | CC35 | CC30;
    ezcu_flags_check_dev_props(flags);
    flags = CC60 | CC50 | CC20 | CC35 | CC30;
    ezcu_flags_check_dev_props(flags);
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

  TEST_F(FlagsTest, flags_to_vendors_flags) {
    ezcu_vendor_flags_t v;
    ezcu_flags_t flags = ALL | AMD | READ_ONLY;
    ASSERT_EQ(ezcu_flags_to_vendors_flags(flags, &v), 1);
    ASSERT_EQ(v, AMD);
    flags = DEFAULT;
    ASSERT_EQ(ezcu_flags_to_vendors_flags(flags, &v), 1);
    ASSERT_EQ(v, NVIDIA);
    flags = ALL | READ_ONLY | NVIDIA;
    ASSERT_EQ(ezcu_flags_to_vendors_flags(flags, &v), 1);
    ASSERT_EQ(v, NVIDIA);
  }

  TEST_F(FlagsTest, flags_to_dev_types_flags) {
    ezcu_dev_type_flags_t v[EZCU_NB_DEV_TYPES];
    ezcu_flags_t flags = CPU | AMD | READ_ONLY;
    ASSERT_EQ(ezcu_flags_to_dev_types_flags(flags, v), 1);
    ASSERT_EQ(v[0], CPU);
    flags = DEFAULT;
    ASSERT_EQ(ezcu_flags_to_dev_types_flags(flags, v), 4);
    ASSERT_EQ(v[0], ALL);
    flags = CPU | READ_ONLY | NVIDIA | GPU;
    ASSERT_EQ(ezcu_flags_to_dev_types_flags(flags, v), 2);
    ASSERT_EQ(v[0], CPU);
    ASSERT_EQ(v[1], GPU);
  }

  TEST_F(FlagsTest, flags_to_dev_props_flags) {
    ezcu_dev_prop_flags_t v[EZCU_NB_DEV_PROPS];
    ezcu_flags_t flags = CC50 | AMD | READ_ONLY;
    ASSERT_EQ(ezcu_flags_to_dev_props_flags(flags, v), 4);
    ASSERT_EQ(v[3], CC50);
    flags = DEFAULT;
    ASSERT_EQ(ezcu_flags_to_dev_props_flags(flags, v), 1);
    ASSERT_EQ(v[0], CC20);
    flags = CC20 | READ_ONLY | NVIDIA | GPU | CC50;
    ASSERT_EQ(ezcu_flags_to_dev_props_flags(flags, v), 4);
    ASSERT_EQ(v[0], CC20);
    ASSERT_EQ(v[3], CC50);
  }

  TEST_F(FlagsTest, flags_to_dev_index) {
    int i=0, N=8;
    ezcu_dev_index_flags_t 
    tab[] = { FIRST, SECOND, THIRD, FOURTH, 
     FIFTH, SIXTH,  SEVENTH, EIGHTH};
     int val[] = { 0, 1, 2, 3, 4, 5, 6, 7};
     while(i < N) {
      ASSERT_EQ(ezcu_flags_to_dev_index(tab[i]), val[i]);
      i++;    
    }        
  }

  TEST_F(FlagsTest, flags_to_dev_prop) {
    int i=0, N=5;
    ezcu_dev_prop_flags_t 
    tab[] = { CC20, CC30, CC35, CC50, CC60};
    int val[] = { 0x20, 0x30, 0x35, 0x50, 0x60};
    while(i < N) {
      ASSERT_EQ(ezcu_flags_to_dev_prop(tab[i]), val[i]);
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
    ezcu_flags_dev_to_str(ACCELERATOR, tmp_0);
    ASSERT_STREQ(tmp_0, "  GPU | ACCELERATOR");
    ezcu_flags_dev_to_str(GPU, tmp_0);
    ASSERT_STREQ(tmp_0, "  GPU");
    ezcu_flags_dev_to_str(CPU, tmp_0);
    ASSERT_STREQ(tmp_0, "  CPU");
    ezcu_flags_dev_to_str(ALL, tmp_0);
    ASSERT_STREQ(tmp_0, "  ALL | GPU | CPU | ACCELERATOR");
  }
}  // namespace
