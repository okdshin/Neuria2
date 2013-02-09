#ifdef BYTEARRAYLISTWRAPPER_UNIT_TEST
#include "ByteArrayListWrapper.h"
#include <iostream>

using namespace neuria::command;

int main(int argc, char* argv[])
{
	auto wrapper = ByteArrayListWrapper();
	wrapper.PushBack(CreateByteArrayFromString("hello"));
	wrapper.PushBack(CreateByteArrayFromString("good"));
	wrapper.PushBack(CreateByteArrayFromString("hello"));

	std::cout << wrapper << std::endl;
	std::cout << CreateStringFromByteArray(wrapper.Serialize()) << std::endl;
	auto serialized_byte_array = wrapper.Serialize();
	//serialized_byte_array.erase(serialized_byte_array.begin(), serialized_byte_array.begin()+2);
	std::cout << "parsed:\n" << ByteArrayListWrapper::Parse(serialized_byte_array) << std::endl;

    return 0;
}

#endif
