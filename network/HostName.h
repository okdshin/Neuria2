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
	os << host_name.ToString();
	return os;
}
}
/*
auto ExtractHostNameAndPort(
		const NodeId& src_node_id, 
		HostName& dst_host_name,
		PortNumber& dst_port_num) -> void {
	std::vector<std::string> splitted;
	auto str = src_node_id.ToString();
	boost::algorithm::split(splitted, str, boost::is_any_of(":"));
	dst_host_name = HostName(splitted.at(0));
	dst_port_num = PortNumber(boost::lexical_cast<int>(splitted.at(1)));
}
*/
}

