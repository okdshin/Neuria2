#pragma once
//ConnectionPool:20130127
#include <iostream>
#include <vector>
#include <boost/scoped_ptr.hpp>
#include <boost/random.hpp>
#include <cassert>
#include "Connection.h"
#include "OnConnectionAdded.h"
#include "OnConnectionRemoved.h"

namespace neuria{
namespace network
{

using ConnectionList = std::vector<Connection::Ptr>;
//thread safe
class ConnectionPool{
public:
	using Ptr = boost::shared_ptr<ConnectionPool>;
	static auto Create(boost::asio::io_service& io_service,
			const OnConnectionAdded& on_added,
			const OnConnectionRemoved& on_removed) -> Ptr {
		return Ptr(new ConnectionPool(io_service, on_added, on_removed));	
	}

	auto Add(const Connection::Ptr& connection) -> void {
		this->strand->post([this, connection](){
			this->on_added(connection);
			this->connection_pool.push_back(connection);
		});
	}

	auto Remove(const Connection::ConstPtr& connection) -> void {		
		this->strand->post([this, connection](){
			const auto iter = 
				find(this->connection_pool.begin(), this->connection_pool.end(), 
					connection);
			if(iter != this->connection_pool.end()){
				this->on_removed(connection);
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

	/*
	auto PickUpAndQuote(
			const HostName& host_name, const PortNumber& port_number,
			boost::function<void (const Connection::Ptr&)> func)const -> void {
		this->ForEach([func, host_name, port_number](const Connection::Ptr& connection){
			if(connection->GetRemoteHostName() == host_name 
					&& connection->GetRemotePortNumber() == port_number){
				func(connection);
			}
		});
	}
	*/
	auto PickUpAndQuote(
			boost::function<bool (const Connection::Ptr&)> decider,
			boost::function<void (const ConnectionList&)> quoter)const -> void {
		this->strand->post([this, decider, quoter]() -> void {
			ConnectionList connection_list;
			for(const auto& connection : this->connection_pool){
				if(decider(connection)){
					connection_list.push_back(connection);	
				}
			}
			quoter(connection_list);
		});
	}

	auto ForEach(boost::function<void (const Connection::Ptr&)> func)const -> void {
		this->strand->post([this, func]() -> void {
			for(const auto& connection : this->connection_pool){
				func(connection);
			}
		});
	}
	
	auto QuoteRandomConnection(
			boost::function<void (const Connection::Ptr&)> func) -> void {
		this->strand->post([this, func](){
			if(this->connection_pool.empty()){
				std::cout
					<< "QuoteRandomConnection called"
					<< "but there is no connection in pool so immediately return..." 
				<< std::endl;
				return;
			}
			boost::random::uniform_int_distribution<> 
				index_distribution(0, this->connection_pool.size()-1);
			func(this->connection_pool.at(index_distribution(this->random_device)));
		});
	}

	friend auto operator<<(std::ostream& os, 
			const ConnectionPool& connection_pool) -> std::ostream&;
private:
    ConnectionPool(boost::asio::io_service& io_service, 
			const OnConnectionAdded& on_added,
			const OnConnectionRemoved& on_removed) 
		: strand(new boost::asio::io_service::strand(io_service)),
		on_added(on_added), on_removed(on_removed),
		random_device(static_cast<unsigned int>(time(0))){}
	
	
	boost::scoped_ptr<boost::asio::io_service::strand> strand;
	OnConnectionAdded on_added;
	OnConnectionRemoved on_removed;
	boost::random::mt19937 random_device;
	std::vector<Connection::Ptr> connection_pool;


};

inline auto operator<<(std::ostream& os, 
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
