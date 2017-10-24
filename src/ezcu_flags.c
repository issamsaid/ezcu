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
/// @file ezcu_flags.c
/// @author Issam SAID
/// @brief The implementation of the flags conversion routines.
///
#include <stdio.h>
#include <string.h>
#include <ezcu/flags.h>
#include <__ezcu/util-inl.h>
#include <__ezcu/error-inl.h>

inline void ezcu_flags_help_default() {
    EZCU_PRINT("The default ezcu flag is DEFAULT (orthogonal*)");
    EZCU_PRINT("DEFAULT: select the default device, "
               "the default memory type or the default behavior of ezcu");
    EZCU_PRINT("*combinations of this flag with others are not possible");
}

inline void ezcu_flags_help_vendors() {
    EZCU_PRINT("The possible vendors ezcu flags are (orthogonal*): "
               "AMD | APPLE | INTEL | NVIDIA");
    EZCU_PRINT("AMD    : select the devices and resources provided by AMD");
    EZCU_PRINT("APPLE  : select the devices and resources provided by APPLE");
    EZCU_PRINT("INTEL  : select the devices and resources provided by INTEL");
    EZCU_PRINT("NVIDIA : select the devices and resources provided by NVIDIA");
    EZCU_PRINT("*combinations of those flags are not possible");
}

inline void ezcu_flags_help_dev_types() {
    EZCU_PRINT("The possible device types ezcu flags are (inclusive*): "
               "ALL | CPU | ACCELERATOR | GPU");
    EZCU_PRINT("ALL         : select all possible devices");
    EZCU_PRINT("CPU         : select CPU devices only");
    EZCU_PRINT("ACCELERATOR : select all the hardware accelerators");
    EZCU_PRINT("GPU         : select GPU devices only");
    EZCU_PRINT("*combinations of those flags are possible");
}

inline void ezcu_flags_help_dev_indexes() {
    EZCU_PRINT("The possible device indexes ezcu flags are (inclusive*): "
               "FIRST | SECOND | THIRD   | FOURTH | "
               "FIFTH | SIXTH  | SEVENTH | EIGHTH");
    EZCU_PRINT("FIRST       : select the 1st device found");
    EZCU_PRINT("SECOND      : select the 2nd device found");
    EZCU_PRINT("THIRD       : select the 3rd device found");
    EZCU_PRINT("FOURTH      : select the 4th device found");
    EZCU_PRINT("FIFTH       : select the 5th device found");
    EZCU_PRINT("SIXTH       : select the 6th device found");
    EZCU_PRINT("SEVENTH     : select the 7th device found");
    EZCU_PRINT("EIGHTH      : select the 8th device found");
    EZCU_PRINT("*combinations of those flags are possible");
}

inline void ezcu_flags_help_dev_props() {
    EZCU_PRINT("The possible device properties ezcu flags are (inclusive*): "
               "CC20 | CC30 | CC35 | CC35 | CC60");
    EZCU_PRINT("CC20        : select devices whose compute capability >= 2.0");
    EZCU_PRINT("CC30        : select devices whose compute capability >= 3.0");
    EZCU_PRINT("CC35        : select devices whose compute capability >= 3.5");
    EZCU_PRINT("CC50        : select devices whose compute capability >= 5.0");
    EZCU_PRINT("CC60        : select devices whose compute capability >= 6.0");
    EZCU_PRINT("*combinations of those flags are possible (NVIDIA GPU only)");
}

inline void ezcu_flags_help_dev() {
    ezcu_flags_help_vendors();
    ezcu_flags_help_dev_types();
    ezcu_flags_help_dev_indexes();
    ezcu_flags_help_dev_props();
}

inline void ezcu_flags_help_mem_locations() {
    EZCU_PRINT("The possible memory locations ezcu flags are (orthogonal*): "
               "HOST | HWA");
    EZCU_PRINT("HOST           : data lives in CPU memory");
    EZCU_PRINT("HWA            : data lives in the hardware accelerator space");
    EZCU_PRINT("*combinations of those flags are not possible");
}

inline void ezcu_flags_help_mem_props() {
    EZCU_PRINT("The possible memory properties ezcu flags are (orthogonal*): "
               "PINNED | ZERO_COPY");
    EZCU_PRINT("PINNED         : data is pinned (PCI featured devices only)");
    EZCU_PRINT("ZERO_COPY      : make the data visible to all devices");
    EZCU_PRINT("*combinations of those flags are not possible");
}

inline void ezcu_flags_help_mem_access_modes() {
    EZCU_PRINT("The possible memory access modes ezcu flags are (orthogonal*): "
               "READ_ONLY | WRITE_ONLY | READ_WRITE");
    EZCU_PRINT("READ_ONLY      : data read-only (device perspectives)");
    EZCU_PRINT("WRITE_ONLY     : data write-only (device perspectives)");
    EZCU_PRINT("READ_WRITE     : data is read-write (device perspectives)");
    EZCU_PRINT("*combinations of those flags are not possible");
}

inline void ezcu_flags_help_mem_types() {
    EZCU_PRINT("The possible memory types ezcu flags are (orthogonal*): "
               "CHAR | INT | UNSIGNED_INT | LONG | UNSIGNED_LONG | "
               "SIZET | FLOAT | DOUBLE | SHORT");
    EZCU_PRINT("CHAR          : data contain chars");
    EZCU_PRINT("INT           : data contain integers");
    EZCU_PRINT("SHORT         : data contain shorts (signed or unsigned)");
    EZCU_PRINT("UNSIGNED_INT  : data contain unsigned integers");
    EZCU_PRINT("LONG          : data contain long integers");
    EZCU_PRINT("UNSIGNED_LONG : data contain unsigned long integers");
    EZCU_PRINT("SIZET         : data contain size_t");
    EZCU_PRINT("FLOAT         : data contain floats");
    EZCU_PRINT("DOUBLE        : data contain doubles");
    EZCU_PRINT("*combinations of those flags are not possible");
}

inline void ezcu_flags_help_mem() {
    ezcu_flags_help_mem_locations();
    ezcu_flags_help_mem_props();
    ezcu_flags_help_mem_access_modes();
    ezcu_flags_help_mem_types();
}

inline void ezcu_flags_help() {
    ezcu_flags_help_default();
    ezcu_flags_help_vendors();
    ezcu_flags_help_dev();
    ezcu_flags_help_mem();
}

inline void ezcu_flags_check_default(ezcu_flags_t flags) { 
    if ((EZCU_FLAGS_HAVE((flags), DEFAULT)) && (flags & (~DEFAULT))) {
        ezcu_flags_help_default();
        EZCU_EXIT("the ezcu DEFAULT flag should be exclusive");
    }
} 

inline void ezcu_flags_check_vendors(ezcu_flags_t flags) {
    ezcu_flags_check_default(flags);
    if( ! (((flags & EZCU_FLAGS_VENDORS_MASK) == 0)   ||
            ( EZCU_FLAGS_HAVE((flags), AMD)             && 
            ! EZCU_FLAGS_HAVE((flags), APPLE)           && 
            ! EZCU_FLAGS_HAVE((flags), INTEL)           && 
            ! EZCU_FLAGS_HAVE((flags), NVIDIA))         || 
            ( EZCU_FLAGS_HAVE((flags), APPLE)           && 
            ! EZCU_FLAGS_HAVE((flags), AMD)             && 
            ! EZCU_FLAGS_HAVE((flags), INTEL)           && 
            ! EZCU_FLAGS_HAVE((flags), NVIDIA))         || 
            ( EZCU_FLAGS_HAVE((flags), INTEL)           && 
            ! EZCU_FLAGS_HAVE((flags), AMD)             && 
            ! EZCU_FLAGS_HAVE((flags), APPLE)           && 
            ! EZCU_FLAGS_HAVE((flags), NVIDIA))         || 
            ( EZCU_FLAGS_HAVE((flags), NVIDIA)          && 
            ! EZCU_FLAGS_HAVE((flags), AMD)             && 
            ! EZCU_FLAGS_HAVE((flags), APPLE)           && 
            ! EZCU_FLAGS_HAVE((flags), INTEL)))) {
        ezcu_flags_help_vendors();
        EZCU_EXIT("ezcu flags to select vendors are invalid");
    }
}

inline void ezcu_flags_check_dev_types(ezcu_flags_t flags) { 
    ezcu_flags_check_default(flags);
    if( ! (((flags & EZCU_FLAGS_DEV_TYPES_MASK) == 0)  || 
            ( EZCU_FLAGS_HAVE((flags), ALL))           ||     
            ( EZCU_FLAGS_HAVE((flags), CPU)            || 
              EZCU_FLAGS_HAVE((flags), GPU)            ||
              EZCU_FLAGS_HAVE((flags), ACCELERATOR)))) {
        ezcu_flags_help_dev_types();
        EZCU_EXIT("ezcu flags for device type are invalid");
    }
}

inline void ezcu_flags_check_dev_indexes(ezcu_flags_t flags) {
    ezcu_flags_check_default(flags);      
    if ( ! (((flags & EZCU_FLAGS_DEV_INDEXES_MASK) == 0) ||  
           ( EZCU_FLAGS_HAVE((flags), FIRST)               || 
             EZCU_FLAGS_HAVE((flags), SECOND)              || 
             EZCU_FLAGS_HAVE((flags), THIRD)               || 
             EZCU_FLAGS_HAVE((flags), FOURTH)              || 
             EZCU_FLAGS_HAVE((flags), FIFTH)               || 
             EZCU_FLAGS_HAVE((flags), SIXTH)               || 
             EZCU_FLAGS_HAVE((flags), SEVENTH)             || 
             EZCU_FLAGS_HAVE((flags), EIGHTH)))) {
        ezcu_flags_help_dev_indexes();
        EZCU_EXIT("ezcu flags for device index are invalid");
    }
}

inline void ezcu_flags_check_dev_props(ezcu_flags_t flags) {
    ezcu_flags_check_default(flags);      
    if ( ! (((flags & (~GPU) & EZCU_FLAGS_DEV_PROPS_MASK) == 0) ||  
           ( EZCU_FLAGS_HAVE((flags), CC20)                     || 
             EZCU_FLAGS_HAVE((flags), CC30)                     || 
             EZCU_FLAGS_HAVE((flags), CC35)                     || 
             EZCU_FLAGS_HAVE((flags), CC50)                     || 
             EZCU_FLAGS_HAVE((flags), CC60)))) {
        ezcu_flags_help_dev_props();
        EZCU_EXIT("ezcu flags for device properties are invalid");
    }
}

inline void ezcu_flags_check_mem_locations(ezcu_flags_t flags) {
    ezcu_flags_check_default(flags);      
    if ( ! (((flags & EZCU_FLAGS_MEM_LOCS_MASK) == 0) ||  
           (EZCU_FLAGS_HAVE((flags), HWA)               &&
           !EZCU_FLAGS_HAVE((flags), HOST))             || 
           (EZCU_FLAGS_HAVE((flags), HOST)              &&
           !EZCU_FLAGS_HAVE((flags), HWA)))) {
        ezcu_flags_help_mem_locations();
        EZCU_EXIT("ezcu flags for memory locations are invalid");
    }
}

inline void ezcu_flags_check_mem_props(ezcu_flags_t flags) {
    ezcu_flags_check_default(flags);      
    if ( ! (((flags & EZCU_FLAGS_MEM_PROPS_MASK) == 0)  ||  
           (EZCU_FLAGS_HAVE((flags), PINNED)              &&
           !EZCU_FLAGS_HAVE((flags), ZERO_COPY))          || 
           (EZCU_FLAGS_HAVE((flags), ZERO_COPY)           &&
           !EZCU_FLAGS_HAVE((flags), PINNED)))) {
        ezcu_flags_help_mem_props();
        EZCU_EXIT("ezcu flags for memory properties are invalid");
    }
}

inline void ezcu_flags_check_mem_access_modes(ezcu_flags_t flags) {
    ezcu_flags_check_default(flags);      
    if ( ! (((flags & EZCU_FLAGS_MEM_ACCESS_MODES_MASK) == 0) ||  
           ( EZCU_FLAGS_HAVE((flags), READ_WRITE)               &&
            !EZCU_FLAGS_HAVE((flags), READ_ONLY)                &&
            !EZCU_FLAGS_HAVE((flags), WRITE_ONLY))              ||
           ( EZCU_FLAGS_HAVE((flags), READ_ONLY)                &&
            !EZCU_FLAGS_HAVE((flags), READ_WRITE)               &&
            !EZCU_FLAGS_HAVE((flags), WRITE_ONLY))              ||
           ( EZCU_FLAGS_HAVE((flags), WRITE_ONLY)               &&
            !EZCU_FLAGS_HAVE((flags), READ_ONLY)                &&
            !EZCU_FLAGS_HAVE((flags), READ_WRITE)))) {
        ezcu_flags_help_mem_access_modes();
        EZCU_EXIT("ezcu flags for memory access modes are invalid");
    }
}

inline void ezcu_flags_check_mem_types(ezcu_flags_t flags) {
    ezcu_flags_check_default(flags);      
    if ( ! (((flags & EZCU_FLAGS_MEM_TYPES_MASK) == 0) ||  
           ( EZCU_FLAGS_HAVE((flags), FLOAT)             &&
           ! EZCU_FLAGS_HAVE((flags), INT)               &&
           ! EZCU_FLAGS_HAVE((flags), SHORT)             &&
           ! EZCU_FLAGS_HAVE((flags), UNSIGNED_INT)      &&
           ! EZCU_FLAGS_HAVE((flags), LONG)              &&
           ! EZCU_FLAGS_HAVE((flags), UNSIGNED_LONG)     &&
           ! EZCU_FLAGS_HAVE((flags), CHAR)              &&
           ! EZCU_FLAGS_HAVE((flags), SIZET)             &&
           ! EZCU_FLAGS_HAVE((flags), DOUBLE))           || 
           ( EZCU_FLAGS_HAVE((flags), DOUBLE)            &&
           ! EZCU_FLAGS_HAVE((flags), INT)               &&
           ! EZCU_FLAGS_HAVE((flags), SHORT)             &&
           ! EZCU_FLAGS_HAVE((flags), CHAR)              &&
           ! EZCU_FLAGS_HAVE((flags), LONG)              &&
           ! EZCU_FLAGS_HAVE((flags), SIZET)             &&
           ! EZCU_FLAGS_HAVE((flags), UNSIGNED_LONG)     &&
           ! EZCU_FLAGS_HAVE((flags), UNSIGNED_INT)      && 
           ! EZCU_FLAGS_HAVE((flags), FLOAT))            || 
           ( EZCU_FLAGS_HAVE((flags), INT)               &&
           ! EZCU_FLAGS_HAVE((flags), FLOAT)             &&
           ! EZCU_FLAGS_HAVE((flags), SHORT)             &&
           ! EZCU_FLAGS_HAVE((flags), CHAR)              && 
           ! EZCU_FLAGS_HAVE((flags), LONG)              &&
           ! EZCU_FLAGS_HAVE((flags), SIZET)             && 
           ! EZCU_FLAGS_HAVE((flags), UNSIGNED_LONG)     &&
           ! EZCU_FLAGS_HAVE((flags), UNSIGNED_INT)      && 
           ! EZCU_FLAGS_HAVE((flags), DOUBLE))           ||
           ( EZCU_FLAGS_HAVE((flags), LONG)              &&
           ! EZCU_FLAGS_HAVE((flags), FLOAT)             &&
           ! EZCU_FLAGS_HAVE((flags), SHORT)             &&
           ! EZCU_FLAGS_HAVE((flags), CHAR)              && 
           ! EZCU_FLAGS_HAVE((flags), INT)               && 
           ! EZCU_FLAGS_HAVE((flags), SIZET)             &&
           ! EZCU_FLAGS_HAVE((flags), UNSIGNED_LONG)     &&
           ! EZCU_FLAGS_HAVE((flags), UNSIGNED_INT)      && 
           ! EZCU_FLAGS_HAVE((flags), DOUBLE))           ||
           ( EZCU_FLAGS_HAVE((flags), UNSIGNED_LONG)     &&
           ! EZCU_FLAGS_HAVE((flags), FLOAT)             &&
           ! EZCU_FLAGS_HAVE((flags), CHAR)              && 
           ! EZCU_FLAGS_HAVE((flags), SHORT)             &&
           ! EZCU_FLAGS_HAVE((flags), INT)               && 
           ! EZCU_FLAGS_HAVE((flags), SIZET)             &&
           ! EZCU_FLAGS_HAVE((flags), LONG)              &&
           ! EZCU_FLAGS_HAVE((flags), UNSIGNED_INT)      && 
           ! EZCU_FLAGS_HAVE((flags), DOUBLE))           ||
           ( EZCU_FLAGS_HAVE((flags), UNSIGNED_INT)      &&
           ! EZCU_FLAGS_HAVE((flags), FLOAT)             &&
           ! EZCU_FLAGS_HAVE((flags), SHORT)             &&
           ! EZCU_FLAGS_HAVE((flags), CHAR)              && 
           ! EZCU_FLAGS_HAVE((flags), INT)               && 
           ! EZCU_FLAGS_HAVE((flags), SIZET)             &&
           ! EZCU_FLAGS_HAVE((flags), UNSIGNED_LONG)     &&
           ! EZCU_FLAGS_HAVE((flags), LONG)              && 
           ! EZCU_FLAGS_HAVE((flags), DOUBLE))           ||
           ( EZCU_FLAGS_HAVE((flags), SHORT)             &&
           ! EZCU_FLAGS_HAVE((flags), FLOAT)             &&
           ! EZCU_FLAGS_HAVE((flags), SIZET)             &&
           ! EZCU_FLAGS_HAVE((flags), CHAR)              && 
           ! EZCU_FLAGS_HAVE((flags), INT)               && 
           ! EZCU_FLAGS_HAVE((flags), LONG)              &&
           ! EZCU_FLAGS_HAVE((flags), UNSIGNED_LONG)     &&
           ! EZCU_FLAGS_HAVE((flags), UNSIGNED_INT)      &&
 	         ! EZCU_FLAGS_HAVE((flags), DOUBLE))           ||
           ( EZCU_FLAGS_HAVE((flags), SIZET)             &&
           ! EZCU_FLAGS_HAVE((flags), FLOAT)             &&
           ! EZCU_FLAGS_HAVE((flags), SHORT)             &&
           ! EZCU_FLAGS_HAVE((flags), CHAR)              &&
           ! EZCU_FLAGS_HAVE((flags), INT)               &&
           ! EZCU_FLAGS_HAVE((flags), LONG)              &&
           ! EZCU_FLAGS_HAVE((flags), UNSIGNED_LONG)     &&
           ! EZCU_FLAGS_HAVE((flags), UNSIGNED_INT)      &&
           ! EZCU_FLAGS_HAVE((flags), DOUBLE))           ||
           ( EZCU_FLAGS_HAVE((flags), CHAR)              &&
           ! EZCU_FLAGS_HAVE((flags), FLOAT)             &&
           ! EZCU_FLAGS_HAVE((flags), SHORT)             &&
           ! EZCU_FLAGS_HAVE((flags), INT)               && 
           ! EZCU_FLAGS_HAVE((flags), LONG)              &&
           ! EZCU_FLAGS_HAVE((flags), SIZET)             &&
           ! EZCU_FLAGS_HAVE((flags), UNSIGNED_LONG)     &&
           ! EZCU_FLAGS_HAVE((flags), UNSIGNED_INT)      && 
           ! EZCU_FLAGS_HAVE((flags), DOUBLE)))) {
        ezcu_flags_help_mem_types();
        EZCU_EXIT("ezcu flags for memory types are invalid");
    }
}

inline void ezcu_flags_check_dev(ezcu_flags_t flags) {
    ezcu_flags_check_default(flags);      
    ezcu_flags_check_vendors(flags);      
    ezcu_flags_check_dev_types(flags);      
    ezcu_flags_check_dev_indexes(flags);      
    ezcu_flags_check_dev_props(flags);      
}

inline void ezcu_flags_check_mem(ezcu_flags_t flags) {
    ezcu_flags_check_default(flags);      
    ezcu_flags_check_mem_locations(flags);      
    ezcu_flags_check_mem_access_modes(flags);      
    ezcu_flags_check_mem_types(flags);      
}

inline void ezcu_flags_check_all(ezcu_flags_t flags) {
    ezcu_flags_check_default(flags);      
    ezcu_flags_check_vendors(flags);      
    ezcu_flags_check_dev_types(flags);      
    ezcu_flags_check_dev_indexes(flags);      
    ezcu_flags_check_dev_props(flags);      
    ezcu_flags_check_mem_locations(flags);      
    ezcu_flags_check_mem_access_modes(flags);      
    ezcu_flags_check_mem_types(flags);      
}

inline void ezcu_flags_dev_to_str(ezcu_flags_t flags, char *string) {
    int len, index = 0, marker = 0;
    char *s = string;
    
    /// Bind flags to their string representations.
    typedef struct {
        ezcu_flags_t  flag;
        const char *to_str;
    } sFlagtoString;

    /// For flags string representation lookup.
    static const sFlagtoString nFlagsStringLookup[] = {
        { DEFAULT,        "DEFAULT"},
        { AMD,            "AMD"},
        { APPLE,          "APPLE"},   
        { INTEL,          "INTEL"}, 
        { NVIDIA,         "NVIDIA"}, 
        { ALL,            "ALL"}, 
        { GPU,            "GPU"}, 
        { CPU,            "CPU"}, 
        { ACCELERATOR,    "ACCELERATOR"},
        { FIRST,          "FIRST"},  
        { SECOND,         "SECOND"},  
        { THIRD,          "THIRD"},  
        { FOURTH,         "FOURTH"},  
        { FIFTH,          "FIFTH"},  
        { SIXTH,          "SIXTH"},  
        { SEVENTH,        "SEVENTH"},  
        { EIGHTH,         "EIGHTH"},
        { CC20,           "CC20"},    
        { CC30,           "CC30"},    
        { CC35,           "CC35"},    
        { CC50,           "CC50"},    
        { CC60,           "CC60"},    
        { 0xFFFFFFFFFFFFFFFF,""}
    };

    while (nFlagsStringLookup[index].flag != 0xFFFFFFFFFFFFFFFF) {
        if (EZCU_FLAGS_HAVE((flags), nFlagsStringLookup[index].flag)) {
            marker = sprintf(s, " | %s", nFlagsStringLookup[index].to_str);
            s     += marker;
        }
        index++;
    }
    if ((len = strlen(string)-strlen(s))) {
        for (index = 1; index < len - 1; index++) 
            string[index] = string[index + 1];
        if(index < len) string[index] = '\0';
    }
}

inline void ezcu_flags_mem_to_str(ezcu_flags_t flags, char *string) {
    int len, index = 0, marker = 0;
    char *s = string;
    
    /// Bind flags to their string representations.
    typedef struct {
        ezcu_flags_t  flag;
        const char *to_str;
    } sFlagtoString;

    /// For flags string representation lookup.
    static const sFlagtoString nFlagsStringLookup[] = {
        { DEFAULT,        "DEFAULT"},
        { HWA,            "HWA"},    
        { HOST,           "HOST"},    
        { PINNED,         "PINNED"},
        { ZERO_COPY,      "ZERO_COPY"},
        { READ_WRITE,     "READ_WRITE"},                
        { READ_ONLY,      "READ_ONLY"},
        { WRITE_ONLY,     "WRITE_ONLY"},                                
        { CHAR,           "CHAR"},                                
        { INT,            "INT"},                                
        { UNSIGNED_INT,   "UNSIGNED_INT"},
        { LONG,           "LONG"},
        { UNSIGNED_LONG,  "UNSIGNED_LONG"},
        { SIZET,          "SIZET"},
        { SHORT,          "SHORT"},
        { FLOAT,          "FLOAT"},
        { DOUBLE,         "DOUBLE"},                                                                
        { 0xFFFFFFFFFFFFFFFF,""}
    };

    while (nFlagsStringLookup[index].flag != 0xFFFFFFFFFFFFFFFF) {
        if (EZCU_FLAGS_HAVE((flags), nFlagsStringLookup[index].flag)) {
            marker = sprintf(s, " | %s", nFlagsStringLookup[index].to_str);
            s     += marker;
        }
        index++;
    }
    if ((len = strlen(string)-strlen(s))) {
        for (index = 1; index < len - 1; index++) 
            string[index] = string[index + 1];
        if(index < len) string[index] = '\0';
    }
}

inline int ezcu_flags_to_vendors_flags(ezcu_flags_t flags, 
                                       ezcu_vendor_flags_t *v) {
    int i;
    static const ezcu_vendor_flags_t 
        nVendorsLookup[] = { AMD, APPLE, INTEL, NVIDIA};
    ezcu_flags_check_vendors(flags);
    
    /// Only one vendor flag should be found.
    for(i=0; i < EZCU_NB_VENDORS; ++i) {
        if (EZCU_FLAGS_HAVE(flags, nVendorsLookup[i])) {
            *v = nVendorsLookup[i]; 
            break;
        }
    }

    /// if not found consider NVIDIA (the default on ezcu).
    if (i == EZCU_NB_VENDORS) *v = NVIDIA;
    return 1;
}

inline int ezcu_flags_to_dev_types_flags(ezcu_flags_t flags, 
                                         ezcu_dev_type_flags_t *tab) {
    int i, nb_dtflags = 0;
    static const ezcu_dev_type_flags_t 
        nDevTypesLookup[] = { ALL, CPU, GPU, ACCELERATOR};
    ezcu_flags_check_dev_types(flags);

    /// if DEFAULT or no type is informed add ALL as default.
    if ((EZCU_FLAGS_HAVE(flags, DEFAULT)) || 
        ((flags & EZCU_FLAGS_DEV_TYPES_MASK) == 0x0)) flags |= ALL;
    
    for(i=0; i < EZCU_NB_DEV_TYPES; i++) {
        if (EZCU_FLAGS_HAVE(flags, nDevTypesLookup[i]))
            tab[nb_dtflags++] = nDevTypesLookup[i]; 
    }
    return nb_dtflags; 
}

inline int ezcu_flags_to_dev_props_flags(ezcu_flags_t flags, 
                                         ezcu_dev_prop_flags_t *tab) {
    int i, nb_dpflags = 0;
    static const ezcu_dev_prop_flags_t 
        nDevPropsLookup[] = { CC20, CC30, CC35, CC50, CC60};
    ezcu_flags_check_dev_props(flags);
  
    for(i=0; i < EZCU_NB_DEV_PROPS; ++i) {
        if (EZCU_FLAGS_HAVE(flags, nDevPropsLookup[i]))
            tab[nb_dpflags++] = nDevPropsLookup[i]; 
    }

    /// if not found consider CC20 as default (always verified).
    if (nb_dpflags == 0) {
        tab[0] = CC20;
        return 1;
    } else { return nb_dpflags; }
}

inline int ezcu_flags_to_dev_indexes_flags(ezcu_flags_t flags, 
                                           ezcu_dev_index_flags_t *tab) {
    int i, nb_diflags = 0;
    static const ezcu_dev_index_flags_t 
        nDevIndexesLookup[] = { FIRST, SECOND, THIRD, FOURTH, 
                                 FIFTH, SIXTH, SEVENTH, EIGHTH};
    ezcu_flags_check_dev(flags);
  
    /// if DEFAULT return FIRST
    if (EZCU_FLAGS_HAVE(flags, DEFAULT)) flags |= FIRST;

    for(i=0; i < EZCU_NB_DEV_INDEXES; ++i) {
        if (EZCU_FLAGS_HAVE(flags, nDevIndexesLookup[i]))
            tab[nb_diflags++] = nDevIndexesLookup[i]; 
    }

    /// if not found return them all FIRST-to-EIGHTH (max supported).
    if (nb_diflags == 0) {
        tab[0] = FIRST;
        tab[1] = SECOND;
        tab[2] = THIRD;
        tab[3] = FOURTH;
        tab[4] = FIFTH;
        tab[5] = SIXTH;
        tab[6] = SEVENTH;
        tab[7] = EIGHTH;
        return EZCU_NB_DEV_INDEXES;
    } else { return nb_diflags; }
}

inline void ezcu_flags_parse_dev(ezcu_flags_t flags, ezcu_vendor_flags_t *v, 
                                 ezcu_dev_type_flags_t  *dt, int *ndt,
                                 ezcu_dev_prop_flags_t  *dp, int *ndp,
                                 ezcu_dev_index_flags_t *di, int *ndi) {
           ezcu_flags_to_vendors_flags(flags, v);
    *ndt = ezcu_flags_to_dev_types_flags(flags, dt);
    *ndp = ezcu_flags_to_dev_props_flags(flags, dp);
    *ndi = ezcu_flags_to_dev_indexes_flags(flags, di);
}


inline int ezcu_flags_to_dev_index(ezcu_dev_index_flags_t f) {
    return (((f) & (EZCU_VAL_MASK)) >> 8);
}

inline int ezcu_flags_to_dev_prop(ezcu_dev_prop_flags_t f) {
    return (((f) & (EZCU_VAL_MASK)) >> 8);
}

inline size_t ezcu_flags_to_mem_unit_size(ezcu_mem_types_flags_t flags) {
  if(EZCU_FLAGS_HAVE(flags, CHAR         )) return sizeof(char);
  if(EZCU_FLAGS_HAVE(flags, SHORT        )) return sizeof(short);
  if(EZCU_FLAGS_HAVE(flags, INT          )) return sizeof(int);
  if(EZCU_FLAGS_HAVE(flags, UNSIGNED_INT )) return sizeof(unsigned int);
  if(EZCU_FLAGS_HAVE(flags, LONG         )) return sizeof(long);
  if(EZCU_FLAGS_HAVE(flags, UNSIGNED_LONG)) return sizeof(unsigned long);
  if(EZCU_FLAGS_HAVE(flags, FLOAT        )) return sizeof(float);
  if(EZCU_FLAGS_HAVE(flags, DOUBLE       )) return sizeof(double);
  if(EZCU_FLAGS_HAVE(flags, SIZET        )) return sizeof(size_t);
  EZCU_EXIT("ezcu flags for memory unit size are invalid");
}
