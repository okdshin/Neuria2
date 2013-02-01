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
	class MessageBodyAndFunction {
	public:
		using Ptr = boost::shared_ptr<MessageBodyAndFunction>;
		static auto Create(
				const MessageBody& message_body, 
				OnSendedFunc on_sended, 
				OnFailedFunc on_failed) -> Ptr {
			return Ptr(
				new MessageBodyAndFunction(message_body, on_sended, on_failed));	
		}

		auto GetMessageBody() -> MessageBody {
			return this->message_body;	
		}

		auto GetOnSendedFunc()const -> OnSendedFunc {
			return this->on_sended;
		}
		
		auto GetOnFailedFunc()const -> OnFailedFunc {
			return this->on_failed;
		}

	private:
		MessageBodyAndFunction(
				const MessageBody& message_body, 
				OnSendedFunc on_sended, 
				OnFailedFunc on_failed):
			message_body(message_body), 
			on_sended(on_sended), 
			on_failed(on_failed){}

		MessageBody message_body;
		OnSendedFunc on_sended;
		OnFailedFunc on_failed;
	};

private:
	Socket::Ptr socket;
	std::deque<MessageBodyAndFunction::Ptr> message_body_and_function_queue;
	//Message //send_message;
	//$B$@$a$@$C$?!#Aw<u?.%P%C%U%!$rD>@\;}$?$J$$$H$$$1$J$$$h$&$@!#(B

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
		this->socket->GetRawSocketRef().get_io_service().post(
			boost::bind(&Connection::DoSend, this->shared_from_this(),
				byte_array, on_sended, on_failed)
		);
	}

private:
	auto DoSend(
			ByteArray byte_array, 
			OnSendedFunc on_sended, 
			OnFailedFunc on_failed) -> void {
		bool is_send_in_progress = !this->message_body_and_function_queue.empty();
		this->message_body_and_function_queue.push_back(
			MessageBodyAndFunction::Create(
				MessageBody(byte_array), 
				on_sended, 
				on_failed));

		if(!is_send_in_progress){ //start new
			const auto message_body_and_function = 
				message_body_and_function_queue.front();
			assert(message_body_and_function != 0);
			assert(message_body_and_function.get() != 0);
			const auto body = 
				message_body_and_function->GetMessageBody();
			const auto header = CreateMessageHeaderFromBody(body);
			const auto message = Message(header, body);
			auto send_byte_array 
				= boost::shared_ptr<ByteArray>(new ByteArray(message.ToByteArray()));
			if(is_debug_mode()){
				std::cout << "Sended:";
				OutputByteArray(std::cout, message.ToByteArray()) << std::endl;
			}
			boost::asio::async_write(
				this->socket->GetRawSocketRef(), 
				boost::asio::buffer(*send_byte_array),
				boost::bind(&Connection::OnSended, this->shared_from_this(), 
					message_body_and_function_queue.front(),
					boost::asio::placeholders::error,
					send_byte_array
				)
			);
		}
	}

	auto OnSended(
			MessageBodyAndFunction::Ptr message_body_and_function, 
			const boost::system::error_code& error_code,
			boost::shared_ptr<ByteArray> alive_byte_array) -> void {
		if(!error_code){
			message_body_and_function->GetOnSendedFunc()();
			this->message_body_and_function_queue.pop_front();
			if(!this->message_body_and_function_queue.empty()){
				const auto body = 
					message_body_and_function_queue.front()->GetMessageBody();
				const auto header = CreateMessageHeaderFromBody(body);
				const auto message = Message(header, body);
				auto send_byte_array 
					= boost::shared_ptr<ByteArray>(new ByteArray(message.ToByteArray()));
				if(is_debug_mode()){
					std::cout << "Sended:";
					OutputByteArray(std::cout, message.ToByteArray()) << std::endl;
				}
				boost::asio::async_write(
					this->socket->GetRawSocketRef(), 
					boost::asio::buffer(*send_byte_array),
					boost::bind(&Connection::OnSended, this->shared_from_this(), 
						message_body_and_function_queue.front(),
						boost::asio::placeholders::error,
						send_byte_array
					)
				);
			}
		}
		else{
			message_body_and_function->GetOnFailedFunc()(ErrorCode(error_code));	
			this->DoClose();
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
			this->DoClose();
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
			this->DoClose();
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
			this->DoClose();		
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

				//post/dispatch$B$9$k$Y$-$+!)(B
				on_received(message_body);
			
				this->received_byte_array.erase(
					this->received_byte_array.begin(),
					this->received_byte_array.begin() + this->header.GetHeaderSize()	
						+this->header.GetBodySize()	
				);

				CheckIsEnableParseHeader(error_code, bytes_transferred);
			}
			else { //$B$^$@A4It<u?.$G$-$F$J$$>l9g(B
				if(this->is_debug_mode()){
					std::cout << "\t-> false" << std::endl;
				}
				ReceiveBody();
			}
		}
		else { 
			this->DoClose();		
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
