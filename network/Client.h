#pragma once
//Client:20130117
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "OnConnectedFunc.h"
#include "OnFailedFunc.h"
#include "HostName.h"

namespace neuria{
namespace network
{
class Client{
public:
    Client(IoService::Ptr io_service) : io_service(io_service){}

	auto Connect(
			const HostName& host_name, 
			const PortNumber& port_num,
			OnConnectedFunc on_connected,
			OnFailedFunc on_failed) -> void {
			
		boost::asio::ip::tcp::resolver resolver(this->io_service->GetRawIoServiceRef());
		auto query = boost::asio::ip::tcp::resolver::query(
			host_name.ToString(), port_num.ToString());
		auto endpoint_iter = resolver.resolve(query);
		auto new_socket = Socket::Create(this->io_service);
		boost::asio::async_connect(
			new_socket->GetRawSocketRef(), endpoint_iter, boost::bind(
				&Client::OnConnected, this, 
				boost::asio::placeholders::error,
				new_socket, 
				on_connected, 
				on_failed
			)
		);	
	}
    ~Client(){}

private:
	auto OnConnected(
			const boost::system::error_code& error_code,
			Socket::Ptr socket,
			OnConnectedFunc on_connected, 
			OnFailedFunc on_failed) -> void {
		if(!error_code){
			on_connected(socket);
		}
		else{
			on_failed(ErrorCode(error_code));
		}
			
	}

private:
	IoService::Ptr io_service;

};
}
}
