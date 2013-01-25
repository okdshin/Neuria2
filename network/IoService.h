#pragma once
//IoService:20130110
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

namespace neuria{
namespace network
{
class IoService{
public:
	using Ptr = boost::shared_ptr<IoService>;
	static auto Create() -> Ptr{
		return Ptr(new IoService());
	}

	auto GetRawIoServiceRef() -> boost::asio::io_service& {
		return this->raw_io_service;
	}

private:
    IoService():raw_io_service(){}

private:
	boost::asio::io_service raw_io_service;

};
}
}
