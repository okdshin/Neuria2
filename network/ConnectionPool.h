#pragma once
//ConnectionPool:20130127
#include <iostream>
#include <vector>
#include "Connection.h"

namespace neuria{
namespace network
{
class ConnectionPool{
public:
    ConnectionPool(){}
    ~ConnectionPool(){}

	auto Add(const Connection::Ptr& connection) -> void {
		this->connection_pool.push_back(connection);
	}

	auto Remove(const Connection::Ptr& connection) -> void {		
		const auto iter = 
			find(this->connection_pool.begin(), this->connection_pool.end(), 
				connection);
		assert(iter != this->connection_pool.end());
		this->connection_pool.erase(iter);
		std::cout << boost::format("Connection %1% removed from pool") 
			% connection.get()
		<< std::endl;
	}

	auto At(unsigned int i)const -> Connection::Ptr {
		return this->connection_pool.at(i);	
	}

	auto Size()const -> unsigned int {
		return this->connection_pool.size();	
	}

	auto ForEach(boost::function<void (const Connection::Ptr&)> func)const -> void {
		for(const auto connection : this->connection_pool){
			func(connection);
		}
	}

private:
	std::vector<Connection::Ptr> connection_pool;

};

auto operator<<(std::ostream& os, 
		ConnectionPool connection_pool) -> std::ostream& {
	os << "ConnectionPool:" << connection_pool.Size() << "{";
	connection_pool.ForEach([&os](const Connection::Ptr& connection){
		os << boost::format("%3%%1%:%2%, ") 
			% connection->GetRemoteHostName().ToString()
			% connection->GetRemotePortNumber().ToInt()
			% connection.get();
	});
	os << "}";
	return os;
}
}
}
