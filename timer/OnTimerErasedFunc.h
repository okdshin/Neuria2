#pragma once
//OnTimerErasedFunc:20130124
#include <iostream>
#include <boost/function.hpp>
#include "Timer.h"

namespace neuria{
namespace timer
{
class OnTimerErasedFunc{
public:
    OnTimerErasedFunc(boost::function<void ()> on_timer_erased) 
		: on_timer_erased(on_timer_erased){}

    OnTimerErasedFunc() 
		: on_timer_erased([](){
			std::cout << "Called(Default)OnTimerErasedFunc" << std::endl;		
			
		}
	){}

	auto operator()()const -> void {
		this->on_timer_erased();
	}

private:
	boost::function<void ()> on_timer_erased;

};
}
}
