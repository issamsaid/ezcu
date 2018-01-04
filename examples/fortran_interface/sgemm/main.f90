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
!! 3. Neither the name of the COPYRIGHT HOLDER nor the names of its contributors
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
!! @file sgemm/main.f90
!! @author Issam SAID 
!! @brief An example of a matrix multiplication code based on 
!! the ezcu Fortran interface.
!!
!<
program sgemm_fortran
    use m_ezcu

    integer(kind=4),    parameter :: N = 128

    real,             allocatable :: a(:,:) 
    real,             allocatable :: b(:,:) 
    real,             allocatable :: c(:,:) 

    integer(kind=4), dimension(3) :: grid  = (/N/16, N/16, 1/)
    integer(kind=4), dimension(3) :: block = (/16, 16, 1/)

    type(ezcu_dev_t),     pointer :: device
    integer(kind=4)               :: n4 = N

    write(*,*) "... start of the ezcu sgemm Fortran example"

    !!
    !!< Initialize ezcu with selecting the default GPU.
    !!
    call ezcu_init()

    !!                 
    !!< Load and build the CUDA kernel from ptx.
    !! 
    call ezcu_load(PREFIX//"/sgemm.cu");

    !!
    !!< Get a pointer to the desired device (in this case the default GPU).
    !!
    call ezcu_dev_find(0, device)

    allocate(a(N,N))
    allocate(b(N,N))
    allocate(c(N,N))
    a = 1.2
    b = 2.1
    c = 0.0

    !!
    !!< Wrap the matrices into ezcu memory objects.
    !!
    call ezcu_mem_wrap(a, device, ior(HWA, READ_ONLY))
    call ezcu_mem_wrap(b, device, ior(HWA, READ_ONLY))
    call ezcu_mem_wrap(c, device, ior(HWA, WRITE_ONLY))
    
    !!
    !!< Set the work size and the dimensions of the kernel.
    !!
    call ezcu_knl_set_wrk("sgemm", 2, grid, block)
    call ezcu_knl_set("sgemm", 0, a)
    call ezcu_knl_set("sgemm", 1, b)
    call ezcu_knl_set("sgemm", 2, c)
    call ezcu_knl_set("sgemm", 3, n4)
        
    !! 
    !!< Run the kernel on the default GPU.
    !!
    call ezcu_knl_exec("sgemm", device);
    
    !!
    !!< Update the C matrix on the CPU side so that the results can be seen
    !!< on the host side.
    !!
    call ezcu_mem_update(c, READ_ONLY)

    deallocate(a, b, c)

    !!
    !!< Release ezcu resources.
    !!
    call ezcu_release()
    write(*,*) "... end   of the ezcu sgemm Fortran example"
end program sgemm_fortran
