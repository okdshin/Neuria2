#ifdef DISPATCHCOMMANDWRAPPER_UNIT_TEST
#include "DispatchCommandWrapper.h"
#include <iostream>

using namespace neuria::command;

int main(int argc, char* argv[])
{
	auto command = DispatchCommandWrapper(CommandId("command_id"), 
		CreateByteArrayFromString("byte array"));
	std::cout << command << std::endl;
	std::cout << CreateStringFromByteArray(command.Serialize()) << std::endl;
	auto serialized_byte_array = command.Serialize();
	//serialized_byte_array.erase(serialized_byte_array.begin(), serialized_byte_array.begin()+2);
	std::cout << "parsed:\n" << DispatchCommandWrapper::Parse(serialized_byte_array) << std::endl;

    return 0;
}

#endif
