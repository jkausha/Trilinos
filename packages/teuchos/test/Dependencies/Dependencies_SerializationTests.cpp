// @HEADER
// ***********************************************************************
// 
//                    Teuchos: Common Tools Package
//                 Copyright (2004) Sandia Corporation
// 
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
// 
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1 of the
// License, or (at your option) any later version.
//  
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//  
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
// Questions? Contact Michael A. Heroux (maherou@sandia.gov) 
// 
// ***********************************************************************
// @HEADER

#include "Teuchos_VerboseObject.hpp"
#include "Teuchos_XMLParameterListHelpers.hpp"
#include "Teuchos_StandardDependencies.hpp"
#include "Teuchos_DependencySheet.hpp"
#include "Teuchos_StandardConditions.hpp"
#include "Teuchos_StandardDependencies.hpp"
#include "Teuchos_UnitTestHarness.hpp"
#include "Teuchos_DependencyXMLConverterDB.hpp"
#include "Teuchos_StandardDependencyXMLConverters.hpp"
#include "Teuchos_ParameterList.cpp"


namespace Teuchos{


typedef unsigned short int ushort;
typedef unsigned int uint;
typedef unsigned long int ulong;
typedef std::string myString_t;
#ifdef HAVE_TEUCHOS_LONG_LONG_INT
typedef long long int llint;
typedef unsigned long long int ullint;
#endif

#define BASIC_DEPENDENCY_TEST( \
  DEPENDENCY, DEPTYPE,  NUM_DEPENDEES, NUM_DEPENDENTS) \
  std::string depXMLTag##DEPENDENCY = \
    DummyObjectGetter< DEPTYPE >::getDummyObject()->getTypeAttributeValue(); \
\
  TEST_ASSERT(DEPENDENCY->getTypeAttributeValue() == depXMLTag##DEPENDENCY ); \
  TEST_ASSERT(DEPENDENCY->getDependents().size() == NUM_DEPENDENTS); \
  TEST_ASSERT(DEPENDENCY->getDependees().size() == NUM_DEPENDEES);

#define VERIFY_DEPENDENT(DEPENDENCY, DEPENDENT) \
  TEST_ASSERT( \
    DEPENDENCY->getDependents().find(DEPENDENT)  \
    != \
    DEPENDENCY->getDependents().end() \
  );

  
#define VERIFY_DEPENDEE(DEPENDENCY, DEPENDEE) \
  TEST_ASSERT( \
    DEPENDENCY->getDependees().find(DEPENDEE)  \
    != \
    DEPENDENCY->getDependees().end());

TEUCHOS_UNIT_TEST(Teuchos_Dependencies, SerializationTestMacros){
  RCP<ParameterEntry> dependee1 = rcp(new ParameterEntry(true));
  RCP<ParameterEntry> dependee2 = rcp(new ParameterEntry(true));
  RCP<ParameterEntry> dependent1 = rcp(new ParameterEntry("blah"));
  RCP<ParameterEntry> dependent2 = rcp(new ParameterEntry("blah"));
  RCP<BoolVisualDependency> simpleDep = 
    rcp(new BoolVisualDependency(dependee1, dependent1));


  Dependency::ParameterEntryList dependentList;
  dependentList.insert(dependent1);
  dependentList.insert(dependent2);

  RCP<BoolVisualDependency> complexDep = 
    rcp(new BoolVisualDependency(dependee2, dependentList));

  BASIC_DEPENDENCY_TEST(simpleDep, BoolVisualDependency, 1, 1);
  VERIFY_DEPENDEE(simpleDep, dependee1);
  VERIFY_DEPENDENT(simpleDep, dependent1);

  BASIC_DEPENDENCY_TEST(complexDep, BoolVisualDependency, 1, 2);
  VERIFY_DEPENDEE(complexDep, dependee2);
  VERIFY_DEPENDENT(complexDep, dependent1);
  VERIFY_DEPENDENT(complexDep, dependent2);

}


TEUCHOS_UNIT_TEST(Teuchos_Dependencies, StringVisualDepSerialization){
  std::string dependee1 = "string param";
  std::string dependee2 = "string param2";
  std::string dependent1 = "dependent param1";
  std::string dependent2 = "dependent param2";
  ParameterList myDepList("String Visual Dep List");
  RCP<DependencySheet> myDepSheet = rcp(new DependencySheet);
  myDepList.set(dependee1, "val1");
  myDepList.set(dependee2, "val2");
  myDepList.set(dependent1, 1.0);
  myDepList.set(dependent2, 1.0);

  StringVisualDependency::ValueList valList1 = tuple<std::string>("val1");

  RCP<StringVisualDependency> basicStringVisDep = rcp(
    new StringVisualDependency(
      myDepList.getEntryRCP(dependee1),
      myDepList.getEntryRCP(dependent1),
      valList1));

  Dependency::ParameterEntryList dependentList;
  dependentList.insert(myDepList.getEntryRCP(dependent1));
  dependentList.insert(myDepList.getEntryRCP(dependent2));
  StringVisualDependency::ValueList valList2 = 
    tuple<std::string>("val1", "val2");

  RCP<StringVisualDependency> complexStringVisDep = rcp(
    new StringVisualDependency(
      myDepList.getEntryRCP(dependee2),
      dependentList,
      valList2,
      false));

  myDepSheet->addDependency(basicStringVisDep);
  myDepSheet->addDependency(complexStringVisDep);

  RCP<DependencySheet> readInDepSheet = rcp(new DependencySheet);

  XMLParameterListWriter plWriter;
  XMLObject xmlOut = plWriter.toXML(myDepList, myDepSheet);
  out << xmlOut.toString();

  RCP<ParameterList> readInList = 
    writeThenReadPL(myDepList, myDepSheet, readInDepSheet); 

  RCP<ParameterEntry> readinDependee1 = readInList->getEntryRCP(dependee1);
  RCP<ParameterEntry> readinDependent1 = readInList->getEntryRCP(dependent1);
  RCP<ParameterEntry> readinDependee2 = readInList->getEntryRCP(dependee2);
  RCP<ParameterEntry> readinDependent2 = readInList->getEntryRCP(dependent2);
  
  RCP<Dependency> readinDep1 =
    *(readInDepSheet->getDependenciesForParameter(readinDependee1)->begin());

  RCP<Dependency> readinDep2 =
    *(readInDepSheet->getDependenciesForParameter(readinDependee2)->begin());

  BASIC_DEPENDENCY_TEST(readinDep1, StringVisualDependency, 1, 1);
  VERIFY_DEPENDEE(readinDep1, readinDependee1);
  VERIFY_DEPENDENT(readinDep1, readinDependent1);

  BASIC_DEPENDENCY_TEST(readinDep2, StringVisualDependency, 1, 2);
  VERIFY_DEPENDEE(readinDep2, readinDependee2);
  VERIFY_DEPENDENT(readinDep2, readinDependent1);
  VERIFY_DEPENDENT(readinDep2, readinDependent2);
    
  RCP<StringVisualDependency> castedDep1 =
    rcp_dynamic_cast<StringVisualDependency>(readinDep1, true);
  RCP<StringVisualDependency> castedDep2 =
    rcp_dynamic_cast<StringVisualDependency>(readinDep2, true);

  TEST_COMPARE_ARRAYS(
    castedDep1->getValues(), basicStringVisDep->getValues());
  TEST_COMPARE_ARRAYS(
    castedDep2->getValues(), complexStringVisDep->getValues());

  TEST_EQUALITY(castedDep1->getShowIf(), basicStringVisDep->getShowIf());
  TEST_EQUALITY(castedDep2->getShowIf(), complexStringVisDep->getShowIf());
}

TEUCHOS_UNIT_TEST(Teuchos_Dependencies, BoolVisualDepSerialization){
  std::string dependee1 = "bool param";
  std::string dependee2 = "bool param2";
  std::string dependent1 = "dependent param1";
  std::string dependent2 = "dependent param2";
  ParameterList myDepList("Bool Visual Dep List");
  RCP<DependencySheet> myDepSheet = rcp(new DependencySheet);
  myDepList.set(dependee1, true);
  myDepList.set(dependee2, true);
  myDepList.set(dependent1, 1.0);
  myDepList.set(dependent2, 1.0);

  RCP<BoolVisualDependency> trueBoolVisDep = rcp(
    new BoolVisualDependency(
      myDepList.getEntryRCP(dependee1),
      myDepList.getEntryRCP(dependent1)));

  Dependency::ParameterEntryList dependentList;
  dependentList.insert(myDepList.getEntryRCP(dependent1));
  dependentList.insert(myDepList.getEntryRCP(dependent2));

  RCP<BoolVisualDependency> falseBoolVisDep = rcp(
    new BoolVisualDependency(
      myDepList.getEntryRCP(dependee2),
      dependentList,
      false));

  myDepSheet->addDependency(trueBoolVisDep);
  myDepSheet->addDependency(falseBoolVisDep);

  RCP<DependencySheet> readInDepSheet = rcp(new DependencySheet);

  XMLParameterListWriter plWriter;
  XMLObject xmlOut = plWriter.toXML(myDepList, myDepSheet);
  out << xmlOut.toString();

  RCP<ParameterList> readInList = 
    writeThenReadPL(myDepList, myDepSheet, readInDepSheet); 

  RCP<ParameterEntry> readinDependee1 = readInList->getEntryRCP(dependee1);
  RCP<ParameterEntry> readinDependent1 = readInList->getEntryRCP(dependent1);
  RCP<ParameterEntry> readinDependee2 = readInList->getEntryRCP(dependee2);
  RCP<ParameterEntry> readinDependent2 = readInList->getEntryRCP(dependent2);
  
  RCP<Dependency> readinDep1 =
    *(readInDepSheet->getDependenciesForParameter(readinDependee1)->begin());

  RCP<Dependency> readinDep2 =
    *(readInDepSheet->getDependenciesForParameter(readinDependee2)->begin());

  BASIC_DEPENDENCY_TEST(readinDep1, BoolVisualDependency, 1, 1);
  VERIFY_DEPENDEE(readinDep1, readinDependee1);
  VERIFY_DEPENDENT(readinDep1, readinDependent1);

  BASIC_DEPENDENCY_TEST(readinDep2, BoolVisualDependency, 1, 2);
  VERIFY_DEPENDEE(readinDep2, readinDependee2);
  VERIFY_DEPENDENT(readinDep2, readinDependent1);
  VERIFY_DEPENDENT(readinDep2, readinDependent2);
    
    
  RCP<BoolVisualDependency> castedDep1 =
    rcp_dynamic_cast<BoolVisualDependency>(readinDep1, true);
  RCP<BoolVisualDependency> castedDep2 =
    rcp_dynamic_cast<BoolVisualDependency>(readinDep2, true);

  TEST_EQUALITY(castedDep1->getShowIf(), trueBoolVisDep->getShowIf());
  TEST_EQUALITY(castedDep2->getShowIf(), falseBoolVisDep->getShowIf());
}

TEUCHOS_UNIT_TEST_TEMPLATE_1_DECL(
  Teuchos_Dependencies, 
  NumberVisualDepSerialization, 
  T)
{
  std::string dependee1 = "num param";
  std::string dependee2 = "num param2";
  std::string dependent1 = "dependent param1";
  std::string dependent2 = "dependent param2";
  ParameterList myDepList("Number Visual Dep List");
  RCP<DependencySheet> myDepSheet = rcp(new DependencySheet);
  myDepList.set(dependee1, ScalarTraits<T>::one());
  myDepList.set(dependee2, ScalarTraits<T>::one());
  myDepList.set(dependent1, true);
  myDepList.set(dependent2, "vale");

  RCP<NumberVisualDependency<T> > simpleNumVisDep = rcp(
    new NumberVisualDependency<T>(
      myDepList.getEntryRCP(dependee1),
      myDepList.getEntryRCP(dependent1)));

  Dependency::ParameterEntryList dependentList;
  dependentList.insert(myDepList.getEntryRCP(dependent1));
  dependentList.insert(myDepList.getEntryRCP(dependent2));

  RCP<NumberVisualDependency<T> > complexNumVisDep = rcp(
    new NumberVisualDependency<T>(
      myDepList.getEntryRCP(dependee2),
      dependentList));

  myDepSheet->addDependency(simpleNumVisDep);
  myDepSheet->addDependency(complexNumVisDep);

  RCP<DependencySheet> readInDepSheet = rcp(new DependencySheet);

  XMLParameterListWriter plWriter;
  XMLObject xmlOut = plWriter.toXML(myDepList, myDepSheet);
  out << xmlOut.toString();

  RCP<ParameterList> readInList = 
    writeThenReadPL(myDepList, myDepSheet, readInDepSheet); 

  RCP<ParameterEntry> readinDependee1 = readInList->getEntryRCP(dependee1);
  RCP<ParameterEntry> readinDependent1 = readInList->getEntryRCP(dependent1);
  RCP<ParameterEntry> readinDependee2 = readInList->getEntryRCP(dependee2);
  RCP<ParameterEntry> readinDependent2 = readInList->getEntryRCP(dependent2);
  
  RCP<Dependency> readinDep1 =
    *(readInDepSheet->getDependenciesForParameter(readinDependee1)->begin());

  RCP<Dependency> readinDep2 =
    *(readInDepSheet->getDependenciesForParameter(readinDependee2)->begin());

  BASIC_DEPENDENCY_TEST(readinDep1, NumberVisualDependency<T>, 1, 1);
  VERIFY_DEPENDEE(readinDep1, readinDependee1);
  VERIFY_DEPENDENT(readinDep1, readinDependent1);

  BASIC_DEPENDENCY_TEST(readinDep2, NumberVisualDependency<T>, 1, 2);
  VERIFY_DEPENDEE(readinDep2, readinDependee2);
  VERIFY_DEPENDENT(readinDep2, readinDependent1);
  VERIFY_DEPENDENT(readinDep2, readinDependent2);
    
    
  RCP<NumberVisualDependency<T> > castedDep1 =
    rcp_dynamic_cast<NumberVisualDependency<T> >(readinDep1, true);
  RCP<NumberVisualDependency<T> > castedDep2 =
    rcp_dynamic_cast<NumberVisualDependency<T> >(readinDep2, true);

  TEST_EQUALITY(castedDep1->getShowIf(), simpleNumVisDep->getShowIf());
  TEST_EQUALITY(castedDep2->getShowIf(), complexNumVisDep->getShowIf());
}

#define NUMBER_VIS_TEST(T) \
TEUCHOS_UNIT_TEST_TEMPLATE_1_INSTANT( \
  Teuchos_Dependencies, NumberVisualDepSerialization, T)

NUMBER_VIS_TEST(int)
NUMBER_VIS_TEST(double)
NUMBER_VIS_TEST(float)
#ifdef HAVE_TEUCHOS_LONG_LONG_INT
NUMBER_VIS_TEST(llint)
#endif

TEUCHOS_UNIT_TEST(Teuchos_Dependencies, ConditionVisualDepSerialization){
  std::string dependee1 = "string param";
  std::string dependee2 = "bool param";
  std::string dependee3 = "int param";
  std::string dependent1 = "dependent param1";
  std::string dependent2 = "dependent param2";
  std::string dependent3 = "dependent param3";
  ParameterList myDepList("Condition Visual Dep List");
  RCP<DependencySheet> myDepSheet = rcp(new DependencySheet);
  myDepList.set(dependee1, "val1");
  myDepList.set(dependee2, true);
  myDepList.set(dependee3, 1);
  myDepList.set(dependent1, 1.0);
  myDepList.set(dependent2, 1.0);
  myDepList.set(dependent3, (float)1.0);

  StringCondition::ValueList conditionVal1 = 
    tuple<std::string>("steve", "blah", "your face");
  RCP<StringCondition> stringCon = 
    rcp(new StringCondition(
      myDepList.getEntryRCP(dependee1), conditionVal1, false));

  RCP<BoolCondition> boolCon = 
    rcp(new BoolCondition(myDepList.getEntryRCP(dependee2)));

  RCP<NumberCondition<int> > numberCon = 
    rcp(new NumberCondition<int>(myDepList.getEntryRCP(dependee3)));

  Condition::ConstConditionList conList = 
    tuple<RCP<const Condition> >(boolCon, numberCon);

  RCP<AndCondition> andCon = rcp(new AndCondition(conList));

  RCP<ConditionVisualDependency> simpleConVisDep = rcp(
    new ConditionVisualDependency(
      stringCon,
      myDepList.getEntryRCP(dependent1)));

  Dependency::ParameterEntryList dependentList;
  dependentList.insert(myDepList.getEntryRCP(dependent2));
  dependentList.insert(myDepList.getEntryRCP(dependent3));

  RCP<ConditionVisualDependency> complexConVisDep = rcp(
    new ConditionVisualDependency(
      andCon,
      dependentList,
      false));

  myDepSheet->addDependency(simpleConVisDep);
  myDepSheet->addDependency(complexConVisDep);

  RCP<DependencySheet> readInDepSheet = rcp(new DependencySheet);

  XMLParameterListWriter plWriter;
  XMLObject xmlOut = plWriter.toXML(myDepList, myDepSheet);
  out << xmlOut.toString();

  RCP<ParameterList> readInList = 
    writeThenReadPL(myDepList, myDepSheet, readInDepSheet); 

  RCP<ParameterEntry> readinDependee1 = readInList->getEntryRCP(dependee1);
  RCP<ParameterEntry> readinDependent1 = readInList->getEntryRCP(dependent1);
  RCP<ParameterEntry> readinDependee2 = readInList->getEntryRCP(dependee2);
  RCP<ParameterEntry> readinDependent2 = readInList->getEntryRCP(dependent2);
  RCP<ParameterEntry> readinDependee3 = readInList->getEntryRCP(dependee3);
  RCP<ParameterEntry> readinDependent3 = readInList->getEntryRCP(dependent3);
  
  RCP<Dependency> readinDep1 =
    *(readInDepSheet->getDependenciesForParameter(readinDependee1)->begin());

  RCP<Dependency> readinDep2 =
    *(readInDepSheet->getDependenciesForParameter(readinDependee2)->begin());

  RCP<Dependency> readinDep3 =
    *(readInDepSheet->getDependenciesForParameter(readinDependee3)->begin());

  BASIC_DEPENDENCY_TEST(readinDep1, ConditionVisualDependency, 1, 1);
  VERIFY_DEPENDEE(readinDep1, readinDependee1);
  VERIFY_DEPENDENT(readinDep1, readinDependent1);

  TEST_ASSERT(readinDep2.get() == readinDep3.get());

  BASIC_DEPENDENCY_TEST(readinDep2, ConditionVisualDependency, 2, 2);
  VERIFY_DEPENDEE(readinDep2, readinDependee2);
  VERIFY_DEPENDEE(readinDep2, readinDependee3);
  VERIFY_DEPENDENT(readinDep2, readinDependent2);
  VERIFY_DEPENDENT(readinDep2, readinDependent3);
    
  RCP<ConditionVisualDependency> castedDep1 =
    rcp_dynamic_cast<ConditionVisualDependency>(readinDep1, true);
  RCP<ConditionVisualDependency> castedDep2 =
    rcp_dynamic_cast<ConditionVisualDependency>(readinDep2, true);

  TEST_EQUALITY(castedDep1->getShowIf(), simpleConVisDep->getShowIf());
  TEST_EQUALITY(castedDep2->getShowIf(), complexConVisDep->getShowIf());

  TEST_EQUALITY(castedDep1->getCondition()->getTypeAttributeValue(),
    simpleConVisDep->getCondition()->getTypeAttributeValue());
  TEST_EQUALITY(castedDep2->getCondition()->getTypeAttributeValue(), 
    complexConVisDep->getCondition()->getTypeAttributeValue());
}

TEUCHOS_UNIT_TEST_TEMPLATE_2_DECL(
  Teuchos_Dependencies, 
  NumberArrayLengthDepSerialization, 
  DependeeType,
  DependentType)
{
  std::string dependee1 = "dependee param";
  std::string dependee2 = "dependee param2";
  std::string dependent1 = "dependent param1";
  std::string dependent2 = "dependent param2";
  ParameterList myDepList("Number Array LenthDep List");
  RCP<DependencySheet> myDepSheet = rcp(new DependencySheet);
  myDepList.set(dependee1, ScalarTraits<DependeeType>::one());
  myDepList.set(dependee2, ScalarTraits<DependeeType>::one());
  myDepList.set(dependent1, Array<DependentType>(8));
  myDepList.set(dependent2, Array<DependentType>(5));


  RCP<NumberArrayLengthDependency<DependeeType, DependentType> > basicArrayDep =
    rcp(new NumberArrayLengthDependency<DependeeType, DependentType>(
      myDepList.getEntryRCP(dependee1),
      myDepList.getEntryRCP(dependent1)));

  myDepSheet->addDependency(basicArrayDep);

  RCP<DependencySheet> readInDepSheet = rcp(new DependencySheet);

  XMLParameterListWriter plWriter;
  XMLObject xmlOut = plWriter.toXML(myDepList, myDepSheet);
  out << xmlOut.toString();

  RCP<ParameterList> readInList = 
    writeThenReadPL(myDepList, myDepSheet, readInDepSheet); 

  RCP<ParameterEntry> readinDependee1 = readInList->getEntryRCP(dependee1);
  RCP<ParameterEntry> readinDependent1 = readInList->getEntryRCP(dependent1);
  RCP<ParameterEntry> readinDependee2 = readInList->getEntryRCP(dependee2);
  RCP<ParameterEntry> readinDependent2 = readInList->getEntryRCP(dependent2);
  
  RCP<Dependency> readinDep1 =
    *(readInDepSheet->getDependenciesForParameter(readinDependee1)->begin());

  typedef NumberArrayLengthDependency<DependeeType, DependentType> deptype;
  BASIC_DEPENDENCY_TEST(readinDep1, deptype, 1, 1);
  VERIFY_DEPENDEE(readinDep1, readinDependee1);
  VERIFY_DEPENDENT(readinDep1, readinDependent1);

  RCP<NumberArrayLengthDependency<DependeeType, DependentType> > castedDep1 =
    rcp_dynamic_cast<NumberArrayLengthDependency<DependeeType, DependentType> >(
      readinDep1, true);
}

#define NUM_ARRAY_LENGTH_TEST(DependeeType, DependentType) \
TEUCHOS_UNIT_TEST_TEMPLATE_2_INSTANT( \
  Teuchos_Dependencies, \
  NumberArrayLengthDepSerialization, \
  DependeeType, \
  DependentType) 

// Need to fix array serialization so we can test this with
// a dependent type of strings. Right now an array of emptyr strings does not
// seralize correctly
// KLN 09.17/2010
#ifdef HAVE_TEUCHOS_LONG_LONG_INT
#define NUM_ARRAY_LENGTH_TEST_GROUP(DependeeType) \
  NUM_ARRAY_LENGTH_TEST(DependeeType, int) \
  NUM_ARRAY_LENGTH_TEST(DependeeType, float) \
  NUM_ARRAY_LENGTH_TEST(DependeeType, double) \
  NUM_ARRAY_LENGTH_TEST(DependeeType, llint) 
#else
#define NUMBER_VIS_TEST_GROUP(DependeeType) \
  NUM_ARRAY_LENGTH_TEST(DependeeType, int) \
  NUM_ARRAY_LENGTH_TEST(DependeeType, double) \
  NUM_ARRAY_LENGTH_TEST(DependeeType, float)
#endif

NUM_ARRAY_LENGTH_TEST_GROUP(int)
NUM_ARRAY_LENGTH_TEST_GROUP(double)
#ifdef HAVE_TEUCHOS_LONG_LONG_INT
NUM_ARRAY_LENGTH_TEST_GROUP(llint)
#endif

TEUCHOS_UNIT_TEST(Teuchos_Dependencies, StringValidatorDepSerialization){
  std::string dependee1 = "string param";
  std::string dependee2 = "string param2";
  std::string dependent1 = "dependent param1";
  std::string dependent2 = "dependent param2";
  ParameterList myDepList("String Vali Dep List");
  RCP<DependencySheet> myDepSheet = rcp(new DependencySheet);
  myDepList.set(dependee1, "val1");
  myDepList.set(dependee2, "val2");
  myDepList.set(dependent1, 2.0);
  myDepList.set(dependent2, 3.0);

	RCP<EnhancedNumberValidator<double> > double1Vali =
    rcp(new EnhancedNumberValidator<double>(0,10));

	RCP<EnhancedNumberValidator<double> > double2Vali =
    rcp(new EnhancedNumberValidator<double>(0,30));

	RCP<EnhancedNumberValidator<double> > defaultVali =
    rcp(new EnhancedNumberValidator<double>(4,90));

  StringValidatorDependency::ValueToValidatorMap valuesAndValidators;
  valuesAndValidators["val1"] = double1Vali;
  valuesAndValidators["val2"] = double2Vali;

  RCP<StringValidatorDependency> basicStringValiDep = rcp(
    new StringValidatorDependency(
      myDepList.getEntryRCP(dependee1),
      myDepList.getEntryRCP(dependent1),
      valuesAndValidators));

  Dependency::ParameterEntryList dependentList;
  dependentList.insert(myDepList.getEntryRCP(dependent1));
  dependentList.insert(myDepList.getEntryRCP(dependent2));

  RCP<StringValidatorDependency> complexStringValiDep = rcp(
    new StringValidatorDependency(
      myDepList.getEntryRCP(dependee2),
      dependentList,
      valuesAndValidators,
      defaultVali));

  myDepSheet->addDependency(basicStringValiDep);
  myDepSheet->addDependency(complexStringValiDep);

  RCP<DependencySheet> readInDepSheet = rcp(new DependencySheet);

  XMLParameterListWriter plWriter;
  XMLObject xmlOut = plWriter.toXML(myDepList, myDepSheet);
  out << xmlOut.toString();

  RCP<ParameterList> readInList = 
    writeThenReadPL(myDepList, myDepSheet, readInDepSheet); 

  RCP<ParameterEntry> readinDependee1 = readInList->getEntryRCP(dependee1);
  RCP<ParameterEntry> readinDependent1 = readInList->getEntryRCP(dependent1);
  RCP<ParameterEntry> readinDependee2 = readInList->getEntryRCP(dependee2);
  RCP<ParameterEntry> readinDependent2 = readInList->getEntryRCP(dependent2);
  
  RCP<Dependency> readinDep1 =
    *(readInDepSheet->getDependenciesForParameter(readinDependee1)->begin());

  RCP<Dependency> readinDep2 =
    *(readInDepSheet->getDependenciesForParameter(readinDependee2)->begin());

  BASIC_DEPENDENCY_TEST(readinDep1, StringValidatorDependency, 1, 1);
  VERIFY_DEPENDEE(readinDep1, readinDependee1);
  VERIFY_DEPENDENT(readinDep1, readinDependent1);

  BASIC_DEPENDENCY_TEST(readinDep2, StringValidatorDependency, 1, 2);
  VERIFY_DEPENDEE(readinDep2, readinDependee2);
  VERIFY_DEPENDENT(readinDep2, readinDependent1);
  VERIFY_DEPENDENT(readinDep2, readinDependent2);
    
    
  RCP<StringValidatorDependency> castedDep1 =
    rcp_dynamic_cast<StringValidatorDependency>(readinDep1, true);
  RCP<StringValidatorDependency> castedDep2 =
    rcp_dynamic_cast<StringValidatorDependency>(readinDep2, true);

  TEST_ASSERT(castedDep1->getValuesAndValidators().size() == 2);
  TEST_ASSERT(castedDep2->getValuesAndValidators().size() == 2);
  TEST_ASSERT(castedDep1->getDefaultValidator().is_null());
  TEST_ASSERT(nonnull(castedDep2->getDefaultValidator()));

  TEST_EQUALITY(
    rcp_dynamic_cast<const EnhancedNumberValidator<double> >(
      castedDep1->getValuesAndValidators().find("val1")->second, true)->getMax(),
    double1Vali->getMax());
  TEST_EQUALITY(
    rcp_dynamic_cast<const EnhancedNumberValidator<double> >(
      castedDep2->getValuesAndValidators().find("val1")->second, true)->getMax(),
    double1Vali->getMax());

  TEST_EQUALITY(
    rcp_dynamic_cast<const EnhancedNumberValidator<double> >(
      castedDep1->getValuesAndValidators().find("val2")->second, true)->getMax(),
    double2Vali->getMax());
  TEST_EQUALITY(
    rcp_dynamic_cast<const EnhancedNumberValidator<double> >(
      castedDep2->getValuesAndValidators().find("val2")->second, true)->getMax(),
    double2Vali->getMax());

  TEST_EQUALITY(
    rcp_dynamic_cast<const EnhancedNumberValidator<double> >(
      castedDep2->getDefaultValidator(), true)->getMax(),
    defaultVali->getMax());

}

TEUCHOS_UNIT_TEST(Teuchos_Dependencies, BoolValidatorDepSerialization){
  std::string dependee1 = "bool param";
  std::string dependee2 = "bool param2";
  std::string dependent1 = "dependent param1";
  std::string dependent2 = "dependent param2";
  ParameterList myDepList("Bool Vali Dep List");
  RCP<DependencySheet> myDepSheet = rcp(new DependencySheet);
  myDepList.set(dependee1, true);
  myDepList.set(dependee2, false);
  myDepList.set(dependent1, 2.0);
  myDepList.set(dependent2, 3.0);

	RCP<EnhancedNumberValidator<double> > true1Vali =
    rcp(new EnhancedNumberValidator<double>(0,10));

	RCP<EnhancedNumberValidator<double> > false1Vali =
    rcp(new EnhancedNumberValidator<double>(0,30));

	RCP<EnhancedNumberValidator<double> > true2Vali =
    rcp(new EnhancedNumberValidator<double>(4,90));


  RCP<BoolValidatorDependency> simpleBoolValiDep = rcp(
    new BoolValidatorDependency(
      myDepList.getEntryRCP(dependee1),
      myDepList.getEntryRCP(dependent1),
      true1Vali,
      false1Vali));

  Dependency::ParameterEntryList dependentList;
  dependentList.insert(myDepList.getEntryRCP(dependent1));
  dependentList.insert(myDepList.getEntryRCP(dependent2));

  RCP<BoolValidatorDependency> complexBoolValiDep = rcp(
    new BoolValidatorDependency(
      myDepList.getEntryRCP(dependee2),
      dependentList,
      true2Vali));

  myDepSheet->addDependency(simpleBoolValiDep);
  myDepSheet->addDependency(complexBoolValiDep);

  RCP<DependencySheet> readInDepSheet = rcp(new DependencySheet);

  XMLParameterListWriter plWriter;
  XMLObject xmlOut = plWriter.toXML(myDepList, myDepSheet);
  out << xmlOut.toString();

  RCP<ParameterList> readInList = 
    writeThenReadPL(myDepList, myDepSheet, readInDepSheet); 

  RCP<ParameterEntry> readinDependee1 = readInList->getEntryRCP(dependee1);
  RCP<ParameterEntry> readinDependent1 = readInList->getEntryRCP(dependent1);
  RCP<ParameterEntry> readinDependee2 = readInList->getEntryRCP(dependee2);
  RCP<ParameterEntry> readinDependent2 = readInList->getEntryRCP(dependent2);
  
  RCP<Dependency> readinDep1 =
    *(readInDepSheet->getDependenciesForParameter(readinDependee1)->begin());

  RCP<Dependency> readinDep2 =
    *(readInDepSheet->getDependenciesForParameter(readinDependee2)->begin());

  BASIC_DEPENDENCY_TEST(readinDep1, BoolValidatorDependency, 1, 1);
  VERIFY_DEPENDEE(readinDep1, readinDependee1);
  VERIFY_DEPENDENT(readinDep1, readinDependent1);

  BASIC_DEPENDENCY_TEST(readinDep2, BoolValidatorDependency, 1, 2);
  VERIFY_DEPENDEE(readinDep2, readinDependee2);
  VERIFY_DEPENDENT(readinDep2, readinDependent1);
  VERIFY_DEPENDENT(readinDep2, readinDependent2);
    
    
  RCP<BoolValidatorDependency> castedDep1 =
    rcp_dynamic_cast<BoolValidatorDependency>(readinDep1, true);
  RCP<BoolValidatorDependency> castedDep2 =
    rcp_dynamic_cast<BoolValidatorDependency>(readinDep2, true);

  TEST_ASSERT(nonnull(castedDep1->getTrueValidator()));
  TEST_ASSERT(nonnull(castedDep1->getFalseValidator()));
  TEST_ASSERT(nonnull(castedDep2->getTrueValidator()));
  TEST_ASSERT(castedDep2->getFalseValidator().is_null());
  TEST_EQUALITY(
    rcp_dynamic_cast<const EnhancedNumberValidator<double> >(
      castedDep1->getTrueValidator(), true)->getMax(),
    true1Vali->getMax());
  TEST_EQUALITY(
    rcp_dynamic_cast<const EnhancedNumberValidator<double> >(
      castedDep1->getFalseValidator(), true)->getMax(),
    false1Vali->getMax());
  TEST_EQUALITY(
    rcp_dynamic_cast<const EnhancedNumberValidator<double> >(
      castedDep2->getTrueValidator(), true)->getMax(),
    true2Vali->getMax());

}


TEUCHOS_UNIT_TEST_TEMPLATE_1_DECL(
  Teuchos_Dependencies, RangeValidatorDepSerialization, T)
{
  std::string dependee1 = "dependee param";
  std::string dependee2 = "dependee param2";
  std::string dependent1 = "dependent param1";
  std::string dependent2 = "dependent param2";
  ParameterList myDepList("Range Vali Dep List");
  RCP<DependencySheet> myDepSheet = rcp(new DependencySheet);
  myDepList.set(dependee1, ScalarTraits<T>::one());
  myDepList.set(dependee2, ScalarTraits<T>::one());
  myDepList.set(dependent1, 2.0);
  myDepList.set(dependent2, 3.0);

	RCP<EnhancedNumberValidator<double> > double1Vali =
    rcp(new EnhancedNumberValidator<double>(0,10));

	RCP<EnhancedNumberValidator<double> > double2Vali =
    rcp(new EnhancedNumberValidator<double>(0,30));

  typename RangeValidatorDependency<T>::Range range1(0,10);
  typename RangeValidatorDependency<T>::Range range2(11,50);

  typename RangeValidatorDependency<T>::RangeToValidatorMap rangeValiMap;
  rangeValiMap[range1] = double1Vali;
  rangeValiMap[range2] = double2Vali;

  RCP<RangeValidatorDependency<T> > simpleRangeValiDep = rcp(
    new RangeValidatorDependency<T> (
      myDepList.getEntryRCP(dependee1),
      myDepList.getEntryRCP(dependent1),
      rangeValiMap));

  Dependency::ParameterEntryList dependentList;
  dependentList.insert(myDepList.getEntryRCP(dependent1));
  dependentList.insert(myDepList.getEntryRCP(dependent2));

  RCP<RangeValidatorDependency<T> > complexRangeValiDep = rcp(
    new RangeValidatorDependency<T> (
      myDepList.getEntryRCP(dependee2),
      dependentList,
      rangeValiMap));

  myDepSheet->addDependency(simpleRangeValiDep);
  myDepSheet->addDependency(complexRangeValiDep);

  RCP<DependencySheet> readInDepSheet = rcp(new DependencySheet);

  XMLParameterListWriter plWriter;
  XMLObject xmlOut = plWriter.toXML(myDepList, myDepSheet);
  out << xmlOut.toString();

  RCP<ParameterList> readInList = 
    writeThenReadPL(myDepList, myDepSheet, readInDepSheet); 

  RCP<ParameterEntry> readinDependee1 = readInList->getEntryRCP(dependee1);
  RCP<ParameterEntry> readinDependent1 = readInList->getEntryRCP(dependent1);
  RCP<ParameterEntry> readinDependee2 = readInList->getEntryRCP(dependee2);
  RCP<ParameterEntry> readinDependent2 = readInList->getEntryRCP(dependent2);
  
  RCP<Dependency> readinDep1 =
    *(readInDepSheet->getDependenciesForParameter(readinDependee1)->begin());

  RCP<Dependency> readinDep2 =
    *(readInDepSheet->getDependenciesForParameter(readinDependee2)->begin());

  BASIC_DEPENDENCY_TEST(readinDep1, RangeValidatorDependency<T>, 1, 1);
  VERIFY_DEPENDEE(readinDep1, readinDependee1);
  VERIFY_DEPENDENT(readinDep1, readinDependent1);

  BASIC_DEPENDENCY_TEST(readinDep2, RangeValidatorDependency<T>, 1, 2);
  VERIFY_DEPENDEE(readinDep2, readinDependee2);
  VERIFY_DEPENDENT(readinDep2, readinDependent1);
  VERIFY_DEPENDENT(readinDep2, readinDependent2);
    
    
  RCP<RangeValidatorDependency<T> > castedDep1 =
    rcp_dynamic_cast<RangeValidatorDependency<T> >(readinDep1, true);
  RCP<RangeValidatorDependency<T> > castedDep2 =
    rcp_dynamic_cast<RangeValidatorDependency<T> >(readinDep2, true);

  typename RangeValidatorDependency<T>::RangeToValidatorMap readinMap1 = 
    castedDep1->getRangeToValidatorMap();
  TEST_EQUALITY(readinMap1.size(), 2);
  typename RangeValidatorDependency<T>::RangeToValidatorMap::const_iterator it =
    readinMap1.begin();
  TEST_EQUALITY(it->first.first, 0);
  TEST_EQUALITY(it->first.second, 10);
  it++;
  TEST_EQUALITY(it->first.first, 11);
  TEST_EQUALITY(it->first.second, 50);

    
  RCP<const ParameterEntryValidator> range1Vali = 
    readinMap1.find(range1)->second;
  RCP<const ParameterEntryValidator> range2Vali = 
    readinMap1.find(range2)->second;
  TEST_EQUALITY(
    rcp_dynamic_cast<const EnhancedNumberValidator<double> >(
      range1Vali, true)->getMax(),
    double1Vali->getMax());
  TEST_EQUALITY(
    rcp_dynamic_cast<const EnhancedNumberValidator<double> >(
      range2Vali, true)->getMax(),
    double2Vali->getMax());

  typename RangeValidatorDependency<T>::RangeToValidatorMap readinMap2 = 
    castedDep2->getRangeToValidatorMap();
  it = readinMap2.begin();
  TEST_EQUALITY(it->first.first, 0);
  TEST_EQUALITY(it->first.second, 10);
  it++;
  TEST_EQUALITY(it->first.first, 11);
  TEST_EQUALITY(it->first.second, 50);

  TEST_EQUALITY(
    rcp_dynamic_cast<const EnhancedNumberValidator<double> >(
      readinMap2.find(range1)->second, true)->getMax(),
    double1Vali->getMax());
  TEST_EQUALITY(
    rcp_dynamic_cast<const EnhancedNumberValidator<double> >(
      readinMap2.find(range2)->second, true)->getMax(),
    double2Vali->getMax());
}


#define RANGE_VALIDATOR_TEST(T) \
TEUCHOS_UNIT_TEST_TEMPLATE_1_INSTANT( \
  Teuchos_Dependencies, RangeValidatorDepSerialization, T)
  
RANGE_VALIDATOR_TEST(int)
RANGE_VALIDATOR_TEST(double)
RANGE_VALIDATOR_TEST(float)
#ifdef HAVE_TEUCHOS_LONG_LONG_INT
RANGE_VALIDATOR_TEST(llint)
#endif

/* General Testing*/
TEUCHOS_UNIT_TEST(Teuchos_Dependencies, DependencySerializationExceptions){
  
  RCP<DependencySheet> depSheet = rcp(new DependencySheet);
  TEST_THROW(RCP<ParameterList> missingDependeeList = 
    getParametersFromXmlFile("MissingDependeeTag.xml", depSheet),
    MissingDependeesException);
  TEST_THROW(RCP<ParameterList> missingDependentsList = 
    getParametersFromXmlFile("MissingDependentTag.xml", depSheet),
    MissingDependentsException);
  TEST_THROW(RCP<ParameterList> missingDependeeList = 
    getParametersFromXmlFile("MissingDependee.xml", depSheet),
    MissingDependeeException);
  TEST_THROW(RCP<ParameterList> missingDependentList = 
    getParametersFromXmlFile("MissingDependent.xml", depSheet),
    MissingDependentException);

  RCP<ParameterEntry> dependeeParam = rcp(new ParameterEntry(true));
  RCP<ParameterEntry> dependentParam = rcp(new ParameterEntry("blah"));
  RCP<BoolVisualDependency> boolVisDep = 
    rcp(new BoolVisualDependency(dependeeParam, dependentParam));

  XMLParameterListWriter::EntryIDsMap entryIDsMap;
  entryIDsMap[dependentParam] = 1;
  ValidatortoIDMap validatorIDsMap;
  BoolVisualDependencyXMLConverter boolVisConverter;
  TEST_THROW(
    boolVisConverter.fromDependencytoXML(
    boolVisDep, entryIDsMap, validatorIDsMap),
    MissingDependeeException);
  entryIDsMap.erase(dependentParam);
  entryIDsMap[dependeeParam] = 3;
  TEST_THROW(
    boolVisConverter.fromDependencytoXML(
    boolVisDep, entryIDsMap, validatorIDsMap),
    MissingDependentException);
}

/* Testing serialization exceptions for NumberVisualDependencies */


#define TOO_MANY_DEPENDEES_TEST( \
  NAME, \
  DependeeDefaultValue, \
  DependentDefaultValue, \
  ConstructorCode, \
  PreConstructorCode \
  ) \
 \
TEUCHOS_UNIT_TEST(Teuchos_Dependencies, NAME##TooManyDependeesTest) \
{ \
  RCP<ParameterEntry> dependeeParam = rcp( \
    new ParameterEntry( DependeeDefaultValue )); \
  RCP<ParameterEntry> extraDependeeParam =  \
    rcp(new ParameterEntry( DependeeDefaultValue )); \
  RCP<ParameterEntry> dependentParam =  \
    rcp(new ParameterEntry( DependentDefaultValue )); \
 \
  XMLParameterListWriter::EntryIDsMap writerEntryMap; \
  writerEntryMap[dependeeParam] = 1; \
  writerEntryMap[dependentParam] = 2; \
  ValidatortoIDMap writerValiMap; \
 \
  XMLParameterListReader::EntryIDsMap readerEntryMap; \
  readerEntryMap[1] = dependeeParam; \
  readerEntryMap[2] = dependentParam; \
  readerEntryMap[3] = extraDependeeParam; \
  IDtoValidatorMap readerValiMap; \
 \
  PreConstructorCode \
  RCP<Dependency> dep =  \
    rcp(new ConstructorCode ); \
 \
  XMLObject depXML = DependencyXMLConverterDB::convertDependency( \
    dep, writerEntryMap, writerValiMap); \
 \
  XMLObject extraDependee(DependencyXMLConverter::getDependeeTagName()); \
  extraDependee.addAttribute<ParameterEntry::ParameterEntryID>( \
    DependencyXMLConverter::getParameterIDAttributeName(), 3); \
  depXML.addChild(extraDependee); \
 \
  TEST_THROW( \
    DependencyXMLConverterDB::convertXML( \
      depXML, readerEntryMap, readerValiMap), \
    TooManyDependeesException); \
 \
} 

TOO_MANY_DEPENDEES_TEST(
  NumberVisualDependency,
  4,
  "blah",
  NumberVisualDependency<int>(dependeeParam, dependentParam),
  {}
)

TOO_MANY_DEPENDEES_TEST(
  BoolVisualDependency,
  true,
  "blah",
  BoolVisualDependency(dependeeParam, dependentParam),
  {}
)

TOO_MANY_DEPENDEES_TEST(
  StringVisualDependency,
  std::string("blah"),
  4,
  StringVisualDependency(dependeeParam, dependentParam, "steve"),
  {}
)

TOO_MANY_DEPENDEES_TEST(
  BoolValidatorDependency,
  true,
  4,
  BoolValidatorDependency(dependeeParam, dependentParam, null),
  {}
)

TOO_MANY_DEPENDEES_TEST(
  StringValidatorDependency,
  std::string("blah"),
  4,
  StringValidatorDependency(dependeeParam, dependentParam, valiMap),
  RCP<FileNameValidator> scrapVali = 
    DummyObjectGetter<FileNameValidator>::getDummyObject();
  StringValidatorDependency::ValueToValidatorMap valiMap;
  valiMap["blah"] = scrapVali;
  writerValiMap.insert(scrapVali);
  readerValiMap.insert(
    IDtoValidatorMap::IDValidatorPair(
      writerValiMap.find(scrapVali)->second,scrapVali));
)

TOO_MANY_DEPENDEES_TEST(
  RangeValidatorDependency,
  4,
  "blah",
  RangeValidatorDependency<int>(dependeeParam, dependentParam, valiMap),
  RCP<FileNameValidator> scrapVali = 
    DummyObjectGetter<FileNameValidator>::getDummyObject();
  RangeValidatorDependency<int>::RangeToValidatorMap valiMap;
  RangeValidatorDependency<int>::Range scrapRange(2,5);
  valiMap[scrapRange] = scrapVali;
  writerValiMap.insert(scrapVali);
  readerValiMap.insert(
    IDtoValidatorMap::IDValidatorPair(
      writerValiMap.find(scrapVali)->second,scrapVali));
)

typedef NumberArrayLengthDependency<int, double> IntDoubleArrayLengthDep;
TOO_MANY_DEPENDEES_TEST(
  NumberArrayLengthDependency,
  4,
  Array<double>(4,3.0),
  IntDoubleArrayLengthDep(dependeeParam, dependentParam),
  {}
)

TEUCHOS_UNIT_TEST(
  Teuchos_Dependencies, RangeValidatorDepSerializationExceptions)
{

  RCP<ParameterEntry> dependeeParam = rcp( 
    new ParameterEntry( 3 )); 
  RCP<ParameterEntry> dependentParam =  
    rcp(new ParameterEntry( "blah" )); 
 
  XMLParameterListWriter::EntryIDsMap writerEntryMap; 
  writerEntryMap[dependeeParam] = 1; 
  writerEntryMap[dependentParam] = 2; 
  ValidatortoIDMap writerValiMap; 
 
  XMLParameterListReader::EntryIDsMap readerEntryMap; 
  readerEntryMap[1] = dependeeParam; 
  readerEntryMap[2] = dependentParam; 
  IDtoValidatorMap readerValiMap; 

  RCP<FileNameValidator> scrapVali = 
    DummyObjectGetter<FileNameValidator>::getDummyObject();
  RangeValidatorDependency<int>::RangeToValidatorMap valiMap;
  RangeValidatorDependency<int>::Range scrapRange(2,5);
  valiMap[scrapRange] = scrapVali;
  writerValiMap.insert(scrapVali);
  readerValiMap.insert(
    IDtoValidatorMap::IDValidatorPair(
      writerValiMap.find(scrapVali)->second,scrapVali));

  RCP<Dependency> rangeDep = 
   rcp(new RangeValidatorDependency<int>(
    dependeeParam, dependentParam, valiMap));

  XMLObject rangeDepXML = DependencyXMLConverterDB::convertDependency(
    rangeDep, writerEntryMap, writerValiMap);

  XMLObject missingRangesXML(rangeDepXML.getTag());
  missingRangesXML.addAttribute(
    DependencyXMLConverter::getTypeAttributeName(),
    rangeDepXML.getAttribute(DependencyXMLConverter::getTypeAttributeName()));
  for(int i =0; i< rangeDepXML.numChildren(); i++){
    if(rangeDepXML.getChild(i).getTag() !=
      RangeValidatorDependencyXMLConverter<int>::getRangesAndValidatorsTag())
    {
      missingRangesXML.addChild(rangeDepXML.getChild(i).deepCopy());
    }
  }

  TEST_THROW(
    DependencyXMLConverterDB::convertXML(
      missingRangesXML, readerEntryMap, readerValiMap),
    MissingRangesAndValidatorsTagException);

  readerValiMap.erase(writerValiMap.find(scrapVali)->second);
  TEST_THROW(
    DependencyXMLConverterDB::convertXML(
     rangeDepXML, readerEntryMap, readerValiMap),
    MissingValidatorException);
}

TEUCHOS_UNIT_TEST(
  Teuchos_Dependencies, StringValidatordDepSerializationExceptions)
{


  RCP<ParameterEntry> dependeeParam = rcp( 
    new ParameterEntry( std::string("blah") )); 
  RCP<ParameterEntry> dependentParam =  
    rcp(new ParameterEntry( "blah" )); 
 
  XMLParameterListWriter::EntryIDsMap writerEntryMap; 
  writerEntryMap[dependeeParam] = 1; 
  writerEntryMap[dependentParam] = 2; 
  ValidatortoIDMap writerValiMap; 
 
  XMLParameterListReader::EntryIDsMap readerEntryMap; 
  readerEntryMap[1] = dependeeParam; 
  readerEntryMap[2] = dependentParam; 
  IDtoValidatorMap readerValiMap; 

  RCP<FileNameValidator> scrapVali = 
    DummyObjectGetter<FileNameValidator>::getDummyObject();
  RCP<FileNameValidator> scrapVali2 = 
    DummyObjectGetter<FileNameValidator>::getDummyObject();
  StringValidatorDependency::ValueToValidatorMap valiMap;
  valiMap["blah"] = scrapVali;
  writerValiMap.insert(scrapVali);
  writerValiMap.insert(scrapVali2);
  readerValiMap.insert(
    IDtoValidatorMap::IDValidatorPair(
      writerValiMap.find(scrapVali)->second,scrapVali));
  readerValiMap.insert(
    IDtoValidatorMap::IDValidatorPair(
      writerValiMap.find(scrapVali2)->second,scrapVali2));

   RCP<Dependency> stringValiDep = 
     rcp(new StringValidatorDependency(
      dependeeParam, dependentParam, valiMap, scrapVali2));

   XMLObject stringValiDepXML = DependencyXMLConverterDB::convertDependency(
    stringValiDep, writerEntryMap, writerValiMap); 

  XMLObject missingValuesXML(stringValiDepXML.getTag());
  missingValuesXML.addAttribute(
    DependencyXMLConverter::getTypeAttributeName(),
    stringValiDepXML.getAttribute(
      DependencyXMLConverter::getTypeAttributeName()));

  for(int i =0; i< stringValiDepXML.numChildren(); i++){
    if(stringValiDepXML.getChild(i).getTag() !=
      StringValidatorDependencyXMLConverter::getValuesAndValidatorsTag())
    {
      missingValuesXML.addChild(stringValiDepXML.getChild(i).deepCopy());
    }
  }
  TEST_THROW(
    DependencyXMLConverterDB::convertXML(
      missingValuesXML, readerEntryMap, readerValiMap),
    MissingValuesAndValidatorsTagException);

  readerValiMap.erase(writerValiMap.find(scrapVali)->second);
  TEST_THROW(
    DependencyXMLConverterDB::convertXML(
      stringValiDepXML, readerEntryMap, readerValiMap),
    MissingValidatorException);

  readerValiMap.insert(
    IDtoValidatorMap::IDValidatorPair(
      writerValiMap.find(scrapVali)->second,scrapVali));
  readerValiMap.erase(writerValiMap.find(scrapVali2)->second);

  TEST_THROW(
    DependencyXMLConverterDB::convertXML(
      stringValiDepXML, readerEntryMap, readerValiMap),
    MissingValidatorException);
}

TEUCHOS_UNIT_TEST(
  Teuchos_Dependencies, ConditionVisualDepSerializationExceptions)
{


}


} //namespace Teuchos

