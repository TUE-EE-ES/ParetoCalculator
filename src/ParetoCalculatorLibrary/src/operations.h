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
// Defines the ParetoCalculatorOperation class and its subclasses
// representing the operations to be performed by the calculator.
//

#ifndef PARETO_OPERATIONS_H
#define PARETO_OPERATIONS_H

#include <vector>
#include <string>
#include "exceptions.h"
#include "support.h"
#include "storage.h"
#include "configuration.h"


namespace Pareto {

	class ParetoCalculator;

	/// An operation to be executed on a Pareto Calculator
	class ParetoCalculatorOperation {
	public:		
		/// executes the operation on calculator 'c'
		virtual void executeOn(ParetoCalculator& c) = 0;
	};

	/// The block-based minimization operator
	class POperation_Minimize: public ParetoCalculatorOperation {
	public:
		POperation_Minimize(void);
		virtual void executeOn(ParetoCalculator& c);
	};

	/// The generalized multidimansional Dividide-and-Conquer minimization operator
	class POperation_EfficientMinimize: public ParetoCalculatorOperation {
	public:
		POperation_EfficientMinimize(void);
		virtual void executeOn(ParetoCalculator& c);
	};

	/// The product operator
	class POperation_Product: public ParetoCalculatorOperation {
	public:
		POperation_Product(void);
		virtual void executeOn(ParetoCalculator& c);
	};

	/// A push operation to push a named element from the memory onto the stack
	class POperation_Push: public ParetoCalculatorOperation {
	public:
		std::string o;
		POperation_Push(const std::string& oname);
		virtual void executeOn(ParetoCalculator& c);
	};

	/// The abstraction operator
	class POperation_Abstract: public ParetoCalculatorOperation {
	public:
		ListOfQuantityNames& lqn;
		POperation_Abstract(ListOfQuantityNames& qnames);
		virtual void executeOn(ParetoCalculator& c);
	};

	/// The hiding operator
	class POperation_Hide: public ParetoCalculatorOperation {
	public:
		ListOfQuantityNames& lqn;
		POperation_Hide(ListOfQuantityNames& qnames);
		virtual void executeOn(ParetoCalculator& c);
	};

	/// A store operation pops an object from the stack and stores it in the 
	/// memory of the calculator.
	class POperation_Store: public ParetoCalculatorOperation {
	public:
		std::string o;
		POperation_Store(const std::string& oname);
		virtual void executeOn(ParetoCalculator& c);
	};

	/// Pops an element from the calculator stack
	class POperation_Pop: public ParetoCalculatorOperation {
	public:
		POperation_Pop(void);
		virtual void executeOn(ParetoCalculator& c);
	};

	/// Duplicates the object on top of the stack
	class POperation_Duplicate: public ParetoCalculatorOperation {
	public:
		POperation_Duplicate(void);
		virtual void executeOn(ParetoCalculator& c);
	};

	/// pops an object from the stack and prints it if the caalculator
	/// is being monitored.
	class POperation_Print: public ParetoCalculatorOperation {
	public:
		POperation_Print(void);
		virtual void executeOn(ParetoCalculator& c);
	};

	/// The (naive implementation of) producer-consumer constraint operation between two quantities.
	class POperation_ProdCons: public ParetoCalculatorOperation {
	public:
		std::string p_quant, c_quant;
		POperation_ProdCons(const std::string& pqname, const std::string& cqname);
		static bool testConstraint(const Configuration& c, const std::string& p_quant, const std::string& c_quant);
		virtual void executeOn(ParetoCalculator& c);
	};

	/// An efficient implementation of a producer-concumer constraint operation. Dimensions 
	/// user as producer or consumer must be totally ordered.
	class POperation_EfficientProdCons: public ParetoCalculatorOperation {
	public:
		std::string p_quant, c_quant;
		POperation_EfficientProdCons(const std::string& pqname, const std::string& cqname);
		virtual void executeOn(ParetoCalculator& c);
		bool default_match(const QuantityValue& va, const QuantityValue& vb) const;
	};

	/// Superclass for operations for computing various derived quantities
	class POperation_Derived: public ParetoCalculatorOperation {
	public:
		POperation_Derived(const std::string& aqname, const std::string& bqname);
		virtual void executeOn(ParetoCalculator& c);
	private:
		std::string a_quant, b_quant;
		virtual double derive(double a, double b) = 0;
		virtual std::string description() = 0;
	};

	/// Operation for computing derived quantity: sum of two quantities
	class POperation_Sum: public  POperation_Derived {
	public:
		POperation_Sum(const std::string& aqname, const std::string& bqname);
	private:
		virtual double derive(double a, double b);
		virtual std::string description();
	};

	/// Operation for computing derived quantity: max of two quantities
	class POperation_Max: public POperation_Derived {
	public:
		POperation_Max(const std::string& aqname, const std::string& bqname);
	private:
		virtual double derive(double a, double b);
		virtual std::string description();
	};

	/// Operation for computing derived quantity: min of two quantities
	class POperation_Min: public POperation_Derived {
	public:
		POperation_Min(const std::string& aqname, const std::string& bqname);
	private:
		virtual double derive(double a, double b);
		virtual std::string description();
	};

	/// Operation for computing derived quantity: product of two quantities
	class POperation_Multiply: public POperation_Derived {
	public:
		POperation_Multiply(const std::string& aqname, const std::string& bqname);
	private:
		virtual double derive(double a, double b);
		virtual std::string description();
	};

	/// Operation for computing the aggregation (sum) of all quantities given in
	/// a list of quantity names
	class POperation_Aggregate: public ParetoCalculatorOperation {
	public:
		POperation_Aggregate(ListOfQuantityNames& ag_quants, std::string& agname);
		virtual void executeOn(ParetoCalculator& c);
	private:
		ListOfQuantityNames& aggregate_quants;
		std::string newName;
	};

	/// An STL map from strings to string expressing how two configuration
	/// are going to be joined in teh join operation
	class JoinMap : public StringMap{};

	/// The (naive) join operation. First constructs the entire product, then selects.
	/// Consider using EfficientJoin
	class POperation_Join: public ParetoCalculatorOperation {
	public:
		const JoinMap& j_quants;
		POperation_Join(const JoinMap& jqnamemap);
		static bool testConstraint(const Configuration& c, const std::vector<int>& qan, const std::vector<int>& qbn);
		virtual void executeOn(ParetoCalculator& c);
	};

	/// An efficient sort-merge join operation. Works with an arbitrary number of totally ordered or
	/// unorderedquantities
	/// 
	/// The operation will be extended to multiple quantities and non-totally-ordered quantities 
	/// in future versions. For now consider using the naive join operation...
	class POperation_EfficientJoin: public ParetoCalculatorOperation {
	public:
		JoinMap& j_quants;
		POperation_EfficientJoin(JoinMap& jqnamemap);
		virtual void executeOn(ParetoCalculator& c);
	};

}

#endif
