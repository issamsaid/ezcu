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
!! @brief An example of a 3D finite difference stencil code based on 
!! the ezcu Fortran interface.
!!
!<
program stencil_3d_fortran
    use m_ezcu

    integer(kind=4),    parameter :: N = 320
    integer(kind=4)               :: n4

    real,             allocatable :: u(:,:,:) 
    real,             allocatable :: v(:,:,:) 
    real,             allocatable :: coeffs(:) 

    integer(kind=4), dimension(3) :: grid  = (/N/16, N/16, 1/)
    integer(kind=4), dimension(3) :: block = (/16, 16, 1/)

    type(ezcu_dev_t),     pointer :: device

    write(*,*) "... start of the ezcu 3D stencil Fortran example"

    !!
    !!< Initialize ezcu with selecting the default GPU.
    !!
    call ezcu_init()

    !!                 
    !!< Load and build the CUDA kernel from ptx.
    !! 
    call ezcu_load(PREFIX//"/stencil_3d.cu");

    !!
    !!< Get a pointer to the desired device (in this case the default GPU).
    !!
    call ezcu_dev_find(0, device)

    allocate(u(N+8, N+8, N+8))
    allocate(v(N+8, N+8, N+8))
    allocate(coeffs(5))
    u  = 0
    v  = 2.1
    n4 = N

    coeffs(1) = -2.6;
    coeffs(2) =  1.6;
    coeffs(3) = -0.2;
    coeffs(4) =  0.02;
    coeffs(5) = -0.001;

    !!
    !!< Wrap the buffers into ezcu memory objects.
    !!
    call ezcu_mem_wrap(u, device, ior(HWA, READ_WRITE))
    call ezcu_mem_wrap(v, device, ior(HWA, READ_WRITE))
    call ezcu_mem_wrap(coeffs, device, ior(HWA, READ_ONLY))
    
    !!
    !!< Set the work size and the dimensions of the kernel.
    !!
    call ezcu_knl_set_wrk("stencil_3d", 2, grid, block)
    
    !!
    !!< Set the arguments of the kernel.
    !!< Unfortunately variadic functions cannot be used in Fortran, each 
    !!< argument has to be set apart. 
    !!
    call ezcu_knl_set("stencil_3d", 0, v)
    call ezcu_knl_set("stencil_3d", 1, u)
    call ezcu_knl_set("stencil_3d", 2, coeffs)
    call ezcu_knl_set("stencil_3d", 3, n4)
    call ezcu_knl_set("stencil_3d", 4, n4)
    call ezcu_knl_set("stencil_3d", 5, n4)
        
    !! 
    !!< Run the kernel on the default GPU.
    !!
    call ezcu_knl_exec("stencil_3d", device);
    
    !!
    !!< Update the u buffer on the CPU side so that the results can be seen
    !!< on the host side.
    !!
    call ezcu_mem_update(u, READ_ONLY)

    deallocate(u, v, coeffs)

    !!
    !!< Release ezcu resources.
    !!
    call ezcu_release()
    write(*,*) "... end   of the ezcu 3D stencil Fortran example"
end program stencil_3d_fortran
