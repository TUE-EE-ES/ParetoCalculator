
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
// Unit Testing Application
//

//#include <calculator.h>

#include "unittester.h"
#include <stdexcept>
#include <iostream>

using namespace Pareto;

int main(void)
{
	UnitTester UT;
	try {
		try {
			if (!(UT.test_all())) {
				return -1;
			}
			return 0;
		} catch (EParetoCalculatorError& e) {
			std::cout << e.errorMsg;
			throw std::runtime_error("An error occurred. Closing.");
		}
	}
	catch (std::runtime_error & e) {
		std::cout << e.what();

		return -1;
	}
}
