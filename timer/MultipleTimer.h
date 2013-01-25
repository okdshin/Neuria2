#pragma once
//MultipleTimer:20121015
#include <iostream>
#include <boost/shared_ptr.hpp>
#include "Timer.h"
#include "OnTimerErasedFunc.h"

namespace neuria{
namespace timer{

class MultipleTimer{
public:
	using Ptr = boost::shared_ptr<MultipleTimer>;

    static auto Create(boost::asio::io_service& service) -> Ptr {
		return Ptr(new MultipleTimer(service));	
	}

	auto AddCallbackFuncAndStartTimer(unsigned int sec_interval, 
			TimerCallbackFunc timer_callback, 
			OnTimerErasedFunc on_timer_erased) -> void {
		auto timer = Timer::Create(this->service, sec_interval, 
			timer_callback, 
			Timer::OnErasedFunc([on_timer_erased, this](const Timer::Ptr& timer){
				on_timer_erased(); 
				this->timer_list.erase(std::find(
					this->timer_list.begin(), this->timer_list.end(), timer));
				std::cout << "timer was erased." << std::endl;
			})
		);
		timer_list.push_back(timer);
		timer->Start();
	}

private:
    MultipleTimer(boost::asio::io_service& service) : service(service){}
	
	std::vector<Timer::Ptr> timer_list;

	boost::asio::io_service& service;

};

}
}

