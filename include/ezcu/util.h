#ifndef __EZCU_UTIL_H_
#define __EZCU_UTIL_H_
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
/// @file ezcu/util.h
/// @author Issam SAID
/// @brief Function prototypes of set of utilities used by ezcu.
/// 
/// @details this file contains a list of helpers and util functions provided 
/// by the ezcu library.
///
#include <stdbool.h>
#include <ezcu/flags.h>
#include <ezcu/types.h>
#include <__ezcu/config/guard.h>

CPPGUARD_BEGIN();

///
/// @brief Convert a string to ezcu resources flags.
///
/// This routine takes as an input a string and converts it to ezcu flags that 
/// are used to pick and allocate CUDA resources. This routine is useful when
/// parsing command line argument of a program using ezcu.
/// @param str is the input string provided by the user.
/// @return the corresponding ezcu flags.
///
ezcu_flags_t ezcu_str2flags(const char *str);

///
/// @brief Return the count of CUDA devices loaded by ezcu.
///
/// This routine returns mainly the number of CUDA devices that are activated
/// by ezcu.
/// @return The number of the corresponding activated devices.
///
size_t ezcu_count();

CPPGUARD_END();

#endif  // __EZCU_UTIL_H_
