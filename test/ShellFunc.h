#pragma once
//ShellFunc:20130112
#include <iostream>
#include <vector>
#include <string>
#include <boost/function.hpp>

namespace neuria{
namespace test
{
class ShellFunc{
public:
	ShellFunc():shell_func([](const std::vector<std::string>& arg_list){
			std::cout << "Called(Default)ShellFunc" << std::endl;
		}){}
    ShellFunc(boost::function<void (const std::vector<std::string>& arg_list)> shell_func) : shell_func(shell_func){}

	auto operator()(const std::vector<std::string>& arg_list) -> void {
		this->shell_func(arg_list);	
	}

private:
	boost::function<void (const std::vector<std::string>& arg_list)> shell_func;

};
}
}
