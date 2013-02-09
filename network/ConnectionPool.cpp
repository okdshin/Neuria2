#ifdef CONNECTIONPOOL_UNIT_TEST
#include "ConnectionPool.h"
#include <iostream>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

using namespace neuria;
using namespace neuria::network;

int main(int argc, char* argv[])
{
	boost::asio::io_service io_service;
	boost::asio::io_service::work work(io_service);

	boost::thread_group thread_group;
	for(unsigned int i = 0; i < 5; ++i){
		thread_group.create_thread(boost::bind(
			&boost::asio::io_service::run, &io_service));
	}
	
	ConnectionPool connection_pool(io_service);
	const auto socket = Socket::Create(io_service);

	for(unsigned int i = 0; i < 1000; ++i){
		const auto connection = Connection::Create(socket, BufferSize(256));
		io_service.post([&connection_pool, connection](){
			connection_pool.Add(connection);
			//connection_pool.Remove(connection);	
		});	
		/*
		io_service.post([&connection_pool](){
			connection_pool.PickUpAndApply(
				HostName("localhost"), PortNumber(54321),
				[](const Connection::Ptr& connection){
					connection->Close();
				}
			);	
		});
		*/
	}	
	connection_pool.QuoteRandomConnection([](const Connection::Ptr& connection){
		std::cout << connection << std::endl;	
	});
	connection_pool.QuoteRandomConnection([](const Connection::Ptr& connection){
		std::cout << connection << std::endl;	
	});

	//sleep();
	thread_group.join_all();

    return 0;
}

#endif
