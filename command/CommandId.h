#pragma once
//CommandId:20130110
#include <iostream>
#include <string>

namespace neuria{
namespace command
{
class CommandId{
public:
    CommandId(const std::string& command_id_str) : command_id_str(command_id_str){}
    ~CommandId(){}

	auto ToString()const -> std::string {
		return this->command_id_str;
	}

private:
	std::string command_id_str;
};

auto operator<<(std::ostream& os, const CommandId& host_name) -> std::ostream& {
	os << host_name.ToString();
	return os;
}

auto operator==(const CommandId& left, const CommandId& right)->bool{
	return left.ToString() == right.ToString();
}

//for std::map
auto operator<(const CommandId& left, const CommandId& right)->bool{
	return left.ToString() < right.ToString();
}
}
}
