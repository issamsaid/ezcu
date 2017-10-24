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
!! @file m_ezcu_base
!! @author Issam SAID
!! @brief This file implements the Fortran interface of the ezcu main setup 
!! routines.
!! @see ezcu/base.h
!<
module m_ezcu_base
    use, intrinsic :: iso_c_binding
    use m_ezcu_flags
    use m_ezcu_types

    implicit none

    private

    interface
        subroutine c_ezcu_init(flags) bind(c, name = "ezcu_init")
            use, intrinsic :: iso_c_binding, only: c_int64_t
            implicit none
            integer(kind=c_int64_t), intent(in), value :: flags
        end subroutine c_ezcu_init

        subroutine c_ezcu_load(filename, options) &
            bind(c, name = "ezcu_load_fort")
            use, intrinsic :: iso_c_binding, only: c_char
            implicit none
            character(kind=c_char), intent(in) :: filename
            character(kind=c_char), intent(in) :: options
        end subroutine c_ezcu_load
    end interface

    interface
        subroutine ezcu_release() bind(c, name = "ezcu_release")
        end subroutine ezcu_release

        subroutine ezcu_info() bind(c, name = "ezcu_info")
        end subroutine ezcu_info 
    end interface

    public :: ezcu_init
    public :: ezcu_load
    public :: ezcu_release
    public :: ezcu_info

contains
    
    subroutine ezcu_init(flags)
        integer(kind=c_int64_t), optional, intent(in) :: flags
        if (present(flags)) then
            call c_ezcu_init(flags)
        else
            call c_ezcu_init(DEFAULT)
        end if
    end subroutine ezcu_init
    
    subroutine ezcu_load(filename, options)
        character(len=*),           intent(in) :: filename
        character(len=*), optional, intent(in) :: options
        write(*,*) filename
        write(*,*) trim(filename)
        if (present(options)) then
            call c_ezcu_load(trim(filename) // c_null_char, &
                             trim(options) // c_null_char)
        else
            call c_ezcu_load(trim(filename) // c_null_char, c_null_char)
        endif
    end subroutine ezcu_load
    
end module m_ezcu_base
