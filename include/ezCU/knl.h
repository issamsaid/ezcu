#ifndef __EZCU_KNL_H_
#define __EZCU_KNL_H_
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
/// @file ezcu/knl.h
/// @author Issam SAID
/// @brief Function prototypes of CUDA devices manipulation routines.
///
/// @details This file describes the library functions used by ezcu to 
/// initialize, release and manipulate an CUDA kernel descriptor.
///
#include <stdint.h>
#include <ezcu/types.h>
#include <ezcu/flags.h>

CPPGUARD_BEGIN();

///
/// @brief Set a char value for a specific kernel argument.
///
/// This routine sets the value of a kernel argument to a char 
/// of a given CUDA kernel (identified by a ezcu kernel descriptor).
/// @param name is the string that identifies the kernel descriptor.
/// @param idx is the position of the argument in the list 
/// @return Nothing.
///
void ezcu_knl_set_char(const char *name, int idx, char c);

///
/// @brief Set an integer (32 bits) value for a specific kernel argument.
///
/// This routine sets the value of a kernel argument to an integer (32 bits)
/// of a given CUDA kernel (identified by a ezcu kernel descriptor).
/// @param name is the string that identifies the kernel descriptor.
/// @param idx is the position of the argument in the list 
/// @return Nothing.
///
void    ezcu_knl_set_int32(const char *name, int idx, int32_t i32);

///
/// @brief Set an unsigned (32 bits) value for a specific kernel argument.
///
/// This routine sets the value of a kernel argument to an unsigned (32 bits)
/// of a given CUDA kernel (identified by a ezcu kernel descriptor).
/// @param name is the string that identifies the kernel descriptor.
/// @param idx is the position of the argument in the list 
/// @return Nothing.
///
void    ezcu_knl_set_uint32(const char *name, int idx, uint32_t i32);

///
/// @brief Set an integer (64 bits) value for a specific kernel argument.
///
/// This routine sets the value of a kernel argument to an integer (64 bits)
/// of a given CUDA kernel (identified by a ezcu kernel descriptor).
/// @param name is the string that identifies the kernel descriptor.
/// @param idx is the position of the argument in the list 
/// @return Nothing.
///
void    ezcu_knl_set_int64(const char *name, int idx, int64_t i64);

///
/// @brief Set an unsigned (64 bits) value for a specific kernel argument.
///
/// This routine sets the value of a kernel argument to an unsigned (64 bits)
/// of a given CUDA kernel (identified by a ezcu kernel descriptor).
/// @param name is the string that identifies the kernel descriptor.
/// @param idx is the position of the argument in the list 
/// @return Nothing.
///
void    ezcu_knl_set_uint64(const char *name, int idx, uint64_t i64);

///
/// @brief Set a float value for a specific kernel argument.
///
/// This routine sets the value of a kernel argument to a float
/// of a given CUDA kernel (identified by a ezcu kernel descriptor).
/// @param name is the string that identifies the kernel descriptor.
/// @param idx is the position of the argument in the list 
/// @return Nothing.
///
void    ezcu_knl_set_float(const char *name, int idx, float f);

///
/// @brief Set a double value for a specific kernel argument.
///
/// This routine sets the value of a kernel argument to a double
/// of a given CUDA kernel (identified by a ezcu kernel descriptor).
/// @param name is the string that identifies the kernel descriptor.
/// @param idx is the position of the argument in the list 
/// @return Nothing.
///
void    ezcu_knl_set_double(const char *name, int idx, double d);

///
/// @brief Set a memory address for a specific kernel argument.
///
/// This routine sets the value of a kernel argument to a memory address
/// of a given CUDA kernel (identified by a ezcu kernel descriptor). 
/// @param name is the string that identifies the kernel descriptor.
/// @param idx is the position of the argument in the list 
/// @return Nothing.
///
void    ezcu_knl_set_mem(const char *name, int idx, void *h);

///
/// @brief Set a list of arguments for a given CUDA kernel.
///
/// This routine uses the C variadic functions to set a collection of
/// arguments for a given CUDA kernel all at once.
/// @param name is the string that identifies the kernel descriptor.
/// @param ... are the kernel arguments.
/// @return Nothing.
///
void    ezcu_knl_set_args(const char *name, ...);

///
/// @brief Set the amount of shared memory the CUDA kernel will use.
///
/// @param name is the string that identifies the kernel descriptor.
/// @param bytes the amount of memory in bytes to be allocated.
/// @return Nothing.
///
void    ezcu_knl_set_shared(const char *name, unsigned int bytes);

///
/// @brief Set the work sizes and dimension for a given CUDA kernel.
///
/// This routine sets the work size (the global size and the local size) and 
/// the thread blocks dimensions for a given CUDA kernel.
/// @param name is the string that identifies the kernel descriptor.
/// @param wrk is the work dimension of the kernel.
/// @param gws is and array of dimension 'wrk' that contains the global sizes.
/// @param lws is and array of dimension 'wrk' that contains the local sizes.
/// @return Nothing.
///
void    ezcu_knl_set_wrk(const char *name, unsigned int wrk, 
                         unsigned int *grid, unsigned int *block);

///
/// @brief Run (asynchronously) a given CUDA kernel after synchronizing its 
///        related memory objects between the host and a given device.
///
/// This routine ensures the integrity, between the host and a 
/// given CUDA device, of the memory objects being manipulated by 
/// given CUDA kernel, and runs the latter on that same device. The
/// execution is asynchronous which is the default behavior in ezcu.
/// @param name is the string that identifies the kernel descriptor.
/// @param d is the device descriptor of the CUDA device to run the kernel
///        on.
/// @param ... is the list of arguments to pass to the kernel.
/// @return Nothing.
///
void    ezcu_knl_run(const char *name, ezcu_dev_t d, ...);
void    ezcu_knl_exec(const char *name, ezcu_dev_t d);

///
/// @brief Run (synchronously) a given CUDA kernel after synchronizing its 
///        related memory objects between the host and a given device.
///
/// This routine ensures the integrity, between the host and a 
/// given CUDA device, of the memory objects being manipulated by 
/// given CUDA kernel, and runs the latter on that same device. Finally
/// it flushes the command queue relqted to the device.
/// @param name is the string that identifies the kernel descriptor.
/// @param d is the device descriptor of the CUDA device to run the kernel
///        on.
/// @param ... is the list of arguments to pass to the kernel.
/// @return Nothing.
///
void    ezcu_knl_sync_run(const char *name, ezcu_dev_t d, ...);
void    ezcu_knl_sync_exec(const char *name, ezcu_dev_t d);

///
/// @brief Run (and time the execution) a given CUDA kernel after 
///        synchronizing its related memory objects between the 
///        host and a given device.
///
/// This routine ensures the integrity, between the host and a 
/// given CUDA device, of the memory objects being manipulated by 
/// given CUDA kernel, and runs the latter on that same device while timing
/// the execution.
/// @param name is the string that identifies the kernel descriptor.
/// @param d is the device descriptor of the CUDA device to run the kernel
///        on.
/// @param ... is the list of arguments to pass to the kernel.
/// @return Nothing.
///
double  ezcu_knl_timed_run(const char *name, ezcu_dev_t d, ...);
double  ezcu_knl_timed_exec(const char *name, ezcu_dev_t d);

CPPGUARD_END();

#endif  // __EZCU_KNL_H_
