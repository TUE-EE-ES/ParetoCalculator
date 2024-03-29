
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

#pragma once

#ifndef TESTS_UNITTESTER_H
#define TESTS_UNITTESTER_H

#include <string>
#include <random>
#include "calculator.h"	


namespace Pareto {

	class UnitTesterException {
	public:
		// Constructor
		UnitTesterException(const std::string s);

		/// the error message of the exception
		std::string errorMsg;
	};

	class UnitTester
	{
	public:
		UnitTester();

		bool test_all(void);

		bool test_calculator(void);

		bool test_PCStore(void);
		bool test_DCMinimization(void);

	private:
		// for reproducable pseudo random input
		std::default_random_engine generator;

		// calculator to use
		ParetoCalculator PC;

	};

}


#endif
