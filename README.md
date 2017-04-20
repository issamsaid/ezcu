# ezCU 

<b>ezCU</b> is a C/C++ and Fortran wrapper that makes it easier to use CUDA
for scientific computing. Writing a CUDA code evolves including multiple 
lines of host code, which often requires additional efforts and coding 
skills, especially in the case of big projects with lots of legacy code. 
In scientific computing, host code is usually cumbersome
in such a manner scientists would spend more time putting 
together the host code rather than focusing on accelerating their workloads
on GPUs (or other CUDA capable hardware, if any).<br/>
<b>ezCU</b> extensively reduces the need to focus on the host code and offers 
a set of functionalities in C/C++ and Fortran to help efficiently exploit 
hardware accelerators for scientific computing.<br/>
<b>ezCU</b> offers a transparent way to manage memory objects on different
hardware accelerators with different memory models thanks to a set of bitwise 
flags.

# Content
<!-- MarkdownTOC depth=2 -->

- Getting started
    - Branches and cloning
    - Setting up
    - Dependencies
    - Building the C/C++ interface
    - Building the Fortran interface
    - Building the unit tests
    - Building the examples
    - Generating the documentation
    - Installing
- Using ezCU
- How to contribute
- License
- Contact

<!-- /MarkdownTOC -->

# Getting started
The following section is a step by step guide that will take you from fetching
the source code from the repository branches to running your <b>ezCU</b> first 
examples on your machine.

## Branches and cloning
The project contains four git main branches: **master**, **develop**, 
**feature/runtime_api** and **feature/driver_api**. 
The **master** branch only contains the major releases, and 
is intended to use the library as is.
We recommend to clone from this branch if you would like to use 
the latest stable version. 
The releases are tagged on the master branch and each version has a major
number and a minor number which are used as the tagging string (.e.g. the 
first release is tagged 1.0 on the master branch).
Cloning the master branch and checking out the latest release can
be done as follows:
```
git clone -b master https://github.com/issamsaid/ezcu.git
```
If you wish to clone a specific release (here we use the 1.0 release as
an example) you may add:
```
pushd ezCU
git checkout tags/1.0
popd
``` 
The following table summarizes the different details about all the 
releases of the <b>ezCU</b> library:</br>

Release number (tag)  | Date         | Description                                    
--------------------- | ------------ | -----------------------------------------------
1.0                   | 19/04/2017   | The initial release of the <b>ezCU</b> library

On the other hand, the **develop** branch contains the latest builds and is
intended to be used by the developers who are willing to contribute or improve 
the library. Two variants of the library are available, the first one is based on the CUDA
runtime API (see the branch **feature/runtime_api**) and the second one  is built on top of the 
CUDA driver API (see the branch **feature/driver_api**). The latter is considered the default
since for the time being we are experiencing some limitations in terms of C to Fortran interoperability
with the first one.

To get started, you can clone on of these branches as follows:
```
git clone -b develop https://github.com/issamsaid/ezcu.git
git clone -b feature/runtime_api https://github.com/issamsaid/ezcu.git
git clone -b feature/driver_api https://github.com/issamsaid/ezcu.git
```

## Setting up
The <b>ezCU</b> project has multiple components, each in a subdirectory of the
root directory (ezCU):
- The [src](https://github.com/issamsaid/ezcu/tree/master/src) subdirectory is the C/C++ interface.
- The [fortran_interface](https://github.com/issamsaid/ezcu/tree/master/fortran_interface) subdirectory is the Fortran interface.
- The [test](https://github.com/issamsaid/ezcu/tree/master/test) subdirectory contains the unit tests of the library. 
- The [doc](https://github.com/issamsaid/ezcu/tree/master/doc) subdirectory is 
 where the documentation of the library is to be generated.
- The [examples](https://github.com/issamsaid/ezcu/tree/master/examples) includes a set of examples of how to use the library.

The project compilation framework is to be setup using the 
[cmake](https://cmake.org/) utility. Depending on your operating system
you may choose a specific [cmake generator](https://cmake.org/cmake/help/v3.0/manual/cmake-generators.7.html) to build the project.
As an example, if you wish to build <b>ezCU</b> on Unix based operating
systems you can use the following (the rest of the examples in this material 
are also intended to be used on Unix based systems):
```
pushd ezCU
mkdir build
pushd build
cmake -G"Unix Makefiles" ../
popd
```
The current version of <b>ezCU</b> had been tested on various Linux 
distributions with the GNU, Cray and Intel compilers. 
Nevertheless, if you face issues with other compilers you are kindly invited to report them.
Note that if you are using Cray compilers you have to specify where the 
Fortran compiler is wrapped. For example if you are using `ftn` you have to add:
```
pushd ezCU
mkdir build
pushd build
cmake -DCMAKE_Fortran_COMPILER=ftn -G"Unix Makefiles" ../
popd
```
**Important note** 
the compute capability of the target GPU should be informed as following:
```
cmake -DEZCU_CC=X -G"Unix Makefiles" ../ \\ XX=20 or 30 or 35 or .... 60
```
Otherwise the default value is 20 which might be outdated in some cases.
In addition if you are using ezCU you should always compile your kernels with 
the nvcc flags ```-gencode arch=compute_XX,code=compute_XX -ptx``` (XX being
the compute capability) since ezCU loads the generated ptx files on runtime.

## Dependencies
It goes without saying that <b>ezCU</b> depends on CUDA.
If your installed CUDA implementation is not found by `cmake` 
 where we try to look for your implementation in the usual install directories, 
 you can help setting it manually as follows:
```
export CUDA_INCLUDE_DIR="YOUR_CUDA_HEADERS_PATH"
export CUDA_LIBRARY_DIR="YOUR_CUDA_LIBRARY_PATH"
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CUDA_LIBRARY_DIR
export LIBRARY_PATH=$LIBRARY_PATH:$CUDA_LIBRARY_DIR
```
Besides, <b>ezCU</b> uses internally the 
[ulist](https://github.com/issamsaid/ulist/tree/master) and
[urb_tree](https://github.com/issamsaid/urb_tree/tree/master) libraries (
respectively a generic linked list library and a red-black trees generic
libraries) in order to track and efficiently manipulate 
the CUDA resources. It also relies on the 
[googletest](https://github.com/google/googletest/) framework for unit 
testing.
Those libraries are automatically downloaded, compiled and installed when
building <b>ezCU</b>. Alternatively you can set the path to those libraries
if they are already installed on your system as follows:
```
export ULIST_DIR="YOUR_PATH_TO_ULIST"
export URB_TREE_DIR="YOUR_PATH_TO_URB_TREE"
export GTEST_DIR="YOUR_PATH_TO_GTEST"
```

## Building the C/C++ interface
To build the <b>ezCU</b> C static library you can run the default Makefile 
target as follows:
```
pushd build
make ezcu 
popd 
```
This Makefile target will build the static library `libezCU.a` from the C/C++ 
source files in the [src](https://github.com/issamsaid/ezcu/tree/master/src)
subdirectory. 
Since this is the default target you can also build the static C library
as follows:
```
pushd build
make  
popd 
```

## Building the Fortran interface
If you would like to build the Fortran interface additionally, 
you can do so as follows:
```
pushd build
make ezcu_fortran
popd
```
This target will build another static library `libezCU_fortran.a` from the
Fortran source files present in the 
[fortran_interface](https://github.com/issamsaid/ezcu/tree/master/fortran_interface) subdirectory. Note that the Fortran interface is only
an additional layer based on the Fortran 2003 standard (ISO/IEC 1539-1:2004(E)),
which generates procedure and derived-type declarations and global variables that are interoperable with C. Therefor, if the C/C++ interface is not built this target will build it as well.

## Building the unit tests
The library comes with a set of unit tests and performance 
tests to validate the new features. You can check the unit testing 
directory [here](https://github.com/issamsaid/ezcu/tree/master/test).
The testing framework is used to thoroughly test <b>ezCU</b> in C/C++ 
([test/src](https://github.com/issamsaid/ezcu/tree/master/test/src)) and 
Fortran ([test/fortran](https://github.com/issamsaid/ezcu/tree/master/test/fortran_interface)). The C/C++ interface unit tests are based on top of the 
[googletest](https://github.com/google/googletest/) Framework). To build
all the unit tests, C/C++ and Fortran included, you can invoke the 
following target:
```
pushd build
make build_tests
popd
```
Alternatively `make ezcu_test` will only build the test suit for the C/C++ interface, and `make ezcu_test_fortran` will build the unit tests for the
Fortran interface.
Tests should be written for any new code, and changes should be verified to not 
break existing tests before they are submitted for review. 

## Building the examples
The project comes with a set of C/C++ and Fortran samples that you can browse in the   
[examples](https://github.com/issamsaid/ezcu/tree/master/examples) subdirectory. 
Those can be built as follows:
```
pushd build
make build_examples
pupd
```
Alternatively `make c_examples` will only build and 
install the C/C++ examples, and `make fortran_examples` will build and install the Fortran examples.

## Generating the documentation
The documentation of the library can be generated, in the [doc](https://github.com/issamsaid/ezcu/tree/master/doc) subdirectory,
with the help of [doxygen](http://www.stack.nl/~dimitri/doxygen/) by simply running:
```
pushd build
make doc
popd
```

## Installing 
In order to install the <b>ezCU</b> project you can invoke the classic 
Makefile install target:
```
pushd build
make install
popd
```
This target mainly installs the <b>ezCU</b> C/C++ static library in the `lib` subdirectory on the project root directory. If the Fortran static library, the unit tests binaries and the examples binaries are built, they will be installed 
respectively in the `lib`, `test/bin` and `examples/bin` subdirectories.


# Using ezCU
In order to use the <b>ezCU</b> C/C++ link your code against libezCU.a 
additionally to the CUDA library (by adding 
`-lezCU -lcuda` to your linker options), 
however if your code is based on Fortran the 
latter should linked against both the C/C++ library and the Fortran interface (
with the help of the options `-lezCU_fortran -lezCU -lcuda`).<br/>

To perform the unit tests you can run:
```
pushd test
./bin/ezcu_test         // for C/C++
./bin/ezcu_test_fortran // for Fortran
popd
```
The examples binaries can be browsed in the `examples/bin` subdirectory.

It is now up to you to read the documentation and check the examples in order 
to use <b>ezCU</b> to write your own CUDA codes for scientific purposes.
Using the library only requires adding few lines to your original code.

# How to contribute
We believe that <b>ezCU</b> can be used by scientific programmers very 
efficiently. We tend to extend the functionalities of the library. For this to 
do we need your feedbacks and proposals of features and use cases.
If you are willing to contribute please visit the contributors guide
[CONTRIBUTING](https://github.com/issamsaid/ezcu/tree/master/CONTRIBUTING.md),
or feel free to contact us.

# License
<b>ezCU</b> is a free software licensed under 
[BSD](https://github.com/issamsaid/ezcu/tree/master/LICENSE.md).


# Contact
For bug report, feature requests or if you willing to contribute please 
feel free to contact Issam SAID by dropping a line to said.issam@gmail.com.
