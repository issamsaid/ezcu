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
/// 3. Neither the name of the copyright holder nor the names of its contributors
///    may be used to endorse or promote products derived from this software
///    without specific prior written permission.
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
/// @file ezcu_util.c
/// @author Issam SAID
/// @brief The implementation of the ezcu utilities.
///
#include <string.h>
#include <ezcu/util.h>
#include <ezcu/flags.h>
#include <__ezcu/types-inl.h>
#include <__ezcu/dev-inl.h>
#include <__ezcu/mem-inl.h>
#include <__ezcu/knl-inl.h>
#include <__ezcu/util-inl.h>

extern ezcu_env_t ezcu;

ezcu_flags_t ezcu_str2flags(const char* str) {
  ezcu_flags_t flags;
  if (strcmp(str, "HWA") == 0)
    flags = HWA;
  else if (strcmp(str, "HOST") == 0)
    flags = HOST;
  else if (strcmp(str, "CPU") == 0)
    flags = CPU;
  else if (strcmp(str, "GPU") == 0)
    flags = GPU;
  else if (strcmp(str, "ALL") == 0)
    flags = ALL;
  else if (strcmp(str, "ACCELERATOR") == 0)
    flags = ACCELERATOR;
  else if (strcmp(str, "NVIDIA") == 0)
    flags = NVIDIA;
  else if (strcmp(str, "AMD") == 0)
    flags = AMD;
  else if (strcmp(str, "APPLE") == 0)
    flags = APPLE;
  else if (strcmp(str, "INTEL") == 0)
    flags = INTEL;
  else if (strcmp(str, "FIRST") == 0)
    flags = FIRST;
  else if (strcmp(str, "SECOND") == 0)
    flags = SECOND;
  else if (strcmp(str, "THIRD") == 0)
    flags = THIRD;
  else if (strcmp(str, "FOURTH") == 0)
    flags = FOURTH;
  else if (strcmp(str, "FIFTH") == 0)
    flags = FIFTH;
  else if (strcmp(str, "SIXTH") == 0)
    flags = SIXTH;
  else if (strcmp(str, "SEVENTH") == 0)
    flags = SEVENTH;
  else if (strcmp(str, "EIGHTH") == 0)
    flags = EIGHTH;
  else if (strcmp(str, "CC20") == 0)
    flags = CC20;
  else if (strcmp(str, "CC30") == 0)
    flags = CC30;
  else if (strcmp(str, "CC35") == 0)
    flags = CC35;
  else if (strcmp(str, "CC50") == 0)
    flags = CC50;
  else if (strcmp(str, "CC60") == 0)
    flags = CC60;
  else if (strcmp(str, "PINNED") == 0)
    flags = PINNED;
  else if (strcmp(str, "ZERO_COPY") == 0)
    flags = ZERO_COPY;
  else if (strcmp(str, "READ_ONLY") == 0)
    flags = READ_ONLY;
  else if (strcmp(str, "WRITE_ONLY") == 0)
    flags = WRITE_ONLY;
  else if (strcmp(str, "READ_WRITE") == 0)
    flags = READ_WRITE;
  else
    EZCU_EXIT("unknown ezcu_flags_t");
  return flags;
}

bool ezcu_has(ezcu_flags_t flags) { 
  if (ezcu != NULL) {
    return ezcu_count(flags) > 0; 
  }
  return false;
}

size_t ezcu_count(ezcu_flags_t flags) {
  size_t nb = 0;
  if (ezcu != NULL) {

        /// Parse flags. 
    int i, j, k, ndt, ndp, ndi;
    ezcu_vendor_flags_t v;
    ezcu_dev_type_flags_t  dt[EZCU_NB_DEV_TYPES];
    ezcu_dev_prop_flags_t  dp[EZCU_NB_DEV_PROPS];
    ezcu_dev_index_flags_t di[EZCU_NB_DEV_INDEXES];

    ezcu_flags_check_dev(flags);

    if (EZCU_FLAGS_HAVE(flags, ALL)) {
      flags &= ~ALL;
      flags |= CPU | GPU;
    } else {
      if (EZCU_FLAGS_HAVE(flags, ACCELERATOR)) {
        flags &= ~ACCELERATOR;
        flags |= GPU;
      }
    }

    ezcu_flags_parse_dev(flags, &v, dt, &ndt, dp, &ndp, di, &ndi);

    if (ezcu->initialized[v & EZCU_LKP_MASK]) {
      for (k=0; k < ndt; k++) {
        for (j=0; j < ndp; j++) {
          for (i=0; i < ndi; i++) {
            if (ezcu->lookup[dt[k] & EZCU_LKP_MASK]
                            [dp[j] & EZCU_LKP_MASK]
                            [di[i] & EZCU_LKP_MASK] != -1) nb++;
          }
        }
      }
    }
  }
  return nb;
}
