!>
!! @copyright Copyright (c) 2016-, Issam SAID <said.issam@gmail.com>
!! All rights reserved.
!!
!! Redistribution and use in source and binary forms, with or without
!! modification, are permetted provided that the following conditions
!! are met:
!!
!! 1. Redistributions of source code must retain the above copyright
!!    notice, this list of conditions and the following disclaimer.
!! 2. Redistributions in binary form must reproduce the above copyright
!!    notice, this list of conditions and the following disclaimer inthe
!!    documentation and/or other materials provided with the distribution.
!! 3. Neither the name of the COPYRIGHT HOLDER nor the names of its 
!!    contributors may be used to endorse or promote products derived from 
!!    this software without specific prior written permission.
!!
!! THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
!! INCLUDING, BUT NOT LIMITED TO, WARRANTIES OF MERCHANTABILITY AND FITNESS
!! FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
!! HOLDER OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
!! SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
!! PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
!! PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
!! LIABILITY, WETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
!! NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
!! SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
!!
!! @file m_ezcu_types.f90
!! @author Issam SAID
!! @brief This file implements the ezcu types in Fortran.
!! @see ezcu/types.h
!<
module m_ezcu_types
    use, intrinsic :: iso_c_binding

    implicit none

    private

    public :: ezcu_dev_t
    type, bind(c) :: ezcu_dev_t
        type(c_ptr) :: id
        type(c_ptr) :: streams
    end type ezcu_dev_t

    public :: ezcu_mem_t
    type, bind(c) :: ezcu_mem_t
        integer(c_int64_t) :: flags
        integer(c_size_t)  :: size
        integer(c_size_t)  :: unit_size
        type(c_ptr)        :: id
        type(c_ptr)        :: h
        integer(c_int)     :: refs
    end type ezcu_mem_t

    public :: ezcu_knl_t
    type, bind(c) :: ezcu_knl_t
        integer(c_int) :: grid(3)
        integer(c_int) :: block(3)
        integer(c_int) :: num_args
        integer(c_int) :: smem
        type(c_ptr)    :: id
        type(c_ptr)    :: args
        type(c_ptr)    :: args_types
        type(c_ptr)    :: vals
        type(c_ptr)    :: mems
    end type ezcu_knl_t

    enum, bind(c)
        enumerator :: NANO_SECONDS  = 1
        enumerator :: MICRO_SECONDS = 1000
        enumerator :: MILLI_SECONDS = 1000000
        enumerator :: SECONDS       = 1000000000
    end enum

    public :: NANO_SECONDS
    public :: MICRO_SECONDS
    public :: MILLI_SECONDS
    public :: SECONDS
end module m_ezcu_types
