#ifndef __EZCU_ERROR_INL_H_
#define __EZCU_ERROR_INL_H_
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
/// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE copyright 
/// holder OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
/// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
/// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
/// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
/// LIABILITY, WETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
/// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
/// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///
/// @file __ezcu/error-inl.h
/// @author Issam SAID
/// @brief Private functions used to handle CUDA errors in ezcu.    
///
#include <stdio.h>
#include <ezcu/types.h>
#include <__ezcu/types-inl.h>
#include <__ezcu/config/guard.h>
#include <__ezcu/config/private.h>
#include <__ezcu/config/log.h>
#include <__ezcu/config/cuda.h>

CPPGUARD_BEGIN();

#define __STR(CONSTANT) #CONSTANT

extern ezcu_env_t ezcu;

extern void ezcu_release();

#define EZCU_FAIL(fmt,...)                                          \
  do{                                                               \
    fprintf(stderr, "%s[EZCU FATAL]: " __STR(fmt) " @%s:%d.\n%s",   \
            EZCU_RED, ##__VA_ARGS__, __FILE__, __LINE__, EZCU_END); \
    exit(EXIT_FAILURE);                                             \
  } while(0)

#define EZCU_FAIL_IF(predicate, fmt,...)                              \
  do{                                                                 \
    if (predicate) {                                                  \
      fprintf(stderr, "%s[EZCU FATAL]: " __STR(fmt) " @%s:%d.\n%s",   \
              EZCU_RED, ##__VA_ARGS__, __FILE__, __LINE__, EZCU_END); \
      exit(EXIT_FAILURE);                                             \
    }                                                                 \
  } while(0)

#define EZCU_ABORT(status, fmt,...)                                  \
  do {                                                               \
    if(status != CUDA_SUCCESS) {                                     \
      char __tmp_0[__EZCU_STR_SIZE];                                 \
      char __tmp_1[__EZCU_BUFFER_SIZE];                              \
      cuGetErrorName(status, (const char**)&__tmp_0);                \
      cuGetErrorString(status, (const char**)&__tmp_1);              \
      fprintf(stderr,                                                \
              "%s[EZCU ABORT(%s)]: " __STR(fmt) " (%s) @%s:%d.\n%s", \
              EZCU_RED, __tmp_0,                                     \
              ##__VA_ARGS__, __tmp_1, __FILE__, __LINE__, EZCU_END); \
      exit(EXIT_FAILURE);                                            \
    }                                                                \
  } while(0)

#define EZCU_EXIT(fmt,...)                                           \
  do {                                                               \
    fprintf(ezcu->fderr, "%s[EZCU ERR]: " __STR(fmt) " @%s:%d.\n%s", \
            EZCU_RED, ##__VA_ARGS__, __FILE__, __LINE__, EZCU_END);  \
    ezcu_release();                                                  \
    exit(EXIT_FAILURE);                                              \
  } while(0)

#define EZCU_EXIT_IF(predicate, fmt,...)                               \
  do {                                                                 \
    if (predicate) {                                                   \
      fprintf(ezcu->fderr, "%s[EZCU ERR]: " __STR(fmt) " @%s:%d.\n%s", \
              EZCU_RED, ##__VA_ARGS__, __FILE__, __LINE__,EZCU_END);   \
      ezcu_release();                                                  \
      exit(EXIT_FAILURE);                                              \
    }                                                                  \
  } while(0)

#define EZCU_CHECK(status, fmt,...)                                  \
  do {                                                               \
    if(status != CUDA_SUCCESS) {                                     \
      char* __tmp_0;                                                 \
      char* __tmp_1;                                                 \
      cuGetErrorName(status, (const char**)&__tmp_0);                \
      cuGetErrorString(status, (const char**)&__tmp_1);              \
      fprintf(ezcu->fderr,                                           \
              "%s[EZCU ERR(%s)]: " __STR(fmt) " (%s) @%s:%d.\n%s",   \
              EZCU_RED, __tmp_0,                                     \
              ##__VA_ARGS__, __tmp_1, __FILE__, __LINE__, EZCU_END); \
      ezcu_release();                                                \
      exit(EXIT_FAILURE);                                            \
    }                                                                \
  } while(0)

#define EZCU_NVRTC_CHECK(status, fmt,...)                                    \
  do {                                                                       \
    if(status != NVRTC_SUCCESS) {                                            \
      const char* __tmp = nvrtcGetErrorString(status);                       \
      fprintf(ezcu->fderr,                                                   \
              "%s[EZCU ERR(%s)]: " __STR(fmt) " @%s:%d.\n%s",                \
              EZCU_RED, __tmp, ##__VA_ARGS__, __FILE__, __LINE__, EZCU_END); \
      ezcu_release();                                                        \
      exit(EXIT_FAILURE);                                                    \
    }                                                                        \
  } while(0)

CPPGUARD_END();

#endif  // __EZCU_ERROR_INL_H_