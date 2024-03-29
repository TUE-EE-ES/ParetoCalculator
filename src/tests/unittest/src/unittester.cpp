
//
// The MIT License
//
// Copyright (c) 2008-2019 Eindhoven University of Technology
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

//
// Author: Marc Geilen, e-mail: m.c.w.geilen@tue.nl
// Electronic Systems Group (ES), Department of Electrical Engineering,
// Eindhoven University of Technology
//
// Description:
// UnitTester class
//


#include "unittester.h"
#include <stdexcept>

#include "quantity.h"
#include "configuration.h"

#define ASSERT_THROW( condition, msg )                              \
{                                                                   \
  if( !( condition ) )                                              \
  {                                                                 \
    throw std::runtime_error(   std::string( msg )                  \
                              + std::string( "\nIn:" )              \
                              + std::string( __FILE__ )             \
                              + std::string( ":" )                  \
                              + std::to_string( __LINE__ )          \
                              + std::string( " in " )               \
                              + std::string( __FUNCTION__ )         \
    );                                                              \
  }                                                                 \
}

#define ASSERT_EQUAL( x, y, msg )                                   \
{                                                                   \
  if( ( x ) != ( y ) )                                              \
  {                                                                 \
    throw std::runtime_error(   std::string( msg )                  \
                              + std::string( "\nIn:" )              \
                              + std::string( __FILE__ )             \
                              + std::string( ":" )                  \
                              + std::to_string( __LINE__ )          \
                              + std::string( " in " )               \
                              + std::string( __FUNCTION__ )         \
                              + std::string( ": " )                 \
                              + std::to_string( ( x ) )             \
                              + std::string( " != " )               \
                              + std::to_string( ( y ) )             \
    );                                                              \
  }                                                                 \
}



using namespace Pareto;

UnitTesterException::UnitTesterException(const std::string s) {
	errorMsg = s;
}

UnitTester::UnitTester(){
	this->generator.seed(4321);
}


bool UnitTester::test_all(void) {

	// checking if the asserts work
	ASSERT_THROW(true, "ASSERT failed!");
	ASSERT_EQUAL(0, 0, "Zero is not zero!");

	ASSERT_THROW(this->test_calculator(), "Calculator tests failed.");
	return true;
}

bool UnitTester::test_calculator(void) {
	ASSERT_THROW(this->test_PCStore(), "Store test failed.");
	ASSERT_THROW(this->test_DCMinimization(), "Divide and Conquer minimization failed.");
	return true;
}

bool UnitTester::test_PCStore(void)
{

	StorableStringPtr s1 = std::make_shared<StorableString>("TestString1");
	StorableStringPtr s2 = std::make_shared<StorableString>("TestString2");
	
	// put s1 and s2 on the stack
	this->PC.push(s1);
	this->PC.push(s2);

	// store s1 in memory under its proper name
	this->PC.store(s1);

	// store s2 in memory under the name "key2"
	this->PC.store(s2, "key2");

	// pop s2 from the stack and store it in memory, filed under its name
	this->PC.store();

	// pop s1 from the stack and store it in memory, filed under 'key1'
	this->PC.store("key1");

	// retriev results from memory
	StorableStringPtr t1 = this->PC.retrieveStorableString("TestString1");
	StorableStringPtr t2 = this->PC.retrieveStorableString("TestString2");
	StorableStringPtr t3 = this->PC.retrieveStorableString("key1");
	StorableStringPtr t4 = this->PC.retrieveStorableString("key2");

	// check results
	ASSERT_THROW(t1->name == t3->name, "Memory objects are different.")
	ASSERT_THROW(t2->name == t2->name, "Memory objects are different.")

	// check if stack is empty
	ASSERT_THROW(this->PC.stack.empty(), "Stack is not empty.")

	// clear memory
	this->PC.eraseMemory();

	// check if memory is empty
	ASSERT_THROW(this->PC.memory.empty(), "Memory is not empty.")

	return true;
}

bool UnitTester::test_DCMinimization(void) {

	// create a configuration space
	ConfigurationSpacePtr CS = std::make_shared< ConfigurationSpace>("TestDC");
	QuantityTypePtr TA = std::make_shared<QuantityType_Integer>("QuantityA");
	QuantityTypePtr TB = std::make_shared<QuantityType_Real>("QuantityB");
	QuantityTypePtr TC = std::make_shared<QuantityType_Integer>("QuantityC");
	QuantityTypePtr TD = std::make_shared<QuantityType_Real>("QuantityD");
	QuantityType_EnumPtr TE = std::make_shared<QuantityType_Enum>("QuantityE_Unordered");
	TE->addQuantity("U1");
	TE->addQuantity("U2");
	TE->addQuantity("U3");
	TE->addQuantity("U4");
	QuantityType_EnumPtr TF = std::make_shared<QuantityType_Enum>("QuantityF_Ordered");
	TF->addQuantity("O1");
	TF->addQuantity("O2");
	TF->addQuantity("O3");
	TF->addQuantity("O4");

	CS->addQuantity(TA);
	CS->addQuantity(TB);
	CS->addQuantity(TC);
	CS->addQuantity(TD);
	CS->addQuantity(TE);
	CS->addQuantity(TF);

	// create random configuration set
	ConfigurationSetPtr C = std::make_shared<ConfigurationSet>(CS, "TestConfigurationSet");
	std::uniform_int_distribution<> dis_int(1, 1000);
	std::uniform_int_distribution<> dis_enum(0, 3);
	std::uniform_real_distribution<> dis_real(1.0, 2.0);
	for (unsigned int i = 0; i < 1000; i++) {
		ConfigurationPtr c = std::make_shared<Configuration>(CS);
		QuantityValuePtr qA = std::make_shared<QuantityValue_Integer>(*TA, dis_int(this->generator));
		c->addQuantity(qA);
		QuantityValuePtr qB = std::make_shared<QuantityValue_Real>(*TB, dis_real(this->generator));
		c->addQuantity(qB);
		QuantityValuePtr qC = std::make_shared<QuantityValue_Integer>(*TC, dis_int(this->generator));
		c->addQuantity(qC);
		QuantityValuePtr qD = std::make_shared<QuantityValue_Real>(*TD, dis_real(this->generator));
		c->addQuantity(qD);
		QuantityValuePtr qE = std::make_shared<QuantityValue_Enum>(*TE, dis_enum(this->generator));
		c->addQuantity(qE);
		QuantityValuePtr qF = std::make_shared<QuantityValue_Enum>(*TF, dis_enum(this->generator));
		c->addQuantity(qF);
		C->addConfiguration(c);
	}

	// compute Pareto points with simple cull and with DC approach and see of the results are the same
	// use low thresholds in DC minimize to ensure it tests the rescursive approach.
	ConfigurationSetPtr CM1 = PC.efficient_minimize(C, 10, 10);
	ConfigurationSetPtr CM2 = PC.minimize(C);

	// check if both sets are equal by computing their intersection
	ConfigurationSetPtr CM3 = PC.constraint(CM1, CM2);

	// check that the number of Pareto points are equal
	ASSERT_EQUAL(CM1->confs.size(), CM3->confs.size(), "DC and SC minimization give different results.")

	return true;
}