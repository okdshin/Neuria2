#pragma once
//NodeId:20130110
#include <iostream>
#include <string>

namespace neuria{
namespace network
{
class NodeId{
public:
    NodeId(const std::string& node_id_str) : node_id_str(node_id_str){}
    ~NodeId(){}

	auto ToString()const -> std::string {
		return this->node_id_str;
	}

private:
	std::string node_id_str;
};
}
}

