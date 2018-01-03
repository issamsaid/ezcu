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
!! @file m_ezcu_flags.f90
!! @author Issam SAID
!! @brief This file implements the ezcu flags in Fortran.
!! @see ezcu/flags.h
!<
module m_ezcu_flags
    use, intrinsic :: iso_c_binding

    implicit none

    private
    integer(kind=c_int64_t), &
    parameter, public :: DEFAULT     = X'1000000000000000'

    integer(kind=c_int64_t), &
    parameter, public :: HOST          = X'0100000000000000' 
    integer(kind=c_int64_t), &
    parameter, public :: HWA           = X'0200000000000001'
  
    integer(kind=c_int64_t), &
    parameter, public :: PINNED        = X'0010000000000000'
    integer(kind=c_int64_t), &
    parameter, public :: ZERO_COPY     = X'0020000000000001'

    integer(kind=c_int64_t), &
    parameter, public :: READ_ONLY     = X'0001000000000000'
    integer(kind=c_int64_t), &
    parameter, public :: WRITE_ONLY    = X'0002000000000001'
    integer(kind=c_int64_t), &
    parameter, public :: READ_WRITE    = X'0004000000000002'
  
    integer(kind=c_int64_t), &
    parameter, public :: CHAR          = X'0000010000000000'
    integer(kind=c_int64_t), &
    parameter, public :: INT           = X'0000020000000001'
    integer(kind=c_int64_t), &
    parameter, public :: UNSIGNED_INT  = X'0000040000000002'
    integer(kind=c_int64_t), &
    parameter, public :: FLOAT         = X'0000080000000003'
    integer(kind=c_int64_t), &
    parameter, public :: LONG          = X'0000100000000004'
    integer(kind=c_int64_t), &
    parameter, public :: UNSIGNED_LONG = X'0000200000000005'
    integer(kind=c_int64_t), &
    parameter, public :: SIZET         = X'0000400000000006'
    integer(kind=c_int64_t), &
    parameter, public :: DOUBLE        = X'0000800000000007'

end module m_ezcu_flags
