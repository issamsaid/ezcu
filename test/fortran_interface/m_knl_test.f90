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
!! @file m_knl_test.f90
!! @author Issam SAID
!! @brief Unit testing file for the ezcu Fortran interface for CUDA
!! kernels manipulation routines.
!<
module m_knl_test
    use iso_c_binding
    use m_ezcu_flags
    use m_ezcu_types
    use m_ezcu_base
    use m_ezcu_mem
    use m_ezcu_dev
    use m_ezcu_knl
    use m_ezcu_util
    use m_handler
    
    implicit none

    private

    type(ezcu_dev_t),  pointer :: d
    integer(kind=4), parameter :: N = 2048  

    public :: knl_test
    
contains

    logical function set_wrk() result(status)
        use, intrinsic :: iso_c_binding
        implicit none
        integer, dimension(3) :: grid  = (/N/64, 1, 1/)
        integer, dimension(3) :: block = (/64, 1, 1/)
        call ezcu_knl_set_wrk("test_knl_1", 1, grid, block);
        status = .true.
    end function set_wrk

    logical function set_arg() result(status)
        implicit none
        integer, parameter :: i = 9
        call ezcu_knl_set("test_knl_1", 2, i)
        status = .true.
    end function set_arg

    logical function sync_run() result(status)
        implicit none
        integer                       :: i
        integer(kind=4), dimension(3) :: grid = (/N/64, 1, 1/)
        integer(kind=4), dimension(3) :: block = (/64, 1, 1/)
        real, allocatable :: src(:) 
        real, allocatable :: dst(:) 

        allocate(src(N), dst(N))
        src = 1.2
        
        call ezcu_dev_find(DEFAULT, d)
        call ezcu_mem_wrap(src, d, ior(HWA, READ_ONLY))
        call ezcu_mem_wrap(dst, d, ior(HWA, WRITE_ONLY))
        call ezcu_mem_update(src, WRITE_ONLY)
        status = .true.
        call ezcu_knl_set_wrk("test_knl_1", 1, grid, block)
        call ezcu_knl_set("test_knl_1", 0, src)
        call ezcu_knl_set("test_knl_1", 1, dst)
        call ezcu_knl_set("test_knl_1", 0, src)
        call ezcu_knl_set("test_knl_1", 1, dst)
        call ezcu_knl_set("test_knl_1", 2, 64)
        call ezcu_knl_sync_exec("test_knl_1", d);
        call ezcu_mem_update(dst, READ_ONLY)
        do i=1,N
            status = status .and. (abs(dst(i) - 1.2) < 1.e-4)
        end do
        deallocate(src, dst)
    end function sync_run

    logical function async_run() result(status)
        implicit none
        integer                       :: i
        integer(kind=4), dimension(3) :: grid  = (/N/64, 1, 1/)
        integer(kind=4), dimension(3) :: block = (/64, 1, 1/)
        real, allocatable :: src(:) 
        real, allocatable :: dst(:) 

        allocate(src(N), dst(N))
        src = 1.2

        call ezcu_dev_find(DEFAULT, d)
        call ezcu_mem_wrap(src, d, ior(HWA, READ_ONLY))
        call ezcu_mem_wrap(dst, d, ior(HWA, WRITE_ONLY))
        call ezcu_mem_update(src, WRITE_ONLY)
        status = .true.
        call ezcu_knl_set_wrk("test_knl_1", 1, grid, block)
        call ezcu_knl_set("test_knl_1", 0, src)
        call ezcu_knl_set("test_knl_1", 1, dst)
        call ezcu_knl_set("test_knl_1", 2, 64)
        call ezcu_knl_exec("test_knl_1", d);
        call ezcu_mem_update(dst, READ_ONLY)
        do i=1,N
            status = status .and. (abs(dst(i) - 1.2) < 1.e-4)
        end do
        deallocate(src, dst)
    end function async_run

    logical function sync_run_hzc() result(status)
        implicit none
        integer                       :: i
        integer(kind=4), dimension(3) :: grid  = (/N/64, 1, 1/)
        integer(kind=4), dimension(3) :: block = (/64, 1, 1/)
        real,             allocatable :: src(:) 
        real,             allocatable :: dst(:) 
 
        allocate(src(N), dst(N))
        src = 1.2
       
        call ezcu_dev_find(DEFAULT, d)
        call ezcu_mem_wrap(src, d, ior(ior(HOST, ZERO_COPY), READ_WRITE))
        call ezcu_mem_wrap(dst, d, ior(ior(HOST, ZERO_COPY), WRITE_ONLY))
        call ezcu_mem_update(src, WRITE_ONLY)

        call ezcu_knl_set_wrk("test_knl_1", 1, grid, block)
        call ezcu_knl_set("test_knl_1", 0, src)
        call ezcu_knl_set("test_knl_1", 1, dst)
        call ezcu_knl_set("test_knl_1", 2, 64)
        call ezcu_knl_sync_exec("test_knl_1", d);
        call ezcu_mem_update(dst, READ_ONLY)
        status = .true.
        do i=1,N
            status = status .and. (abs(dst(i) - 1.2) < 1.e-4)
        end do
        deallocate(src, dst)
    end function sync_run_hzc

    logical function sync_run_pinned() result(status)
        implicit none
        integer                       :: i
        integer(kind=4), dimension(3) :: grid = (/N/64, 1, 1/)
        integer(kind=4), dimension(3) :: block = (/64, 1, 1/)

        real, allocatable :: src(:) 
        real, allocatable :: dst(:)

        allocate(src(N))
        allocate(dst(N))
        src = 1.2
        
        call ezcu_dev_find(DEFAULT, d)
        call ezcu_mem_wrap(src, d, ior(ior(HWA, PINNED), READ_ONLY))
        call ezcu_mem_wrap(dst, d, ior(ior(HWA, PINNED), WRITE_ONLY))

        call ezcu_knl_set_wrk("test_knl_1", 1, grid, block)
        call ezcu_knl_set("test_knl_1", 0, src)
        call ezcu_knl_set("test_knl_1", 1, dst)
        call ezcu_knl_set("test_knl_1", 2, 64)
        call ezcu_knl_sync_exec("test_knl_1", d);
        call ezcu_mem_update(dst, READ_ONLY)
        status = .true.
        do i=1,N
            status = status .and. (abs(dst(i) - 1.2) < 1.e-4)
        end do
        deallocate(src, dst)
    end function sync_run_pinned

    subroutine init_buffer(buff, nt)
        integer,   intent(in) :: nt 
        real,   intent(inout) :: buff(:)
        real    :: dt, lam, pi, t
        integer :: k
        pi  = 4*atan(1.)
        lam = pi*pi*15.*15.
        dt  = 0.00248
        do k=1, nt
            t       = -0.2+(k-1)*dt
            buff(k) = -2.*lam*(2.*lam*t*t-1)*exp(-lam*t*t)
        enddo
    end subroutine init_buffer

    logical function test_precision_one_thread() result(status)
        implicit none
        real,  allocatable :: src(:) 
        real,  allocatable :: fld(:,:) 
        real,  allocatable :: cpu(:,:) 
        integer(kind=4), dimension(3), parameter :: grid  = (/1, 1, 1/)
        integer(kind=4), dimension(3), parameter :: block = (/1, 1, 1/)
        
        integer            :: i, nt, n

        nt = 4925
        n  = 16

        allocate(src(nt))
        allocate(fld(-3:n+4,-3:n+4))
        allocate(cpu(-3:n+4,-3:n+4))
        
        fld = 0.
        cpu = 0.
        call init_buffer(src, nt)

        call ezcu_dev_find(DEFAULT, d)
        call ezcu_mem_wrap(src, d, ior(HWA, READ_ONLY))
        call ezcu_mem_wrap(fld, d, ior(HWA, READ_WRITE))
                
        call ezcu_knl_set_wrk("add_source_2d", 1, grid, block)
        call ezcu_knl_set("add_source_2d", 0, 10)
        call ezcu_knl_set("add_source_2d", 1, 10)
        call ezcu_knl_set("add_source_2d", 2, 11)
        call ezcu_knl_set("add_source_2d", 3, 11)
        call ezcu_knl_set("add_source_2d", 4, 0.25)
        call ezcu_knl_set("add_source_2d", 5, 0.)
        call ezcu_knl_set("add_source_2d", 6, n)
        call ezcu_knl_set("add_source_2d", 7, 4)
        call ezcu_knl_set("add_source_2d", 8, 4)
        call ezcu_knl_set("add_source_2d", 9, src)
        call ezcu_knl_set("add_source_2d", 10, 0)
        call ezcu_knl_set("add_source_2d", 11, fld)

        status = .true.
        do i = 1, nt
            call ezcu_knl_set("add_source_2d", 10, i-1)
            call ezcu_knl_sync_exec("add_source_2d", d)
            call ezcu_mem_update(fld, READ_ONLY)

            cpu(11, 11) = cpu(11, 11) + src(i)*(0.75)
            cpu(11, 12) = cpu(11, 12) + src(i)*0.*(1.0-0.25)
            cpu(12, 11) = cpu(12, 11) + src(i)*(1.0-0.)*0.25
            cpu(12, 12) = cpu(12, 12) + src(i)*0.
            status = (abs(cpu(11,11) - fld(11,11)) < 1.e-4)
            if (.not.status) then
                write(*, '(a, i0, a, e20.7, a, e20.7)') "it: ", &
                    i, " gpu ", fld(11, 11), " cpu ", cpu(11, 11)
                write(*, '(a, i0, a, e20.7, a, e20.7)') "it: ", &
                    i, " gpu ", fld(11, 12), " cpu ", cpu(11, 12)
                write(*, '(a, i0, a, e20.7, a, e20.7)') "it: ", &
                    i, " gpu ", fld(12, 11), " cpu ", cpu(12, 11)
                write(*, '(a, i0, a, e20.7, a, e20.7)') "it: ", &
                    i, " gpu ", fld(12, 12), " cpu ", cpu(12, 12)
                write(*, *)
            endif
        enddo
        deallocate(cpu, fld, src)
    end function test_precision_one_thread


    subroutine setup()
        call ezcu_init(ALL)
        call ezcu_dev_find(DEFAULT, d)
        call ezcu_load(PREFIX//"/knl_test.cu")
    end subroutine setup

    subroutine teardown()
        call ezcu_release()
    end subroutine teardown

    subroutine knl_test()
        call run(setup, teardown, set_wrk, &
               "knl_test.set_wrk")
        call run(setup, teardown, set_arg, &
              "knl_test.set_arg")
        call run(setup, teardown, sync_run, &
            "knl_test.sync_run")
        call run(setup, teardown, async_run, &
            "knl_test.async_run")
        call run(setup, teardown, sync_run_hzc, &
            "knl_test.sync_run_hzc")
        call run(setup, teardown, sync_run_pinned, &
            "knl_test.sync_run_pinned")
        call run(setup, teardown, test_precision_one_thread, &
            "knl_test.test_precision_one_thread")
    end subroutine knl_test

end module m_knl_test
