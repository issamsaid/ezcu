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
/// @file ezCU/flags.h
/// @author Issam SAID
/// @brief Bitwise flags used by ezCU to select CUDA resources.
///
/// @details This file describes the bitwise flags that users can use to 
/// select CUDA resources (devices, memory objects) or to determine the 
/// ezCU library behavior.
///
#include <stdio.h>
#include <stdint.h>
#include <__ezcu/config/guard.h>

CPPGUARD_BEGIN()

///
/// @brief Define a bitfield type to be used to select ezCU resources.
///
typedef uint64_t ezcu_flags_t;

/// @brief The ezCU default flag (orthogonal).
#define DEFAULT 0x1000000000000000 ///< The ezCU default behavior.  

///
/// Device selection flags.
///
///
/// @brief Vendor flags (orthogonal).
typedef enum {
    AMD     = 0x0800000000000000, ///< AMD devices.
    APPLE   = 0x0400000000000001, ///< APPLE devices.
    INTEL   = 0x0200000000000002, ///< INTEL devices.
    NVIDIA  = 0x0100000000000003  ///< NVIDIA platform.
} ezcu_vendor_flags_t;

/// @brief Device vendors mask.
#define EZCU_FLAGS_VENDORS_MASK \
    ((AMD | APPLE | NVIDIA | INTEL) & 0x0F00000000000000)

/// @brief The number of the device vendors.
#define EZCU_NB_VENDORS 4

/// @brief Device types flags (hierarchical, inclusive).
/// ALL:         can be anything.
/// ACCELERATOR: can be a GPU.
typedef enum { 
    ALL         = 0x00F0000000000000,  ///< All devices in list.
    CPU         = 0x0080000000000001,  ///< CPU devices in list.
    ACCELERATOR = 0x0070000000000002,  ///< Accelerators in list.
    GPU         = 0x0040000000000003   ///< GPU devices in list.
} ezcu_dev_type_flags_t;

/// @brief Device types mask.
#define EZCU_FLAGS_DEV_TYPES_MASK \
    ((ALL | CPU | ACCELERATOR | GPU) & 0x00F0000000000000)

/// @brief The number of device types.
#define EZCU_NB_DEV_TYPES 4

/// @brief Device indexes flags (inclusive).
typedef enum {
    FIRST       = 0x0000100000000000, ///< The 1st device in list.
    SECOND      = 0x0000200000000101, ///< The 2nd device in list.
    THIRD       = 0x0000400000000202, ///< The 3rd device in list.
    FOURTH      = 0x0000800000000303, ///< The 4th device in list.
    FIFTH       = 0x0001000000000404, ///< The 5th device in list.
    SIXTH       = 0x0002000000000505, ///< The 6th device in list.
    SEVENTH     = 0x0004000000000606, ///< The 7th device in list.
    EIGHTH      = 0x0008000000000707  ///< The 8th device in list.
} ezcu_dev_index_flags_t;

/// @brief Device indexes mask.
#define EZCU_FLAGS_DEV_INDEXES_MASK       \
    ((FIRST | SECOND | THIRD   | FOURTH | \
      FIFTH | SIXTH  | SEVENTH | EIGHTH) & 0x000FF00000000000)

/// @brief The number of device types.
#define EZCU_NB_DEV_INDEXES 8

/// @brief Devices properties flags (hierarchal, inclusive).
/// CC60 supports CC50 - CC20
/// CC50 supports CC35 - CC20
/// CC35 supports CC30 - CC20
/// CC30 supports CC20
/// NB: those are for GPUs so we set the GPU bit as well .
typedef enum {
    CC20    = 0x004000F000002000, ///< Compute capability 2.0.  
    CC30    = 0x004001F000003001, ///< Compute capability 3.0.
    CC35    = 0x004003F000003502, ///< Compute capability 3.5.
    CC50    = 0x004007F000005003, ///< Compute capability 5.0.
    CC60    = 0x00400FF000006004  ///< Compute capability 6.0.        
    /// reserved              
    /// reserved              
    /// reserved              
} ezcu_dev_prop_flags_t;

/// @brief Device properties mask.
#define EZCU_FLAGS_DEV_PROPS_MASK \
    ((CC20 | CC30 | CC35 | CC50 | CC60) & 0x00F00FF000000000)
                          
/// @brief The number of device props.
#define EZCU_NB_DEV_PROPS 5

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
#define EZCU_FLAGS_MEM_TYPES_MASK \
    ((CHAR  | INT   | UNSIGNED_INT | LONG | UNSIGNED_LONG | \
      SIZET | FLOAT | DOUBLE | SHORT) & 0x0000FFF00000000)

/// @brief Extract the internal values from flags.
#define EZCU_VAL_MASK 0x000000000000FF00

/// @brief Extract the internal indexes from flags.
#define EZCU_LKP_MASK 0x00000000000000FF

/// @brief Macro to check whether a set of flags contains a given mask.
#define EZCU_FLAGS_HAVE(flags, mask) \
    (((flags & 0xFFFFFFFF00000000) & \
       (mask & 0xFFFFFFFF00000000)) == (mask & 0xFFFFFFFF00000000))

///
/// @brief Print a help message about the default ezCU flag.
///
/// @param None.
/// @return Nothing.
///
void ezcu_flags_help_default();

///
/// @brief Print a help message about the vendors ezCU flags.
///
/// @param None.
/// @return Nothing.
///
void ezcu_flags_help_vendors();

///
/// @brief Print a help message about the device types ezCU flags.
///
/// @param None.
/// @return Nothing.
///
void ezcu_flags_help_dev_types();

///
/// @brief Print a help message about the device indexes ezCU flags.
///
/// @param None.
/// @return Nothing.
///
void ezcu_flags_help_dev_indexes();

///
/// @brief Print a help message about the device properties ezCU flags.
///
/// @param None.
/// @return Nothing.
///
void ezcu_flags_help_dev_props();

///
/// @brief Print a help message about the devices ezCU flags.
///
/// @param None.
/// @return Nothing.
///
void ezcu_flags_help_dev();

///
/// @brief Print a help message about the memory locations ezCU flags.
///
/// @param None.
/// @return Nothing.
///
void ezcu_flags_help_mem_locations();

///
/// @brief Print a help message about the memory properties ezCU flags.
///
/// @param None.
/// @return Nothing.
///
void ezcu_flags_help_mem_props();

///
/// @brief Print a help message about the memory acces modes ezCU flags.
///
/// @param None.
/// @return Nothing.
///
void ezcu_flags_help_mem_access_modes();

///
/// @brief Print a help message about the memory types ezCU flags.
///
/// @param None.
/// @return Nothing.
///
void ezcu_flags_help_mem_types();

///
/// @brief Print a help message about the memory objects ezCU flags.
///
/// @param None.
/// @return Nothing.
///
void ezcu_flags_help_mem();

///
/// @brief Print a help message about all the possible ezCU flags.
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
/// @brief Make sure that the vendors flags are valid.
///
/// @param flags are the user defined flags.
/// @return Nothing.
///
void ezcu_flags_check_vendors(ezcu_flags_t flags);

///
/// @brief Make sure that the device types flags are valid.
///
/// @param flags are the user defined flags.
/// @return Nothing.
///
void ezcu_flags_check_dev_types(ezcu_flags_t flags);

///
/// @brief Make sure that the device indexes flags are valid.
///
/// @param flags are the user defined flags.
/// @return Nothing.
///
void ezcu_flags_check_dev_indexes(ezcu_flags_t flags);

///
/// @brief Make sure that the device properties flags are valid.
///
/// @param flags are the user defined flags.
/// @return Nothing.
///
void ezcu_flags_check_dev_props(ezcu_flags_t flags);

///
/// @brief Make sure that the device flags are valid.
///
/// @param flags are the user defined flags.
/// @return Nothing.
///
void ezcu_flags_check_dev(ezcu_flags_t flags);

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
/// @brief Make sure that the flags are valid.
///
/// @param flags are the user defined flags.
/// @return Nothing.
///
void ezcu_flags_check_all(ezcu_flags_t flags);

///
/// @brief Return the string that represents devices flags.
///
/// @param flags are the user defined flags.
/// @return Nothing.
///
void ezcu_flags_dev_to_str(ezcu_flags_t flags, char *string);

///
/// @brief Return the string that represents memory flags.
///
/// @param flags are the user defined flags.
/// @return Nothing.
///
void ezcu_flags_mem_to_str(ezcu_flags_t flags, char *string);

///
/// @brief Return the vendor flags found in a set of ezCU flags.
///
/// @param flags are the user defined flags.
/// @param v will contain the found vendor flags. 
/// @return An integer that represents the number of vendors flags found (1).
///
int ezcu_flags_to_vendors_flags(ezcu_flags_t flags, ezcu_vendor_flags_t *v);

///
/// @brief Return the device types flags found in a set of ezCU flags.
///
/// @param flags are the user defined flags.
/// @param tab is the array that will contain the found device types flags. 
/// @return An integer that represents the number of device types flags found.
///
int ezcu_flags_to_dev_types_flags(ezcu_flags_t flags, 
                                  ezcu_dev_type_flags_t *tab);

///
/// @brief Return the device properties flags found in a set of ezCU flags.
///
/// @param flags are the user defined flags.
/// @param tab is the array that will contain the found device properties flags. 
/// @return An integer that represents the number of properties flags found.
///
int ezcu_flags_to_dev_props_flags(ezcu_flags_t flags, 
                                  ezcu_dev_prop_flags_t *tab);

///
/// @brief Return the device indexes flags found in a set of ezCU flags.
///
/// @param flags are the user defined flags.
/// @param tab is the array that will contain the found device indexes flags. 
/// @return An integer that represents the number of device indexes flags found.
///
int ezcu_flags_to_dev_indexes_flags(ezcu_flags_t flags, 
                                    ezcu_dev_index_flags_t *tab);

///
/// @brief Demestify a set of ezCU flags for devices selection.
///
/// @param flags are the user defined flags.
/// @param tab is the array that will contain the found device indexes flags. 
/// @return An integer that represents the number of device indexes flags found.
///
void ezcu_flags_parse_dev(ezcu_flags_t flags, ezcu_vendor_flags_t *v,
                          ezcu_dev_type_flags_t  *dt, int *ndt,
                          ezcu_dev_prop_flags_t  *dp, int *ndp,
                          ezcu_dev_index_flags_t *di, int *ndi);

///
/// @brief Return the index based on a user defined flag.
///
/// @param f is the user defined flag.
/// @return An integer that represents the device index.
///
int ezcu_flags_to_dev_index(ezcu_dev_index_flags_t f);

///
/// @brief Return the device properties values based on a user defined flag.
///
/// @param f is the user defined flag.
/// @return An integer that represents the device property.
///
int ezcu_flags_to_dev_prop(ezcu_dev_prop_flags_t f);

///
/// @brief Return the memory unit size based on a user defined flag.
///
/// @param flags are the user defined flags.
/// @return An integer that represents the memory unit size.
///
size_t ezcu_flags_to_mem_unit_size(ezcu_mem_types_flags_t flags);

CPPGUARD_END()

#endif  // __EZCU_FLAGS_H_
