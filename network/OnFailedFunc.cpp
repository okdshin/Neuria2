#ifdef ONFAILEDFUNC_UNIT_TEST
#include "OnFailedFunc.h"
#include <iostream>

using namespace neuria;
using namespace neuria::network;

int main(int argc, char* argv[])
{
	auto f = OnFailedFunc();
	f(ErrorCode("test"));

	auto f2 = OnFailedFunc([](const ErrorCode& error_code){
		std::cout << error_code << std::endl;	
	});
	f2(ErrorCode("test"));
    return 0;
}

#endif
