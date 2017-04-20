///
/// @copyright Copyright (c) 2016-, Issam SAID <said.issam@gmail.com>
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without
/// modification, are permetted provided that the following conditions
/// are met:
///
/// 1. Redistributions of source code must retain the above copyright
///    notice, this list of conditions and the following disclaimer.
/// 2. Redistributions in binary form must reproduce the above copyright
///    notice, this list of conditions and the following disclaimer in the
///    documentation and/or other materials provided with the distribution.
/// 3. Neither the name of the COPYRIGHT HOLDER nor the names of its contributors
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
/// @file knl_performance.h
/// @author Issam SAID
/// @brief Extra routines used to evaluate the performance of the ezCU kernels
/// manipulation routines.
///
#include <__ezcu/error-inl.h>
#include <__ezcu/mem-inl.h>
#include <__ezcu/knl-inl.h>
#include <__ezcu/config/dev.h>

inline void ezcu_fknl_sync_exec(ezcu_knl_t k, ezcu_dev_t d) {
    urb_tree_walk(&k->mems, NULL, __ezcu_mem_sync);
    EZCU_CHECK(cuCtxPushCurrent(d->ctx), "failed to push the context");
    EZCU_CHECK(cuLaunchKernel(k->id, 
                              k->grid[0], k->grid[1], k->grid[2],
                              k->block[0], k->block[1], k->block[2],
                              0, d->streams[__EZCU_DEV_CPT_STREAM], 
                              k->args, NULL),
               "failed to launch CUDA kernel");
    EZCU_CHECK(cuCtxSynchronize(), "failed to synchronize the device context");
    EZCU_CHECK(cuCtxPopCurrent(&d->ctx), "failed to pop the context");
}

inline void ezcu_fknl_exec(ezcu_knl_t k, ezcu_dev_t d) {
    urb_tree_walk(&k->mems, NULL, __ezcu_mem_sync);
    EZCU_CHECK(cuCtxPushCurrent(d->ctx), "failed to push the context");
    EZCU_CHECK(cuLaunchKernel(k->id, 
                              k->grid[0], k->grid[1], k->grid[2],
                              k->block[0], k->block[1], k->block[2],
                              0, d->streams[__EZCU_DEV_CPT_STREAM], 
                              k->args, NULL),
               "failed to launch CUDA kernel");
    EZCU_CHECK(cuCtxPopCurrent(&d->ctx), "failed to pop the context");
}

inline void ezcu_fknl_sync_run(ezcu_knl_t k, ezcu_dev_t d, ...) {
    va_list list;
    va_start(list, d);
    __ezcu_knl_set_args_valist(k, list);
    va_end(list);
    urb_tree_walk(&k->mems, NULL, __ezcu_mem_sync);
    EZCU_CHECK(cuCtxPushCurrent(d->ctx), "failed to push the context");
    EZCU_CHECK(cuLaunchKernel(k->id, 
                              k->grid[0], k->grid[1], k->grid[2],
                              k->block[0], k->block[1], k->block[2],
                              0, d->streams[__EZCU_DEV_CPT_STREAM], 
                              k->args, NULL),
               "failed to launch CUDA kernel");
    EZCU_CHECK(cuCtxSynchronize(), "failed to synchronize the device context");
    EZCU_CHECK(cuCtxPopCurrent(&d->ctx), "failed to pop the context");
}

inline void ezcu_fknl_run(ezcu_knl_t k, ezcu_dev_t d, ...) {
    va_list list;
    va_start(list, d);
    __ezcu_knl_set_args_valist(k, list);
    va_end(list);
    urb_tree_walk(&k->mems, NULL, __ezcu_mem_sync);
    EZCU_CHECK(cuCtxPushCurrent(d->ctx), "failed to push the context");
    EZCU_CHECK(cuLaunchKernel(k->id, 
                              k->grid[0], k->grid[1], k->grid[2],
                              k->block[0], k->block[1], k->block[2],
                              0, d->streams[__EZCU_DEV_CPT_STREAM], 
                              k->args, NULL),
               "failed to launch CUDA kernel");
    EZCU_CHECK(cuCtxPopCurrent(&d->ctx), "failed to pop the context");
}
