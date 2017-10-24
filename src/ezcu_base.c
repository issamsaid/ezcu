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
/// @brief The implementation of the main routines to setup ezcu.
///
#include <stdlib.h>
#include <time.h>
#include <string.h>
#ifdef __EZCU_MPI
#include <mpi.h>
#endif // __EZCU_MPI
#include <ezcu/base.h>
#include <ezcu/timer.h>

#include <__ezcu/config/dev.h>
#include <__ezcu/config/knl.h>
#include <__ezcu/config/timer.h>

#include <__ezcu/types-inl.h>
#include <__ezcu/util-inl.h>
#include <__ezcu/error-inl.h>
#include <__ezcu/dev-inl.h>
#include <__ezcu/mem-inl.h>
#include <__ezcu/knl-inl.h>
#include <__ezcu/fortran-bind.h>

///
/// @brief A static instance of ezcu_env_t.
///
ezcu_env_t ezcu = NULL;

/// 
/// @brief Initialize the ezcu environment. 
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
    /// This test ensures that ezcu is initialized only once.
    if (ezcu == NULL) {
        ezcu         = (ezcu_env_t) malloc(sizeof(struct __ezcu_env_t));
        ezcu->devs   = &urb_sentinel;
        ezcu->knls   = &urb_sentinel;
        ezcu->mems   = &urb_sentinel;
        for (i=0; i < EZCU_NB_VENDORS; i++)  ezcu->initialized[i] = false;
        for (k=0; k < EZCU_NB_DEV_TYPES; k++)
            for (j=0; j < EZCU_NB_DEV_PROPS; j++)
                for (i=0; i < EZCU_NB_DEV_INDEXES; i++)
                    ezcu->lookup[k][j][i] = (CUdevice)-1;
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
            EZCU_FAIL("failed to open ezcu output log file");
        ezcu->fderr = fopen(tmp_1, "w");
        if (ezcu->fderr == NULL) 
            EZCU_FAIL("failed to open ezcu error log file");
#endif // __EZCU_LOG_STD
    
#ifdef __EZCU_DEBUG
        ezcu_flags_dev_to_str(flags, tmp_0);
        EZCU_DEBUG("ezcu_init: translated flags =%s", tmp_0);
#endif
        /// init the driver API.
        EZCU_CHECK(cuInit(0), "failed to initialize ezcu");

        /// start devices selection, based on the user defined flags.
        ezcu_flags_check_dev(flags);
        ezcu_flags_parse_dev(flags, &v, dt, &ndt, dp, &ndp, di, &ndi);

        /// first, load all possible devices.
        n = __ezcu_dev_query();
        EZCU_EXIT_IF(n == 0, "no CUDA capable devices found, aborting");
        EZCU_EXIT_IF(n > EZCU_NB_DEV_INDEXES, 
                     "ezcu supports only %d devices at a time, aborting",
                     EZCU_NB_DEV_INDEXES);

        EZCU_DEBUG("ezcu_init: the platform has %d possible device(s)", n);
        int m_dt, m_di, m_dp;
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
        EZCU_DEBUG("%d type(s) detected", ndt);
        for (k=0; k < ndt; k++) {
            ezcu_flags_dev_to_str(dt[k], tmp_0);
            EZCU_DEBUG("parsing type: %s", tmp_0);
            if ((m_dt = __ezcu_dev_select_by_dev_type(ids, n, 
                                                      dt_ids, dt[k])) == 0) {
                ezcu_flags_dev_to_str(dt[k], tmp_0);
                EZCU_DEBUG("devices of type '%s' not found", tmp_0);
                continue;
            }
            /// select by devices properties.
            /// the device properties should be informed or we select all.
            /// if informed and not found we raise an error.
            CUdevice dp_ids[n];
            for (j=0; j < ndp; j++) {
                if((m_dp = 
                    __ezcu_dev_select_by_dev_prop(dt_ids, n, 
                                                  dp_ids, dp[j])) == 0) {
                    ezcu_flags_dev_to_str(dp[j], tmp_0);
                    EZCU_DEBUG("devices with property '%s' not found", tmp_0);
                    continue;
                }
                /// select by devices indexes.
                /// the device indexes should be informed or we select all.
                /// if informed and not found we raise an error.
                CUdevice di_ids[n];
                for (i=0; i < ndi; i++) {
                    if((m_di = 
                        __ezcu_dev_select_by_dev_index(dp_ids, n, 
                                                       di_ids, di[i]))==0) {
                        //ezcu_flags_dev_to_str(di[i], tmp_0);
                        //EZCU_WARN("device index (%s) out of bound", tmp_0);
                        continue;
                    }
                    ezcu->lookup[dt[k] & EZCU_LKP_MASK]
                                [dp[j] & EZCU_LKP_MASK]
                                [di[i] & EZCU_LKP_MASK] = di_ids[0];
                }
            }
        }   

        /// Post-process the lookup table: gather the non -1. 
        int ip; bool found = false;
        CUdevice itmp, stmp, swap[EZCU_NB_DEV_INDEXES];
        for (k=0; k < EZCU_NB_DEV_TYPES; k++) {
            for (j=0; j < EZCU_NB_DEV_PROPS; j++) {
                ip = 0;
                for (i=0; i < EZCU_NB_DEV_INDEXES; i++) {
                    if (ezcu->lookup[k][j][i] > -1) {
                        swap[ip++] = ezcu->lookup[k][j][i];
                    }
                }
                for (i=0; i < ip; i++) {
                    ezcu->lookup[k][j][i] = swap[i];
                }
                for (i=ip; i < EZCU_NB_DEV_INDEXES; i++) {
                    ezcu->lookup[k][j][i] = -1;
                }                
            }
        }        
         
        /// Copy GPU lookup to ACCELERATOR: in case where only GPU is selected
        /// GPU included in ACCELERATOR
        if (!EZCU_FLAGS_HAVE(flags, ACCELERATOR)) {
            for (j=0; j < EZCU_NB_DEV_PROPS; j++) {
                for (i=0; i < EZCU_NB_DEV_INDEXES; i++) {
                    if ((itmp=ezcu->lookup[GPU & EZCU_LKP_MASK][j][i]) > -1) {
                        found = false;
                        for (k=0; k < EZCU_NB_DEV_INDEXES; k++) {
                            stmp = ezcu->lookup[ACCELERATOR & 
                                EZCU_LKP_MASK][j][k];
                            if (stmp == itmp) { found = true; break; }
                            if (stmp == -1) { ip=k; break; }
                        }
                        if (!found) 
                            ezcu->lookup[ACCELERATOR & EZCU_LKP_MASK][j][k] = 
                            itmp;
                    } 
                }
            }
        } 

        /// Accumulate ACCELERATOR + CPU into ALL 
        if (!EZCU_FLAGS_HAVE(flags, ALL)) {  
            for (j=0; j < ndp; j++) {
                for (i=0; i < ndi; i++) {
                    if ((itmp=ezcu->lookup[CPU & EZCU_LKP_MASK][j][i]) > -1) {
                                                found = false;
                        for (k=0; k < EZCU_NB_DEV_INDEXES; k++) {
                            stmp = ezcu->lookup[ALL & EZCU_LKP_MASK][j][k];
                            if (stmp == itmp) { found = true; break; }
                            if (stmp == -1) { ip=k; break; }
                        }
                        if (!found) 
                            ezcu->lookup[ALL & EZCU_LKP_MASK][j][k] = 
                            itmp;
                    }
                }
                for (i=0; i < ndi; i++) {
                    if ((itmp=ezcu->lookup[ACCELERATOR & EZCU_LKP_MASK]
                                          [j][i]) > -1) {
                        for (k=0; k < EZCU_NB_DEV_INDEXES; k++) {
                            stmp = ezcu->lookup[ALL & EZCU_LKP_MASK][j][k];
                            if (stmp == itmp) { found = true; break; }
                            if (stmp == -1) { ip=k; break; }
                        }
                        if (!found) 
                            ezcu->lookup[ALL & EZCU_LKP_MASK][j][k] = 
                            itmp;
                    } 
                }
            }
        }

#ifdef __EZCU_DEBUG
        ezcu_dev_type_flags_t  typs[] = {ALL, CPU, ACCELERATOR, GPU};
        for (k=0; k < EZCU_NB_DEV_TYPES; k++) {
            ezcu_flags_dev_to_str(typs[k], tmp_0);
            EZCU_DEBUG("Type: %s", tmp_0);
            for (j=0; j < EZCU_NB_DEV_PROPS; j++) {
                for (i=0; i < EZCU_NB_DEV_INDEXES; i++) {
                    printf("%d ", ezcu->lookup[k][j][i]);
                }
                printf("\n");
            }
        }
#endif

        /// Get the number of the selected devices.
        /// The selected ids are in ALL/CC20/INDEXES.
        ezcu->nids = 0;
        for (i=0; i < EZCU_NB_DEV_INDEXES; i++) {
            if ((itmp=ezcu->lookup[ALL  & EZCU_LKP_MASK]
                                  [CC20 & EZCU_LKP_MASK][i]) > -1) {
                ezcu->nids++;
            }
        }

        /// Populate the device ids array in ezcu and create descriptors.
        int tmp_ccs[n], max_ccs=0, dev_cc;
        j = 0; 
        ezcu->ids = (CUdevice*)malloc(sizeof(CUdevice)*ezcu->nids);
        for (i=0; i < EZCU_NB_DEV_INDEXES; i++) {
            if ((itmp=ezcu->lookup[ALL  & EZCU_LKP_MASK]
                                  [CC20 & EZCU_LKP_MASK][i]) > -1) {
                ezcu->ids[j++] = itmp;
                EZCU_DEBUG("creating device for id '%d'", itmp);
                CUdevice *ikey = (CUdevice*)malloc(sizeof(CUdevice));
                *ikey          = itmp;
                ezcu_dev_t   d = __ezcu_dev_init(itmp);
                urb_tree_put(&ezcu->devs, 
                    urb_tree_create(ikey, d), __ezcu_devid_cmp);
                dev_cc = d->major*10 + d->minor; 
                for (k=0; k<max_ccs; k++) if (tmp_ccs[k] == dev_cc) break;
                if (k == max_ccs) tmp_ccs[max_ccs++] = dev_cc;
            }
        }
   
        /// Gather the compute capabilities of the selected devices.
        ezcu->nccs = max_ccs;
        ezcu->ccs  = (int*)malloc(sizeof(int)*max_ccs);
        EZCU_DEBUG("max ccs is '%d'", max_ccs);
        for (i=0; i < max_ccs; i++) ezcu->ccs[i] = tmp_ccs[i];

        /// Generate the right compile option to be used when loading kernels. 
        char *cptr = ezcu->cc_opts;
        for (i=0; i<ezcu->nccs; i++) {
            char tmp[__EZCU_STR_SIZE];
            sprintf(tmp, 
                    "--generate-code arch=compute_%d,code=sm_%d", 
                    ezcu->ccs[i], ezcu->ccs[i]);
            sprintf(cptr, "%s", tmp);
            cptr += strlen(tmp) + 1;
        }
        EZCU_DEBUG("cc options are '%s'", ezcu->cc_opts);
        
        /// Inform the user about the loaded devices.
        {
            EZCU_PRINT("%u CUDA device%s %s loaded",
                       ezcu->nids,
                       ezcu->nids == 1 ? ""   : "s",
                       ezcu->nids == 1 ? "is" : "are");
            urb_tree_walk(&ezcu->devs, NULL, __ezcu_dev_get_name);
        }

        /// Setup timer.
        ezcu_timer_uset(__EZCU_TIMER_UNIT);
    }
}

/// 
/// @brief Load CUDA files.
/// Supported extensions: cu. 
/// cu -- nvcc -> fatbin
/// TODO: support ptx, cubin and fatbin (if it is possible to extract 
///       kernel names).
void ezcu_load(const char *filename, const char *options_format, ...) {

    /// check if the filename is valid.
    __ezcu_file_check_ext(filename);
    
    /// for debugging.
    EZCU_DEBUG("start load file '%s'", filename);

    /// collect the options.
    char options[__EZCU_STR_SIZE];
    va_list args;
    va_start(args, options_format);
    vsprintf(options, options_format, args);
    va_end(args);
    
    /// compile to fatbin.
    __ezcu_knl_compile(filename, options);

    /// Generate the name of the compiled binary.
    char bin_filename[__EZCU_STR_SIZE];
    __ezcu_generate_bin_filename(filename, bin_filename);
    
    /// load the source code in order to generate the kernel descriptors.
    size_t num_args, code_size = __ezcu_tell_file(filename);
    
    char *pch, *name, *code=(char*)malloc(code_size+1);
    char code_knl[__EZCU_BUFFER_SIZE];
    __ezcu_read_from_file(code, code_size+1, filename);

    /// load the compiled module to each selected device context.
    int i; ezcu_dev_t d;
    CUmodule module;
    for (i=0; i < ezcu->nids; i++) {
        d = (ezcu_dev_t)urb_tree_find(&ezcu->devs, 
                                      &ezcu->ids[i], __ezcu_devid_cmp)->value;
        EZCU_CHECK(cuCtxPushCurrent(d->ctx), 
            "failed to push the device (%s) context", d->name);
        EZCU_CHECK(cuModuleLoad(&module, bin_filename),
            "failed to load CUDA binary module '%s'", bin_filename);
    
        pch = code;
        while((pch=__ezcu_knl_extract(pch, &name, code_knl)) != NULL) {
            if (urb_tree_find(&ezcu->knls, name, 
                              __ezcu_str_cmp) == &urb_sentinel) {
                EZCU_DEBUG("create kernel name = '%s'", name);
                num_args = __ezcu_knl_get_arg_types(code_knl, NULL);
                ezcu_knl_t k;
                CUfunction func;

                EZCU_CHECK(cuModuleGetFunction(&func, module, name),
                   "failed to extract function '%s' from cubin module, "
                   "use extern \"C\" to avoid C++ name mangling", name);
                
                k        = __ezcu_knl_init(name, func, num_args);
                __ezcu_knl_get_arg_types(code_knl, k->args_types);
                urb_tree_put(&ezcu->knls, 
                             urb_tree_create(name, k), __ezcu_str_cmp);
            }
        }

        // EZCU_CHECK(cuModuleUnload(module),
        //     "failed to unload CUDA binary module '%s'", bin_filename);

        EZCU_CHECK(cuCtxPopCurrent(&d->ctx), 
            "failed to pop  the device (%s) context", d->name);
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
    /// This test ensures that ezcu is released only once.
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
                    "FATAL ERROR: failed to close the ezcu output log file");
            free(ezcu); ezcu = NULL;
            exit(EXIT_FAILURE);
        }
        if (fclose(ezcu->fderr)) {
            fprintf(stderr, 
                    "FATAL ERROR: failed to close the ezcu error log file");
            free(ezcu); ezcu = NULL;
            exit(EXIT_FAILURE);
        }
    #endif
        free(ezcu->ccs);
        free(ezcu->ids);
        free(ezcu); ezcu = NULL;
    }
}

void ezcu_info() {
    EZCU_PRINT("");
    EZCU_PRINT("ezcu resources information");
    urb_tree_walk(&ezcu->devs, NULL, __ezcu_dev_info);
    urb_tree_walk(&ezcu->mems, NULL, __ezcu_mem_info);
    urb_tree_walk(&ezcu->knls, NULL, __ezcu_knl_info);
    EZCU_PRINT("");
}
