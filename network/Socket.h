#pragma once
//Socket:20130112
#include <iostream>
#include <boost/asio.hpp>
#include "IoService.h"
#include "HostName.h"
#include "PortNumber.h"
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

	auto GetRemoteHostName()const -> HostName {
		return HostName(this->raw_socket.remote_endpoint().address().to_string());	
	}

	auto GetRemotePortNumber()const -> PortNumber {
		return PortNumber(this->raw_socket.remote_endpoint().port());	
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
