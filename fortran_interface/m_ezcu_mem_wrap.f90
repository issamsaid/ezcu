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
!! 3. Neither the name of the copyright holder nor the names of its contributors
!!    may be used to endorse or promote products derived from this software
!!    without specific prior written permission.
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
!! @file m_ezcu_mem_wrap.f90
!! @author Issam SAID
!! @brief This file implements the Fortran interface of the ezcu memory 
!! wrap routines.
!! @see ezcu/mem.h
!<
module m_ezcu_mem_wrap
    use, intrinsic :: iso_c_binding
    use m_ezcu_flags
    use m_ezcu_types

    use m_ezcu_mem_interface
    
    implicit none
    
    private

    public :: ezcu_mem_wrap
    interface ezcu_mem_wrap
        module procedure ezcu_mem_wrap_int32_1d
        module procedure ezcu_mem_wrap_int32_1d_allocatable
        module procedure ezcu_mem_wrap_int64_1d
        module procedure ezcu_mem_wrap_int64_1d_allocatable            
        module procedure ezcu_mem_wrap_float_1d
        module procedure ezcu_mem_wrap_float_1d_allocatable
        module procedure ezcu_mem_wrap_double_1d
        module procedure ezcu_mem_wrap_double_1d_allocatable

        module procedure ezcu_mem_wrap_int32_2d
        module procedure ezcu_mem_wrap_int32_2d_allocatable
        module procedure ezcu_mem_wrap_int64_2d
        module procedure ezcu_mem_wrap_int64_2d_allocatable
        module procedure ezcu_mem_wrap_float_2d
        module procedure ezcu_mem_wrap_float_2d_allocatable
        module procedure ezcu_mem_wrap_double_2d
        module procedure ezcu_mem_wrap_double_2d_allocatable

        module procedure ezcu_mem_wrap_int32_3d
        module procedure ezcu_mem_wrap_int32_3d_allocatable
        module procedure ezcu_mem_wrap_int64_3d
        module procedure ezcu_mem_wrap_int64_3d_allocatable
        module procedure ezcu_mem_wrap_float_3d
        module procedure ezcu_mem_wrap_float_3d_allocatable
        module procedure ezcu_mem_wrap_double_3d
        module procedure ezcu_mem_wrap_double_3d_allocatable

        module procedure ezcu_mem_wrap_int32_4d
        module procedure ezcu_mem_wrap_int32_4d_allocatable
        module procedure ezcu_mem_wrap_int64_4d
        module procedure ezcu_mem_wrap_int64_4d_allocatable
        module procedure ezcu_mem_wrap_float_4d
        module procedure ezcu_mem_wrap_float_4d_allocatable
        module procedure ezcu_mem_wrap_double_4d
        module procedure ezcu_mem_wrap_double_4d_allocatable
    end interface ezcu_mem_wrap

    !! NOTE: With the CRAY and PGI compilers, subroutines
    !! with the same signature except the arrays for one are allocatable
    !! and pointers for the other, are TKR incompatible and thus ambiguous.
    !! In order to remove the ambiguity, the _ptr extension is added.    
    public :: ezcu_mem_wrap_ptr
    interface ezcu_mem_wrap_ptr
        module procedure ezcu_mem_wrap_int32_1d_pointer
        module procedure ezcu_mem_wrap_int64_1d_pointer
        module procedure ezcu_mem_wrap_float_1d_pointer
        module procedure ezcu_mem_wrap_double_1d_pointer

        module procedure ezcu_mem_wrap_int32_2d_pointer
        module procedure ezcu_mem_wrap_int64_2d_pointer
        module procedure ezcu_mem_wrap_float_2d_pointer
        module procedure ezcu_mem_wrap_double_2d_pointer

        module procedure ezcu_mem_wrap_int32_3d_pointer
        module procedure ezcu_mem_wrap_int64_3d_pointer
        module procedure ezcu_mem_wrap_float_3d_pointer
        module procedure ezcu_mem_wrap_double_3d_pointer

        module procedure ezcu_mem_wrap_int32_4d_pointer
        module procedure ezcu_mem_wrap_int64_4d_pointer
        module procedure ezcu_mem_wrap_float_4d_pointer
        module procedure ezcu_mem_wrap_double_4d_pointer
    end interface ezcu_mem_wrap_ptr

contains

    !!
    !! ezcu_mem_wrap 1d
    !!
    subroutine ezcu_mem_wrap_int32_1d(h, d, size_x, flags)
        integer,                              intent(in) :: size_x
        integer(kind=4),              target, intent(in) :: h(size_x)
        type(ezcu_dev_t),               pointer, intent(in) :: d
        integer(kind=c_int64_t),              intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size_x
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_int32_1d

    subroutine ezcu_mem_wrap_int32_1d_allocatable(h, d, flags)
        integer(kind=4), allocatable, target ,intent(in) :: h(:)
        type(ezcu_dev_t),               pointer, intent(in) :: d
        integer(kind=c_int64_t),              intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size(h, 1)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_int32_1d_allocatable

    subroutine ezcu_mem_wrap_int64_1d(h, d, size_x, flags)
        integer,                 intent(in) :: size_x
        integer(kind=8), target, intent(in) :: h(size_x)
        type(ezcu_dev_t), pointer,  intent(in) :: d
        integer(kind=c_int64_t), intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size_x
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_int64_1d

    subroutine ezcu_mem_wrap_int64_1d_allocatable(h, d, flags)
        integer(kind=8), allocatable, target, intent(in) :: h(:)
        type(ezcu_dev_t),               pointer, intent(in) :: d
        integer(kind=c_int64_t),              intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size(h, 1)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_int64_1d_allocatable

    subroutine ezcu_mem_wrap_float_1d(h, d, size_x, flags)
        integer,                           intent(in) :: size_x
        real(kind=4),              target, intent(in) :: h(size_x)
        type(ezcu_dev_t),            pointer, intent(in) :: d
        integer(kind=c_int64_t),           intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size_x
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_float_1d
   
    subroutine ezcu_mem_wrap_float_1d_allocatable(h, d, flags)
        real(kind=4), allocatable, target, intent(in) :: h(:)
        type(ezcu_dev_t),            pointer, intent(in) :: d
        integer(kind=c_int64_t),           intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size(h, 1)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_float_1d_allocatable

    subroutine ezcu_mem_wrap_double_1d(h, d, size_x, flags)
        integer,                           intent(in) :: size_x
        real(kind=8),              target, intent(in) :: h(size_x)
        type(ezcu_dev_t),            pointer, intent(in) :: d
        integer(kind=c_int64_t),           intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size_x
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_double_1d

    subroutine ezcu_mem_wrap_double_1d_allocatable(h, d, flags)
        real(kind=8), allocatable, target, intent(in) :: h(:)
        type(ezcu_dev_t),            pointer, intent(in) :: d
        integer(kind=c_int64_t),           intent(in) :: flags
        integer(kind=c_size_t)                        :: s8
        type(c_ptr) :: tmp
        s8  = size(h, 1)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_double_1d_allocatable

    !! 2d
    subroutine ezcu_mem_wrap_int32_2d(h, d, size_x, size_y, flags)
        integer,                              intent(in) :: size_x
        integer,                              intent(in) :: size_y
        integer(kind=4),              target ,intent(in) :: h(size_x, size_y)
        type(ezcu_dev_t),               pointer, intent(in) :: d
        integer(kind=c_int64_t),              intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size_x*size_y
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_int32_2d

    subroutine ezcu_mem_wrap_int32_2d_allocatable(h, d, flags)
        integer(kind=4), allocatable, target ,intent(in) :: h(:,:)
        type(ezcu_dev_t),               pointer, intent(in) :: d
        integer(kind=c_int64_t),              intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size(h, 1)*size(h, 2)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_int32_2d_allocatable

    subroutine ezcu_mem_wrap_int64_2d(h, d, size_x, size_y, flags)
        integer,                              intent(in) :: size_x
        integer,                              intent(in) :: size_y
        integer(kind=8),              target ,intent(in) :: h(size_x, size_y)
        type(ezcu_dev_t),               pointer, intent(in) :: d
        integer(kind=c_int64_t),              intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size_x*size_y
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_int64_2d

    subroutine ezcu_mem_wrap_int64_2d_allocatable(h, d, flags)
        integer(kind=8), allocatable, target ,intent(in) :: h(:,:)
        type(ezcu_dev_t),               pointer, intent(in) :: d
        integer(kind=c_int64_t),              intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size(h, 1)*size(h, 2)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_int64_2d_allocatable

    subroutine ezcu_mem_wrap_float_2d(h, d, size_x, size_y, flags)
        integer,                           intent(in) :: size_x
        integer,                           intent(in) :: size_y
        real(kind=4),              target, intent(in) :: h(size_x, size_y)
        type(ezcu_dev_t),            pointer, intent(in) :: d
        integer(kind=c_int64_t),           intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size_x*size_y
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_float_2d

    subroutine ezcu_mem_wrap_float_2d_allocatable(h, d, flags)
        real(kind=4), allocatable, target, intent(in) :: h(:,:)
        type(ezcu_dev_t),            pointer, intent(in) :: d
        integer(kind=c_int64_t),           intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size(h, 1)*size(h, 2)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_float_2d_allocatable


    subroutine ezcu_mem_wrap_double_2d(h, d, size_x, size_y, flags)
        integer,                     intent(in) :: size_x
        integer,                     intent(in) :: size_y
        real(kind=8),        target, intent(in) :: h(size_x, size_y)
        type(ezcu_dev_t),      pointer, intent(in) :: d
        integer(kind=c_int64_t),     intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size(h, 1)*size(h, 2)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_double_2d
    
    subroutine ezcu_mem_wrap_double_2d_allocatable(h, d, flags)
        real(kind=8), allocatable, target, intent(in) :: h(:,:)
        type(ezcu_dev_t),            pointer, intent(in) :: d
        integer(kind=c_int64_t),           intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size(h, 1)*size(h, 2)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_double_2d_allocatable

    !! 3d
    subroutine ezcu_mem_wrap_int32_3d(h, d, size_x, size_y, size_z, flags)
        integer,                 intent(in) :: size_x
        integer,                 intent(in) :: size_y
        integer,                 intent(in) :: size_z
        integer(kind=4), target ,intent(in) :: h(size_x, size_y, size_z)
        type(ezcu_dev_t),  pointer, intent(in) :: d
        integer(kind=c_int64_t), intent(in) :: flags
        integer(kind=c_size_t)              :: s8
        type(c_ptr) :: tmp
        s8  = size_x*size_y*size_z
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_int32_3d

    subroutine ezcu_mem_wrap_int32_3d_allocatable(h, d, flags)
        integer(kind=4), allocatable, target ,intent(in) :: h(:,:,:)
        type(ezcu_dev_t),               pointer, intent(in) :: d
        integer(kind=c_int64_t),              intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size(h, 1)*size(h, 2)*size(h, 3)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_int32_3d_allocatable
    
    subroutine ezcu_mem_wrap_int64_3d(h, d, size_x, size_y, size_z, flags)
        integer,                 intent(in) :: size_x
        integer,                 intent(in) :: size_y
        integer,                 intent(in) :: size_z
        integer(kind=8), target, intent(in) :: h(size_x, size_y, size_z)
        type(ezcu_dev_t),  pointer, intent(in) :: d
        integer(kind=c_int64_t), intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size_x*size_y*size_z
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_int64_3d

    subroutine ezcu_mem_wrap_int64_3d_allocatable(h, d, flags)
        integer(kind=8), allocatable, target, intent(in) :: h(:,:,:)
        type(ezcu_dev_t),               pointer, intent(in) :: d
        integer(kind=c_int64_t),              intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size(h, 1)*size(h, 2)*size(h, 3)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_int64_3d_allocatable

    subroutine ezcu_mem_wrap_float_3d(h, d, size_x, size_y, size_z, flags)
        integer,                  intent(in) :: size_x
        integer,                  intent(in) :: size_y
        integer,                  intent(in) :: size_z
        real(kind=4),     target, intent(in) :: h(size_x, size_y, size_z)
        type(ezcu_dev_t),   pointer, intent(in) :: d
        integer(kind=c_int64_t),  intent(in) :: flags
        integer(kind=c_size_t)               :: s8
        type(c_ptr) :: tmp
        s8  = size_x*size_y*size_z
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_float_3d

    subroutine ezcu_mem_wrap_float_3d_allocatable(h, d, flags)
        real(kind=4), allocatable, target, intent(in) :: h(:,:,:)
        type(ezcu_dev_t),            pointer, intent(in) :: d
        integer(kind=c_int64_t),           intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size(h, 1)*size(h, 2)*size(h, 3)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_float_3d_allocatable

    subroutine ezcu_mem_wrap_double_3d(h, d, &
                                       size_x, size_y, size_z, flags)
        integer,                 intent(in) :: size_x
        integer,                 intent(in) :: size_y
        integer,                 intent(in) :: size_z
        real(kind=8),    target, intent(in) :: h(size_x, size_y, size_z)
        type(ezcu_dev_t),  pointer, intent(in) :: d
        integer(kind=c_int64_t), intent(in) :: flags
        integer(kind=c_size_t)              :: s8
        type(c_ptr) :: tmp
        s8  = size_x*size_y*size_z
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_double_3d

    subroutine ezcu_mem_wrap_double_3d_allocatable(h, d, flags)
        real(kind=8), allocatable, target, intent(in) :: h(:, :, :)
        type(ezcu_dev_t),            pointer, intent(in) :: d
        integer(kind=c_int64_t),           intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size(h, 1)*size(h, 2)*size(h, 3)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_double_3d_allocatable

    !! 4d
    subroutine ezcu_mem_wrap_int32_4d(h, d, size_x, size_y, &
                                      size_z, size_w, flags)
        integer,                 intent(in) :: size_x
        integer,                 intent(in) :: size_y
        integer,                 intent(in) :: size_z
        integer,                 intent(in) :: size_w
        integer(kind=4), target ,intent(in) :: h(size_x, size_y, &
                                                 size_z, size_w)
        type(ezcu_dev_t),  pointer, intent(in) :: d
        integer(kind=c_int64_t), intent(in) :: flags
        integer(kind=c_size_t)              :: s8
        type(c_ptr) :: tmp
        s8  = size_x*size_y*size_z*size_w
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_int32_4d

    subroutine ezcu_mem_wrap_int32_4d_allocatable(h, d, flags)
        integer(kind=4), allocatable, target ,intent(in) :: h(:,:,:,:)
        type(ezcu_dev_t),               pointer, intent(in) :: d
        integer(kind=c_int64_t),              intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size(h, 1)*size(h, 2)*size(h, 3)*size(h, 4)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_int32_4d_allocatable

    subroutine ezcu_mem_wrap_int64_4d(h, d, size_x, size_y, &
                                            size_z, size_w, flags)
        integer,                 intent(in) :: size_x
        integer,                 intent(in) :: size_y
        integer,                 intent(in) :: size_z
        integer,                 intent(in) :: size_w
        integer(kind=8), target, intent(in) :: h(size_x, size_y, &
                                                 size_z, size_w)
        type(ezcu_dev_t),  pointer, intent(in) :: d
        integer(kind=c_int64_t), intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size_x*size_y*size_z*size_w
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_int64_4d

    subroutine ezcu_mem_wrap_int64_4d_allocatable(h, d, flags)
        integer(kind=8), allocatable, target, intent(in) :: h(:,:,:,:)
        type(ezcu_dev_t),               pointer, intent(in) :: d
        integer(kind=c_int64_t),              intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size(h, 1)*size(h, 2)*size(h, 3)*size(h, 4)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_int64_4d_allocatable

    subroutine ezcu_mem_wrap_float_4d(h, d, size_x, size_y, &
                                            size_z, size_w, flags)
        integer,                  intent(in) :: size_x
        integer,                  intent(in) :: size_y
        integer,                  intent(in) :: size_z
        integer,                  intent(in) :: size_w
        real(kind=4),     target, intent(in) :: h(size_x, size_y, &
                                                  size_z, size_w)
        type(ezcu_dev_t),   pointer, intent(in) :: d
        integer(kind=c_int64_t),  intent(in) :: flags
        integer(kind=c_size_t)               :: s8
        type(c_ptr) :: tmp
        s8  = size_x*size_y*size_z*size_w
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_float_4d

    subroutine ezcu_mem_wrap_float_4d_allocatable(h, d, flags)
        real(kind=4), allocatable, target, intent(in) :: h(:,:,:,:)
        type(ezcu_dev_t),            pointer, intent(in) :: d
        integer(kind=c_int64_t),           intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size(h, 1)*size(h, 2)*size(h, 3)*size(h, 4)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_float_4d_allocatable

    subroutine ezcu_mem_wrap_double_4d(h, d, &
                                       size_x, size_y, size_z, size_w, flags)
        integer,                 intent(in) :: size_x
        integer,                 intent(in) :: size_y
        integer,                 intent(in) :: size_z
        integer,                 intent(in) :: size_w
        real(kind=8),    target, intent(in) :: h(size_x, size_y, &
                                                 size_z, size_w)
        type(ezcu_dev_t),  pointer, intent(in) :: d
        integer(kind=c_int64_t), intent(in) :: flags
        integer(kind=c_size_t)              :: s8
        type(c_ptr) :: tmp
        s8  = size_x*size_y*size_z*size_w
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_double_4d

    subroutine ezcu_mem_wrap_double_4d_allocatable(h, d, flags)
        real(kind=8), allocatable, target, intent(in) :: h(:,:,:,:)
        type(ezcu_dev_t),            pointer, intent(in) :: d
        integer(kind=c_int64_t),           intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size(h, 1)*size(h, 2)*size(h, 3)*size(h, 4)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h), s8, flags)
    end subroutine ezcu_mem_wrap_double_4d_allocatable

    !!
    !! ezcu_mem_wrap_ptr
    !! 

    !! 1d
    subroutine ezcu_mem_wrap_int32_1d_pointer(h, d, flags)
        integer(kind=4), pointer, dimension(:), intent(in) :: h
        type(ezcu_dev_t),                 pointer, intent(in) :: d
        integer(kind=c_int64_t),                intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size(h, 1)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h(lbound(h, 1))), s8, flags)
    end subroutine ezcu_mem_wrap_int32_1d_pointer

    subroutine ezcu_mem_wrap_int64_1d_pointer(h, d, flags)
        integer(kind=8), pointer, dimension(:), intent(in) :: h
        type(ezcu_dev_t),                 pointer, intent(in) :: d
        integer(kind=c_int64_t),                intent(in) :: flags
        integer(kind=c_size_t)                             :: s8
        type(c_ptr) :: tmp
        s8  = size(h, 1)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h(lbound(h, 1))), s8, flags)
    end subroutine ezcu_mem_wrap_int64_1d_pointer

    subroutine ezcu_mem_wrap_float_1d_pointer(h, d, flags)
        real(kind=4), pointer, dimension(:), intent(in) :: h
        type(ezcu_dev_t),              pointer, intent(in) :: d
        integer(kind=c_int64_t),             intent(in) :: flags
        integer(kind=c_size_t)                          :: s8
        type(c_ptr) :: tmp
        s8  = size(h, 1)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h(lbound(h, 1))), s8, flags)
    end subroutine ezcu_mem_wrap_float_1d_pointer

    subroutine ezcu_mem_wrap_double_1d_pointer(h, d, flags)
        real(kind=8), pointer, dimension(:), intent(in) :: h
        type(ezcu_dev_t),              pointer, intent(in) :: d
        integer(kind=c_int64_t),             intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size(h, 1)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h(lbound(h, 1))), s8, flags)
    end subroutine ezcu_mem_wrap_double_1d_pointer

    !! 2d
    subroutine ezcu_mem_wrap_int32_2d_pointer(h, d, flags)
        integer(kind=4), pointer, dimension(:,:), intent(in) :: h
        type(ezcu_dev_t),                   pointer, intent(in) :: d
        integer(kind=c_int64_t),                  intent(in) :: flags
        integer(kind=c_size_t)                               :: s8
        type(c_ptr) :: tmp
        s8  = size(h, 1)*size(h, 2)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h(lbound(h, 1), &
                                                lbound(h, 2))), s8, flags)
    end subroutine ezcu_mem_wrap_int32_2d_pointer

    subroutine ezcu_mem_wrap_int64_2d_pointer(h, d, flags)
        integer(kind=8), pointer, dimension(:,:), intent(in) :: h
        type(ezcu_dev_t),                   pointer, intent(in) :: d
        integer(kind=c_int64_t),                  intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size(h, 1)*size(h, 2)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h(lbound(h, 1), &
                                                lbound(h, 2))), s8, flags)
    end subroutine ezcu_mem_wrap_int64_2d_pointer

    subroutine ezcu_mem_wrap_float_2d_pointer(h, d, flags)
        real(kind=4), pointer, dimension(:,:), intent(in) :: h
        type(ezcu_dev_t),                pointer, intent(in) :: d
        integer(kind=c_int64_t),               intent(in) :: flags
        integer(kind=c_size_t)                            :: s8
        type(c_ptr) :: tmp
        s8  = size(h, 1)*size(h, 2)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h(lbound(h, 1), &
                                                lbound(h, 2))), s8, flags)
    end subroutine ezcu_mem_wrap_float_2d_pointer

    subroutine ezcu_mem_wrap_double_2d_pointer(h, d, flags)
        real(kind=8), pointer, dimension(:,:), intent(in) :: h
        type(ezcu_dev_t),                pointer, intent(in) :: d
        integer(kind=c_int64_t),               intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size(h, 1)*size(h, 2)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h(lbound(h, 1), &
                                                lbound(h, 2))), s8, flags)
    end subroutine ezcu_mem_wrap_double_2d_pointer


    !! 3d
    subroutine ezcu_mem_wrap_int32_3d_pointer(h, d, flags)
        integer(kind=4), pointer, dimension(:,:,:), intent(in) :: h
        type(ezcu_dev_t),                     pointer, intent(in) :: d
        integer(kind=c_int64_t),                    intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size(h, 1)*size(h, 2)*size(h, 3)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h(lbound(h, 1), &
                                                lbound(h, 2), &
                                                lbound(h, 3))), s8, flags)
    end subroutine ezcu_mem_wrap_int32_3d_pointer

    subroutine ezcu_mem_wrap_int64_3d_pointer(h, d, flags)
        integer(kind=8), pointer, dimension(:,:,:), intent(in) :: h
        type(ezcu_dev_t),                     pointer, intent(in) :: d
        integer(kind=c_int64_t),                    intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr)            :: tmp
        s8  = size(h, 1)*size(h, 2)*size(h, 3)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h(lbound(h, 1), &
                                                lbound(h, 2), &
                                                lbound(h, 3))), s8, flags)
    end subroutine ezcu_mem_wrap_int64_3d_pointer

    subroutine ezcu_mem_wrap_float_3d_pointer(h, d, flags)
        real(kind=4), pointer, dimension(:,:,:), intent(in) :: h
        type(ezcu_dev_t),                  pointer, intent(in) :: d
        integer(kind=c_int64_t),                 intent(in) :: flags
        integer(kind=c_size_t)                              :: s8
        type(c_ptr)                                         :: tmp
        s8  = size(h, 1)*size(h, 2)*size(h, 3)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h(lbound(h, 1), &
                                                lbound(h, 2), &
                                                lbound(h, 3))), s8, flags)
    end subroutine ezcu_mem_wrap_float_3d_pointer

    subroutine ezcu_mem_wrap_double_3d_pointer(h, d, flags)
        real(kind=8), pointer, dimension(:,:,:), intent(in) :: h
        type(ezcu_dev_t),                  pointer, intent(in) :: d
        integer(kind=c_int64_t),                 intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr)            :: tmp
        s8  = size(h, 1)*size(h, 2)*size(h, 3)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h(lbound(h, 1), &
                                                lbound(h, 2), &
                                                lbound(h, 3))), s8, flags)
    end subroutine ezcu_mem_wrap_double_3d_pointer

    !! 4d
    subroutine ezcu_mem_wrap_int32_4d_pointer(h, d, flags)
        integer(kind=4), pointer, dimension(:,:,:,:), intent(in) :: h
        type(ezcu_dev_t),                       pointer, intent(in) :: d
        integer(kind=c_int64_t),                      intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr) :: tmp
        s8  = size(h, 1)*size(h, 2)*size(h, 3)*size(h, 4)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h(lbound(h, 1), &
                                                lbound(h, 2), &
                                                lbound(h, 3), &
                                                lbound(h, 4))), s8, flags)
    end subroutine ezcu_mem_wrap_int32_4d_pointer

    subroutine ezcu_mem_wrap_int64_4d_pointer(h, d, flags)
        integer(kind=8), pointer, dimension(:,:,:,:), intent(in) :: h
        type(ezcu_dev_t),                       pointer, intent(in) :: d
        integer(kind=c_int64_t),                      intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr)            :: tmp
        s8  = size(h, 1)*size(h, 2)*size(h, 3)*size(h, 4)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h(lbound(h, 1), &
                                                lbound(h, 2), &
                                                lbound(h, 3), &
                                                lbound(h, 4))), s8, flags)
    end subroutine ezcu_mem_wrap_int64_4d_pointer

    subroutine ezcu_mem_wrap_float_4d_pointer(h, d, flags)
        real(kind=4), pointer, dimension(:,:,:,:), intent(in) :: h
        type(ezcu_dev_t),                    pointer, intent(in) :: d
        integer(kind=c_int64_t),                   intent(in) :: flags
        integer(kind=c_size_t)                                :: s8
        type(c_ptr)                                           :: tmp
        s8  = size(h, 1)*size(h, 2)*size(h, 3)*size(h, 4)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h(lbound(h, 1), &
                                                lbound(h, 2), &
                                                lbound(h, 3), &
                                                lbound(h, 4))), s8, flags)
    end subroutine ezcu_mem_wrap_float_4d_pointer

    subroutine ezcu_mem_wrap_double_4d_pointer(h, d, flags)
        real(kind=8), pointer, dimension(:,:,:,:), intent(in) :: h
        type(ezcu_dev_t),                    pointer, intent(in) :: d
        integer(kind=c_int64_t),                   intent(in) :: flags
        integer(kind=c_size_t) :: s8
        type(c_ptr)            :: tmp
        s8  = size(h, 1)*size(h, 2)*size(h, 3)*size(h, 4)
        tmp = c_ezcu_mem_wrap(c_loc(d), c_loc(h(lbound(h, 1), &
                                                lbound(h, 2), &
                                                lbound(h, 3), &
                                                lbound(h, 4))), s8, flags)
    end subroutine ezcu_mem_wrap_double_4d_pointer

end module m_ezcu_mem_wrap
