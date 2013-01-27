#pragma once
//IsDebugMode:20130110
#include <iostream>
#include <boost/lexical_cast.hpp>

namespace neuria{
namespace network
{
class IsDebugMode{
public:
    IsDebugMode(bool is_debug_mode_bool) : is_debug_mode_bool(is_debug_mode_bool){}
    ~IsDebugMode(){}

	auto operator()()const -> bool {
		return this->is_debug_mode_bool;	
	}
	
private:
	bool is_debug_mode_bool;
};

}
}
