#ifdef CLIENT_UNIT_TEST
#include "Client.h"
#include <iostream>
#include <string>
#include <boost/thread.hpp>
#include "Connection.h"
#include "OnFailedFunc.h"

using namespace neuria;
using namespace neuria::network;

int main(int argc, char* argv[])
{
	boost::asio::io_service io_service;
	boost::asio::io_service::work w(io_service);
	Client client(io_service);
	Connection::Ptr connection;	
	client.Connect(HostName("localhost"), PortNumber(54322),
		OnConnectedFunc([connection](Socket::Ptr socket){
			connection = Connection::Create(
				socket, 
				BufferSize(256),
				OnReceivedFunc(), 
				Connection::OnPeerClosedFunc(), 
				OnFailedFunc()
			);
			connection->StartReceive();
		}),
		OnFailedFunc()
	);
	boost::thread t([&io_service](){
		io_service->GetRawIoServiceRef().run();	
	});
	while(true){
		std::string text;
		std::cin >> text;
		connection->Send(
			CreateByteArrayFromString(text), 
			OnSendedFunc(), 
			OnFailedFunc()
		);
	}
	t.join();

    return 0;
}

#endif
