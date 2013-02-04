#ifdef SERVER_UNIT_TEST
#include "Server.h"
#include <iostream>
#include <boost/thread.hpp>
#include "OnReceivedFunc.h"
#include "Connection.h"
using namespace neuria::network;

int main(int argc, char* argv[])
{
	auto io_service = IoService::Create();
	boost::asio::io_service::work w(io_service->GetRawIoServiceRef());
	Server server(io_service, PortNumber(54322), 
		OnAcceptedFunc([](Socket::Ptr socket){
			std::cout << "OnAccepted!:" << socket << std::endl;
			auto connection = 
				Connection::Create(
					socket, 
					BufferSize(5)
				);
			//connection->StartReceive();
		}),
		OnFailedFunc([](const ErrorCode&){
			std::cout << "OnFailed!" << std::endl;
			
		})
	);
	server.StartAccept();
	boost::thread t([io_service](){
		io_service->GetRawIoServiceRef().run();	
	});
	t.join();

    return 0;
}

#endif
