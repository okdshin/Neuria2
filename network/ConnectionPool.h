#pragma once
//ConnectionPool:20130127
#include <iostream>
#include <vector>
#include <boost/scoped_ptr.hpp>
#include "Connection.h"

namespace neuria{
namespace network
{
//thread safe
class ConnectionPool{
public:
    ConnectionPool(boost::asio::io_service& io_service) 
		: strand(new boost::asio::io_service::strand(io_service)){}

	auto Add(const Connection::Ptr& connection) -> void {
		this->strand->post([this, connection](){
			this->connection_pool.push_back(connection);
		});
	}

	auto Remove(const Connection::ConstPtr& connection) -> void {		
		this->strand->post([this, connection](){
			const auto iter = 
				find(this->connection_pool.begin(), this->connection_pool.end(), 
					connection);
			if(iter != this->connection_pool.end()){
				this->connection_pool.erase(iter);
				std::cout << boost::format("Connection %1% removed from pool") 
					% connection.get()
				<< std::endl;
			}
			else {
				std::cout << boost::format("Connection %1% has ALREADY or YET not in pool") 
					% connection.get()
				<< std::endl;
				
			}
		});
	}

	auto PickUpAndApply(
			const HostName& host_name, const PortNumber& port_number,
			boost::function<void (const Connection::Ptr&)> func)const -> void {
		this->ForEach([func, host_name, port_number](const Connection::Ptr& connection){
			if(connection->GetRemoteHostName() == host_name 
					&& connection->GetRemotePortNumber() == port_number){
				func(connection);
			}
		});
	}

	auto ForEach(boost::function<void (const Connection::Ptr&)> func)const -> void {
		this->strand->post([this, func]() -> void {
			for(const auto& connection : this->connection_pool){
				func(connection);
			}
		});
	}
	
	friend auto operator<<(std::ostream& os, 
			const ConnectionPool& connection_pool) -> std::ostream&;
private:
	boost::scoped_ptr<boost::asio::io_service::strand> strand;
	std::vector<Connection::Ptr> connection_pool;

};

auto operator<<(std::ostream& os, 
		const ConnectionPool& connection_pool) -> std::ostream& {
	connection_pool.strand->post([&connection_pool, &os]() -> void {
		os << connection_pool.connection_pool.size() << "{";
		for(const auto& connection : connection_pool.connection_pool){
			os << boost::format("%3%:%1%:%2%, ") 
				% connection->GetRemoteHostName().ToString()
				% connection->GetRemotePortNumber().ToInt()
				% connection.get();
		}
		os << "}"<<std::flush;
	});
	return os;
}
/*
auto operator<<(std::ostream& os, 
		const ConnectionPool& connection_pool) -> std::ostream& {
	connection_pool.ForEach([&os](const Connection::ConstPtr& connection){
		os << boost::format("%3%%1%:%2%, ") 
			% connection->GetRemoteHostName().ToString()
			% connection->GetRemotePortNumber().ToInt()
			% connection.get();
	});
	return os;
}
*/
}
}
