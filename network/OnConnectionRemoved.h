#pragma once
//OnConnectionRemoved:20130305
#include <iostream>
#include "Connection.h"
namespace neuria{
namespace network{
class OnConnectionRemoved{
private:
	using FuncType = boost::function<void (const Connection::ConstPtr&)>;
public:
    OnConnectionRemoved() : func([](const Connection::ConstPtr&){
		std::cout << "Default OnConnectionRemovedFunc Called." << std::endl;	
	}){}
    OnConnectionRemoved(FuncType func) : func(func){}

	auto operator()(const Connection::ConstPtr& connection)const -> void {
		func(connection);
	}
private:
	const FuncType func;
};
}
}
