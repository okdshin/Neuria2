#ifdef SERVER_UNIT_TEST
#include "Server.h"
#include <iostream>
#include <boost/thread.hpp>
#include "OnReceivedFunc.h"
#include "Connection.h"
using namespace neuria::network;

int main(int argc, char* argv[])
{
	boost::asio::io_service io_service;
	boost::asio::io_service::work w(io_service);
	Server server(io_service, PortNumber(54322), 
		OnAcceptedFunc([](Socket::Ptr socket){
			std::cout << "OnAccepted!:" << socket << std::endl;
			auto connection = 
				Connection::Create(
					socket, 
					BufferSize(512)
				);
			connection->StartReceive(
				OnReceivedFunc([](const ByteArray& byte_array){
					//std::cout << CreateStringFromByteArray(byte_array) << std::endl;	
				}), 
				Connection::OnPeerClosedFunc(), 
				OnFailedFunc());
		}),
		OnFailedFunc([](const ErrorCode&){
			std::cout << "OnFailed!" << std::endl;
			
		})
	);
	server.StartAccept();
	boost::thread_group thread_group;
	for(unsigned int i = 0; i < 1; ++i){
		thread_group.create_thread(
			[&io_service](){
				io_service.run();	
			}
		);
	}
	thread_group.join_all();

    return 0;
}

#endif
