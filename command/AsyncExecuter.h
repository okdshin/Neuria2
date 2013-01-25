#pragma once
//AsyncExecuter:20130118
#include <iostream>
#include <boost/function.hpp>

namespace neuria{
namespace command
{
class AsyncExecuter{
public:
    AsyncExecuter(boost::function<void (boost::function<void ()>)> executer)
		:executer(executer){}

	auto operator()(boost::function<void ()> func) -> void {
		executer(func);
	}

private:
	boost::function<void (boost::function<void ()>)> executer;

};
}
}
