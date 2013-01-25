#pragma once
//TimerCallbackFunc:20130124
#include <iostream>
#include <boost/function.hpp>
#include "IsContinue.h"

namespace neuria{
namespace timer
{
class TimerCallbackFunc{
public:
    TimerCallbackFunc(boost::function<IsContinue ()> timer_callback) 
		: timer_callback(timer_callback){}

	auto operator()() -> IsContinue {
		return this->timer_callback();	
	}
private:
	boost::function<IsContinue ()> timer_callback;
};
}
}
