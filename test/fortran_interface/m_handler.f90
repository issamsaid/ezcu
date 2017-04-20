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
!! @file m_handler.f90
!! @author Issam SAID
!! @brief The ezCU Fortran interface unit testing handler.
!! 
!! @details This file contains a list of Fortran routines used to ease the 
!! unit testing of the ezCU Fortran interface.
!<
module m_handler
    use, intrinsic :: iso_c_binding
    use m_ezcu_flags
    use m_ezcu_types
    use m_ezcu_timer

    implicit none

    private

    public :: handler_start, handler_end, handler_stat
    public :: global_setup, global_teardown, run

    integer                           :: passed = 0
    integer                           :: failed = 0
    character(len=48), dimension(128) :: failed_tests
    real(kind=8)                      :: elapsed
 
contains

    subroutine global_setup()
        implicit none
    end subroutine global_setup

    subroutine global_teardown()
        implicit none
    end subroutine global_teardown

    subroutine run(local_setup, local_teardown, func, name)
        implicit none
        external                     :: local_setup
        external                     :: local_teardown
        logical, external            :: func
        character(len=*), intent(in) :: name
        logical                      :: status
        write (*,"(A, A)") '\x1B[32m[ RUN      ]\x1B[0m ', name
        call global_setup()
        call local_setup()
        status = func()
        call local_teardown()
        call global_teardown()
        if (status .eqv. .true.) then
            write (*,"(A)") '\x1B[32m[       OK ]\x1B[0m'
            passed = passed + 1
        else
            write (*,"(A)") '\x1B[31m[ FAILED   ]\x1B[0m'
            failed = failed + 1
            failed_tests(failed) = name
        end if
        write (*,*)
    end subroutine run

    subroutine handler_start()
        implicit none
        integer(kind(SECONDS)) :: s  = SECONDS
        write (*,"(A)") '\x1B[32m[----------]\x1B[0m'
        call ezcu_timer_uset(s)
        call ezcu_timer_tick()
    end subroutine handler_start

    subroutine handler_end()
        implicit none
        call ezcu_timer_read(elapsed)
        write (*,"(A)") '\x1B[32m[----------]\x1B[0m'
        write (*,*)
    end subroutine handler_end

    subroutine handler_stat()
        implicit none
        integer :: i
        write (*,*)
        write (*,"(A, I3, A, F20.10, A)") '\x1B[32m[==========]\x1B[0m Total of',&
              passed + failed, ' tests ran (', elapsed,' s).'
        if (passed > 0) then
            write (*,"(A, I3, A)") '\x1B[32m[  PASSED  ]\x1B[0m ', &
                  passed, ' tests.'
        end if
        if (failed > 0) then
            write (*,"(A, I3, A)") '\x1B[31m[  FAILED  ]\x1B[0m ', &
                  failed, ' tests:'
            do i=1,failed
               write (*,"(A,I3,A,A,A)") '\x1B[31m', &
                        i, " - ", failed_tests(i),'\x1B[0m'
            end do
        end if
        write (*,*)
    end subroutine handler_stat

end module m_handler
