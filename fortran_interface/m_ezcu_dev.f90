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
!! @file m_ezcu_dev.f90
!! @author Issam SAID
!! @brief This file implements the Fortran interface of the ezcu device 
!! manipulation routines.
!! @see ezcu/dev.h
!<
module m_ezcu_dev
    use, intrinsic :: iso_c_binding
    use m_ezcu_flags
    use m_ezcu_types

    implicit none
    
    private

    interface
        type(c_ptr) &
        function c_ezcu_dev_find(idx) bind(c, name="ezcu_dev_find")
            use iso_c_binding, only: c_ptr, c_int
            integer(kind=c_int), value, intent(in) :: idx
        end function c_ezcu_dev_find

        subroutine c_ezcu_dev_wait(c_dev_ptr) bind(c, name = "ezcu_dev_wait")
            use iso_c_binding, only: c_ptr
            type(c_ptr), value, intent(in) :: c_dev_ptr
        end subroutine c_ezcu_dev_wait

    end interface

    public :: ezcu_dev_find
    public :: ezcu_dev_wait

contains

    subroutine ezcu_dev_find(idx, d)
        integer(kind=4),             value, intent(in) :: idx
        type(ezcu_dev_t),       pointer, intent(inout) :: d
        call c_f_pointer(c_ezcu_dev_find(idx), d)
    end subroutine ezcu_dev_find
    
    subroutine ezcu_dev_wait(d)
        type(ezcu_dev_t), pointer, intent(in) :: d
        call c_ezcu_dev_wait(c_loc(d))
    end subroutine ezcu_dev_wait

end module m_ezcu_dev
