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
!! @file m_base_test.f90
!! @author Issam SAID
!! @brief Unit testing file for the ezCU Fortran interface for the main 
!! library setup routines.
!<
module m_base_test
    use m_ezcu_flags
    use m_ezcu_types
    use m_ezcu_base
    use m_ezcu_util
    use m_handler
    
    implicit none

    private

    public :: base_test
    
contains
    
    logical function get_default() result(status)
        implicit none
        call ezcu_init()
        call ezcu_release()
        status = .true.
    end function get_default

    logical function get_explicit_default() result(status)
        implicit none
        call ezcu_init(DEFAULT)
        call ezcu_release()
        status = .true.
    end function get_explicit_default

    logical function get_nvidia() result(status)
        implicit none
        call ezcu_init(NVIDIA)
        call ezcu_release()
        status = .true.
    end function get_nvidia

    logical function get_first_gpu() result(status)
        implicit none
        call ezcu_init(ior(GPU, FIRST))
        call ezcu_release()
        status = .true.
    end function get_first_gpu
    
    subroutine setup()
    end subroutine setup

    subroutine teardown()
    end subroutine teardown

    subroutine base_test()
        call run(setup, teardown, get_default, &
              "base_test.get_default")
        call run(setup, teardown, get_explicit_default, &
              "base_test.get_explicit_default")
        call run(setup, teardown, get_nvidia, &
               "base_test.get_nvidia")
        call run(setup, teardown, get_first_gpu, &
               "base_test.get_first_gpu")
    end subroutine base_test

end module m_base_test