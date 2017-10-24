#ifndef __EZCU_TYPES_INL_H_
#define __EZCU_TYPES_INL_H_
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
/// @file __ezcu/types-inl.h
/// @author Issam SAID
/// @brief  Types and structures used by ezcu to describe the CUDA resources.
///
/// @details This file contains the definition of the types used by 
/// ezcu to describe the CUDA resources such as the devices the kernels 
/// and the memory objects.
///
#include <stdio.h>
#include <ezcu/flags.h>
#include <urb_tree/urb_tree.h>
#include <__ezcu/config/guard.h>
#include <__ezcu/config/knl.h>
#include <__ezcu/config/util.h>
#include <__ezcu/config/cuda.h>

CPPGUARD_BEGIN();

///
/// @brief The ezcu device descriptor.
///
/// This structure is a high level description of an CUDA device.
/// It contains an CUDA command queue that is dedicated to run kernels
/// on that device.
///
__host__ struct __ezcu_dev_t {
    CUdevice id;                 ///< the CUDA device identifier.
    int major, minor;            ///< the CUDA device major/minor.
    char name[__EZCU_STR_SIZE];  ///< the CUDA device name.
    CUcontext ctx;               ///< the CUDA device context.
    CUstream *streams;           ///< CUDA streams dedicated to the device.
};

///
/// @brief The ezcu memory object descriptor.
///
/// This structure is a high level description of a CUDA memory object.
///
__host__ struct __ezcu_mem_t {
    ezcu_flags_t flags;     ///< the ezcu flags used to allocate the memory.
    size_t size;            ///< the size of the memory in number of elements.
    size_t unit_size;       ///< the size of each element in bytes.
    CUdeviceptr id;         ///< the CUDA identifier of the memory object.  
    void *h;                ///< a pointer to the host memory.
    int refs;               ///< the number of kernels using this memory.
    CUstream stream;
};

///
/// @brief The ezcu kernel descriptor.
///
/// This structure is a high level description of an CUDA kernel.
///
__host__ struct __ezcu_knl_t {
    unsigned int grid[__EZCU_KNL_MAX_GRID_DIM];  ///< the grid dimensions.
    unsigned int block[__EZCU_KNL_MAX_GRID_DIM]; ///< the block dimensions.
    unsigned int num_args;              ///< the number of arguments.
    unsigned int smem;                  ///< the amount of shared memory.
    CUfunction id;                      ///< the CUDA function pointer.
    void **args;                        ///< the function arguments.
    ezcu_mem_types_flags_t *args_types; ///< the function arguments types.
    urb_t *vals;                        ///< a red-black tree of intrinsics. 
    urb_t *mems;                        ///< a red-black tree of mem objects. 
};

///
/// @brief The ezcu environment descriptor.
///
/// This structure represents the ezcu resources manager.
///
__host__ struct __ezcu_env_t {
    urb_t   *devs;                 ///< red-black tree of the used devices.
    urb_t   *knls;                 ///< red-black tree of the used kernels.
    urb_t   *mems;                 ///< red-black tree of active mem objects.
    FILE*   fdout;                 ///< file descriptor for logging.
    FILE*   fderr;                 ///< file descriptor for error reporting.
    int      nids;                 ///< number of selected devices.
    CUdevice *ids;                 ///< selected devices.
    int      nccs;                 ///< number of selected architectures.
    int      *ccs;                 ///< selected architectures.
    char cc_opts[__EZCU_STR_SIZE]; ///< compute capabilities compile flags.
    bool initialized[EZCU_NB_VENDORS];
    CUdevice lookup[EZCU_NB_DEV_TYPES][EZCU_NB_DEV_PROPS][EZCU_NB_DEV_INDEXES];
};

CPPGUARD_END();

#endif  // __EZCU_TYPES_INL_H_
