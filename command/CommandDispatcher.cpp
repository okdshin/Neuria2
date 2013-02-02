#ifdef COMMANDDISPATCHER_UNIT_TEST
#include "CommandDispatcher.h"
#include <iostream>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
using namespace neuria::command;

int main(int argc, char* argv[])
{
	boost::asio::io_service io_service;
	boost::asio::io_service::work w(io_service);
	auto dispatcher = CommandDispatcher(AsyncExecuter(
		[&io_service](boost::function<void ()> func){
			io_service.post(func);
		}), 
		OnFailedFunc()
	);
	dispatcher.RegisterFunc(
		CommandId("test_command_id"), 
		OnReceivedFunc([](const ByteArraySender&, const ByteArray& byte_array){
			std::cout << "1111" 
				<< CreateStringFromByteArray(byte_array) << std::endl;
			for(unsigned int i = 0; i < 10; ++i){
				std::cout << "1" << std::endl;
				sleep(1);
			}
		})
	);
	dispatcher.RegisterFunc(
		CommandId("test_command_id2"), 
		OnReceivedFunc([](const ByteArraySender&, const ByteArray& byte_array){
			std::cout << "2222" 
				<< CreateStringFromByteArray(byte_array) << std::endl;
			for(unsigned int i = 0; i < 10; ++i){
				std::cout << "2" << std::endl;
				sleep(1);
			}
		})
	);
	std::cout << dispatcher << std::endl;
	dispatcher.Dispatch(ByteArraySender(),
		DispatchCommandWrapper(CommandId("test_command_id"), CreateByteArrayFromString("hello")).Serialize());
	dispatcher.Dispatch(ByteArraySender(),
		DispatchCommandWrapper(CommandId("test_command_id2"), CreateByteArrayFromString("hello")).Serialize());
	boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));
	boost::thread t2(boost::bind(&boost::asio::io_service::run, &io_service));
	t.join();
	t2.join();

    return 0;
}

#endif
