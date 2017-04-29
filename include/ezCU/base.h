#ifndef __EZCU_BASE_H_
#define __EZCU_BASE_H_
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
/// @file ezCU/base.h
/// @author Issam SAID
/// @brief Function prototypes of the main routines to setup ezCU.
/// 
/// @details This file contains the main ezCU routines to setup a CUDA
/// environment which includes the desired devices and the 
/// CUDA kernels source code.
///
#include <ezCU/types.h>
#include <ezCU/flags.h>
#include <__ezcu/config/guard.h>

CPPGUARD_BEGIN()

/// 
/// @brief Initialize the CUDA environment. 
///
/// This routine selects the CUDA platform and the devices indicated
/// by the user, with the help of bitwise flags, and wraps them into an
/// CUDA context. Flags are used by ezCU to select the platform vendor
/// (AMD, APPLE, INTEL or NVIDIA) or to select the device type (CPU, GPU,
/// ACCELERATOR).
/// @see flags.h for more information.
/// @param flags is the indication used to select the appropriate hardware.
/// @return Nothing.
///
void ezcu_init(ezcu_flags_t flags);

///
/// @brief Release the CUDA resources and quits the ezCU library.
///
/// This routine releases the CUDA memory objects, kernels and command
/// queues that are allocated by the user when running ezCU.
/// @return Nothing. 
///
void ezcu_release();

//
/// @brief Parse a '.cu' file and to create CUDA kernels.
///
/// This routine loads a source code from a given file (with a '.cu'
/// extention) in order to create the CUDA kernels contained by this file.
/// @param filename is the path to the kernels file.
/// @param options are the nvcc options to compile the kernels.
/// @return Nothing.
///
void ezcu_load(const char *filename, const char *options_format, ...);

///
/// @brief Shows information about the used CUDA resources.
///
/// This routine prints information about the CUDA platform,
/// the CUDA devices, the CUDA kernels and the CUDA memory
/// objects allocated by the user when running ezCU.
/// @return Nothing.
///
void ezcu_info();

CPPGUARD_END()

#endif  // __EZCU_BASE_H_
