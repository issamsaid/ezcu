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
/// @brief The implementation of the ezCU utilities.
///
#include <ezCU/util.h>
#include <__ezcu/dev-inl.h>
#include <__ezcu/mem-inl.h>
#include <__ezcu/knl-inl.h>
#include <__ezcu/util-inl.h>
#include <string.h>

extern ezcu_env_t ezcu;

ezcu_flags_t ezcu_str_to_ezcu_flags_t(const char* str) {
    ezcu_flags_t flags;
    if (strcmp(str, "HWA") == 0)
        flags = HWA;
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

bool ezcu_has(ezcu_flags_t flags) { return ezcu_count(flags) > 0; }

unsigned int ezcu_count(ezcu_flags_t flags) {
    return __ezcu_dev_count();
}

unsigned int ezcu_mem_count() {
    if (ezcu != NULL) return urb_tree_size(&ezcu->mems);
    else return 0;
}

unsigned int ezcu_knl_count() { 
    if (ezcu != NULL) return urb_tree_size(&ezcu->knls);
    else return 0;
}