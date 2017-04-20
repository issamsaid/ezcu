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
/// @file ezcu_base.c
/// @author Issam SAID
/// @brief The implementation of the main routines to setup ezCU.
///
#include <stdlib.h>
#include <time.h>
#include <string.h>
#ifdef __EZCU_MPI
#include <mpi.h>
#endif // __EZCU_MPI
#include <ezCU/base.h>
#include <ezCU/timer.h>

#include <__ezcu/config/dev.h>
#include <__ezcu/config/knl.h>
#include <__ezcu/config/timer.h>

#include <__ezcu/util-inl.h>
#include <__ezcu/error-inl.h>
#include <__ezcu/dev-inl.h>
#include <__ezcu/mem-inl.h>
#include <__ezcu/knl-inl.h>

///
/// @brief A static instance of ezcu_env_t.
///
ezcu_env_t ezcu = NULL;

/// 
/// @brief Initialize the ezCU environment. 
///
__host__ void ezcu_init(ezcu_flags_t flags) {
    int i, j, k, n;
    char tmp_0[__EZCU_STR_SIZE];
#ifndef __EZCU_LOG_STD   
    char tmp_1[__EZCU_STR_SIZE];
#ifdef __EZCU_MPI
    char stime[__EZCU_STR_SIZE];
    time_t now          = time(NULL);
    struct tm time_desc = *(localtime(&now));
    int mpi_on, mpi_rank;
#endif //   __EZCU_MPI
#endif // ! __EZCU_LOG_STD
   
    /// User selected flags. 
    int ndt, ndp, ndi;
    ezcu_vendor_flags_t    v;
    ezcu_dev_type_flags_t  dt[EZCU_NB_DEV_TYPES];
    ezcu_dev_prop_flags_t  dp[EZCU_NB_DEV_PROPS];
    ezcu_dev_index_flags_t di[EZCU_NB_DEV_INDEXES];

    /// if ezcu == NULL means that the library is not initialized yet.
    /// This test ensures that ezCU is initialized only once.
    if (ezcu == NULL) {
        ezcu         = (ezcu_env_t) malloc(sizeof(struct __ezcu_env_t));
        ezcu->devs   = &urb_sentinel;
        ezcu->knls   = &urb_sentinel;
        ezcu->mems   = &urb_sentinel;
        for (i=0; i < EZCU_NB_VENDORS; i++)  ezcu->initialized[i] = false;
        for (k=0; k < EZCU_NB_DEV_TYPES; k++)
            for (j=0; j < EZCU_NB_DEV_PROPS; j++)
                for (i=0; i < EZCU_NB_DEV_INDEXES; i++)
                    ezcu->lookup[k][j][i] = -1;
#ifdef __EZCU_LOG_STD 
        ezcu->fdout  = stdout;
        ezcu->fderr  = stderr;
#else
#ifdef  __EZCU_MPI
        strftime(stime, __EZCU_STR_SIZE, "%m%d%y%H%M%S", &time_desc);
        MPI_Initialized(&mpi_on);
        if (mpi_on) {
            MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
            sprintf(tmp_0, "ezcu_log_%s_%d.out", stime, mpi_rank);
            sprintf(tmp_1, "ezcu_log_%s_%d.err", stime, mpi_rank);
        } else {
            sprintf(tmp_0, "ezcu_log_%s.out", stime);
            sprintf(tmp_1, "ezcu_log_%s.err", stime);
        }
#else
        sprintf(tmp_0, "ezcu_log.out");
        sprintf(tmp_1, "ezcu_log.err");
#endif // __EZCU_MPI
        ezcu->fdout = fopen(tmp_0, "w");
        if (ezcu->fdout == NULL) 
            HICL_FAIL("failed to open ezCU output log file");
        ezcu->fderr = fopen(tmp_1, "w");
        if (ezcu->fderr == NULL) 
            HICL_FAIL("failed to open ezCU error log file");
#endif // __EZCU_LOG_STD
    
#ifdef __EZCU_DEBUG
        ezcu_flags_dev_to_str(flags, tmp_0);
        EZCU_DEBUG("ezcu_init: translated flags =%s", tmp_0);
#endif
        /// init the driver API.
        EZCU_CHECK(cuInit(0), "failed to initialize ezCU");

        /// start devices selection, based on the user defined flags.
        ezcu_flags_check_dev(flags);
        ezcu_flags_parse_dev(flags, &v, dt, &ndt, dp, &ndp, di, &ndi);

        /// first, load all possible devices.
        n = __ezcu_dev_count();
        EZCU_EXIT_IF(n == 0, "no CUDA capable devices found, aborting");
        int matched;
        CUdevice ids[n];
        for (i=0; i < n; ++i) {
            EZCU_CHECK(cuDeviceGet(&ids[i], i), 
                "failed to query the device descriptor");
        }

        /// second, select by vendors names (only one at a time).
        /// the vendor should be informed or we fail to NVIDIA.
        /// if informed and not found we raise an error.
        if ((n = __ezcu_dev_select_by_vendor(ids, n, v)) == 0) {
            ezcu_flags_dev_to_str(v, tmp_0);
            ezcu_flags_help_vendors();
            EZCU_EXIT("failed to find '%s' devices, aborting", tmp_0);
        } else { ezcu->initialized[v & EZCU_LKP_MASK] = true; }

        /// then, select by devices types, properties and indexes.
        /// the device types should be informed or we fail to ALL.
        /// if informed and not found we raise an error.
        CUdevice dt_ids[n];
        for (k=0; k < ndt; k++) {
            if ((matched = __ezcu_dev_select_by_dev_type(ids, n, 
                                                         dt_ids, dt[k])) == 0) {
                ezcu_flags_dev_to_str(dt[k], tmp_0);
                EZCU_DEBUG("devices of type '%s' not found", tmp_0);
                continue;
            }
            /// select by devices properties.
            /// the device properties should be informed or we select all.
            /// if informed and not found we raise an error.
            CUdevice dp_ids[matched];
            for (j=0; j < ndp; j++) {
                if((matched = 
                    __ezcu_dev_select_by_dev_prop(dt_ids, matched, 
                                                  dp_ids, dp[j])) == 0) {
                    ezcu_flags_dev_to_str(dp[j], tmp_0);
                    EZCU_DEBUG("devices with property '%s' not found", tmp_0);
                    continue;
                }
                /// select by devices indexes.
                /// the device indexes should be informed or we select all.
                /// if informed and not found we raise an error.
                CUdevice di_ids[matched];
                for (i=0; i < ndi; i++) {
                    if((matched = 
                            __ezcu_dev_select_by_dev_index(dp_ids, matched, 
                                                           di_ids, di[i]))==0) {
                        ezcu_flags_dev_to_str(di[i], tmp_0);
                        EZCU_WARN("device index (%s) out of bound", tmp_0);
                        continue;
                    }
                    ezcu->lookup[dt[k] & EZCU_LKP_MASK]
                                [dp[j] & EZCU_LKP_MASK]
                                [di[i] & EZCU_LKP_MASK] = di_ids[0];
                }
            }
        }   

        /// post-process the lookup table. 
        int ip=0;
        CUdevice itmp;
        for (k=0; k < EZCU_NB_DEV_TYPES; k++) {
            for (j=0; j < EZCU_NB_DEV_PROPS; j++) {
                ip = 0;
                for (i=0; i < EZCU_NB_DEV_INDEXES; i++) {
                    if (ezcu->lookup[k][j][i] > -1) {
                        while(ezcu->lookup[k][j][ip] == -1)
                            if (++ip >= EZCU_NB_DEV_INDEXES){ ip = 0; break; }
                        itmp = ezcu->lookup[k][j][ip];
                        ezcu->lookup[k][j][ip] = ezcu->lookup[k][j][i];
                        ezcu->lookup[k][j][i]  = itmp;    
                    }
                }
            }
        }        
         
        /// copy GPU lookup to ACCELERATOR.
        if (!EZCU_FLAGS_HAVE(flags, ACCELERATOR)) {
            for (j=0; j < EZCU_NB_DEV_PROPS; j++) {
                ip = 0;
                for (i=0; i < EZCU_NB_DEV_INDEXES; i++) {
                    if ((itmp=ezcu->lookup[GPU & EZCU_LKP_MASK][j][i]) > -1) {
                        while(ezcu->lookup[ACCELERATOR & EZCU_LKP_MASK]
                                          [j][ip] == -1)
                            if (++ip >= EZCU_NB_DEV_INDEXES){ ip = 0; break; }
                        ezcu->lookup[ACCELERATOR & EZCU_LKP_MASK][j][ip] = itmp;
                    } 
                }
            }
        } 

        /// Accumulate ACCELERATOR + CPU into ALL 
        if (!EZCU_FLAGS_HAVE(flags, ALL)) {  
            for (j=0; j < ndp; j++) {
                ip = 0;
                for (i=0; i < ndi; i++) {
                    if ((itmp=ezcu->lookup[CPU & EZCU_LKP_MASK][j][i]) > -1) {
                        while(ezcu->lookup[ALL & EZCU_LKP_MASK]
                                          [j][ip] == -1)
                            if (++ip >= EZCU_NB_DEV_INDEXES){ ip = 0; break; }
                        ezcu->lookup[ALL & EZCU_LKP_MASK][j][ip] = itmp;
                    } 
                }
                for (i=0; i < ndi; i++) {
                    if ((itmp=ezcu->lookup[ACCELERATOR & EZCU_LKP_MASK]
                                          [j][i]) > -1) {
                        while(ezcu->lookup[ALL & EZCU_LKP_MASK]
                                          [j][ip] == -1)
                            if (++ip >= EZCU_NB_DEV_INDEXES){ ip = 0; break; }
                        ezcu->lookup[ALL & EZCU_LKP_MASK][j][ip] = itmp;
                    } 
                }
            }
        }

        /// setup device descriptors.
        /// gather the compute capabilities of the selected devices.
        int ccs[n], max_ccs=0, dev_cc;
        for (i=0; i<n; ++i) {
            int *ikey    = (int*)malloc(sizeof(int));
            *ikey        = i;
            ezcu_dev_t d = __ezcu_dev_init(i);
            urb_tree_put(&ezcu->devs, 
                         urb_tree_create(ikey, d), __ezcu_int_cmp);
            dev_cc = d->major*10 + d->minor; 
            for (j=0; j<max_ccs; j++) if (ccs[j] == dev_cc) break;
            if (j == max_ccs) ccs[max_ccs++] = dev_cc;  
        }

        /// generate the right compile option to be used when loading kernels. 
        for (j=0; j<max_ccs; j++) {
            sprintf(ezcu->cc_opts, 
                    "%s --generate-code arch=compute_%d,code=sm_%d",
                    ezcu->cc_opts, ccs[j], ccs[j]);
        }

        /// Inform the user about the loaded devices.
        {
            EZCU_PRINT("%u CUDA device%s %s loaded",
                       n,
                       n == 1 ? ""   : "s",
                       n == 1 ? "is" : "are");
            urb_tree_walk(&ezcu->devs, NULL, __ezcu_dev_get_name);
        }

        /// setup timer.
        ezcu_timer_uset(__EZCU_TIMER_UNIT);
    }
}

/// 
/// @brief Load CUDA files.
/// Supported extensions: cu. 
/// cu -- nvcc -> fatbin
/// TODO: support ptx, cubin and fatbin (if it is possible to extract 
///       kernel names).
void ezcu_load(const char *filename, const char *options) {
    /// check if the filename is valid.
    __ezcu_file_check_ext(filename);
    
    /// for debugging.
    EZCU_DEBUG("start load file '%s'", filename);

    /// compile to fatbin.
    __ezcu_knl_compile(filename, options);

    /// load the compiled code into a module.
    char bin_filename[__EZCU_STR_SIZE];
    CUmodule module;
    __ezcu_generate_bin_filename(filename, bin_filename);
    EZCU_CHECK(cuModuleLoad(&module, bin_filename),
               "failed to load CUDA binary module '%s'", bin_filename);

    /// build the kernel descriptors.
    CUfunction func;
    size_t num_args, code_size = __ezcu_tell_file(filename);
    ezcu_knl_t k;
    char *pch, *name, *code=(char*)malloc(code_size+1);
    char code_knl[__EZCU_BUFFER_SIZE];
    __ezcu_read_from_file(code, code_size+1, filename);

    pch = code;
    while((pch=__ezcu_knl_extract(pch, &name, code_knl)) != NULL) {
        if (urb_tree_find(&ezcu->knls, name, __ezcu_str_cmp) == &urb_sentinel) {
            EZCU_DEBUG("create kernel name = '%s'", name);
            num_args = __ezcu_knl_get_arg_types(code_knl, NULL);
            EZCU_CHECK(cuModuleGetFunction(&func, module, name),
                   "failed to extract function '%s' from cubin module, "
                   "use extern \"C\" to avaid C++ name mangling", name);
            k        = __ezcu_knl_init(name, func, num_args);
            __ezcu_knl_get_arg_types(code_knl, k->args_types);
            urb_tree_put(&ezcu->knls, urb_tree_create(name, k), __ezcu_str_cmp);
        }
    }

    /// free temporary buffers.
    free(code);
    
    /// for debugging.
    EZCU_DEBUG("end   load file '%s'", filename);
    EZCU_DEBUG("");
}

void ezcu_release() {
    ///
    /// if ezcu != NULL means that the library is not released yet.
    /// This test ensures that ezCU is released only once.
    ///
    if (ezcu != NULL) {
        EZCU_DEBUG("releasing CUDA resources");
        urb_tree_delete(&ezcu->knls, __ezcu_ptr_del, __ezcu_knl_release);
        urb_tree_delete(&ezcu->mems,           NULL, __ezcu_mem_release);
        urb_tree_delete(&ezcu->devs, __ezcu_ptr_del, __ezcu_dev_release);
        EZCU_PRINT("CUDA resources are released");
    #ifndef __EZCU_LOG_STD
        if (fclose(ezcu->fdout)) {
            fprintf(stderr, 
                    "FATAL ERROR: failed to close the ezCU output log file");
            free(ezcu); ezcu = NULL;
            exit(EXIT_FAILURE);
        }
        if (fclose(ezcu->fderr)) {
            fprintf(stderr, 
                    "FATAL ERROR: failed to close the ezCU error log file");
            free(ezcu); ezcu = NULL;
            exit(EXIT_FAILURE);
        }
    #endif
        free(ezcu); 
        ezcu = NULL;
    }
}

void ezcu_info() {
    EZCU_PRINT("");
    EZCU_PRINT("ezCU resources information");
    urb_tree_walk(&ezcu->devs, NULL, __ezcu_dev_info);
    urb_tree_walk(&ezcu->mems, NULL, __ezcu_mem_info);
    urb_tree_walk(&ezcu->knls, NULL, __ezcu_knl_info);
    EZCU_PRINT("");
}