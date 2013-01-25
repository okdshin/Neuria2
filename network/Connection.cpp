#ifdef CONNECTION_UNIT_TEST
#include "Connection.h"
#include <iostream>
#include <boost/thread.hpp>

using namespace neuria::network;

void connect_handle(const boost::system::error_code& error)
{
	if (error)
		std::cout << "error occurred!:" << error << std::endl;
	else
		std::cout << "connect success" << std::endl;
}

int main(int argc, char* argv[])
{
	auto io_service = IoService::Create();
	boost::asio::io_service::work w(io_service->GetRawIoServiceRef());
	
	auto socket = Socket::Create(io_service);
	socket->GetRawSocketRef().async_connect(
		boost::asio::ip::tcp::endpoint(
			boost::asio::ip::address::from_string("127.0.0.1"), 54322), 
			connect_handle);

	auto connection = Connection::Create(socket, BufferSize(256),
		OnReceivedFunc(),
		Connection::OnPeerClosedFunc(),
		OnFailedFunc()
	);
	connection->StartReceive();
	boost::thread t([io_service](){
		io_service->GetRawIoServiceRef().run();
	});

	std::string message_str;
	while(true){
		std::cin >> message_str;
		connection->Send(CreateByteArrayFromString(message_str), 
			OnSendedFunc(), OnFailedFunc());
	}
	t.join();

    return 0;
}

#endif
