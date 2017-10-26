#ifndef __EZCU_MEM_INL_H_
#define __EZCU_MEM_INL_H_
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
/// @file __ezcu/mem-inl.h
/// @author Issam SAID
/// @brief Private functions used to implement the ezcu memory descriptor.
///
#include <stdio.h>
#include <stdbool.h>
#include <ezcu/flags.h>
#include <ezcu/types.h>
#include <__ezcu/config/cuda.h>
#include <__ezcu/config/private.h>
#include <__ezcu/config/guard.h>
#include <__ezcu/config/util.h>
#include <__ezcu/types-inl.h>
#include <__ezcu/error-inl.h>
#include <__ezcu/util-inl.h>

CPPGUARD_BEGIN();

extern ezcu_env_t ezcu;

///< Synchronization flags and masks.
#define __EZCU_MEM_HOST_DIRTY            0x1000000000000000 
#define __EZCU_MEM_HWA_DIRTY             0x2000000000000000
#define __EZCU_MEM_HOST_ALLOCATED        0x8000000000000000

#define __EZCU_MEM_HOST_IS_DIRTY(flags)  (flags &  __EZCU_MEM_HOST_DIRTY)
#define __EZCU_MEM_HWA_IS_DIRTY(flags)   (flags &  __EZCU_MEM_HWA_DIRTY)

#define __EZCU_MEM_CLEAR_HOST(flags)     (flags &= (~__EZCU_MEM_HOST_DIRTY))
#define __EZCU_MEM_CLEAR_HWA(flags)      (flags &= (~__EZCU_MEM_HWA_DIRTY))
#define __EZCU_MEM_TOUCH_HOST(flags)     (flags |= __EZCU_MEM_HOST_DIRTY)
#define __EZCU_MEM_TOUCH_HWA(flags)      (flags |= __EZCU_MEM_HWA_DIRTY)


__EZCU_PRIVATE __host__ void
__ezcu_mem_htod(void *hPtr, CUdeviceptr dPtr, 
  size_t bytes, CUstream stream, bool blocking) {
  EZCU_DEBUG("mem htod   {h=%p, id=%llu} (size=%12.5f MB)", 
              hPtr, dPtr, (double)bytes/1024./1024.);
  if (blocking) {
    EZCU_CHECK(cuMemcpyHtoD(dPtr, hPtr, bytes),
               "failed to write into CUDA memory");
  } else {
    EZCU_CHECK(cuMemcpyHtoDAsync(dPtr, hPtr, bytes, stream),
               "failed to write (async) into CUDA memory");
  }
}

__EZCU_PRIVATE __host__ void 
__ezcu_mem_dtoh(void *hPtr, CUdeviceptr dPtr, 
  size_t bytes, CUstream stream, bool blocking) { 
  EZCU_DEBUG("mem dtoh   {h=%p, id=%llu} (size=%12.5f MB)", 
              hPtr, dPtr, (double)bytes/1024./1024.);
  if (blocking) {
    EZCU_CHECK(cuMemcpyDtoH(hPtr, dPtr, bytes),
     "failed to read from CUDA memory");
  } else {
    EZCU_CHECK(cuMemcpyDtoHAsync(hPtr, dPtr, bytes, stream),
     "failed to read (async) from CUDA memory");
  }
}

__EZCU_PRIVATE __host__ void
__ezcu_mem_touch(ezcu_mem_t m) {
  if (!EZCU_FLAGS_HAVE(m->flags, READ_ONLY)) {
    __EZCU_MEM_TOUCH_HWA(m->flags);
    EZCU_DEBUG("mem touch  {h=%p, id=%llu}", m->h, m->id);
  }
}

__EZCU_PRIVATE __host__ void
__ezcu_mem_sync(void *pointer) {
  ezcu_mem_t m = (ezcu_mem_t)pointer;
  if (EZCU_FLAGS_HAVE(m->flags, ZERO_COPY)) {
    if (__EZCU_MEM_HOST_IS_DIRTY(m->flags) &&
         !EZCU_FLAGS_HAVE(m->flags, WRITE_ONLY)) {
      EZCU_DEBUG("start mem sync   {h=%p, id=%llu}", m->h, m->id);
      __EZCU_MEM_CLEAR_HOST(m->flags);
      EZCU_DEBUG("end   mem sync   {h=%p, id=%llu}", m->h, m->id);
    }
  } else {
    if (__EZCU_MEM_HOST_IS_DIRTY(m->flags) && 
         !EZCU_FLAGS_HAVE(m->flags, WRITE_ONLY)) {
      EZCU_DEBUG("start mem sync   {h=%p, id=%llu}", m->h, m->id);
      __ezcu_mem_htod(m->h, m->id, 
                      m->size*m->unit_size, m->stream, false);
      __EZCU_MEM_CLEAR_HOST(m->flags);
      EZCU_DEBUG("end   mem sync   {h=%p, id=%llu}", m->h, m->id);
    }
  }
  __ezcu_mem_touch(m);
}

__EZCU_PRIVATE __host__ ezcu_mem_t
__ezcu_mem_find(void *h) {
  return (ezcu_mem_t)urb_tree_find(&ezcu->mems, h, __ezcu_ptr_cmp)->value;
}

///
/// @brief Release a given ezcu memory descriptor.
///
/// This routine frees the CUDA resources related to a ezcu memory descriptor,
/// identified by a host pointer, which include the CUDA memory identifier.
/// @param h is the host memory pointer that identifies the descriptor.
/// @return Nothing.
///
__EZCU_PRIVATE __host__ 
void __ezcu_mem_release(void *pointer) {
  ezcu_mem_t m = (ezcu_mem_t)pointer;
  if ((m != NULL) && (m->refs >= 0)) {   
    EZCU_DEBUG("mem refs-- {h=%p, id=%llu} (refs %d => %d), "
               "host allocated: %s, pinned: %s, zero-copy: %s", 
               m->h, m->id, m->refs, m->refs-1, 
               EZCU_FLAGS_HAVE(m->flags, 
                                 __EZCU_MEM_HOST_ALLOCATED) ? "Yes":"No",
               EZCU_FLAGS_HAVE(m->flags, PINNED) ? "Yes":"No",
               EZCU_FLAGS_HAVE(m->flags, ZERO_COPY) ? "Yes":"No");
    m->refs = m->refs-1; 
    if (m->refs == 0) {
      EZCU_DEBUG("mem release {h=%p, id=%llu} (refs count = %d)", 
                 m->h, m->id, m->refs);
      if (EZCU_FLAGS_HAVE(m->flags, HOST)) {
        if (EZCU_FLAGS_HAVE(m->flags, ZERO_COPY)) {
            // NOT SUPPORTED YET
        } else { free(m->h); }
      } else if (EZCU_FLAGS_HAVE(m->flags, HWA)) {
        if (EZCU_FLAGS_HAVE(m->flags, PINNED)) {
          //__ezcu_mem_unmap(m->h, true);
          EZCU_CHECK(cuMemHostUnregister(m->h), 
                     "failed to unmap page-locked memory region");
        } else if (EZCU_FLAGS_HAVE(m->flags, ZERO_COPY)) {
          //__ezcu_mem_unmap(m->id, true);
        } else {
          if (!EZCU_FLAGS_HAVE(m->flags, __EZCU_MEM_HOST_ALLOCATED)) free(m->h);
        }
        EZCU_ABORT(cuMemFree(m->id), "failed to release CUDA memory");
      }
      free(m); m = NULL;
    }
  }
}

#define __EZCU_MEM_INFO_LEVEL_0(fdout, fmt, ...)   \
  fprintf(fdout, "%s\no CUDA " __STR(fmt) ":\n%s", \
    EZCU_GREEN, ##__VA_ARGS__, EZCU_END)

#define __EZCU_MEM_INFO_LEVEL_1(fdout, fmt, ...) \
  fprintf(fdout, "\to %-20s: " __STR(fmt) "\n", ##__VA_ARGS__)

__EZCU_PRIVATE __host__ void
__ezcu_mem_info(void *pointer) {
  ezcu_mem_t m = (ezcu_mem_t)pointer;
  FILE* fdout  = ezcu->fdout;
  char tmp[__EZCU_STR_SIZE];
  ezcu_flags_mem_to_str(m->flags, tmp);
  __EZCU_MEM_INFO_LEVEL_0(fdout, "memory object {h=%p, id=%u} (flags=%s)", 
                          m->h, (unsigned int)m->id, tmp);
  __EZCU_MEM_INFO_LEVEL_1(fdout, "%-12.5f %s", "size", 
                          (double)m->size*m->unit_size/1024./1024., "MB");
}

CPPGUARD_END();

#endif  // __EZCU_MEM_INL_H_
