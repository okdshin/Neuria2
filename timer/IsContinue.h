#pragma once
//IsContinue:20130124
#include <iostream>

namespace neuria{
namespace timer{
class IsContinue{
public:
    IsContinue(bool is_continue):is_continue(is_continue){}

	auto operator()()const -> bool {
		return this->is_continue;	
	}
private:
	bool is_continue;

};
}
}
