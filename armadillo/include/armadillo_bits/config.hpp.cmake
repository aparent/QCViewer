// Copyright (C) 2008-2011 NICTA (www.nicta.com.au)
// Copyright (C) 2008-2011 Conrad Sanderson
// 
// This file is part of the Armadillo C++ library.
// It is provided without any warranty of fitness
// for any purpose. You can redistribute this file
// and/or modify it under the terms of the GNU
// Lesser General Public License (LGPL) as published
// by the Free Software Foundation, either version 3
// of the License or (at your option) any later version.
// (see http://www.opensource.org/licenses for more info)



#if !defined(ARMA_USE_LAPACK)
#cmakedefine ARMA_USE_LAPACK
//// Uncomment the above line if you have LAPACK or a fast replacement for LAPACK,
//// such as Intel's MKL, AMD's ACML, or the Accelerate framework.
//// LAPACK is required for matrix decompositions (eg. SVD) and matrix inverse.
#endif

#if !defined(ARMA_USE_BLAS)
#cmakedefine ARMA_USE_BLAS
//// Uncomment the above line if you have BLAS or a fast replacement for BLAS,
//// such as GotoBLAS, Intel's MKL, AMD's ACML, or the Accelerate framework.
//// BLAS is used for matrix multiplication.
//// Without BLAS, matrix multiplication will still work, but might be slower.
#endif

// #define ARMA_BLAS_LONG
//// Uncomment the above line if your BLAS and LAPACK libraries use "long" instead of "int"

// #define ARMA_BLAS_LONG_LONG
//// Uncomment the above line if your BLAS and LAPACK libraries use "long long" instead of "int"

#define ARMA_BLAS_UNDERSCORE
//// Uncomment the above line if your BLAS and LAPACK libraries have function names with a trailing underscore.
//// Conversely, comment it out if the function names don't have a trailing underscore.

// #define ARMA_BLAS_CAPITALS
//// Uncomment the above line if your BLAS and LAPACK libraries have capitalised function names (eg. ACML on 64-bit Windows)

#if !defined(ARMA_MAT_PREALLOC)
  #define ARMA_MAT_PREALLOC 16
#endif
//// This is the number of preallocated elements used by matrices and vectors;
//// it must be an integer that is at least 1.
//// If you mainly use lots of very small vectors (eg. <= 4 elements),
//// change the number to the size of your vectors.

#cmakedefine ARMA_USE_ATLAS
#define ARMA_ATLAS_INCLUDE_DIR ${ARMA_ATLAS_INCLUDE_DIR}/
//// If you're using ATLAS and the compiler can't find cblas.h and/or clapack.h
//// uncomment the above define and specify the appropriate include directory.
//// Make sure the directory has a trailing /

#cmakedefine ARMA_USE_BOOST
#cmakedefine ARMA_USE_BOOST_DATE

#cmakedefine ARMA_HAVE_STD_ISFINITE
#cmakedefine ARMA_HAVE_STD_ISINF
#cmakedefine ARMA_HAVE_STD_ISNAN
#cmakedefine ARMA_HAVE_STD_SNPRINTF

#cmakedefine ARMA_HAVE_LOG1P
#cmakedefine ARMA_HAVE_GETTIMEOFDAY

// #define ARMA_EXTRA_DEBUG
// #define ARMA_NO_DEBUG
