#pragma once
//Server:20130112
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "Socket.h"
#include "PortNumber.h"


#include "BufferSize.h"
#include "OnAcceptedFunc.h"
#include "OnClosedFunc.h"
#include "OnFailedFunc.h"
namespace neuria{
namespace network
{
class Server{
public:
    Server(
		IoService::Ptr io_service,
		const PortNumber& port_num,
		OnAcceptedFunc on_accepted,
		OnFailedFunc on_failed)
			:io_service(io_service),
			acceptor(
				io_service->GetRawIoServiceRef(), 
				boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 
				port_num.ToInt())),
			on_accepted(on_accepted), 
			on_failed(on_failed){}
    ~Server(){}

	auto StartAccept() -> void {
		auto new_socket = Socket::Create(this->io_service);
		this->acceptor.async_accept(
			new_socket->GetRawSocketRef(),
			boost::bind(
				&Server::OnAccepted, this, 
				new_socket, 
				boost::asio::placeholders::error
			)
		);
		
	}

private:
	auto OnAccepted(
			Socket::Ptr socket,
			const boost::system::error_code& error_code) -> void {
		if(!error_code){
			this->on_accepted(socket);
		}
		else{
			this->on_failed(ErrorCode(error_code));
		}
		this->StartAccept();
	}
private:
	const IoService::Ptr io_service;
	boost::asio::ip::tcp::acceptor acceptor;
	
	OnAcceptedFunc on_accepted;
	OnFailedFunc on_failed;

};
}
}
