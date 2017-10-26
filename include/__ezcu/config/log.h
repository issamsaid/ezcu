#ifndef __EZCU_CONFIG_LOG_H_
#define __EZCU_CONFIG_LOG_H_
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
/// @file config/log.h
/// @author Issam SAID
/// @brief Configuration file for logging in ezcu.
///

///
/// @brief Use tty as the output for logging, otherwise use files.
///
#ifdef __LOG_STD
#endif // __LOG_STD

///
/// @brief Set the verbose mode of ezcu up.
///
#ifdef __VERBOSE
#endif // __VERBOSE          

///
/// @brief Set the debug mode of ezcu up.
///
#ifdef __DEBUG

#ifndef __VERBOSE
#define __VERBOSE
#endif // __VERBOSE         

#ifndef __LOG_STD
#define __LOG_STD
#endif // __LOG_STD 

#endif // __DEBUG           

///
/// @brief Terminal colors (only available if out/err to tty).
///
#ifdef __LOG_STD
#define EZCU_GREEN  "\x1B[32m"
#define EZCU_PURPLE "\x1B[35m"
#define EZCU_YELLOW "\x1B[33m"
#define EZCU_RED    "\x1B[31m"
#define EZCU_END    "\x1B[0m"
#else
#define EZCU_GREEN  ""
#define EZCU_PURPLE ""
#define EZCU_YELLOW ""
#define EZCU_RED    ""
#define EZCU_END    ""
#endif  // __LOG_STD

#endif  // __EZCU_CONFIG_LOG_H_
