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
/// 3. Neither the func of the copyright holder nor the funcs of its 
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
/// @file ezcu_knl.c
/// @author Issam SAID
/// @brief The implementation of the ezCU kernels manipulation routines.
///
#include <stdio.h>
#include <string.h>
#include <ezCU/knl.h>
#include <ezCU/timer.h>
#include <ezCU/mem.h>
#include <__ezcu/config/cuda.h>
#include <__ezcu/config/util.h>
#include <__ezcu/config/dev.h>
#include <__ezcu/mem-inl.h>
#include <__ezcu/knl-inl.h>

inline void ezcu_knl_set_wrk(const char *name,
                             unsigned int wrk, 
                             unsigned int *grid, unsigned int *block) {
    ezcu_knl_t k = __ezcu_knl_find(name);
    unsigned int i;
    EZCU_DEBUG("start knl set wrk %s", name);
    for (i=0; i<__EZCU_KNL_MAX_GRID_DIM; i++) {
        k->grid[i]  = grid[i];
        k->block[i] = block[i];
    }
    EZCU_DEBUG("grid = {%u, %u, %u}, block = {%u, %u, %u}",
               k->grid[0], k->grid[1], k->grid[2],
               k->block[0], k->block[1], k->block[2]);
    EZCU_DEBUG("end   knl set wrk %s", name);
    EZCU_DEBUG("");
}

inline void ezcu_knl_set_char(const char *name, int idx, char c) {
    ezcu_knl_t k = __ezcu_knl_find(name);
    return __ezcu_knl_set_char(k, idx, &c);
}

inline void ezcu_knl_set_int32(const char *name, int idx, int32_t i32) {
    ezcu_knl_t k = __ezcu_knl_find(name);
    return __ezcu_knl_set_int32(k, idx, &i32);
}

inline void ezcu_knl_set_uint32(const char *name, int idx, uint32_t i32) {
    ezcu_knl_t k = __ezcu_knl_find(name);
    return __ezcu_knl_set_uint32(k, idx, &i32);
}

inline void ezcu_knl_set_int64(const char *name, int idx, int64_t i64) {
    ezcu_knl_t k = __ezcu_knl_find(name);
    return __ezcu_knl_set_int64(k, idx, &i64);
}

inline void ezcu_knl_set_uint64(const char *name, int idx, uint64_t i64) {
    ezcu_knl_t k = __ezcu_knl_find(name);
    return __ezcu_knl_set_uint64(k, idx, &i64);
}

inline void ezcu_knl_set_float(const char *name, int idx, float f) {
    ezcu_knl_t k = __ezcu_knl_find(name);
    return __ezcu_knl_set_float(k, idx, &f);
}

inline void ezcu_knl_set_double(const char *name, int idx, double d) {
    ezcu_knl_t k = __ezcu_knl_find(name);
    return __ezcu_knl_set_double(k, idx, &d);
}

inline void ezcu_knl_set_mem(const char *name, int idx, void *h) {
    ezcu_knl_t k = __ezcu_knl_find(name);
    return __ezcu_knl_set_mem(k, idx, h);
}

inline void ezcu_knl_set_args(const char *name, ...) {
    ezcu_knl_t k = __ezcu_knl_find(name);
    va_list list;
    va_start(list, name);
    __ezcu_knl_set_args_valist(k, list);
    va_end(list);
}

inline void ezcu_knl_set_shared(const char *name, unsigned int bytes) {
    ezcu_knl_t k = __ezcu_knl_find(name);
    k->smem = bytes;
}

///
/// WARNING: 
///   - run (default) : asynchronous run
///   - sync run      : synchronous run
///   - timed run     : timed synchronous execution
///
inline void ezcu_knl_exec(const char *name, ezcu_dev_t d) {
    ezcu_knl_t k = __ezcu_knl_find(name);
    EZCU_DEBUG("start exec (async) kernel : %s", name);
    urb_tree_walk(&k->mems, NULL, __ezcu_mem_sync);
    EZCU_CHECK(cuCtxPushCurrent(d->ctx), "failed to push the context");
    EZCU_CHECK(cuLaunchKernel(k->id, 
                              k->grid[0], k->grid[1], k->grid[2],
                              k->block[0], k->block[1], k->block[2],
                              k->smem, d->streams[__EZCU_DEV_CPT_STREAM], 
                              k->args, NULL),
               "failed to launch CUDA kernel '%s'", name);
    EZCU_CHECK(cuCtxPopCurrent(&d->ctx), "failed to pop the context");
    EZCU_DEBUG("end   exec (async) kernel : %s", name);
    EZCU_DEBUG("");
}

inline void ezcu_knl_sync_exec(const char *name, ezcu_dev_t d) {
    ezcu_knl_t k = __ezcu_knl_find(name);
    EZCU_DEBUG("start exec (sync) kernel  : %s", name);
    urb_tree_walk(&k->mems, NULL, __ezcu_mem_sync);
    EZCU_CHECK(cuCtxPushCurrent(d->ctx), "failed to push the context");
    EZCU_CHECK(cuLaunchKernel(k->id, 
                              k->grid[0], k->grid[1], k->grid[2],
                              k->block[0], k->block[1], k->block[2],
                              k->smem, d->streams[__EZCU_DEV_CPT_STREAM], 
                              k->args, NULL),
               "failed to launch CUDA kernel '%s'", name);
    EZCU_CHECK(cuCtxSynchronize(), "failed to synchronize the device context");
    EZCU_CHECK(cuCtxPopCurrent(&d->ctx), "failed to pop the context");
    EZCU_DEBUG("end  exec (sync) kernel  : %s", name);
    EZCU_DEBUG("");
}

inline double ezcu_knl_timed_exec(const char *name, ezcu_dev_t d) {
    ezcu_knl_t k = __ezcu_knl_find(name);
    EZCU_DEBUG("start exec (timed) kernel : %s", name);
    urb_tree_walk(&k->mems, NULL, __ezcu_mem_sync);
    ezcu_timer_tick();
    EZCU_CHECK(cuCtxPushCurrent(d->ctx), "failed to push the context");
    EZCU_CHECK(cuLaunchKernel(k->id, 
                              k->grid[0], k->grid[1], k->grid[2],
                              k->block[0], k->block[1], k->block[2],
                              k->smem, d->streams[__EZCU_DEV_CPT_STREAM], 
                              k->args, NULL),
               "failed to launch CUDA kernel '%s'", name);
    EZCU_CHECK(cuCtxSynchronize(), "failed to synchronize the device context");
    EZCU_CHECK(cuCtxPopCurrent(&d->ctx), "failed to pop the context"); 
    EZCU_DEBUG("end  exec (timed) kernel  : %s", name);  
    EZCU_DEBUG("");
    return ezcu_timer_read();
}

inline void ezcu_knl_run(const char *name, ezcu_dev_t d, ...) {
    ezcu_knl_t k = __ezcu_knl_find(name);
    EZCU_DEBUG("start run (async) kernel : %s", name);
    va_list list;
    va_start(list, d);
    __ezcu_knl_set_args_valist(k, list);
    va_end(list);
    urb_tree_walk(&k->mems, NULL, __ezcu_mem_sync);
    EZCU_CHECK(cuCtxPushCurrent(d->ctx), "failed to push the context");
    EZCU_CHECK(cuLaunchKernel(k->id, 
                              k->grid[0], k->grid[1], k->grid[2],
                              k->block[0], k->block[1], k->block[2],
                              k->smem, d->streams[__EZCU_DEV_CPT_STREAM], 
                              k->args, NULL),
               "failed to launch CUDA kernel '%s'", name);
    EZCU_CHECK(cuCtxPopCurrent(&d->ctx), "failed to pop the context");
    EZCU_DEBUG("end   run (async) kernel : %s", name);
    EZCU_DEBUG("");
}

inline void ezcu_knl_sync_run(const char *name, ezcu_dev_t d, ...) {
    ezcu_knl_t k = __ezcu_knl_find(name);
    EZCU_DEBUG("start run (sync) kernel  : %s", name);
    va_list list;
    va_start(list, d);
    __ezcu_knl_set_args_valist(k, list);
    va_end(list);
    urb_tree_walk(&k->mems, NULL, __ezcu_mem_sync);
    EZCU_CHECK(cuCtxPushCurrent(d->ctx), "failed to push the context");
    EZCU_CHECK(cuLaunchKernel(k->id, 
                              k->grid[0], k->grid[1], k->grid[2],
                              k->block[0], k->block[1], k->block[2],
                              k->smem, d->streams[__EZCU_DEV_CPT_STREAM], 
                              k->args, NULL),
               "failed to launch CUDA kernel '%s'", name);
    EZCU_CHECK(cuCtxSynchronize(), "failed to synchronize the device context");
    EZCU_CHECK(cuCtxPopCurrent(&d->ctx), "failed to pop the context");   
    EZCU_DEBUG("end   run (sync) kernel  : %s", name);
    EZCU_DEBUG("");
}

inline double ezcu_knl_timed_run(const char *name, ezcu_dev_t d, ...) {
    ezcu_knl_t k = __ezcu_knl_find(name);
    EZCU_DEBUG("start run (timed) kernel : %s", name);
    va_list list;
    va_start(list, d);
    __ezcu_knl_set_args_valist(k, list);
    va_end(list);
    urb_tree_walk(&k->mems, NULL, __ezcu_mem_sync);
    ezcu_timer_tick();
    EZCU_CHECK(cuCtxPushCurrent(d->ctx), "failed to push the context");
    EZCU_CHECK(cuLaunchKernel(k->id, 
                              k->grid[0], k->grid[1], k->grid[2],
                              k->block[0], k->block[1], k->block[2],
                              k->smem, d->streams[__EZCU_DEV_CPT_STREAM], 
                              k->args, NULL),
               "failed to launch CUDA kernel '%s'", name);
    EZCU_CHECK(cuCtxSynchronize(), "failed to synchronize the device context");
    EZCU_CHECK(cuCtxPopCurrent(&d->ctx), "failed to pop the context"); 
    EZCU_DEBUG("end   run (timed) kernel  : %s", name);
    EZCU_DEBUG("");
    return ezcu_timer_read();
}