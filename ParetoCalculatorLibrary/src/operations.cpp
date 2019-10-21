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
// Implementation of the ParetoCalculatorOperation class and its subclasses
// Representing the operations to be performed by the calculator.
//

#include "operations.h"
#include "calculator.h"
#include <string>
#include <sstream>

using namespace Pareto;


// Some C++ thing, making me initialise the static members in POperation_ProdCons here...
std::string* POperation_ProdCons::p_test = nullptr;
std::string* POperation_ProdCons::c_test = nullptr;
std::vector<int> POperation_Join::qan;
std::vector<int> POperation_Join::qbn;



ParetoCalculatorOperation::ParetoCalculatorOperation() {
}

void ParetoCalculatorOperation::executeOn(ParetoCalculator& c) {
	throw EParetoCalculatorError("Cannot execute abstract superclass of operations in ParetoCalculatorOperation::executeOn.");
}


POperation_Minimize::POperation_Minimize() : ParetoCalculatorOperation() {
}

void POperation_Minimize::executeOn(ParetoCalculator& c) {
	c.efficient_minimize();
}

POperation_EfficientMinimize::POperation_EfficientMinimize() : ParetoCalculatorOperation() {
}

void POperation_EfficientMinimize::executeOn(ParetoCalculator& c) {
	c.efficient_minimize();
}

POperation_Product::POperation_Product() : ParetoCalculatorOperation() {
}

void POperation_Product::executeOn(ParetoCalculator& c) {
	c.product();
};

POperation_Push::POperation_Push(const std::string& oname) : ParetoCalculatorOperation() {
	o = oname;
}

void POperation_Push::executeOn(ParetoCalculator& c) {
	c.push(o);
}

POperation_Store::POperation_Store(const std::string& oname) : ParetoCalculatorOperation() {
	o = oname;
}

void POperation_Store::executeOn(ParetoCalculator& c) {
	StorableObjectPtr ob = c.pop();
	StorableObjectPtr nob = ob->copy();
	nob->name = o;
	c.store(nob);
}

POperation_Duplicate::POperation_Duplicate() : ParetoCalculatorOperation() {
}

void POperation_Duplicate::executeOn(ParetoCalculator& c) {
	c.duplicate();
}

POperation_Pop::POperation_Pop() : ParetoCalculatorOperation() {
}

void POperation_Pop::executeOn(ParetoCalculator& c) {
	c.pop();
}

POperation_Print::POperation_Print() : ParetoCalculatorOperation() {
}

void POperation_Print::executeOn(ParetoCalculator& c) {
	c.print();
}

POperation_ProdCons::POperation_ProdCons(const std::string& pqname, const std::string& cqname) : ParetoCalculatorOperation() {
	p_quant = pqname;
	c_quant = cqname;
}


void POperation_ProdCons::executeOn(ParetoCalculator& c) {
	p_test = &p_quant;
	c_test = &c_quant;
	ConfigurationSetPtr cs = c.popConfigurationSet();
	ConfigurationSetPtr ncs = c.constraint(cs, &POperation_ProdCons::testConstraint);
	c.push(ncs);
}

bool POperation_ProdCons::testConstraint(const Pareto::Configuration& c) {
	QuantityValue_RealPtr pq = std::dynamic_pointer_cast<QuantityValue_Real>(c.getQuantity(*p_test));
	QuantityValue_RealPtr cq = std::dynamic_pointer_cast<QuantityValue_Real>(c.getQuantity(*c_test));
	return cq->value <= (1.0 / pq->value);
}

POperation_Derived::POperation_Derived(const std::string& aqname, const std::string& bqname) : ParetoCalculatorOperation() {
	a_quant = aqname;
	b_quant = bqname;
}

void POperation_Derived::executeOn(ParetoCalculator& c) {

	ConfigurationSetPtr cs = c.popConfigurationSet();

	// define new sum quantity type
	std::ostringstream sqn;
	sqn << this->description() << " of " << a_quant << " and " << b_quant;
	std::shared_ptr<QuantityType_Real> sqt = std::make_shared<QuantityType_Real>(sqn.str());

	// build new configuration space
	std::ostringstream sqsn;
	sqsn << this->description() << " (" << cs->confspace->name << ")";
	ConfigurationSpacePtr scs = std::make_shared<ConfigurationSpace>(sqsn.str());
	scs->addQuantitiesOf(*(cs->confspace));
	scs->addQuantity(*sqt);

	// build new set of configurations
	std::ostringstream sconfsn;
	sconfsn << this->description() << " (" << cs->name << ", " << a_quant << ", " << b_quant << ")";

	ConfigurationSetPtr sconfs = std::make_shared<ConfigurationSet>(scs, sconfsn.str());

	SetOfConfigurations::iterator i;
	for (i = cs->confs.begin(); i != cs->confs.end(); i++) {
		ConfigurationPtr c = *i;
		ConfigurationPtr scf = std::make_shared<Configuration>(*scs);
		scf->addQuantitiesOf(c);

		double va = (*(std::dynamic_pointer_cast<QuantityValue_RealPtr>(c->getQuantity(a_quant))))->value;
		double vb = (*(std::dynamic_pointer_cast<QuantityValue_RealPtr>(c->getQuantity(b_quant))))->value;

		QuantityValue_RealPtr sqv = std::make_shared<QuantityValue_Real>(*sqt, this->derive(va, +vb));
		scf->addQuantity(sqv);
		sconfs->addUniqueConfiguration(scf);
	}
	c.push(sconfs);
}

POperation_Aggregate::POperation_Aggregate(ListOfQuantityNames& ag_quants, std::string& agname):
	aggregate_quants(ag_quants)
{
	this->newName = agname;
}

void POperation_Aggregate::executeOn(ParetoCalculator& c)
{
	// get the configuratoin set to aggregate on
	ConfigurationSetPtr cs = c.popConfigurationSet();

	// define new aggregate quantity type
	std::shared_ptr<QuantityType_Real> sqt = std::make_shared<QuantityType_Real>(this->newName);

	// build new configuration space
	std::ostringstream sqsn;
	sqsn << "Aggregation" << " (" << cs->confspace->name << ")";
	ConfigurationSpacePtr scs = std::make_shared<ConfigurationSpace>(sqsn.str());
	scs->addQuantitiesOf(*(cs->confspace));
	scs->addQuantity(*sqt);

	// build new set of configurations
	std::ostringstream sconfsn;
	sconfsn << "Aggregation" << " (" << this->newName << ")";
	ConfigurationSetPtr sconfs = std::make_shared<ConfigurationSet>(scs, sconfsn.str());

	// find positions of quantities to aggregate
	std::vector<unsigned int> idx;
	for (ListOfQuantityNames::iterator i = this->aggregate_quants.begin(); i != this->aggregate_quants.end(); i++) {
		std::string& qn = *i;
		idx.push_back(cs->confspace->indexOfQuantity(qn));
	}

	SetOfConfigurations::iterator i;
	for (i = cs->confs.begin(); i != cs->confs.end(); i++) {
		ConfigurationPtr cf = *i;
		ConfigurationPtr scf = std::make_shared<Configuration>(*scs);
		scf->addQuantitiesOf(cf);

		double sum = 0.0;

		std::vector<unsigned int>::iterator j;
		for (j = idx.begin(); j != idx.end(); j++) {
			unsigned int k = *j;
			QuantityValue_RealPtr qvr = std::dynamic_pointer_cast<QuantityValue_Real>(cf->getQuantity(k));
			sum = sum + qvr->value;

		}

		QuantityValue_RealPtr sqv = std::make_shared<QuantityValue_Real>(*sqt, sum);
		scf->addQuantity(sqv);
		sconfs->addUniqueConfiguration(scf);
	}
	c.push(sconfs);
}



POperation_Sum::POperation_Sum(const std::string& aqname, const std::string& bqname) : POperation_Derived(aqname, bqname) {
}

double POperation_Sum::derive(double a, double b) {
	return a + b;
}

std::string POperation_Sum::description() {
	return "Sum";
}

POperation_Max::POperation_Max(const std::string& aqname, const std::string& bqname) : POperation_Derived(aqname, bqname) {
}

double POperation_Max::derive(double a, double b) {
	if (a > b) { return a; }
	else { return b; }
}

std::string POperation_Max::description() {
	return "Max";
}

POperation_Min::POperation_Min(const std::string& aqname, const std::string& bqname) : POperation_Derived(aqname, bqname) {
}

double POperation_Min::derive(double a, double b) {
	if (a > b) { return b; }
	else { return a; }
}

std::string POperation_Min::description() {
	return "Min";
}

POperation_Multiply::POperation_Multiply(const std::string& aqname, const std::string& bqname) : POperation_Derived(aqname, bqname) {
}

double POperation_Multiply::derive(double a, double b) {
	return a * b;
}

std::string POperation_Multiply::description() {
	return "Multiply";
}


POperation_Abstract::POperation_Abstract(ListOfQuantityNames& qnames):
	lqn(qnames)
{
}

void POperation_Abstract::executeOn(ParetoCalculator& c) {
	ConfigurationSetPtr cs = c.popConfigurationSet();
	// TODO: Surely, this can be done a lot more efficiently...
	c.push(cs);
	ListOfQuantityNames::iterator i;
	for (i = this->lqn.begin(); i != this->lqn.end(); i++) {
		c.push(std::make_shared<StorableString>(*i));
		c.abstract();
	}
}


POperation_Hide::POperation_Hide(ListOfQuantityNames& qnames): lqn(qnames) {
}

void POperation_Hide::executeOn(ParetoCalculator& c) {
	ConfigurationSetPtr cs = c.popConfigurationSet();
	c.push(cs);
	c.hide(lqn);
}



POperation_Join::POperation_Join(JoinMap& jqnamemap) :
	j_quants(jqnamemap)
{
}

bool POperation_Join::testConstraint(const Pareto::Configuration& c) {
	bool diff = false;
	std::vector<int>::iterator i = qan.begin();
	std::vector<int>::iterator j = qbn.begin();
	for (; !diff && i != qan.end(); i++, j++) {
		unsigned int k = *i;
		unsigned int m = *j;
#ifdef _DEBUG
		QuantityValuePtr qva = c.getQuantity(k);
		QuantityValuePtr qvb = c.getQuantity(m);
#else
		QuantityValuePtr qva = c.getQuantity(k);
		QuantityValuePtr qvb = c.getQuantity(m);
#endif
		diff = diff || !(*qva == *qvb);
	}
	return !diff;;
}

void POperation_Join::executeOn(ParetoCalculator& c) {
	if (c.stack.size() < 2) {
		throw EParetoCalculatorError("Not enough configuration spaces on stack to perform join.");
	}
	ConfigurationSetPtr csa = c.popConfigurationSet();
	ConfigurationSetPtr csb = c.popConfigurationSet();

	qan.clear();
	qbn.clear();

	JoinMap::iterator i;

	for (i = j_quants.begin(); i != j_quants.end(); i++) {
		qan.push_back(csa->confspace->indexOfQuantity(i->first));
		qbn.push_back(csb->confspace->indexOfQuantity(i->second) + (int)csa->confspace->quantities.size());
	}

	c.push(csb);
	c.push(csa);
	c.product();

	ConfigurationSetPtr cs = c.popConfigurationSet();
	ConfigurationSetPtr ncs = c.constraint(cs, &POperation_Join::testConstraint);
	c.push(ncs);
};


POperation_EfficientJoin::POperation_EfficientJoin(JoinMap& jqnamemap): j_quants(jqnamemap) {
}

ConfigurationSetPtr recursiveEfficientJoin(ParetoCalculator& c, JoinMap& jqnamemap, ConfigurationSetPtr A, ConfigurationSetPtr B, ConfigurationSpacePtr productspace) {
	if (jqnamemap.size() == 0) {
		// return the product of A and B
		return ParetoCalculator::productInSpace(A, B, productspace);
	}
	else if (A->confs.size() == 0 || B->confs.size() == 0) {
		ConfigurationSetPtr result = std::make_shared<ConfigurationSet>(productspace, "result");
		return result;
	}
	else {
		QuantityName qa = jqnamemap.begin()->first;
		QuantityName qb = jqnamemap.begin()->second;
		std::shared_ptr<IndexOnConfigurationSet> ia, ib;
		if (A->confspace->getQuantity(qa).isTotallyOrdered()) {
			ia = std::make_shared<IndexOnTotalOrderConfigurationSet>(qa, A);
		}
		else {
			ia = std::make_shared<IndexOnUnorderedConfigurationSet>(qa, A);
		}
		if (B->confspace->getQuantity(qb).isTotallyOrdered()) {
			ib = std::make_shared<IndexOnTotalOrderConfigurationSet>(qb, B);
		}
		else {
			ib = std::make_shared<IndexOnUnorderedConfigurationSet>(qb, B);
		}

		JoinMap newnamemap;
		JoinMap::iterator i = jqnamemap.begin();
		i++;
		for (; i != jqnamemap.end(); i++) {
			newnamemap[i->first] = i->second;
		}

		ConfigurationSetPtr result = std::make_shared<ConfigurationSet>(productspace, "result");

		unsigned int j = 0;
		int l_x, u_x, v_x, w_x;
		while (j < ia->confset->confs.size()) {
			std::shared_ptr<ConfigurationIndexReference> x = ia->get(j);
			l_x = ia->lower(*x);
			u_x = ia->upper(*x);
			v_x = ib->lower(*x);
			w_x = ib->upper(*x);

			ConfigurationSetPtr Asub = ia->copyFromTo(l_x, u_x);
			ConfigurationSetPtr Bsub = ib->copyFromTo(v_x, w_x);

			ConfigurationSetPtr temp = recursiveEfficientJoin(c, newnamemap, Asub, Bsub, productspace);
			result->addUniqueConfigurationsOf(temp);
			//delete &temp;

			j = u_x + 1;
		}
		return result;
	}
}

void POperation_EfficientJoin::executeOn(ParetoCalculator& c) {
	ConfigurationSetPtr csa = c.popConfigurationSet();
	ConfigurationSetPtr csb = c.popConfigurationSet();

	ConfigurationSpacePtr productspace = csa->confspace->productWith(csb->confspace);
	ConfigurationSetPtr result = recursiveEfficientJoin(c, this->j_quants, csa, csb, productspace);
	c.push(result);
};



POperation_EfficientProdCons::POperation_EfficientProdCons(const std::string& pqname, const std::string& cqname) : p_quant(pqname), c_quant(cqname) {
}

bool POperation_EfficientProdCons::default_match(const QuantityValue& vp, const QuantityValue& vc) const {
#ifdef _DEBUG
	const QuantityValue_Real rp = dynamic_cast<const QuantityValue_Real&>(vp);
	const QuantityValue_Real rc = dynamic_cast<const QuantityValue_Real&>(vc);
#else
	const QuantityValue_Real rp = (const QuantityValue_Real&)(vp);
	const QuantityValue_Real rc = (const QuantityValue_Real&)(vc);
#endif
	return rc.value <= (1.0 / rp.value);
}

void POperation_EfficientProdCons::executeOn(ParetoCalculator& c) {
	const ConfigurationSetPtr csc = c.popConfigurationSet();
	const ConfigurationSetPtr csp = c.popConfigurationSet();

	if (!(csp->confspace->getQuantity(p_quant).isTotallyOrdered() && csc->confspace->getQuantity(c_quant).isTotallyOrdered())) {
		throw EParetoCalculatorError("Dimensions must be totally ordered for efficient producer-consumer");
	}

	// To do: I'm computing products of spaces everywhere, make one method in ConfSpace class.
	ConfigurationSpacePtr nspace = csp->confspace->productWith(csc->confspace);	
	std::string name = "Producer-Consumer ( " + csp->name + ", " + csc->name + ", " + p_quant + ", " + c_quant + ")";
	ConfigurationSetPtr ns = std::make_shared<ConfigurationSet>(nspace, name);

	IndexOnTotalOrderConfigurationSet ip(p_quant, csp);
	IndexOnTotalOrderConfigurationSet ic(c_quant, csc);


	unsigned int i = 0;
	int j = ip.size() - 1;
	while (i < ic.size() && j >= 0) {
		if (default_match(*(ip.at(j)->value()), *(ic.at(i)->value()))) {
			ConfigurationPtr c = std::make_shared<Configuration>(*nspace);
			c->addQuantitiesOf(ic.at(i)->conf);
			c->addQuantitiesOf(ip.at(j)->conf);
			ns->addConfiguration(c);
			i = i + 1;
		}
		else {
			j = j - 1;
			if (j >= 0) {
				for (unsigned int k = 0; k < i; k++) {
					ConfigurationPtr c = std::make_shared<Configuration>(*nspace);
					c->addQuantitiesOf(ic.at(k)->conf);
					c->addQuantitiesOf(ip.at(j)->conf);
					ns->addConfiguration(c);
				}
			}
		}
	}
	if (i == ic.size()) {
		for (int l = 0; l < j; l++) {
			for (unsigned int k = 0; k < ic.size(); k++) {
				ConfigurationPtr c = std::make_shared<Configuration>(*nspace);
				c->addQuantitiesOf(ic.at(k)->conf);
				c->addQuantitiesOf(ip.at(l)->conf);
				ns->addConfiguration(c);
			}
		}
	}
	c.push(ns);
}
