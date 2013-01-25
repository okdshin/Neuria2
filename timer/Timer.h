#pragma once
//Timer:20121013
#include <iostream>
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "Time.h"
#include "IsContinue.h"
#include "TimerCallbackFunc.h"

namespace neuria{
namespace timer{

class Timer : public boost::enable_shared_from_this<Timer> {
public:
	using Ptr = boost::shared_ptr<Timer>;

	class OnErasedFunc{
	public:
		OnErasedFunc(boost::function<void (const Timer::Ptr&)> on_timer_erased) 
			: on_timer_erased(on_timer_erased){}

		OnErasedFunc() 
			: on_timer_erased([](const Timer::Ptr& timer){
				std::cout << "Called(Default)OnErasedFunc" << std::endl;		
				
			}
		){}

		auto operator()(const Timer::Ptr& timer) -> void {
			this->on_timer_erased(timer);
		}
	private:
		boost::function<void (const Timer::Ptr&)> on_timer_erased;

	};

	static auto Create(boost::asio::io_service& service, 
			unsigned int interval_sec, 
			TimerCallbackFunc callback_func, 
			OnErasedFunc on_timer_erased) -> Ptr {
		return Ptr(new Timer(service, boost::posix_time::seconds(interval_sec), 
			callback_func, on_timer_erased));
	}

	auto Start() -> void {
		this->Call();
	}

private:
	Timer(boost::asio::io_service& service, 
		const boost::posix_time::time_duration& interval, 
		TimerCallbackFunc callback_func, OnErasedFunc on_timer_erased) 
		: service(service), timer(service, interval), interval(interval), 
		callback_func(callback_func), on_timer_erased(on_timer_erased){}

	auto Call() -> void {
		this->timer.expires_at(
			this->timer.expires_at() + this->interval);
		this->timer.async_wait([this](const boost::system::error_code&){
			const auto is_continue = this->callback_func();
			if(!is_continue()){
				this->on_timer_erased(this->shared_from_this());
				return;
			}
			this->Call();	
		});	
	}

	boost::asio::io_service& service;
	boost::asio::deadline_timer timer;
	boost::posix_time::time_duration interval;
	TimerCallbackFunc callback_func;
	OnErasedFunc on_timer_erased;
};


}
}

