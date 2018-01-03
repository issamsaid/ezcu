#ifndef __EZCU_FLAGS_H_
#define __EZCU_FLAGS_H_
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
/// @file ezcu/flags.h
/// @author Issam SAID
/// @brief Bitwise flags used by ezcu to select CUDA resources.
///
/// @details This file describes the bitwise flags that users can use to 
/// select CUDA resources (devices, memory objects) or to determine the 
/// ezcu library behavior.
///
#include <stdio.h>
#include <stdint.h>
#include <__ezcu/config/guard.h>

CPPGUARD_BEGIN()

///
/// @brief Define a bit-field type to be used to select ezcu resources.
///
typedef uint64_t ezcu_flags_t;

/// @brief The ezcu default flag (orthogonal).
#define DEFAULT 0x1000000000000000 ///< The ezcu default behavior.  

///
/// Memory manipulation flags.
///
///
/// @brief Memory location flags (orthogonal).
typedef enum {
  HOST = 0x0100000000000000, ///< Store memory on the host.
  HWA  = 0x0200000000000001  ///< Store memory on the device.
} ezcu_mem_locs_flags_t;

/// @brief Memory locations mask.
#define EZCU_FLAGS_MEM_LOCS_MASK ((HWA | HOST) & 0x0F00000000000000)

/// @brief Memory properties flags (inclusive).
typedef enum {
  PINNED    = 0x0010000000000000, ///< Use pinned memory.
  ZERO_COPY = 0x0020000000000001  ///< Make the memory zero-copy.
} ezcu_mem_props_flags_t;

/// @brief Memory properties mask.
#define EZCU_FLAGS_MEM_PROPS_MASK ((PINNED | ZERO_COPY) & 0x00F0000000000000)

/// @brief Memory access mode flags (orthogonal).
typedef enum {
  READ_ONLY  = 0x0001000000000000, ///< Use read-only data.  
  WRITE_ONLY = 0x0002000000000001, ///< Use write-only data.  
  READ_WRITE = 0x0004000000000002  ///< Use read-write data.  
} ezcu_mem_access_flags_t;

/// @brief Memory access modes mask.
#define EZCU_FLAGS_MEM_ACCESS_MODES_MASK \
  ((READ_WRITE | WRITE_ONLY | READ_ONLY) & 0x000F000000000000)

/// @brief Memory data-types flags (orthogonal).
typedef enum {
  CHAR          = 0x0000010000000000, ///< Data contain chars.  
  INT           = 0x0000020000000001, ///< Data contain integers.  
  UNSIGNED_INT  = 0x0000040000000002, ///< Data contain unsigned ints.  
  FLOAT         = 0x0000080000000003, ///< Data contain floats.  
  LONG          = 0x0000100000000004, ///< Data contain long ints.  
  UNSIGNED_LONG = 0x0000200000000005, ///< Data contain unsigned longs.  
  SIZET         = 0x0000400000000006, ///< Data contain size_ts.  
  DOUBLE        = 0x0000800000000007, ///< Data contain doubles.  
  POINTER       = 0x0000001000000008, ///< Data is a pointer.  
  SHORT         = 0x0000002000000009, ///< Data contain shorts.  
} ezcu_mem_types_flags_t;

/// @brief Memory data-types mask.
#define EZCU_FLAGS_MEM_TYPES_MASK            \
  ((CHAR          | INT    |                 \
    UNSIGNED_INT  | LONG   | UNSIGNED_LONG | \
    SIZET | FLOAT | DOUBLE | SHORT) & 0x0000FFF000000000)

/// @brief Extract the internal values from flags.
#define EZCU_VAL_MASK 0x000000000000FF00

/// @brief Extract the internal indexes from flags.
#define EZCU_LKP_MASK 0x00000000000000FF

/// @brief Macro to check whether a set of flags contains a given mask.
#define EZCU_FLAGS_HAVE(flags, mask) \
  (((flags & 0xFFFFFFFF00000000) &   \
    (mask & 0xFFFFFFFF00000000)) == (mask & 0xFFFFFFFF00000000))

///
/// @brief Print a help message about the default ezcu flag.
///
/// @param None.
/// @return Nothing.
///
void ezcu_flags_help_default();

///
/// @brief Print a help message about the memory locations ezcu flags.
///
/// @param None.
/// @return Nothing.
///
void ezcu_flags_help_mem_locations();

///
/// @brief Print a help message about the memory properties ezcu flags.
///
/// @param None.
/// @return Nothing.
///
void ezcu_flags_help_mem_props();

///
/// @brief Print a help message about the memory acces modes ezcu flags.
///
/// @param None.
/// @return Nothing.
///
void ezcu_flags_help_mem_access_modes();

///
/// @brief Print a help message about the memory types ezcu flags.
///
/// @param None.
/// @return Nothing.
///
void ezcu_flags_help_mem_types();

///
/// @brief Print a help message about the memory objects ezcu flags.
///
/// @param None.
/// @return Nothing.
///
void ezcu_flags_help_mem();

///
/// @brief Print a help message about all the possible ezcu flags.
///
/// @param None.
/// @return Nothing.
///
void ezcu_flags_help();

///
/// @brief Make sure that the DEFAULT flag is orthogonal.
///
/// @param flags are the user defined flags.
/// @return Nothing.
///
void ezcu_flags_check_default(ezcu_flags_t flags);

///
/// @brief Make sure that the memory locations flags are valid.
///
/// @param flags are the user defined flags.
/// @return Nothing.
///
void ezcu_flags_check_mem_locations(ezcu_flags_t flags);

///
/// @brief Make sure that the memory properties flags are valid.
///
/// @param flags are the user defined flags.
/// @return Nothing.
///
void ezcu_flags_check_mem_props(ezcu_flags_t flags);

///
/// @brief Make sure that the memory access modes flags are valid.
///
/// @param flags are the user defined flags.
/// @return Nothing.
///
void ezcu_flags_check_mem_access_modes(ezcu_flags_t flags);

///
/// @brief Make sure that the memory types flags are valid.
///
/// @param flags are the user defined flags.
/// @return Nothing.
///
void ezcu_flags_check_mem_types(ezcu_flags_t flags);

///
/// @brief Make sure that the memory flags are valid.
///
/// @param flags are the user defined flags.
/// @return Nothing.
///
void ezcu_flags_check_mem(ezcu_flags_t flags);

///
/// @brief Return the string that represents memory flags.
///
/// @param flags are the user defined flags.
/// @return Nothing.
///
void ezcu_flags_mem_to_str(ezcu_flags_t flags, char *string);

///
/// @brief Return the memory unit size based on a user defined flag.
///
/// @param flags are the user defined flags.
/// @return An integer that represents the memory unit size.
///
size_t ezcu_flags_to_mem_unit_size(ezcu_mem_types_flags_t flags);

///
/// @brief Make sure that the flags are valid.
///
/// @param flags are the user defined flags.
/// @return Nothing.
///
void ezcu_flags_check(ezcu_flags_t flags);

CPPGUARD_END()

#endif  // __EZCU_FLAGS_H_
