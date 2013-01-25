#pragma once
//BufferSize:20130110
#include <iostream>

namespace neuria{
namespace network
{
class BufferSize{
public:
    BufferSize(int int_buffer_size) : int_buffer_size(int_buffer_size){}
    ~BufferSize(){}

	auto ToInt()const -> int {
		return this->int_buffer_size;	
	}
private:
	int int_buffer_size;

};
}
}
