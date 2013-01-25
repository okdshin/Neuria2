#pragma once
//Socket:20130112
#include <iostream>
#include <boost/asio.hpp>
#include "IoService.h"
namespace neuria{
namespace network
{
class Socket{
public:
	using Ptr = boost::shared_ptr<Socket>;
	static auto Create(IoService::Ptr io_service) -> Ptr {
		std::cout << "SocketCreated!" << std::endl;	
		return Ptr(new Socket(io_service));
	}
	auto GetRawSocketRef() -> boost::asio::ip::tcp::socket& {
		return this->raw_socket;
	}

	~Socket(){
		std::cout << "SocketDeleted!" << std::endl;	
	}
private:
    Socket(IoService::Ptr io_service) 
		: raw_socket(io_service->GetRawIoServiceRef()){}

private:
	boost::asio::ip::tcp::socket raw_socket;

};
}
}
