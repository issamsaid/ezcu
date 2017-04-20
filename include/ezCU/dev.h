#ifndef __EZCU_DEV_H_
#define __EZCU_DEV_H_
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
/// 3. Neither the name of the copyright holder nor the names of its contributors
///    may be used to endorse or promote products derived from this software
///    without specific prior written permission.
///
/// THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
/// INCLUDING, BUT NOT LIMITED TO, WARRANTIES OF MERCHANTABILITY AND FITNESS
/// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE copyright holder OR
/// ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
/// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
/// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
/// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
/// LIABILITY, WETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
/// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
/// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///
/// @file ezCU/dev.h
/// @author Issam SAID
/// @brief Function prototypes of CUDA devices manipulation routines.
///
/// @details This file describes the library functions used by ezCU to 
/// initialize, release and manipulate an CUDA device descriptor.
///
#include <stdbool.h>
#include <ezCU/types.h>

CPPGUARD_BEGIN()

///
/// @brief Select a device descriptor from those initialized by ezCU.
///
/// This routine selects a ezCU device, based on bitwise flags, from a list of 
/// the CUDA devices initially created when launching the ezCU library.
/// @param flags the bitwise flags provided by the user.
/// @return The device descriptor corresponding to the user request.
///
ezcu_dev_t  ezcu_dev_find(ezcu_flags_t flags);

///
/// @brief Wait until a given device finishes all its operations.
///
/// This routine is used to synchronize with the command queue related to a 
/// ezCU device descriptor.
/// @param d the ezCU device descriptor.
/// @return Nothing.
///
void ezcu_dev_wait(ezcu_dev_t d);

///
/// @brief Check whether a given device supports an extension.
///
/// This routine query a given CUDA device extensions string and checks if 
/// it contains a user provided extension.
/// @param d the ezCU device descriptor.
/// @param extension is the string describing the CUDA extension.
/// @return A boolean to inform if the extension is supported.
///
bool ezcu_dev_support(ezcu_dev_t d, char* extension);

CPPGUARD_END()

#endif  // __EZCU_DEV_H_
