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
#ifdef __MPI
#include <mpi.h>
#endif // __MPI
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
/// @brief a static instance of ezcu_env_t.
///
ezcu_env_t ezcu = NULL;

/// 
/// @brief Initialize the ezcu environment. 
///
__host__ void ezcu_init(ezcu_flags_t flags) {
  int i;
  char tmp_0[__EZCU_STR_SIZE];
#ifndef __LOG_STD   
  char tmp_1[__EZCU_STR_SIZE];
#ifdef __MPI
  char stime[__EZCU_STR_SIZE];
  time_t now          = time(NULL);
  struct tm time_desc = *(localtime(&now));
  int mpi_on, mpi_rank;
#endif //   __MPI
#endif // ! __LOG_STD

  /// if ezcu == NULL means that the library is not initialized yet.
  /// This test ensures that ezcu is initialized only once.
  if (ezcu == NULL) {
    ezcu         = (ezcu_env_t) malloc(sizeof(struct __ezcu_env_t));
    ezcu->devs   = &urb_sentinel;
    ezcu->knls   = &urb_sentinel;
    ezcu->mems   = &urb_sentinel;
#ifdef __LOG_STD 
    ezcu->fdout  = stdout;
    ezcu->fderr  = stderr;
#else
#ifdef  __MPI
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
#endif // __MPI
    ezcu->fdout = fopen(tmp_0, "w");
    if (ezcu->fdout == NULL) 
      EZCU_FAIL("failed to open ezcu output log file");
    ezcu->fderr = fopen(tmp_1, "w");
    if (ezcu->fderr == NULL) 
      EZCU_FAIL("failed to open ezcu error log file");
#endif // __LOG_STD
    
    /// init the driver API.
    EZCU_CHECK(cuInit(0), "failed to initialize ezcu");
    /// first, load all possible devices.
    EZCU_EXIT_IF((ezcu->nids = __ezcu_dev_query()) == 0, 
                  "no CUDA capable devices found, aborting");
    /// Populate the device ids array in ezcu and create descriptors.
    ezcu->ids  = (CUdevice*)malloc(sizeof(CUdevice)*ezcu->nids);
        
    for (i=0; i < ezcu->nids; ++i) {
      EZCU_CHECK(cuDeviceGet(&ezcu->ids[i], i), 
                 "failed to query the device descriptor");
      CUdevice *ikey = (CUdevice*)malloc(sizeof(CUdevice));
      *ikey          = ezcu->ids[i];
      ezcu_dev_t   d = __ezcu_dev_init(ezcu->ids[i]);
      urb_tree_put(&ezcu->devs, 
                   urb_tree_create(ikey, d), __ezcu_devid_cmp);
    }
   
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
#ifndef __LOG_STD
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
#endif // __LOG_STD
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
