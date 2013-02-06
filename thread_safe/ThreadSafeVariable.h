#pragma once
//ThreadSafeVariable:20130205
#include <iostream>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>

namespace neuria{
namespace thread_safe
{
template<class VariableType>
class ThreadSafeVariable{
public:
    ThreadSafeVariable(
		boost::asio::io_service& io_service, const VariableType& initial_value) 
		: strand(new boost::asio::io_service::strand(io_service)),
		variable(initial_value){}

	auto Assign(const VariableType& variable) -> void {
		this->strand->post([this, variable](){
			this->variable = variable;
			std::cout << "assigned" << std::endl;
		});
	}

	auto Quote(boost::function<void (const VariableType&)> func)const -> void {
		this->strand->post([this, func](){
			func(this->variable);	
		});
	}

private:
	boost::scoped_ptr<boost::asio::io_service::strand> strand;
	VariableType variable;
};
}
}
