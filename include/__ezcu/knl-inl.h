#ifndef __EZCU_KNL_INL_H_
#define __EZCU_KNL_INL_H_
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
/// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
//  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
/// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
/// LIABILITY, WETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
/// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
/// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///
/// @file __ezcu/knl-inl.h
/// @author Issam SAID
/// @brief Private functions used to implement the ezCU kernel descriptor.
///
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <ezCU/types.h>
#include <__ezcu/config/cuda.h>
#include <__ezcu/config/private.h>
#include <__ezcu/config/guard.h>
#include <__ezcu/config/util.h>
#include <__ezcu/config/log.h>
#include <__ezcu/util-inl.h>

CPPGUARD_BEGIN();

extern void __ezcu_mem_release(void*);
extern ezcu_env_t ezcu; 

__EZCU_PRIVATE char*
__ezcu_knl_extract(char* code, char** name, char* container) {
    char *scode =NULL, *ecode = NULL;
    char *sname =NULL, *ename = NULL;
    if (code == NULL) return NULL;
    if ((scode=strstr(code, "__global__")) == NULL) return NULL;    
    sname = strstr(scode, "void")+5;
    ename = strstr(sname, "(");
    ecode = strstr(scode, "}")+1;
    while(isspace(*sname)) sname++;
    while(isspace(*(ename-1))) ename--;
    snprintf(container, strlen(scode)-strlen(ecode)+1, "%s", scode);
    *name = (char*)malloc((strlen(sname)-strlen(ename)+1)*sizeof(char));
    snprintf(*name, strlen(sname)-strlen(ename)+1, "%s", sname);
    return ecode;
}

__EZCU_PRIVATE int
__ezcu_knl_get_arg_types(char* code, ezcu_mem_types_flags_t* types) {
    int num_args=0;
    char tmp[__EZCU_BUFFER_SIZE], *pch, *s, *e;
    s = strstr(code, "(");
    e = strstr(code, ")");
    snprintf(tmp, strlen(s+1)-strlen(e)+1, "%s", s+1);
    pch = strtok(tmp, ",");
    while (pch != NULL) {
        if (types != NULL) {
            if (strstr(pch, "*")) {
                types[num_args] = POINTER;
                EZCU_DEBUG("get arg type %d = pointer", num_args);
            } else if (strstr(pch, "float")) {
                types[num_args] = FLOAT;
                EZCU_DEBUG("get arg type %d = float", num_args);
            } else if (strstr(pch, "double")) {
                types[num_args] = DOUBLE;
                EZCU_DEBUG("get arg type %d = double", num_args);
            } else if (strstr(pch, "int")) {
                types[num_args] = INT;
                EZCU_DEBUG("get arg type %d = int", num_args);
            } else if ((strstr(pch, "int") && (strstr(pch, "unsigned"))) ||
                       (strstr(pch, "uint"))) {
                types[num_args] = UNSIGNED_INT;
                EZCU_DEBUG("get arg type %d = unsigned int", num_args);
            } else if (strstr(pch, "size_t")) {
                types[num_args] = SIZET;
                EZCU_DEBUG("get arg type %d = size_t", num_args);
            } else if (strstr(pch, "long")) {
                types[num_args] = LONG;
            } else if ((strstr(pch, "long") && (strstr(pch, "unsigned"))) ||
                       (strstr(pch, "ulong"))) {
                types[num_args] = UNSIGNED_LONG;
                EZCU_DEBUG("get arg type %d = unsigned long", num_args);
            } else if (strstr(pch, "char") || strstr(pch, "uchar")) {
                types[num_args] = CHAR;
                EZCU_DEBUG("get arg type %d = char", num_args);
            } else {
                EZCU_EXIT("unknown CUDA kernel argument type");
            }
        }

        pch = strtok(NULL, ",");
        num_args++;
    }
    return num_args;
}

///
/// @brief Create a ezCU kernel descriptor.
///
/// This routine creates a ezCU kernel descriptor, provided an CUDA function,
/// which contains information about the work size, see types.h. 
/// @param func is the CUDA function.
/// @return a ezCU kernel descriptor.
///
__EZCU_PRIVATE __host__ ezcu_knl_t 
__ezcu_knl_init(const char* name, const CUfunction func, int num_args) {
    ezcu_knl_t k  = (ezcu_knl_t)malloc(sizeof(struct __ezcu_knl_t));
    k->id         = func;
    k->smem       = 0;
    k->vals       = &urb_sentinel;
    k->mems       = &urb_sentinel;
    k->num_args   = num_args;
    k->args       = (void**)malloc(sizeof(void*)*num_args);
    k->args_types = (ezcu_mem_types_flags_t*)
                        malloc(sizeof(ezcu_mem_types_flags_t)*num_args);
    return k;
}

///
/// @brief Compile cu to cubin/fatbin.
///
/// This routine compiles a cu file using nvcc.
/// @param code is the CUDA source code.
/// @param options are the nvcc options.
/// @return Nothing.
///
__EZCU_PRIVATE __host__ void
__ezcu_knl_compile(const char *filename, const char *options) {
    char nvcc_cmd[__EZCU_BUFFER_SIZE], cpy[__EZCU_STR_SIZE];
    const char *env_opts = getenv("EZCU_BUILD_OPTIONS");
    sprintf(cpy, "%s", filename);
    sprintf(nvcc_cmd, "%s %s", "nvcc -v -fatbin -odir", 
            dirname(cpy));
#ifdef __EZCU_DEBUG
    sprintf(nvcc_cmd, "%s %s", nvcc_cmd, "-g -G");
#endif
    sprintf(nvcc_cmd, "%s %s", nvcc_cmd, "-lineinfo -Xptxas -v,-Werror");

    if (ezcu->cc_opts != NULL)
        sprintf(nvcc_cmd, "%s %s", nvcc_cmd, ezcu->cc_opts);
    if (options != NULL)
        sprintf(nvcc_cmd, "%s %s", nvcc_cmd, options);
    if (env_opts != NULL)
        sprintf(nvcc_cmd, "%s %s", nvcc_cmd, env_opts);
    sprintf(nvcc_cmd, "%s %s", nvcc_cmd, filename);
    EZCU_DEBUG("compiling cmd '%s'", nvcc_cmd); 
    EZCU_EXIT_IF(system(nvcc_cmd), "failed to compile '%s'", filename);
}

// ///
// /// @brief Compile cu kernels to ptx using .
// ///
// /// This routine compiles a CUDA file to a ptx file using NVRTC.
// /// @param filename is the CUDA file.
// /// @param options are the nvcc options.
// /// @return Nothing.
// ///
// __EZCU_PRIVATE __host__ void
// __ezcu_knl_compile_cu(const char* code, const char* options) {
//     char **opt_buffer;
//     char tmp[__EZCU_BUFFER_SIZE];
//     const char *env_opts;
// #ifdef __EZCU_DEBUG
//     const char *cst_opts = "-G -lineinfo";
// #else
//     const char *cst_opts = "-lineinfo";
// #endif

//     /// NVRTC resources.
//     nvrtcProgram nvrtc_prog;

    
//     /// create an NVRTC program.
//     EZCU_NVRTC_CHECK(nvrtcCreateProgram(&nvrtc_prog, code, 
//                                         basename((char*)filename), 
//                                         0, NULL, NULL),
    
//                                         "failed to create NVRTC program");         
    
//     /// collect the options.
//     env_opts = getenv("EZCU_BUILD_OPTIONS");
//     if ((env_opts != NULL) && (strlen(env_opts) !=0 )) {
//         if ((options != NULL) && (strlen(options) !=0 ))
//             sprintf(tmp, "%s %s %s",
//                     cst_opts, env_opts, options);
//         else
//             sprintf(tmp, "%s %s", cst_opts, env_opts);
//     } else {
//         if ((options != NULL) && (strlen(options) !=0 ))
//             sprintf(tmp, "%s %s", cst_opts, options);
//         else
//             sprintf(tmp, "%s", cst_opts);
//     }

//     int i, num_ops = __ezcu_str_split(tmp, &opt_buffer);

//     EZCU_PRINT("NVRTC compiling '%s' with %d option%s", 
//                 filename, num_ops, num_ops == 1 ? "" : "s");
//     for(i=0; i< num_ops; i++) EZCU_PRINT("opt[%d] %s", i, opt_buffer[i]);

//     EZCU_NVRTC_CHECK(nvrtcCompileProgram(nvrtc_prog, num_ops, 
//                     (const char**)opt_buffer),
//                     "failed to compile the NVRTC program");

//     /// obtain the compilation log from the program.
//     size_t log_size;
//     EZCU_NVRTC_CHECK(nvrtcGetProgramLogSize(nvrtc_prog, &log_size),
//                      "failed to get the log size from NVRTC program");
//     char *log_buffer = (char*)malloc(log_size*sizeof(char));
//     EZCU_NVRTC_CHECK(nvrtcGetProgramLog(nvrtc_prog, log_buffer),
//                      "failed to get the compilation log from NVRTC program");
//     if (strlen(log_buffer)) EZCU_PRINT("PTX log (%s): %s", 
//                                         filename, log_buffer);

//     /// obtain PTX from the program.
//     size_t ptx_size;
//     EZCU_NVRTC_CHECK(nvrtcGetPTXSize(nvrtc_prog, &ptx_size),
//                      "failed to get the PTX size from NVRTC program");
//     *ptx_buffer = (char*)malloc(ptx_size*sizeof(char));
//     EZCU_NVRTC_CHECK(nvrtcGetPTX(nvrtc_prog, *ptx_buffer), 
//                     "failed to extract PTX from NVRTC program");
    
//     /// destroy the nvrtc program.
//     EZCU_NVRTC_CHECK(nvrtcDestroyProgram(&nvrtc_prog), 
//                      "failed to destroy the NVRTC program");

//     /// save ptx to disk.
//     __ezcu_write_to_file(ptx_buffer, ptx_size, ptx_name);

//     /// free temporary buffers.
//     free(log_buffer);
//     for(i=0; i<num_ops; i++) free(opt_buffer[i]);
//     free(opt_buffer);
//     return ptx_size;
// }

///
/// @brief Find a ezCU kernel descriptor given a function pointer.
///
/// @param name is the CUDA function name.
/// @return a ezCU kernel descriptor.
///
__EZCU_PRIVATE __host__ ezcu_knl_t  
__ezcu_knl_find(const char *name) {
    // urb_t *i_knl = urb_tree_find(&ezcu->knls, (void*)name, __ezcu_str_cmp);
    // EZCU_EXIT_IF(i_knl == &urb_sentinel, 
    //              "CUDA kernel '%s' not found", name);
    // return (ezcu_knl_t)i_knl->value;
    // urb_t *i_knl = 
    return (ezcu_knl_t)urb_tree_find(&ezcu->knls, 
                                    (void*)name, __ezcu_str_cmp)->value;
}

///
/// @brief Create a ezCU kernel descriptor.
///
/// This routine creates a ezCU kernel descriptor, provided an CUDA cl_kernel,
/// which contains information about the work size and also the offset, see 
/// types.h. 
/// @param id is the CUDA cl_kernel.
/// @return a ezCU kernel descriptor.
///
__EZCU_PRIVATE __host__ void
__ezcu_knl_release(void *pointer) {
    ezcu_knl_t k = (ezcu_knl_t)pointer;
    if (k != NULL) {
        urb_tree_delete(&k->mems, __ezcu_ptr_del, __ezcu_mem_release);
        urb_tree_delete(&k->vals, __ezcu_ptr_del, __ezcu_ptr_del);
        free(k->args);
        free(k->args_types);
        free(k); k = NULL;
    }
}

///
/// @brief Macros used to print information about a given ezCU kernel.
///
#define __EZCU_KNL_INFO_LEVEL_0(fmt, ...) fprintf(ezcu->fdout,\
EZCU_GREEN"\no CUDA "fmt":\n"EZCU_END, ##__VA_ARGS__)

#define __EZCU_KNL_INFO_LEVEL_1(fmt, ...) fprintf(ezcu->fdout,\
"\to %-20s: "fmt"\n", ##__VA_ARGS__)

#define __EZCU_KNL_INFO_LEVEL_2(fmt, ...) fprintf(ezcu->fdout,\
"\t %-22s "fmt"\n", " ", ##__VA_ARGS__)

///
/// @brief Print information about a given ezCU kernel descriptor.
///
/// @param pointer is the kernel descriptor.
/// @return Nothing.
///
__EZCU_PRIVATE __host__ void
__ezcu_knl_info(void *pointer) {
    ezcu_knl_t k = (ezcu_knl_t)pointer;
    __EZCU_KNL_INFO_LEVEL_0("kernel {id=%p}", k->id);
    __EZCU_KNL_INFO_LEVEL_1("%u", "#args", k->num_args);
    __EZCU_KNL_INFO_LEVEL_1("%lu", "#memory objects", urb_tree_size(&k->mems));
}

__EZCU_PRIVATE __host__ void 
__ezcu_knl_set_char(ezcu_knl_t k, unsigned int idx, char *c) {
    EZCU_EXIT_IF(idx >= k->num_args, "invalid kernel argument index");
    urb_t *n;
    int   *ikey;
    char  *v;
    if ((n = urb_tree_find(&k->vals, &idx, __ezcu_int_cmp)) == &urb_sentinel) {
        ikey  = (int*)malloc(sizeof(int));
        v     = (char*)malloc(sizeof(char));
        *ikey = idx;
        *v    = *c;
        urb_tree_put(&k->vals, urb_tree_create(ikey, v), __ezcu_int_cmp);
        k->args[idx] = (void*)v;
        EZCU_DEBUG("knl set char: v=%c idx=%d", *(char*)k->args[idx], idx);
    } else {
        if (*c != *((char*)n->value)) {
            *((char*)n->value) = *c; 
            EZCU_DEBUG("knl modify char: v=%c idx=%d", *(char*)k->args[idx], idx);
        }
    }
}

__EZCU_PRIVATE __host__ void 
__ezcu_knl_set_int32(ezcu_knl_t k, unsigned int idx, int32_t *i32) {
    EZCU_EXIT_IF(idx >= k->num_args, "invalid kernel argument index");
    urb_t *n;
    int   *ikey;
    int   *v;
    if ((n = urb_tree_find(&k->vals, &idx, __ezcu_int_cmp)) == &urb_sentinel) {
        ikey  = (int*)malloc(sizeof(int));
        v     = (int*)malloc(sizeof(int));
        *ikey = idx;
        *v    = *i32;
        urb_tree_put(&k->vals, urb_tree_create(ikey, v), __ezcu_int_cmp);
        k->args[idx] = (void*)v;
        EZCU_DEBUG("knl set int32: v=%d idx=%d", *(int*)k->args[idx], idx);
    } else {
        if (*i32 != *((int*)n->value)) {
            *((int*)n->value) = *i32; 
            EZCU_DEBUG("knl modify int32: v=%d idx=%d", *(int*)k->args[idx], idx);
        }
    }
}

__EZCU_PRIVATE __host__ void 
__ezcu_knl_set_uint32(ezcu_knl_t k, unsigned int idx, uint32_t *i32) {
    EZCU_EXIT_IF(idx >= k->num_args, "invalid kernel argument index");
    urb_t        *n;
    int          *ikey;
    unsigned int *v;
    if ((n = urb_tree_find(&k->vals, &idx, __ezcu_int_cmp)) == &urb_sentinel) {
        ikey  = (int*)malloc(sizeof(int));
        v     = (unsigned int*)malloc(sizeof(unsigned int));
        *ikey = idx;
        *v    = *i32;
        urb_tree_put(&k->vals, urb_tree_create(ikey, v), __ezcu_int_cmp);
        k->args[idx] = (void*)v;
        EZCU_DEBUG("knl set uint32: v=%u idx=%d", 
                 *(unsigned int*)k->args[idx], idx);
    } else {
        if (*i32 != *((unsigned int*)n->value)) {
            *((unsigned int*)n->value) = *i32; 
            EZCU_DEBUG("knl modify uint32: v=%u idx=%d", 
                     *(unsigned int*)k->args[idx], idx);
        }
    }
}

__EZCU_PRIVATE __host__ void 
__ezcu_knl_set_int64(ezcu_knl_t k, unsigned int idx, int64_t *i64) {
    EZCU_EXIT_IF(idx >= k->num_args, "invalid kernel argument index");
    urb_t   *n;
    int     *ikey;
    int64_t *v;
    if ((n = urb_tree_find(&k->vals, &idx, __ezcu_int_cmp)) == &urb_sentinel) {
        ikey  = (int*)malloc(sizeof(int));
        v     = (int64_t*)malloc(sizeof(int64_t));
        *ikey = idx;
        *v    = *i64;
        urb_tree_put(&k->vals, urb_tree_create(ikey, v), __ezcu_int_cmp);
        k->args[idx] = (void*)v;
        EZCU_DEBUG("knl set int64: v=%ld idx=%d", *(int64_t*)k->args[idx], idx);
    } else {
        if (*i64 != *((int64_t*)n->value)) {
            *((int64_t*)n->value) = *i64; 
            EZCU_DEBUG("knl modify int64: v=%ld idx=%d", 
                     *(int64_t*)k->args[idx], idx);
        }
    }
}

__EZCU_PRIVATE __host__ void 
__ezcu_knl_set_uint64(ezcu_knl_t k, unsigned int idx, uint64_t *i64) {
    EZCU_EXIT_IF(idx >= k->num_args, "invalid kernel argument index");
    urb_t    *n;
    int      *ikey;
    uint64_t *v;
    if ((n = urb_tree_find(&k->vals, &idx, __ezcu_int_cmp)) == &urb_sentinel) {
        ikey  = (int*)malloc(sizeof(int));
        v     = (uint64_t*)malloc(sizeof(uint64_t));
        *ikey = idx;
        *v    = *i64;
        urb_tree_put(&k->vals, urb_tree_create(ikey, v), __ezcu_int_cmp);
        k->args[idx] = (void*)v;
        EZCU_DEBUG("knl set uint64: v=%lu idx=%d", 
                 *(uint64_t*)k->args[idx], idx);
    } else {
        if (*i64 != *((uint64_t*)n->value)) {
            *((uint64_t*)n->value) = *i64; 
            EZCU_DEBUG("knl modify uint64: v=%lu idx=%d", 
                     *(uint64_t*)k->args[idx], idx);
        }
    }
}

__EZCU_PRIVATE __host__ void 
__ezcu_knl_set_float(ezcu_knl_t k, unsigned int idx, float *f) {
    EZCU_EXIT_IF(idx >= k->num_args, "invalid kernel argument index");
    urb_t *n;
    int   *ikey;
    float *v;
    if ((n = urb_tree_find(&k->vals, &idx, __ezcu_int_cmp)) == &urb_sentinel) {
        ikey  = (int*)malloc(sizeof(int));
        v     = (float*)malloc(sizeof(float));
        *ikey = idx;
        *v    = *f;
        urb_tree_put(&k->vals, urb_tree_create(ikey, v), __ezcu_int_cmp);
        k->args[idx] = (void*)v;
        EZCU_DEBUG("knl set float: v=%f idx=%d", *(float*)k->args[idx], idx);
    } else {
        if (*f != *((float*)n->value)) {
            *((float*)n->value) = *f; 
            EZCU_DEBUG("knl modify float: v=%f idx=%d", 
                     *(float*)k->args[idx], idx);
        }
    }
}

__EZCU_PRIVATE __host__ void 
__ezcu_knl_set_double(ezcu_knl_t k, unsigned int idx, double *d) {
    EZCU_EXIT_IF(idx >= k->num_args, "invalid kernel argument index");
    urb_t  *n;
    int    *ikey;
    double *v;
    if ((n = urb_tree_find(&k->vals, &idx, __ezcu_int_cmp)) == &urb_sentinel) {
        ikey  = (int*)malloc(sizeof(int));
        v     = (double*)malloc(sizeof(double));
        *ikey = idx;
        *v    = *d;
        urb_tree_put(&k->vals, urb_tree_create(ikey, v), __ezcu_int_cmp);
        k->args[idx] = (void*)v;
        EZCU_DEBUG("knl set double: v=%f idx=%d", *(double*)k->args[idx], idx);
    } else {
        if (*d != *((double*)n->value)) {
            *((double*)n->value) = *d; 
            EZCU_DEBUG("knl modify double: v=%f idx=%d", 
                     *(double*)k->args[idx], idx);
        }
    }
}

__EZCU_PRIVATE __host__ void 
__ezcu_knl_set_mem(ezcu_knl_t k, unsigned int idx, void *h) {
    urb_t *n, *i;
    int *ikey;
    ezcu_mem_t m;
    EZCU_EXIT_IF(idx >= k->num_args, "invalid kernel argument index");
    EZCU_EXIT_IF((i = urb_tree_find(&ezcu->mems, h, __ezcu_ptr_cmp)) == 
                  &urb_sentinel,
                 "memory buffer is unknown, check if it is already wrapped");
    m = (ezcu_mem_t)i->value;
    EZCU_DEBUG("start knl set mem: func=%p", k->id);
    if ((n = urb_tree_find(&k->mems, &idx, __ezcu_int_cmp)) == &urb_sentinel) {
        ikey  = (int*)malloc(sizeof(int));
        *ikey = idx;
        urb_tree_put(&k->mems, urb_tree_create(ikey, m), __ezcu_int_cmp);
        m->refs++;
        k->args[idx] = &m->id;
        EZCU_DEBUG("mem insert {h=%p, id=%llu, refs = %d} for kernel %p, idx=%d", 
                    m->h, m->id, m->refs, k->id, idx);
    } else {
        if (m != (ezcu_mem_t)n->value) {
            ((ezcu_mem_t)n->value)->refs = ((ezcu_mem_t)n->value)->refs-1;
            m->refs++;
            EZCU_DEBUG("mem modify {h=%p, id=%llu, refs = %d} to"
                       " {h=%p, id=%llu, refs = %d} for kernel %p, idx=%d", 
                        ((ezcu_mem_t)n->value)->h, 
                        ((ezcu_mem_t)n->value)->id, 
                        ((ezcu_mem_t)n->value)->refs, 
                        m->h, m->id, m->refs, k->id, idx);
            n->value = m; 
            k->args[idx] = &m->id;
        }
    }
    EZCU_DEBUG("end   knl set mem: v=%p (%llu)", k->args[idx], m->id);
    EZCU_DEBUG("");
}

__EZCU_PRIVATE __host__ void
__ezcu_knl_set_args_valist(ezcu_knl_t k, va_list list) {
    unsigned int a;
    char c;
    void *p;
    int i;
    unsigned int ui;
    long l;
    unsigned long ul;
    float f;
    double d;
    size_t st;
    for (a = 0; a < k->num_args; ++a) {
        switch (k->args_types[a]) {
        case POINTER:
            p = va_arg(list, void*);
            __ezcu_knl_set_mem(k, a, p);
            break;
        case FLOAT:
            f = va_arg(list, double);
            __ezcu_knl_set_float(k, a, &f);
            break;
        case DOUBLE:
            d = va_arg(list, double);
            __ezcu_knl_set_double(k, a, &d);
            break;
        case CHAR:
            c = va_arg(list, int);
            __ezcu_knl_set_char(k, a, &c);
            break;
        case INT:
            i = va_arg(list, int);
            __ezcu_knl_set_int32(k, a, &i);
            break;
        case UNSIGNED_INT:
            ui = va_arg(list, unsigned int);
            __ezcu_knl_set_uint32(k, a, &ui);
            break;
        case LONG:
            l = va_arg(list, long);
            __ezcu_knl_set_int64(k, a, &l);
            break;
        case UNSIGNED_LONG:
            ul = va_arg(list, unsigned long);
            __ezcu_knl_set_uint64(k, a, &ul);
            break;
        case SIZET:
            st = va_arg(list, size_t);
            __ezcu_knl_set_uint64(k, a, &st);
            break;
        default:
            EZCU_EXIT("unknown CUDA kernel argument type");
        }
    }
}

CPPGUARD_END();

#endif  // __EZCU_KNL_INL_H_
