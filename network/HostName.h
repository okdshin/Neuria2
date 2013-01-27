#pragma once
//HostName:20130110
#include <iostream>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include "NodeId.h"
#include "PortNumber.h"

namespace neuria{
namespace network
{
class HostName{
public:
    HostName(const std::string& host_name_str) : host_name_str(host_name_str){}
	HostName(const NodeId& node_id){
		std::vector<std::string> splitted;
		auto str = node_id.ToString();
		boost::algorithm::split(splitted, str, boost::is_any_of(":"));
		this->host_name_str = splitted.at(0);
	}
    ~HostName(){}

	auto ToString()const -> std::string {
		return this->host_name_str;
	}

private:
	std::string host_name_str;
};
auto operator<<(std::ostream& os, const HostName& host_name) -> std::ostream& {
	os << "HostName:" << host_name.ToString();
	return os;
}
}
}

