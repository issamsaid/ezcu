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
!! @file m_dev_test.f90
!! @author Issam SAID
!! @brief Unit testing file for the ezCU Fortran interface for CUDA
!! devices manipulation routines.
!<
module m_dev_test
    use, intrinsic :: iso_c_binding
    use m_ezcu_flags    
    use m_ezcu_types
    use m_ezcu_base
    use m_ezcu_dev
    use m_ezcu_util
    use m_handler
    
    implicit none

    private

    public :: dev_test

contains
    
    logical function find() result(status)
        type(ezcu_dev_t), pointer :: dev0
        call ezcu_dev_find(DEFAULT, dev0)
        status = associated(dev0)
    end function find

    logical function all_devs() result(status)
        type(ezcu_dev_t), pointer :: dev0
        type(ezcu_dev_t), pointer :: dev1
        type(ezcu_dev_t), pointer :: dev2
        call ezcu_dev_find(ALL, dev0);
        call ezcu_dev_find(ior(ALL, FIRST), dev1);
        call ezcu_dev_find(FIRST, dev2);
        status = &
              associated(dev0, dev1) .and. &
              associated(dev1, dev2) .and. &
              associated(dev2, dev0) 
    end function all_devs

    logical function default_dev() result(status)
        type(ezcu_dev_t), pointer :: dev0
        type(ezcu_dev_t), pointer :: dev2
        call ezcu_dev_find(DEFAULT, dev0);
        call ezcu_dev_find(FIRST, dev2);
        status = associated(dev0, dev2)  
    end function default_dev

    logical function gpu_dev() result(status)
        type(ezcu_dev_t), pointer :: dev0
        type(ezcu_dev_t), pointer :: dev1
        if (ezcu_has(GPU)) then
            call ezcu_dev_find(GPU, dev0);
            call ezcu_dev_find(ior(GPU, FIRST), dev1);
            status = associated(dev0, dev1) 
        else
            status = .true.
        endif 
    end function gpu_dev

    subroutine setup()
        call ezcu_init(ALL)
    end subroutine setup

    subroutine teardown()
        call ezcu_release()
    end subroutine teardown

    subroutine dev_test()
        call run(setup, teardown, find, &
              "dev_test.find")
        call run(setup, teardown, all_devs, &
              "dev_test.all_devs")
        call run(setup, teardown, default_dev, &
              "dev_test.default_dev")
        call run(setup, teardown, gpu_dev, &
              "dev_test.gpu_dev")
    end subroutine dev_test

end module m_dev_test