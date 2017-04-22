#ifndef __EZCU_DEV_INL_H_
#define __EZCU_DEV_INL_H_
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
/// @file __ezcu/dev-inl.h
/// @author Issam SAID
/// @brief Private functions used to implement the ezCU device descriptor.
///
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ezCU/flags.h>
#include <ezCU/types.h>
#include <__ezcu/config/util.h>
#include <__ezcu/config/dev.h>
#include <__ezcu/config/private.h>
#include <__ezcu/config/guard.h>
#include <__ezcu/config/cuda.h>
#include <__ezcu/util-inl.h>     
#include <__ezcu/error-inl.h>     

CPPGUARD_BEGIN();

extern ezcu_env_t ezcu;

#define __EZCU_DEV_GET(id, device_attr, value)                \
    EZCU_CHECK(cuDeviceGetAttribute(&value, device_attr, id), \
               "failed to query device attribute")

__EZCU_PRIVATE int
__ezcu_dev_query() {
    int nb_devices;
    EZCU_CHECK(cuDeviceGetCount(&nb_devices),
               "failed to query the number of CUDA devices");
    return nb_devices;
}

__EZCU_PRIVATE int
__ezcu_dev_get_cc_hex(int minor, int major) {
    return ((major << 4) + minor);
}

__EZCU_PRIVATE int
__ezcu_dev_select_by_vendor(CUdevice *dev_ids, 
                            CUdevice in_devices, ezcu_vendor_flags_t f) {
    if (f != NVIDIA) {
        EZCU_WARN("for the time being only NVIDIA is supported by ezCU");
        return 0;
    } else{ return in_devices; }
}

__EZCU_PRIVATE int
__ezcu_dev_select_by_dev_type(CUdevice *dev_ids, int in_devices, 
                              CUdevice *out_ids, ezcu_dev_type_flags_t f) {
    int i;
    if (f == CPU) {
        EZCU_WARN("for the time being only GPUs are CUDA capable devices");
        return 0;
    } else { 
        for(i=0; i<in_devices; i++) out_ids[i] = dev_ids[i];
        return in_devices; 
    }
}

__EZCU_PRIVATE int
__ezcu_dev_select_by_dev_prop(CUdevice *dev_ids, int in_devices, 
                              CUdevice *out_ids, ezcu_dev_prop_flags_t f) {
    int i, out_devices = 0;
    int minor, major;
    CUdevice d;
    for (i = 0; i < in_devices; ++i) {
        d = dev_ids[i];
        __EZCU_DEV_GET(d, 
                       CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MAJOR, major);      
        __EZCU_DEV_GET(d, 
                       CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MINOR, minor);
        if (__ezcu_dev_get_cc_hex(minor, major) >= ezcu_flags_to_dev_prop(f)) {
            out_ids[out_devices++] = dev_ids[i];
        }
    }  
    return out_devices;
}

__EZCU_PRIVATE int
__ezcu_dev_select_by_dev_index(CUdevice *dev_ids, int in_devices, 
                               CUdevice *out_id, ezcu_dev_index_flags_t f) {
    int findex = ezcu_flags_to_dev_index(f);
    if ( findex < in_devices) {
        *out_id = dev_ids[findex];
        return 1;
    } else { return 0; }
}

///
/// @brief Initialize a device descriptor.
///
/// This routine creates a ezCU device descriptor, provided an CUDA device id,
/// which contains an CUDA command queue that is associated to the device id,
/// see types.h.
/// @param id the CUDA device id of the descriptor to create.
/// @return A ezCU device descriptor.
///
__EZCU_PRIVATE ezcu_dev_t 
__ezcu_dev_init(CUdevice id) {
    int i;
    ezcu_dev_t d = (ezcu_dev_t)malloc(sizeof(struct __ezcu_dev_t));
    d->id = id;
    EZCU_CHECK(cuDeviceGetName(d->name, __EZCU_STR_SIZE, d->id),
               "failed to query the device name");
    __EZCU_DEV_GET(d->id, 
                   CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MAJOR, d->major);      
    __EZCU_DEV_GET(d->id, 
                   CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MINOR, d->minor);
    EZCU_CHECK(cuCtxCreate(&d->ctx,
               CU_CTX_MAP_HOST | CU_CTX_SCHED_YIELD, d->id),
               "failed to create a context for the device %s", d->name);
    EZCU_CHECK(cuCtxPushCurrent(d->ctx), "failed to push the device context");

    d->streams = (CUstream*)malloc(__EZCU_DEV_NSTREAMS*sizeof(CUstream));
    for (i=0; i<__EZCU_DEV_NSTREAMS; i++) {
        EZCU_CHECK(cuStreamCreate(&d->streams[i], CU_STREAM_DEFAULT), 
                   "failed to create CUDA stream");
    }
    EZCU_CHECK(cuCtxSynchronize(), "failed to sync the context");
    EZCU_CHECK(cuCtxPopCurrent(&d->ctx), "failed to pop the device context");
    return d;
}

///
/// @brief Delete a device descriptor.
///
/// This routine deletes a ezCU device descriptor after releasing the 
/// CUDA device id associated with the descriptor.
/// @param pointer the ezCU device descriptor.
/// @return Nothing.
///
__EZCU_PRIVATE void
__ezcu_dev_release(void *pointer) {
    if (pointer != NULL) {
        int i;
        ezcu_dev_t d = (ezcu_dev_t)pointer;
        EZCU_DEBUG("releasing CUDA device %d (%s) with ctx=%p", 
                    d->id, d->name, d->ctx);
        //EZCU_CHECK(cuCtxPushCurrent(d->ctx), "failed to push the device context");
        EZCU_CHECK(cuCtxSetCurrent(d->ctx), "failed to bind the context");
        
        for (i=0; i<__EZCU_DEV_NSTREAMS; i++) {
           EZCU_ABORT(cuStreamSynchronize(d->streams[i]), 
                      "failed to sync stream");
           EZCU_ABORT(cuStreamDestroy(d->streams[i]), 
                      "failed to destroy stream");
        }
        
        EZCU_CHECK(cuCtxSynchronize(), "failed to sync the context");
        free(d->streams);
        EZCU_CHECK(cuCtxPopCurrent(&d->ctx), "failed to pop the device context");
        EZCU_ABORT(cuCtxDestroy(d->ctx), 
                    "failed to destroy context (%p)", d->ctx);
        free(d); d = NULL;
    }
}

///
/// @brief Defines for GPU Architecture types 
///        (using the SM version to determine the # of cores per SM
///
/// @param
/// @return An integer which is the number of cores per SM.
///
__EZCU_PRIVATE int 
__ezcu_dev_smversion2cores(int major, int minor) {
    typedef struct {
        int SM;    /// 0xMm (hexidecimal notation), M = SM Major version, 
        int Cores; ///                          and m = SM minor version
    } sSMtoCores;

    sSMtoCores nGpuArchCoresPerSM[] = {
        { 0x20, 32 }, /// Fermi   Generation (SM 2.0) GF100 class
        { 0x21, 48 }, /// Fermi   Generation (SM 2.1) GF10x class
        { 0x30, 192}, /// Kepler  Generation (SM 3.0) GK10x class
        { 0x32, 192}, /// Kepler  Generation (SM 3.2) GK10x class
        { 0x35, 192}, /// Kepler  Generation (SM 3.5) GK11x class
        { 0x37, 192}, /// Kepler  Generation (SM 3.7) GK21x class
        { 0x50, 128}, /// Maxwell Generation (SM 5.0) GM10x class
        { 0x52, 128}, /// Maxwell Generation (SM 5.2) GM20x class
        { 0x60, 64 }, /// Pascal  Generation (SM 6.0) GP100 class
        { 0x61, 128}, /// Pascal  Generation (SM 6.1) GP102 class
        {   -1, -1 }
    };

    int index = 0;
    while (nGpuArchCoresPerSM[index].SM != -1) {
        if (nGpuArchCoresPerSM[index].SM == __ezcu_dev_get_cc_hex(minor, major))
            return nGpuArchCoresPerSM[index].Cores;
        index++;
    }

    /// If we don't find the values, we default
    /// use the previous one to run properly
    EZCU_EXIT("SM property %d.%d is undefined", major, minor);
    return nGpuArchCoresPerSM[index-1].Cores;
}

///
/// @brief Returns an extended overview of a given device capabilities.
///        Inspired from the NVIDIA Sample "deviceQuery".
///
/// @param pointer: is the device to be queried.
/// @return Nothing.
///
__EZCU_PRIVATE void 
__ezcu_dev_get_name(void *pointer) {
    ezcu_dev_t  d = (ezcu_dev_t)pointer;
    EZCU_PRINT("\t- %s", d->name);
}

#define __EZCU_DEV_INFO_LEVEL_0(fdout, fmt, ...) fprintf(fdout, \
EZCU_GREEN"\no "fmt"\n"EZCU_END, ##__VA_ARGS__)

#ifdef __EZCU_DEV_LONG_INFO_ENABLED
#define __EZCU_DEV_INFO_LEVEL_1(fdout, fmt, ...) fprintf(fdout, \
"\to %-40s: "fmt"\n", ##__VA_ARGS__)
#define __EZCU_DEV_INFO_LEVEL_2(fdout, fmt, ...) fprintf(fdout, \
"\t %42s "fmt"\n", " ", ##__VA_ARGS__)

#else

#define __EZCU_DEV_INFO_LEVEL_1(fdout, fmt, ...) fprintf(fdout, \
"\to %-40s: "fmt"\n", ##__VA_ARGS__)
#define __EZCU_DEV_INFO_LEVEL_2(fdout, fmt, ...) fprintf(fdout, \
"\t %42s "fmt"\n", " ", ##__VA_ARGS__)
#endif  // __EZCU_DEV_LONG_INFO_ENABLED

///
/// @brief Returns an extended overview of a given device capabilities.
///        Inspired from the NVIDIA Sample "deviceQuery".
///
/// @param pointer: is the device to be queried.
/// @return Nothing.
///
__EZCU_PRIVATE void 
__ezcu_dev_info(void *pointer) {
    ezcu_dev_t  d = (ezcu_dev_t)pointer;
    FILE* fdout   = ezcu->fdout;
    int tmp_0, tmp_1, tmp_2;
    size_t bytes;
    EZCU_CHECK(cuCtxPushCurrent(d->ctx), "failed to push the context");
    __EZCU_DEV_INFO_LEVEL_0(fdout, "CUDA Device %d: \"%s\"", 
                            d->id, d->name);
    EZCU_CHECK(cuDriverGetVersion(&tmp_0),
               "failed to query device driver version");
    __EZCU_DEV_INFO_LEVEL_1(fdout, "%d.%d", 
                            "Driver Version",
                            tmp_0/1000, (tmp_0%100)/10);
    __EZCU_DEV_INFO_LEVEL_1(fdout, 
                            "%d.%d",
                            "Capability Major/Minor version number", 
                            d->major, d->minor);
    EZCU_CHECK(cuDeviceTotalMem(&bytes, d->id), 
               "failed to query device memory size");
    __EZCU_DEV_INFO_LEVEL_1(fdout, 
                            "%.0f MBytes (%llu bytes)",
                            "Global memory size",
                            (float)bytes/1048576.0f, 
                            (unsigned long long) bytes);
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT, tmp_0);
    __EZCU_DEV_INFO_LEVEL_1(fdout,
                            "%d",
                            "Number of Multiprocessors",
                            tmp_0);
    __EZCU_DEV_INFO_LEVEL_1(fdout,
                            "%d",
                            "Number of CUDA Cores/MP",
                            __ezcu_dev_smversion2cores(d->major, d->minor));
    __EZCU_DEV_INFO_LEVEL_1(fdout,
                            "%d",
                            "Total number of CUDA Cores",
                            __ezcu_dev_smversion2cores(d->major, 
                                                       d->minor)*tmp_0);
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_CLOCK_RATE, tmp_0);
    __EZCU_DEV_INFO_LEVEL_1(fdout, 
                            "%.0f MHz (%0.2f GHz)", 
                            "GPU Max Clock rate", tmp_0 * 1e-3f, tmp_0 * 1e-6f);
#ifdef __EZCU_DEV_LONG_INFO_ENABLED
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_MEMORY_CLOCK_RATE, tmp_0); 
    __EZCU_DEV_INFO_LEVEL_1(fdout, "%.0f Mhz", "Memory Clock rate", 
                            tmp_0 * 1e-3f);
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_GLOBAL_MEMORY_BUS_WIDTH, tmp_0);
    __EZCU_DEV_INFO_LEVEL_1(fdout, "%d-bit",
                            "Memory Bus Width", tmp_0);
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_L2_CACHE_SIZE, tmp_0);
    if (tmp_0)
        __EZCU_DEV_INFO_LEVEL_1(fdout, "%d bytes", "L2 Cache Size", tmp_0);
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_GLOBAL_L1_CACHE_SUPPORTED, tmp_0);
    __EZCU_DEV_INFO_LEVEL_1(fdout, 
                            "%s", 
                            "L1 global memory caching supported", 
                            tmp_0 ? "Yes" : "No");
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_LOCAL_L1_CACHE_SUPPORTED, tmp_0);
    __EZCU_DEV_INFO_LEVEL_1(fdout, 
                            "%s", 
                            "L1 shared memory caching supported", 
                            tmp_0 ? "Yes" : "No");
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE1D_WIDTH, tmp_0);
    __EZCU_DEV_INFO_LEVEL_1(fdout, "(%d)", 
                            "Maximum Texture 1D Dimension Size", tmp_0);
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_WIDTH, tmp_0); 
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_HEIGHT, tmp_1); 
    __EZCU_DEV_INFO_LEVEL_1(fdout, "(%d, %d)", 
                            "Maximum Texture 2D Dimension Size", tmp_0, tmp_1);
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE3D_WIDTH,  tmp_0); 
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE3D_HEIGHT, tmp_1);
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE3D_DEPTH,  tmp_2); 
    __EZCU_DEV_INFO_LEVEL_1(fdout, "(%d, %d, %d)", 
                            "Maximum Texture 3D Dimension Size", 
                            tmp_0, tmp_1, tmp_2);
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_TOTAL_CONSTANT_MEMORY, tmp_0); 
    __EZCU_DEV_INFO_LEVEL_1(fdout, "%d bytes", 
                            "Total amount of constant memory", tmp_0);
    __EZCU_DEV_GET(d->id, 
                   CU_DEVICE_ATTRIBUTE_MAX_SHARED_MEMORY_PER_BLOCK, tmp_0); 
    __EZCU_DEV_INFO_LEVEL_1(fdout, "%d bytes", 
                            "Total shared memory / block", tmp_0);
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_MAX_REGISTERS_PER_BLOCK, tmp_0); 
    __EZCU_DEV_INFO_LEVEL_1(fdout, "%d", 
                            "Total registers available / block", tmp_0);
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_WARP_SIZE, tmp_0); 
    __EZCU_DEV_INFO_LEVEL_1(fdout, "%d", "Warp size", tmp_0);
    __EZCU_DEV_GET(d->id, 
                   CU_DEVICE_ATTRIBUTE_MAX_THREADS_PER_MULTIPROCESSOR, tmp_0); 
    __EZCU_DEV_INFO_LEVEL_1(fdout, "%d", 
                            "Maximum number of threads per SM", tmp_0);
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_MAX_THREADS_PER_BLOCK, tmp_0); 
    __EZCU_DEV_INFO_LEVEL_1(fdout, "%d", 
                            "Maximum number of threads per block", tmp_0);

    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_X, tmp_0); 
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_Y, tmp_1); 
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_Z, tmp_2); 
    __EZCU_DEV_INFO_LEVEL_1(fdout, "(%d, %d, %d)", 
                            "Max dim size of a thread block (x,y,z)", 
                            tmp_0, tmp_1, tmp_2);
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_MAX_GRID_DIM_X, tmp_0); 
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_MAX_GRID_DIM_Y, tmp_1); 
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_MAX_GRID_DIM_Z, tmp_2); 
    __EZCU_DEV_INFO_LEVEL_1(fdout, "(%d, %d, %d)", 
                            "Max dim size of a grid size (x,y,z)", 
                            tmp_0, tmp_1, tmp_2);
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_MAX_PITCH, tmp_0); 
    __EZCU_DEV_INFO_LEVEL_1(fdout, "%d bytes", "Maximum memory pitch", tmp_0);
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_TEXTURE_ALIGNMENT, tmp_0); 
    __EZCU_DEV_INFO_LEVEL_1(fdout, "%d bytes", "Texture alignment", tmp_0);
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_GPU_OVERLAP, tmp_0); 
    __EZCU_DEV_INFO_LEVEL_1(fdout, "%s", 
                            "Concurrent copy and kernel execution", 
                            (tmp_0 ? "Yes" : "No"));
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_CONCURRENT_KERNELS, tmp_0); 
    __EZCU_DEV_INFO_LEVEL_1(fdout, "%s", 
                            "Concurrent kernels execution", 
                            (tmp_0 ? "Yes" : "No"));
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_KERNEL_EXEC_TIMEOUT, tmp_0); 
    __EZCU_DEV_INFO_LEVEL_1(fdout, "%s", 
                            "Run time limit on kernels", tmp_0 ? "Yes" : "No");
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_INTEGRATED, tmp_0); 
    __EZCU_DEV_INFO_LEVEL_1(fdout, "%s", 
                            "Integrated GPU sharing Host Memory", 
                            tmp_0 ? "Yes" : "No");
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_CAN_MAP_HOST_MEMORY, tmp_0);
    __EZCU_DEV_INFO_LEVEL_1(fdout, "%s", 
                            "Support host page-locked memory mapping", 
                            tmp_0 ? "Yes" : "No");
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_ECC_ENABLED, tmp_0); 
    __EZCU_DEV_INFO_LEVEL_1(fdout, "%s", 
                            "Device has ECC support", 
                            tmp_0 ? "Enabled" : "Disabled");
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_TCC_DRIVER, tmp_0); 
    __EZCU_DEV_INFO_LEVEL_1(fdout, "%s", 
                            "CUDA Device Driver Mode (TCC or WDDM)", 
                            tmp_0 ? 
                            "TCC (Tesla Compute Cluster Driver)" : 
                            "WDDM (Windows Display Driver Model)");
#endif
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_UNIFIED_ADDRESSING, tmp_0); 
    __EZCU_DEV_INFO_LEVEL_1(fdout, "%s",  
                            "Device supports Unified Addressing (UVA)",
                            tmp_0 ? "Yes" : "No");
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_PCI_BUS_ID, tmp_0); 
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_PCI_DEVICE_ID, tmp_1); 
    __EZCU_DEV_INFO_LEVEL_1(fdout, "%d/%d",  
                            "Bus ID/location ID", tmp_0, tmp_1);
    __EZCU_DEV_GET(d->id, CU_DEVICE_ATTRIBUTE_COMPUTE_MODE, tmp_0); 
    const char *sComputeMode[] =
    {
        "Default (multiple host threads can use this device simultaneously)",
        "Exclusive (1 host thread in 1 process is able to use this device)",
        "Prohibited (no host thread can use this device)",
        "Exclusive Process (n threads in 1 process are able to use this device)",
        "Unknown",
        NULL
    };
    __EZCU_DEV_INFO_LEVEL_1(fdout, 
                            "< %s >", "Compute Mode", sComputeMode[tmp_0]);
    
#endif  // __EZCU_DEV_LONG_INFO_ENABLED
    EZCU_CHECK(cuCtxPopCurrent(&d->ctx), "failed to pop the context");
}

CPPGUARD_END();

#endif  // __EZCU_DEV_INL_H_
