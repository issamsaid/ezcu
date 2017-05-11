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
///    contributors may be used to endorse or promote products derived 
///    from this software without specific prior written permission.
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
/// @file ezcu_mem.c
/// @author Issam SAID
/// @brief The implementation of the ezCU memory objects manipulation routines.
///
#include <stdio.h>
#include <ezCU/mem.h>
#include <__ezcu/config/mem.h>
#include <__ezcu/config/dev.h>
#include <__ezcu/config/util.h>
#include <__ezcu/types-inl.h>
#include <__ezcu/mem-inl.h>
#include <__ezcu/dev-inl.h>
#include <__ezcu/util-inl.h>

ezcu_mem_t ezcu_mem_wrap(ezcu_dev_t d, void *h, 
                         size_t size, ezcu_flags_t flags) {
    EZCU_EXIT_IF(d    == NULL, "invalid device");
    EZCU_EXIT_IF(size ==    0, "invalid memory size");
    EZCU_EXIT_IF(h    == NULL, "invalid memory pointer");
    urb_t *n;
    size_t bytes;
    if ((n=urb_tree_find(&ezcu->mems, h, __ezcu_ptr_cmp)) != &urb_sentinel) {
#ifdef __EZCU_DEBUG  
        char __tmp[__EZCU_STR_SIZE];
        ezcu_mem_t m = n->value;
        ezcu_flags_mem_to_str(m->flags, __tmp);
        EZCU_DEBUG("reuse mem wrap: flags=%s, found existing object", __tmp);
        EZCU_DEBUG("end   mem wrap: {h=%p, id=%llu} "
                   "(size= %12.5f MB, refs= %d)", 
                   m->h, m->id, 
                   (double)m->size*m->unit_size/1024./1024., m->refs);
        EZCU_DEBUG("");
#endif
        return n->value;
    } else {
        int tmp;
        ezcu_mem_t m = (ezcu_mem_t) malloc(sizeof(struct __ezcu_mem_t));
        ezcu_flags_check_mem(flags);    
        
        m->flags      = ((flags & (~(DEFAULT))) | __EZCU_MEM_HOST_ALLOCATED);
        if ((flags & EZCU_FLAGS_MEM_LOCS_MASK)==0)
            m->flags |= HWA;
        if ((flags & EZCU_FLAGS_MEM_ACCESS_MODES_MASK)==0)
            m->flags |= READ_WRITE;
        if ((flags & EZCU_FLAGS_MEM_TYPES_MASK)==0) 
            m->flags |= FLOAT;
        
        __EZCU_MEM_CLEAR_HOST(m->flags);
        __EZCU_MEM_CLEAR_HWA(m->flags);

        m->id         = -1;
        m->size       = size;
        m->unit_size  = ezcu_flags_to_mem_unit_size(m->flags);
        m->stream     = d->streams[__EZCU_DEV_MEM_STREAM];
        bytes         = m->unit_size*m->size;
#ifdef __EZCU_DEBUG  
        char __tmp[__EZCU_STR_SIZE];
        ezcu_flags_mem_to_str(m->flags, __tmp);
        EZCU_DEBUG("start mem wrap: flags=%s", __tmp);
#endif

        EZCU_CHECK(cuCtxPushCurrent(d->ctx), "failed to push the context");
        
        if (EZCU_FLAGS_HAVE(m->flags, ZERO_COPY)) {          
            __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_CAN_MAP_HOST_MEMORY, tmp);
            EZCU_EXIT_IF(!tmp, "the device cannot map host zero-copy buffers");
            /// wrap zero-copy buffers in the CPU memory.
            if (EZCU_FLAGS_HAVE(m->flags, HOST)) {
                EZCU_CHECK(cuMemHostRegister((void*)h, m->size*m->unit_size, 
                                          CU_MEMHOSTREGISTER_DEVICEMAP),
                           "failed to regiter pre-allocated host memory");
                // needed to map the memory on the device memory
                EZCU_CHECK(cuMemHostGetDevicePointer(&m->id, h, 0),
                           "failed to map memory region to device");
            /// wrap zero-copy buffers in the HWA memory.
            } else if (EZCU_FLAGS_HAVE(m->flags, HWA)) {
                EZCU_EXIT("device zero-copy buffers are not wrappable yet");
            } else {
                EZCU_EXIT("invalid flags for wrapping zero-copy buffers");
            }
            m->h = h;
        } else {
            /// regular CPU buffers.
            if (EZCU_FLAGS_HAVE(m->flags, HOST)) {
                m->h  = (void*)h;
            /// regular HWA buffers.
            } else if (EZCU_FLAGS_HAVE(m->flags, HWA)) {
                if (EZCU_FLAGS_HAVE(m->flags, PINNED)) {
                    EZCU_CHECK(cuMemHostRegister((void*)h, m->size*m->unit_size, 
                                                 CU_MEMHOSTREGISTER_PORTABLE),
                               "failed to page-lock preallocated memory");
                    m->h = (void*)h;
                } else { m->h  = (void*)h; }
                    EZCU_CHECK(cuMemAlloc(&m->id, m->size*m->unit_size),
                               "failed to allocate device memory object");
                    __ezcu_mem_htod((void*)h, m->id, bytes, m->stream, true);
            } else {
                EZCU_EXIT("invalid flags combination for wrapping buffers");
            }
        }
        urb_tree_put(&ezcu->mems, urb_tree_create(h, m), __ezcu_ptr_cmp);
        m->refs = 1;
        EZCU_DEBUG("end   mem wrap: {h=%p, id=%llu} "
                   "(size= %12.5f MB, refs= %d)", 
                   m->h, m->id, 
                   (double)m->size*m->unit_size/1024./1024., m->refs);
        EZCU_DEBUG("");
        EZCU_CHECK(cuCtxPopCurrent(&d->ctx), "failed to pop the context");
        return m;
    }
}

void ezcu_mem_release(void *h) {
    urb_t * n = urb_tree_pop(&ezcu->mems, h, __ezcu_ptr_cmp);
    if (n != &urb_sentinel) {
        ((ezcu_mem_t)n->value)->refs = 1; /// force deletion on the device.
        __ezcu_mem_release(n->value);  
    } 
}

void ezcu_mem_update(void* h, ezcu_flags_t f) {
    ezcu_mem_t m = (ezcu_mem_t)urb_tree_find(&ezcu->mems, 
                                             h, __ezcu_ptr_cmp)->value;
#ifdef __EZCU_DEBUG  
    char __tmp[__EZCU_STR_SIZE];
    ezcu_flags_mem_to_str(f, __tmp);
    EZCU_DEBUG("start mem update {h=%p, id=%llu}, : flags=%s "
               "host-dirty: %s, hwa-dirty: %s",
               m->h, m->id, __tmp, 
               __EZCU_MEM_HOST_IS_DIRTY(m->flags) ? "Yes" : "No",
               __EZCU_MEM_HWA_IS_DIRTY(m->flags)  ? "Yes" : "No");

#endif
    if (EZCU_FLAGS_HAVE(m->flags, ZERO_COPY)) {
        if (__EZCU_MEM_HWA_IS_DIRTY(m->flags)) {
            __EZCU_MEM_CLEAR_HWA(m->flags);
        }
    } else {
        if (__EZCU_MEM_HWA_IS_DIRTY(m->flags)) {
            __ezcu_mem_dtoh(m->h, m->id, m->size*m->unit_size, m->stream, true);
            __EZCU_MEM_CLEAR_HWA(m->flags);
        }
    }
    if (EZCU_FLAGS_HAVE(f, READ_WRITE) || 
        EZCU_FLAGS_HAVE(f, WRITE_ONLY))
        __EZCU_MEM_TOUCH_HOST(m->flags);
    EZCU_DEBUG("end   mem update: {h=%p, id=%llu}, "
               "host-dirty: %s, hwa-dirty: %s", m->h, m->id,
               __EZCU_MEM_HOST_IS_DIRTY(m->flags) ? "Yes" : "No",
               __EZCU_MEM_HWA_IS_DIRTY(m->flags)  ? "Yes" : "No");
    EZCU_DEBUG("");
}
