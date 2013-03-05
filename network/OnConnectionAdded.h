#pragma once
//OnConnectionAdded:20130305
#include <iostream>
#include "Connection.h"
namespace neuria{
namespace network{
class OnConnectionAdded{
private:
	using FuncType = boost::function<void (const Connection::ConstPtr&)>;
public:
    OnConnectionAdded() : func([](const Connection::ConstPtr&){
		std::cout << "Default OnConnectionAddedFunc Called." << std::endl;	
	}){}
    OnConnectionAdded(FuncType func) : func(func){}

	auto operator()(const Connection::ConstPtr& connection)const -> void {
		func(connection);
	}
private:
	const FuncType func;
};
}
}
