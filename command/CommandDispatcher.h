#pragma once
//CommandDispatcher:20120913
#include <iostream>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "DispatchCommandWrapper.h"
#include "ByteArray.h"
#include "AsyncExecuter.h"
#include "ErrorCode.h"
#include "OnFailedFunc.h"
#include "OnReceivedFunc.h"

namespace neuria{
namespace command{

class CommandDispatcher {
public:
    CommandDispatcher(AsyncExecuter async_executer, OnFailedFunc on_failed) 
		: async_executer(async_executer), on_failed(on_failed){}
	
	auto RegisterFunc(const CommandId& command_id, 
			OnReceivedFunc func) -> void{
		this->func_dict[command_id] = func;
	}
	
	auto Dispatch(const ByteArray& byte_array) -> void {
		auto command = command::DispatchCommandWrapper::Parse(byte_array);
		std::cout << "command id:" 
			<< command.GetCommandId().ToString() << std::endl;
		if(this->func_dict.find(command.GetCommandId()) == this->func_dict.end()){
			std::cout << command.GetCommandId().ToString() << std::endl;
			this->on_failed(ErrorCode("InvalidCommandId"));
		}
		else{
			this->async_executer(
				[this, command](){
					this->func_dict[command.GetCommandId()](
						command.GetWrappedByteArray()
					);
				}
			);
		}
	}
	
	friend auto operator<<(
		std::ostream& os, const CommandDispatcher& dispatcher) -> std::ostream&;

private:
	AsyncExecuter async_executer;
	OnFailedFunc on_failed;
	std::map<command::CommandId, OnReceivedFunc> func_dict;
};

inline auto operator<<(std::ostream& os, 
		const CommandDispatcher& dispatcher) -> std::ostream& {
	for(auto& pair : dispatcher.func_dict){
		std::cout << pair.first.ToString() << ", " << std::flush;
	}
	return os;
}

}
}
