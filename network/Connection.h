#pragma once
//Connection:20130110
#include <iostream>
#include <deque>
#include <boost/function.hpp>
#include <boost/format.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "Socket.h"
#include "ByteArray.h"
#include "BufferSize.h"
#include "MessageBody.h"
#include "MessageHeader.h"
#include "Message.h"
#include "OnSendedFunc.h"
#include "OnFailedFunc.h"
#include "OnReceivedFunc.h"
#include "IsDebugMode.h"
namespace neuria{
namespace network
{
class Connection 
	: public boost::enable_shared_from_this<Connection> {
public:
	using Ptr = boost::shared_ptr<Connection>;
	class OnPeerClosedFunc{
	public:
		OnPeerClosedFunc():on_peer_closed([](const Ptr&){
				std::cout << "Called(Default)OnPeerClosedFunc" << std::endl;
			
			}){}
		OnPeerClosedFunc(boost::function<void (Ptr)> on_peer_closed)
			: on_peer_closed(on_peer_closed){}
		~OnPeerClosedFunc(){}

		auto operator()(Ptr connection) -> void {
			on_peer_closed(connection);	
		}
	private:
		boost::function<void (Ptr)> on_peer_closed;
	};

	static auto Create(
			Socket::Ptr socket, 
			const BufferSize& buffer_size, 
			OnReceivedFunc on_received, 
			OnPeerClosedFunc on_peer_closed, 
			OnFailedFunc on_failed,
			const IsDebugMode& is_debug_mode = IsDebugMode(true)) -> Ptr {
		auto new_connection = Ptr(new Connection(
			socket, buffer_size, on_received, on_peer_closed, on_failed, is_debug_mode));
		std::cout << "connection created " 
			<< new_connection.get() << std::endl; 
		return new_connection;
	}

	~Connection(){
		std::cout << "connection deleted" << std::endl;	
	}

private:
	Connection(
			Socket::Ptr socket, 
			const BufferSize& buffer_size,
			OnReceivedFunc on_received, 
			OnPeerClosedFunc on_peer_closed, 
			OnFailedFunc on_failed,
			const IsDebugMode& is_debug_mode)
		:socket(socket), 
		send_strand(socket->GetRawSocketRef().get_io_service()),
		received_part_of_byte_array(buffer_size.ToInt()),
		on_received(on_received), 
		on_peer_closed(on_peer_closed), 
		on_failed(on_failed),
		is_debug_mode(is_debug_mode){
	
	}

public:
	auto GetRemoteHostName()const -> HostName {
		return this->socket->GetRemoteHostName();
	}

	auto GetRemotePortNumber()const -> PortNumber {
		return this->socket->GetRemotePortNumber();
	}

private:
	Socket::Ptr socket;
	boost::asio::io_service::strand send_strand;
	
	std::deque<ByteArray> send_queue;

	ByteArray received_part_of_byte_array;
	ByteArray received_byte_array;
	MessageHeader header;
	
	OnReceivedFunc on_received; 
	OnPeerClosedFunc on_peer_closed; 
	OnFailedFunc on_failed;

	IsDebugMode is_debug_mode;

public:
	auto Send(
			const ByteArray& byte_array, 
			const OnSendedFunc& on_sended, 
			const OnFailedFunc& on_failed) -> void {
		this->send_strand.post(
			boost::bind(&Connection::ReserveToSend, this->shared_from_this(),
				byte_array, on_sended, on_failed)
		);
	}

private:
	auto ReserveToSend(
			const ByteArray& byte_array, 
			OnSendedFunc on_sended, 
			OnFailedFunc on_failed) -> void {
		const auto body = byte_array;
		const auto header = CreateMessageHeaderFromBody(body);
		const auto message = Message(header, body);
		
		this->send_queue.push_back(message.ToByteArray());

		if(this->send_queue.size() > 1){
			return;
		}
		this->DoSend(on_sended, on_failed);
	}

	auto DoSend(
			const OnSendedFunc& on_sended, const OnFailedFunc& on_failed) -> void {
		boost::asio::async_write(
			this->socket->GetRawSocketRef(), 
			boost::asio::buffer(this->send_queue.front()),
			this->send_strand.wrap(
				boost::bind(
					&Connection::OnSended, this->shared_from_this(), 
					on_sended, on_failed,
					boost::asio::placeholders::error
				)
			)
		);
	}

	auto OnSended(
			OnSendedFunc on_sended, OnFailedFunc on_failed,
			const boost::system::error_code& error_code) -> void {
		if(error_code){
			on_failed(ErrorCode(error_code));	
			this->/*Do*/Close();
		}
		
		if(is_debug_mode()){
			std::cout << "Sended:";
			OutputByteArray(std::cout, this->send_queue.front()) << std::endl;
		}

		this->send_queue.pop_front();
		on_sended();
		if(!this->send_queue.empty()){
			this->DoSend(on_sended, on_failed);
		}
	}

public:
	auto StartReceive() -> void {
		if(this->is_debug_mode()){
			std::cout << "StartReceive" << std::endl;
		}
		ReceiveHeader();	
	}

private:
	auto ReceiveHeader() -> void {
		if(this->is_debug_mode()){
			std::cout << "ReceiveHeader" << std::endl;
			OutputByteArray(std::cout << 
				"\t:", this->received_byte_array) << std::endl;
		}
		this->socket->GetRawSocketRef().async_read_some(
			boost::asio::buffer(this->received_part_of_byte_array),
			boost::bind(
				&Connection::AppendPartOfHeaderByteArray, 
				this->shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred
			)
		);
	}

	auto AppendPartOfHeaderByteArray(
			const boost::system::error_code& error_code, 
			std::size_t bytes_transferred) -> void {
		if(this->is_debug_mode()){
			std::cout << "AppendPartOfHeaderByteArray" << std::endl;
		}
		if(!error_code)	{
			assert(bytes_transferred != 0);
			std::copy(
				this->received_part_of_byte_array.begin(), 
				this->received_part_of_byte_array.begin() + bytes_transferred,
				std::back_inserter(this->received_byte_array)
			);
			CheckIsEnableParseHeader(error_code, bytes_transferred);
		}
		else {
			this->/*Do*/Close();
			on_peer_closed(this->shared_from_this());
			if(this->socket->GetRawSocketRef().is_open()){
				//peer socket closed.
				if(this->is_debug_mode()){
					std::cout << "peer socket closed." << std::endl;
				}
			}
		}
	}

	auto CheckIsEnableParseHeader(
			const boost::system::error_code& error_code, 
			std::size_t bytes_transferred) -> void {
		if(this->is_debug_mode()){
			std::cout << "CheckIsEnableParseHeader" << std::endl;
		}
		if(!error_code)	{
			if(MessageHeader::IsEnableParse(this->received_byte_array)){
				if(this->is_debug_mode()){
					std::cout << "\t-> true" << std::endl;
				}
				this->header = MessageHeader::Parse(this->received_byte_array);
				CheckIsReceivedWholeMessageByteArray(error_code, bytes_transferred);
			}
			else{
				if(this->is_debug_mode()){
					std::cout << "\t-> false" << std::endl;
				}
				ReceiveHeader();	
			}
		}
		else {
			this->/*Do*/Close();
			on_peer_closed(this->shared_from_this());
			if(this->socket->GetRawSocketRef().is_open()){
				//peer socket closed.
				if(this->is_debug_mode()){
					std::cout << "peer socket closed." << std::endl;
				}
			}
		}
	}

	auto ReceiveBody() -> void {
		if(this->is_debug_mode()){
			std::cout << "ReceiveBody" << std::endl;
			OutputByteArray(std::cout << "\t:", this->received_byte_array) << std::endl;
		}
		this->socket->GetRawSocketRef().async_read_some(
			boost::asio::buffer(this->received_part_of_byte_array),
			boost::bind(&Connection::AppendPartOfBodyByteArray,
				this->shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred
			)
		);
		
	}

	auto AppendPartOfBodyByteArray(
			const boost::system::error_code& error_code, 
			std::size_t bytes_transferred) -> void {
		if(this->is_debug_mode()){
			std::cout << "AppendPartOfBodyByteArray" << std::endl;
		}
		if(!error_code){
			std::copy(
				this->received_part_of_byte_array.begin(), 
				this->received_part_of_byte_array.begin() + bytes_transferred,
				std::back_inserter(this->received_byte_array)
			);
			CheckIsReceivedWholeMessageByteArray(error_code, bytes_transferred);
		}
		else { 
			this->/*Do*/Close();		
			on_peer_closed(this->shared_from_this());
			if(this->socket->GetRawSocketRef().is_open()){
				//peer socket closed.
				if(this->is_debug_mode()){
					std::cout << "peer socket closed." << std::endl;
				}
			}
		}
	}

	auto CheckIsReceivedWholeMessageByteArray(
			const boost::system::error_code& error_code, 
			std::size_t bytes_transferred) -> void {
		if(this->is_debug_mode()){
			std::cout << "CheckIsReceivedWholeMessageByteArray" << std::endl;
		}
		if(!error_code){
			if(IsWholeMessageByteArrayReceived(
					this->header, this->received_byte_array)){
				if(this->is_debug_mode()){
					std::cout << boost::format("need size:%1%, current size:%2%")
						% this->header.GetBodySize()
						% this->received_byte_array.size()
					<< std::endl;
					std::cout << "\t-> true" << std::endl;
				}
				ByteArray message_body;
				std::copy(
					this->received_byte_array.begin() + this->header.GetHeaderSize(),
					this->received_byte_array.begin() + this->header.GetHeaderSize()
						+this->header.GetBodySize(),
					std::back_inserter(message_body)
				);

				try{
					//post/dispatchするべきか？
					on_received(message_body);
				}
				catch(...){
					this->/*Do*/Close();
				}

				this->received_byte_array.erase(
					this->received_byte_array.begin(),
					this->received_byte_array.begin() + this->header.GetHeaderSize()	
						+this->header.GetBodySize()	
				);

				CheckIsEnableParseHeader(error_code, bytes_transferred);
			}
			else { //まだ全部受信できてない場合
				if(this->is_debug_mode()){
					std::cout << "\t-> false" << std::endl;
				}
				ReceiveBody();
			}
		}
		else { 
			this->/*Do*/Close();		
			on_peer_closed(this->shared_from_this());
			if(this->socket->GetRawSocketRef().is_open()){
				//peer socket closed.
				if(this->is_debug_mode()){
					std::cout << "peer socket closed." << std::endl;
				}
			}
		}
		
	}

public:
	auto Close() -> void {
		/*
		this->socket->GetRawSocketRef().get_io_service().post(
			boost::bind(&Connection::DoClose, this->shared_from_this()));	
		*/
		boost::system::error_code ec;
		this->socket->GetRawSocketRef().shutdown(
			boost::asio::ip::tcp::socket::shutdown_send, ec);
	}

private:
	auto DoClose() -> void {
		std::cout << "Connection" << this->shared_from_this().get() << "::DoClose called." << std::endl;
		this->socket->GetRawSocketRef().close();	
	}


};

auto operator<<(std::ostream& os, const Connection::Ptr& connection) -> std::ostream& {
	os << boost::format("Connection:%1%:%2%")
		% connection->GetRemoteHostName().ToString()
		% connection->GetRemotePortNumber().ToInt()
	;
	return os;
}
}
}
