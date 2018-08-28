/*
// @HEADER
// ***********************************************************************
//
//          Tpetra: Templated Linear Algebra Services Package
//                 Copyright (2008) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Michael A. Heroux (maherou@sandia.gov)
//
// ************************************************************************
// @HEADER
*/

#include "Tpetra_TestingUtilities.hpp"
#include "Tpetra_Details_Aliases.hpp"
#include "Tpetra_Details_demangle.hpp"
#include "Tpetra_Details_DefaultTypes.hpp"
#include <complex>
#include <typeinfo>

namespace { // (anonymous)

  //
  // UNIT TESTS
  //

  namespace Classes {

    template<class LO, class GO, class NT>
    class SampleThreeArgumentClass {
    public:
      static_assert (std::is_integral<LO>::value, "LO must be an integer");
      static_assert (std::is_integral<GO>::value, "GO must be an integer");
      static_assert (Tpetra::Details::is_node<NT>::value, "NT must be a Node type");
    };

    template<class SC, class LO, class GO, class NT>
    class SampleFourArgumentClass {
      static_assert (Tpetra::Details::is_valid_scalar<LO>::value, "SC must be a valid Scalar type");
      static_assert (std::is_integral<LO>::value, "LO must be an integer");
      static_assert (std::is_integral<GO>::value, "GO must be an integer");
      static_assert (Tpetra::Details::is_node<NT>::value, "NT must be a Node type");
    };

  } // namespace Classes

  template<class ... Args>
  using SampleThreeArgumentClass =
    typename ::Tpetra::Details::ThreeArgAlias<Classes::SampleThreeArgumentClass, Args...>::type;

  template<class ... Args>
  using SampleFourArgumentClass =
    typename ::Tpetra::Details::FourArgAlias<Classes::SampleFourArgumentClass, Args...>::type;

  TEUCHOS_UNIT_TEST( TpetraUtils, Aliases )
  {
    using Kokkos::Compat::KokkosDeviceWrapperNode;
    using Tpetra::Details::demangle;
    using std::endl;

    using default_scalar_type = Tpetra::Details::DefaultTypes::scalar_type;
    using default_local_ordinal_type = Tpetra::Details::DefaultTypes::local_ordinal_type;
    using default_global_ordinal_type = Tpetra::Details::DefaultTypes::global_ordinal_type;
    using default_node_type = Tpetra::Details::DefaultTypes::node_type;
    using default_execution_space = Tpetra::Details::DefaultTypes::execution_space;
    using default_memory_space = Tpetra::Details::DefaultTypes::execution_space::memory_space;

    out << "default_scalar_type => "
        << demangle (typeid (default_scalar_type).name ()) << endl
        << "default_local_ordinal_type => "
        << demangle (typeid (default_local_ordinal_type).name ()) << endl
        << "default_global_ordinal_type => "
        << demangle (typeid (default_global_ordinal_type).name ()) << endl
        << "default_execution_space => "
        << demangle (typeid (default_execution_space).name ()) << endl
        << "default_memory_space => "
        << demangle (typeid (default_memory_space).name ()) << endl
        << "default_node_type => "
        << demangle (typeid (default_node_type).name ()) << endl
        << endl;

    using Kokkos::Compat::KokkosDeviceWrapperNode;
#ifdef KOKKOS_ENABLE_SERIAL
    using Kokkos::Serial;
#endif // KOKKOS_ENABLE_SERIAL
#ifdef KOKKOS_ENABLE_OPENMP
    using Kokkos::OpenMP;
#endif // KOKKOS_ENABLE_OPENMP
#ifdef KOKKOS_ENABLE_CUDA
    using Kokkos::Cuda;
#endif // KOKKOS_ENABLE_CUDA

    // Check that the 3-argument type alias behaves as expected.
    out << "SampleThreeArgumentClass<> => "
        << demangle (typeid (SampleThreeArgumentClass<>).name ()) << endl;
    static_assert (std::is_same<
                   SampleThreeArgumentClass<>,
                   Classes::SampleThreeArgumentClass<default_local_ordinal_type, default_global_ordinal_type, default_node_type>
                   >::value,
                   "Oops 0 arguments");
    out << "SampleThreeArgumentClass<short> => "
        << demangle (typeid (SampleThreeArgumentClass<short>).name ()) << endl;
    static_assert (std::is_same<
                   SampleThreeArgumentClass<short>,
                   Classes::SampleThreeArgumentClass<short, default_global_ordinal_type, default_node_type>
                   >::value,
                   "Oops 1 argument");
    out << "SampleThreeArgumentClass<short, long> => "
        << demangle (typeid (SampleThreeArgumentClass<short, long>).name ()) << endl;
    static_assert (std::is_same<
                   SampleThreeArgumentClass<short, long>,
                   Classes::SampleThreeArgumentClass<short, long, default_node_type>
                   >::value,
                   "Oops 2 arguments");
    out << "SampleThreeArgumentClass<short, long, default_node_type> => "
        << demangle (typeid (SampleThreeArgumentClass<short, long, default_node_type>).name ()) << endl;
    static_assert (std::is_same<
                   SampleThreeArgumentClass<short, long, default_node_type>,
                   Classes::SampleThreeArgumentClass<short, long, default_node_type>
                   >::value,
                   "Oops 3 arguments");
    out << "SampleThreeArgumentClass<short, long, default_execution_space> => "
        << demangle (typeid (SampleThreeArgumentClass<short, long, default_execution_space>).name ()) << endl;
    static_assert (std::is_same<
                   SampleThreeArgumentClass<short, long, default_execution_space>,
                   Classes::SampleThreeArgumentClass<short, long, default_node_type>
                   >::value,
                   "Oops 3 arguments");
    out << "SampleThreeArgumentClass<short, long, Kokkos::Device<default_execution_space> > => "
        << demangle (typeid (SampleThreeArgumentClass<short, long, Kokkos::Device<default_execution_space, default_memory_space> >).name ()) << endl;
    static_assert (std::is_same<
                   SampleThreeArgumentClass<short, long, Kokkos::Device<default_execution_space, default_memory_space> >,
                   Classes::SampleThreeArgumentClass<short, long, default_node_type>
                   >::value,
                   "Oops 3 arguments");

#ifdef KOKKOS_ENABLE_SERIAL
    static_assert (std::is_same<
                   SampleThreeArgumentClass<short, long, KokkosDeviceWrapperNode<Serial> >,
                   Classes::SampleThreeArgumentClass<short, long, KokkosDeviceWrapperNode<Serial> >
                   >::value,
                   "Oops 3 arguments");
    static_assert (std::is_same<
                   SampleThreeArgumentClass<short, long, Serial>,
                   Classes::SampleThreeArgumentClass<short, long, KokkosDeviceWrapperNode<Serial> >
                   >::value,
                   "Oops 3 arguments");
#endif // KOKKOS_ENABLE_SERIAL

#ifdef KOKKOS_ENABLE_OPENMP
    static_assert (std::is_same<
                   SampleThreeArgumentClass<short, long, KokkosDeviceWrapperNode<OpenMP> >,
                   Classes::SampleThreeArgumentClass<short, long, KokkosDeviceWrapperNode<OpenMP> >
                   >::value,
                   "Oops 3 arguments");
    static_assert (std::is_same<
                   SampleThreeArgumentClass<short, long, OpenMP>,
                   Classes::SampleThreeArgumentClass<short, long, KokkosDeviceWrapperNode<OpenMP> >
                   >::value,
                   "Oops 3 arguments");
#endif // KOKKOS_ENABLE_OPENMP

#ifdef KOKKOS_ENABLE_CUDA
    static_assert (std::is_same<
                   SampleThreeArgumentClass<short, long, KokkosDeviceWrapperNode<Cuda> >,
                   Classes::SampleThreeArgumentClass<short, long, KokkosDeviceWrapperNode<Cuda> >
                   >::value,
                   "Oops 3 arguments");
    static_assert (std::is_same<
                   SampleThreeArgumentClass<short, long, Cuda>,
                   Classes::SampleThreeArgumentClass<short, long, KokkosDeviceWrapperNode<Cuda> >
                   >::value,
                   "Oops 3 arguments");
#endif // KOKKOS_ENABLE_CUDA

    // Check that the 4-argument type alias behaves as expected.
    out << "SampleFourArgumentClass<> => " //
        << demangle (typeid (SampleFourArgumentClass<>).name ()) << endl;
    static_assert (std::is_same<
                   SampleFourArgumentClass<>,
                   Classes::SampleFourArgumentClass<default_scalar_type, default_local_ordinal_type, default_global_ordinal_type, default_node_type>
                   >::value,
                   "Oops 0 arguments");
    out << "SampleFourArgumentClass<float> => "
        << demangle (typeid (SampleFourArgumentClass<float>).name ()) << endl;
    static_assert (std::is_same<
                   SampleFourArgumentClass<float>,
                   Classes::SampleFourArgumentClass<float, default_local_ordinal_type, default_global_ordinal_type, default_node_type>
                   >::value,
                   "Oops 1 argument");
    out << "SampleFourArgumentClass<float, short> => "
        << demangle (typeid (SampleFourArgumentClass<float, short>).name ()) << endl;
    static_assert (std::is_same<
                   SampleFourArgumentClass<float, short>,
                   Classes::SampleFourArgumentClass<float, short, default_global_ordinal_type, default_node_type>
                   >::value,
                   "Oops 2 arguments");
    out << "SampleFourArgumentClass<float, short, long> => "
        << demangle (typeid (SampleFourArgumentClass<float, short, long>).name ()) << endl;
    static_assert (std::is_same<
                   SampleFourArgumentClass<float, short, long>,
                   Classes::SampleFourArgumentClass<float, short, long, default_node_type>
                   >::value,
                   "Oops 3 arguments");
    out << "SampleFourArgumentClass<float, short, long, default_node_type> => "
        << demangle (typeid (SampleFourArgumentClass<float, short, long, default_node_type>).name ()) << endl;
    static_assert (std::is_same<
                   SampleFourArgumentClass<float, short, long, default_node_type>,
                   Classes::SampleFourArgumentClass<float, short, long, default_node_type>
                   >::value,
                   "Oops 4 arguments");
    out << "SampleFourArgumentClass<float, short, long, default_execution_space> => "
        << demangle (typeid (SampleFourArgumentClass<float, short, long, default_execution_space>).name ()) << endl;
    static_assert (std::is_same<
                   SampleFourArgumentClass<float, short, long, default_execution_space>,
                   Classes::SampleFourArgumentClass<float, short, long, default_node_type>
                   >::value,
                   "Oops 4 arguments");
    out << "SampleFourArgumentClass<float, short, long, Kokkos::Device<default_execution_space, default_memory_space> > => "
        << demangle (typeid (SampleFourArgumentClass<float, short, long, Kokkos::Device<default_execution_space, default_memory_space> >).name ()) << endl;
    static_assert (std::is_same<
                   SampleFourArgumentClass<float, short, long, Kokkos::Device<default_execution_space, default_memory_space> >,
                   Classes::SampleFourArgumentClass<float, short, long, default_node_type>
                   >::value,
                   "Oops 4 arguments");

#ifdef KOKKOS_ENABLE_SERIAL
    out << "SampleFourArgumentClass<float, short, long, KokkosDeviceWrapperNode<Serial> > => "
        << demangle (typeid (SampleFourArgumentClass<float, short, long, KokkosDeviceWrapperNode<Serial> >).name ()) << endl;
    static_assert (std::is_same<
                   SampleFourArgumentClass<float, short, long, KokkosDeviceWrapperNode<Serial> >,
                   Classes::SampleFourArgumentClass<float, short, long, KokkosDeviceWrapperNode<Serial> >
                   >::value,
                   "Oops 4 arguments");
    out << "SampleFourArgumentClass<float, short, long, Serial> => "
        << demangle (typeid (SampleFourArgumentClass<float, short, long, Serial>).name ()) << endl;
    static_assert (std::is_same<
                   SampleFourArgumentClass<float, short, long, Serial>,
                   Classes::SampleFourArgumentClass<float, short, long, KokkosDeviceWrapperNode<Serial> >
                   >::value,
                   "Oops 4 arguments");
#endif // KOKKOS_ENABLE_SERIAL

#ifdef KOKKOS_ENABLE_OPENMP
    out << "SampleFourArgumentClass<float, short, long, KokkosDeviceWrapperNode<OpenMP> > => "
        << demangle (typeid (SampleFourArgumentClass<float, short, long, KokkosDeviceWrapperNode<OpenMP> >).name ()) << endl;
    static_assert (std::is_same<
                   SampleFourArgumentClass<float, short, long, KokkosDeviceWrapperNode<OpenMP> >,
                   Classes::SampleFourArgumentClass<float, short, long, KokkosDeviceWrapperNode<OpenMP> >
                   >::value,
                   "Oops 4 arguments");
    out << "SampleFourArgumentClass<float, short, long, OpenMP> => "
        << demangle (typeid (SampleFourArgumentClass<float, short, long, OpenMP>).name ()) << endl;
    static_assert (std::is_same<
                   SampleFourArgumentClass<float, short, long, OpenMP>,
                   Classes::SampleFourArgumentClass<float, short, long, KokkosDeviceWrapperNode<OpenMP> >
                   >::value,
                   "Oops 4 arguments");
#endif // KOKKOS_ENABLE_OPENMP

#ifdef KOKKOS_ENABLE_CUDA
    out << "SampleFourArgumentClass<float, short, long, KokkosDeviceWrapperNode<Cuda> > => "
        << demangle (typeid (SampleFourArgumentClass<float, short, long, KokkosDeviceWrapperNode<Cuda> >).name ()) << endl;
    static_assert (std::is_same<
                   SampleFourArgumentClass<float, short, long, KokkosDeviceWrapperNode<Cuda> >,
                   Classes::SampleFourArgumentClass<float, short, long, KokkosDeviceWrapperNode<Cuda>>
                   >::value,
                   "Oops 4 arguments");
    out << "SampleFourArgumentClass<float, short, long, Cuda> => "
        << demangle (typeid (SampleFourArgumentClass<float, short, long, Cuda>).name ()) << endl;
    static_assert (std::is_same<
                   SampleFourArgumentClass<float, short, long, Cuda>,
                   Classes::SampleFourArgumentClass<float, short, long, KokkosDeviceWrapperNode<Cuda>>
                   >::value,
                   "Oops 4 arguments");
#endif // KOKKOS_ENABLE_CUDA

    // Check that we're not instantiating redundant types.
    out << "SampleThreeArgumentClass<> => "
        << demangle (typeid (SampleThreeArgumentClass<>).name ()) << endl;
    static_assert (std::is_same<
                   SampleThreeArgumentClass<>,
                   SampleThreeArgumentClass<default_local_ordinal_type, default_global_ordinal_type, default_node_type>
                   >::value,
                   "Oops not the same type"
                   );
    out << "SampleThreeArgumentClass<short> => "
        << demangle (typeid (SampleThreeArgumentClass<short>).name ()) << endl;
    static_assert (std::is_same<
                   SampleThreeArgumentClass<short>,
                   SampleThreeArgumentClass<short, default_global_ordinal_type, default_node_type>
                   >::value,
                   "Oops not the same type"
                   );
    out << "SampleThreeArgumentClass<default_node_type> => "
        << demangle (typeid (SampleThreeArgumentClass<default_node_type>).name ()) << endl;
    static_assert (std::is_same<
                   SampleThreeArgumentClass<default_node_type>,
                   SampleThreeArgumentClass<default_local_ordinal_type, default_global_ordinal_type, default_node_type>
                   >::value,
                   "Oops not the same type"
                   );
    // If users only give me one integer type, I assume that it is
    // local_ordinal_type.
    out << "SampleThreeArgumentClass<int> => "
        << demangle (typeid (SampleThreeArgumentClass<int>).name ()) << endl;
    static_assert (std::is_same<
                   SampleThreeArgumentClass<int, default_node_type>,
                   SampleThreeArgumentClass<int, default_global_ordinal_type, default_node_type>
                   >::value,
                   "Oops not the same type"
                   );
    out << "SampleThreeArgumentClass<short, long> => "
        << demangle (typeid (SampleThreeArgumentClass<short, long>).name ()) << endl;
    static_assert (std::is_same<
                   SampleThreeArgumentClass<short, long>,
                   SampleThreeArgumentClass<short, long, default_node_type>
                   >::value,
                   "Oops not the same type"
                   );
    // Check that execution spaces work in place of Nodes.
    out << "SampleThreeArgumentClass<default_execution_space> => "
        << demangle (typeid (SampleThreeArgumentClass<default_execution_space>).name ()) << endl;
    static_assert (std::is_same<
                   SampleThreeArgumentClass<default_execution_space>,
                   SampleThreeArgumentClass<default_local_ordinal_type, default_global_ordinal_type, default_node_type>
                   >::value,
                   "Oops not the same type"
                   );
    out << "SampleThreeArgumentClass<int, default_execution_space> => "
        << demangle (typeid (SampleThreeArgumentClass<int, default_execution_space>).name ()) << endl;
    static_assert (std::is_same<
                   SampleThreeArgumentClass<int, default_execution_space>,
                   SampleThreeArgumentClass<int, default_global_ordinal_type, default_node_type>
                   >::value,
                   "Oops not the same type"
                   );
    out << "SampleThreeArgumentClass<int, long, default_execution_space> => "
        << demangle (typeid (SampleThreeArgumentClass<int, long, default_execution_space>).name ()) << endl;
    static_assert (std::is_same<
                   SampleThreeArgumentClass<int, long, default_execution_space>,
                   SampleThreeArgumentClass<int, long, default_node_type>
                   >::value,
                   "Oops not the same type"
                   );

    // Show that the Node, execution space, or Kokkos::Device argument
    // can go first.
    static_assert (std::is_same<
                   SampleThreeArgumentClass<default_execution_space, int, long>,
                   SampleThreeArgumentClass<int, long, default_execution_space>
                   >::value,
                   "Oops not the same type"
                   );
    static_assert (std::is_same<
                   SampleThreeArgumentClass<default_node_type, int, long>,
                   SampleThreeArgumentClass<int, long, default_node_type>
                   >::value,
                   "Oops not the same type"
                   );
    static_assert (std::is_same<
                   SampleThreeArgumentClass<Kokkos::Device<default_execution_space, default_memory_space>, int, long>,
                   SampleThreeArgumentClass<int, long, default_node_type>
                   >::value,
                   "Oops not the same type"
                   );

    // Show that the order of the integer arguments matters.
    static_assert (! std::is_same<
                   SampleThreeArgumentClass<int, long>,
                   SampleThreeArgumentClass<long, int>
                   >::value,
                   "Oops ARE the same type"
                   );
    static_assert (! std::is_same<
                   SampleThreeArgumentClass<int, long, default_execution_space>,
                   SampleThreeArgumentClass<long, int, default_execution_space>
                   >::value,
                   "Oops ARE the same type"
                   );

  // Uncomment the line below to trigger the static_assert
  // that forbids more than 3 template parameters.
  // SampleThreeArgumentClass<short, long, default_node_type, double> x;
}

} // namespace (anonymous)


