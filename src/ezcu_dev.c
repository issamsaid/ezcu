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
/// @file ezcu_dev.c
/// @author Issam SAID
/// @brief The implementation of the ezCU devices manipulation routines.
///
#include <ezCU/dev.h>
#include <stdio.h>
#include <string.h>
#include <__ezcu/config/cuda.h>
#include <__ezcu/config/util.h>
#include <__ezcu/dev-inl.h>

extern ezcu_env_t ezcu;

ezcu_dev_t ezcu_dev_find(ezcu_flags_t flags) {
    int id, ndt, ndp, ndi;
    ezcu_dev_t device;
    ezcu_vendor_flags_t    v;
    ezcu_dev_type_flags_t  dt[EZCU_NB_DEV_TYPES];
    ezcu_dev_prop_flags_t  dp[EZCU_NB_DEV_PROPS];
    ezcu_dev_index_flags_t di[EZCU_NB_DEV_INDEXES];
    char tmp[__EZCU_STR_SIZE];
    ezcu_flags_dev_to_str(flags, tmp);
    EZCU_DEBUG("start dev find: flags = %s", tmp);
    ezcu_flags_check_dev(flags);    
    ezcu_flags_parse_dev(flags, &v, dt, &ndt, dp, &ndp, di, &ndi);
    EZCU_EXIT_IF(ndi > 1,
                 "flags should correspond to only one device at a time");
    if((ezcu->initialized[v & EZCU_LKP_MASK]) &&
       (id=ezcu->lookup[dt[0] & EZCU_LKP_MASK]
                      [dp[ndp-1] & EZCU_LKP_MASK]
                      [di[0] & EZCU_LKP_MASK]) != -1) {
        device = (ezcu_dev_t)urb_tree_find(&ezcu->devs, 
                                           &id, __ezcu_int_cmp)->value;
    }
    EZCU_EXIT_IF(device == NULL,
                "device can not found, check flags, aborting (flags =%s)", tmp);
    EZCU_DEBUG("end   dev find: found id = %d", device->id);
    return device;
}

void ezcu_dev_wait(ezcu_dev_t d) { 
    EZCU_DEBUG("start dev wait: id = %d", d->id);
    EZCU_CHECK(cuCtxPushCurrent(d->ctx), "failed to push the device context");
    EZCU_CHECK(cuCtxSynchronize(), "failed to synchronize the device context");
    EZCU_CHECK(cuCtxPopCurrent(&d->ctx), "failed to pop the device context");
    EZCU_DEBUG("end   dev wait: id = %d", d->id);
}
