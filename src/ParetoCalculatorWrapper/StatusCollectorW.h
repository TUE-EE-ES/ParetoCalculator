#pragma once

#include "calculator.h"
#include <deque>

using namespace System;
using namespace System::Collections;


using namespace Pareto;


namespace Pareto {


	public class StatusCollector : public StatusCallback {
	public:
		StatusCollector();

	public:
		std::deque<std::string> stats;
		std::deque<std::string> verbs;
		volatile bool lock = false;
	public:

		virtual void setStatus(const std::string& s);

		virtual void verbose(const std::string& s);

		bool get_lock(void);
		bool release_lock(void);
	};

}