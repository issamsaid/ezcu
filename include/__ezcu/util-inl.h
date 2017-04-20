#ifndef __EZCU_UTIL_INL_H_
#define __EZCU_UTIL_INL_H_
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
/// @file __ezcu/util-inl.h
/// @author Issam SAID
/// @brief Private functions used by the ezCU utilities.    
///
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <time.h>
#include <ezCU/types.h>
#include <__ezcu/config/guard.h>
#include <__ezcu/config/private.h>
#include <__ezcu/config/log.h>
#include <__ezcu/config/cuda.h>
#include <__ezcu/error-inl.h>

CPPGUARD_BEGIN();

extern ezcu_env_t ezcu;

#ifdef  __EZCU_DEBUG
#define EZCU_DEBUG(fmt,...)                                          \
    fprintf(ezcu->fdout,                                             \
            EZCU_PURPLE"[EZCU DBG]: "fmt".\n"EZCU_END, ##__VA_ARGS__)
#else 
#define EZCU_DEBUG(fmt,...) 
#endif  // __EZCU_DEBUG

#ifdef __EZCU_VERBOSE                             
#define EZCU_PRINT(fmt,...)                                                \
    fprintf(ezcu->fdout,                                                   \
            EZCU_GREEN"[EZCU MSG]: "fmt".\n"EZCU_END, ##__VA_ARGS__)
#define EZCU_WARN(fmt,...)                                                 \
    fprintf(ezcu->fdout,                                                   \
            EZCU_YELLOW"[EZCU WRN]: "fmt".\n"EZCU_END, ##__VA_ARGS__)
#define EZCU_WARN_IF(predicate, fmt,...)                                   \
    if (predicate) {                                                       \
        fprintf(ezcu->fdout,                                               \
                EZCU_YELLOW"[EZCU WRN]: "fmt".\n"EZCU_END, ##__VA_ARGS__); \
    }
#else 
#define EZCU_PRINT(fmt,...) 
#define EZCU_WARN(fmt,...) 
#define EZCU_WARN_IF(fmt,...) 
#endif  // __EZCU_VERBOSE 

__EZCU_PRIVATE int
__ezcu_ptr_cmp(void *cur_ptr, void *otr_ptr) {
    return (int64_t)cur_ptr - (int64_t)otr_ptr;
}

__EZCU_PRIVATE int
__ezcu_mem_cmp(void *cur_ptr, void *otr_ptr) {
    if((ezcu_mem_t)cur_ptr == (ezcu_mem_t)otr_ptr) return 0;
    else return -1;
}

__EZCU_PRIVATE int
__ezcu_int_cmp(void *cur, void *otr) {
    return *(int*)cur - *(int*)otr;
}

__EZCU_PRIVATE int
__ezcu_str_split(char *sequence, char*** container) {
    int i=0, nb_words = 0;
    char *pch, bkp[__EZCU_BUFFER_SIZE];
    sprintf(bkp, "%s", sequence);
    pch = strtok(sequence, " ");
    while (pch != NULL) { nb_words++; pch = strtok (NULL, " "); }

    *container      = (char**)malloc(sizeof(char*)*nb_words);
    char ** tab_str = *container;

    pch = strtok(bkp, " ");
    while (pch != NULL) {
        tab_str[i] = (char*)malloc(strlen(pch)*sizeof(char)); 
        sprintf(tab_str[i++], "%s", pch);
        pch = strtok (NULL, " ");
    }
    return nb_words;
}

__EZCU_PRIVATE int
__ezcu_str_cmp(void *cur, void *otr) {
    return strcmp((char*)cur, (char*)otr);
}

__EZCU_PRIVATE int
__ezcu_knl_cmp(void *cur, void *otr) {
    return (int64_t)cur - (int64_t)otr;
}

__EZCU_PRIVATE void
__ezcu_ptr_del(void *ptr) { free(ptr); }

__EZCU_PRIVATE int64_t 
__ezcu_nmult64(int64_t val, int64_t q) {
    return val + q - 1 - (val - 1)%q;
}

__EZCU_PRIVATE void
__ezcu_generate_filename(char *filename) {
    time_t tloc;
    struct tm *lc_time;
    time(&tloc);
    lc_time=localtime(&tloc);
    sprintf(filename, "%04d%02d%02d_%02d%02d",
            lc_time->tm_year + 1900, lc_time->tm_mon+1,
            lc_time->tm_mday, lc_time->tm_hour, lc_time->tm_min);
}

__EZCU_PRIVATE bool
__ezcu_check_file(const char* filename) {
    FILE* fd = NULL;
    if ((fd = fopen(filename, "rb")) == NULL) {
        return false;
    } else {
        fclose(fd);
        return true;        
    }
}

__EZCU_PRIVATE size_t
__ezcu_tell_file(const char* filename) {
    FILE* fd = NULL;
    size_t size;
    EZCU_EXIT_IF((fd = fopen(filename, "rb"))==NULL,
               "file '%s' not found", filename);
    fseek(fd, 0, SEEK_END);
    size = ftell(fd);
    fclose(fd);
    return size;
}

__EZCU_PRIVATE void
__ezcu_file_check_ext(const char *filename) {
    const char *fext = strrchr(filename, '.');  
    EZCU_EXIT_IF(((fext == NULL) || (strcmp(fext+1, "cu"))),
                "file '%s' not valid (supported extensions: cu)", filename);
} 

__EZCU_PRIVATE bool
__ezcu_file_has_ext(const char *filename, char *ext) {
    const char *fext = strrchr(filename, '.');
    if ((fext != NULL) && (!strcmp(fext+1, ext))) return true;
    else return false;
}

__EZCU_PRIVATE void
__ezcu_generate_bin_filename(const char *filename, char *bin_filename) {
    char noext[__EZCU_STR_SIZE];
    char* x, *precomp_ext[] = {"fatbin", "cubin", "ptx", NULL};
    const char *fext = strrchr(filename, '.');
    unsigned int i=0, len = strlen(filename) - strlen(fext) + 1;
    snprintf(noext, len, "%s", filename);
    EZCU_DEBUG("looking for compiled binary for '%s'", noext);
    while (precomp_ext[i] != NULL) {
        sprintf(bin_filename, "%s.%s", noext, precomp_ext[i]);
        if (__ezcu_check_file(bin_filename)) return;
        EZCU_WARN("trying '%s' ... not found", bin_filename);
        i++;
    }
    EZCU_EXIT("failed to find any fatbin, cubin or ptx related to '%s'",
              filename);
}

__EZCU_PRIVATE void
__ezcu_read_from_file(char* buffer, size_t size, const char* filename) {
    FILE* fd = NULL;
    EZCU_EXIT_IF((fd = fopen(filename, "rb"))==NULL,
                "file '%s' not found", filename);
    if (fread(buffer, 1, size-1, fd) != (size-1)) {        
        fclose(fd);
        EZCU_EXIT("couldn't read file '%s'", filename);
    }
    fclose(fd);
    buffer[size-1] = '\0';
}

__EZCU_PRIVATE void
__ezcu_write_to_file(const char* buffer, size_t size, const char* filename) {
    FILE* fd = NULL;
    EZCU_EXIT_IF((fd = fopen(filename, "w+"))==NULL,
                "failed to create file '%s'", filename);
    if (fwrite((char*)buffer, 1, size, fd) != (size)) {        
        fclose(fd);
        EZCU_EXIT("couldn't write to file '%s'", filename);
    }
    fclose(fd);
}

CPPGUARD_END();

#endif  // __EZCU_UTIL_INL_H_