// @HEADER
//
// ***********************************************************************
//
//        MueLu: A package for multigrid based preconditioning
//                  Copyright 2012 Sandia Corporation
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
// Questions? Contact
//                    Jonathan Hu       (jhu@sandia.gov)
//                    Andrey Prokopenko (aprokop@sandia.gov)
//                    Tobias Wiesner    (tawiesn@sandia.gov)
//                    Ray Tuminaro      (rstumin@sandia.gov)
//
// ***********************************************************************
//
// @HEADER
#ifndef PACKAGES_MUELU_SRC_GRAPH_MUELU_UNSMOOSHFACTORY_DECL_HPP_
#define PACKAGES_MUELU_SRC_GRAPH_MUELU_UNSMOOSHFACTORY_DECL_HPP_

#include "MueLu_ConfigDefs.hpp"
#include "MueLu_SingleLevelFactoryBase.hpp"
#include "MueLu_UnsmooshFactory_fwd.hpp"
#include "MueLu_Level_fwd.hpp"

namespace MueLu {

  /*!
    @class UnsmooshFactory class.
    @brief Factory for building "unsmooshed" transfer operators from transfer operators associated with a scalar helper problem (built by the VariableDofLaplacianFactory)

    @ingroup MueLuGraphClasses

    ## Input/output of UnsmooshFactory ##

    ### User parameters of UnsmooshFactory ###
    Parameter | type | default | master.xml | validated | requested | description
    ----------|------|---------|:----------:|:---------:|:---------:|------------
     A                  | Factory | null |   | * | * | Generating factory of the matrix A

    The * in the @c master.xml column denotes that the parameter is defined in the @c master.xml file.<br>
    The * in the @c validated column means that the parameter is declared in the list of valid input parameters (see UnsmooshFactory::GetValidParameters).<br>
    The * in the @c requested column states that the data is requested as input with all dependencies (see UnsmooshFactory::DeclareInput).

    ### Variables provided by UnsmooshFactory ###

    After UnsmooshFactory::Build the following data is available (if requested)

    Parameter | generated by | description
    ----------|--------------|------------
    | P       | UnsmooshFactory   | Unsmooshed prolongation operator
  */
  template <class Scalar = double,
            class LocalOrdinal = int,
            class GlobalOrdinal = LocalOrdinal,
            class Node = KokkosClassic::DefaultNode::DefaultNodeType>
  class UnsmooshFactory : public SingleLevelFactoryBase {
#undef MUELU_UNSMOOSHFACTORY_SHORT
#include "MueLu_UseShortNames.hpp"

  public:

    //! @name Constructors/Destructors.
    //@{

    //! Constructor
    UnsmooshFactory();

    //! Destructor
    virtual ~UnsmooshFactory() { }

    RCP<const ParameterList> GetValidParameterList() const;

    //@}

    //! Input
    //@{

    void DeclareInput(Level &currentLevel) const;

    //@}

    void Build(Level &currentLevel) const; // Build

  private:




  }; //class UnsmooshFactory

} //namespace MueLu

#define MUELU_UNSMOOSHFACTORY_SHORT


#endif /* PACKAGES_MUELU_SRC_GRAPH_MUELU_UNSMOOSHFACTORY_DECL_HPP_ */
