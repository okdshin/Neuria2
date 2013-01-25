#pragma once
//Connection:20130110
#include <iostream>
#include <deque>
#include <boost/function.hpp>
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
namespace neuria{
namespace network
{
class Connection 
	: public boost::enable_shared_from_this<Connection> {
public:
	using Ptr = boost::shared_ptr<Connection>;
	class OnPeerClosedFunc{
	public:
		OnPeerClosedFunc():on_peer_closed([](Ptr){
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
			OnFailedFunc on_failed) -> Ptr {
		return Ptr(new Connection(
			socket, buffer_size, on_received, on_peer_closed, on_failed));
	}

private:
	Connection(
			Socket::Ptr socket, 
			const BufferSize& buffer_size,
			OnReceivedFunc on_received, 
			OnPeerClosedFunc on_peer_closed, 
			OnFailedFunc on_failed)
		:socket(socket), 
		received_part_of_byte_array(buffer_size.ToInt()),
		on_received(on_received), 
		on_peer_closed(on_peer_closed), 
		on_failed(on_failed){}

private:
	class MessageBodyAndFunction {
	public:
		MessageBodyAndFunction(
				const MessageBody& message_body, 
				OnSendedFunc on_sended, 
				OnFailedFunc on_failed):
			message_body(message_body), 
			on_sended(on_sended), 
			on_failed(on_failed){}

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
		MessageBody message_body;
		OnSendedFunc on_sended;
		OnFailedFunc on_failed;
	};

private:
	Socket::Ptr socket;
	std::deque<MessageBodyAndFunction> message_body_and_function_queue;
	ByteArray received_part_of_byte_array;
	ByteArray received_byte_array;
	MessageHeader header;
	
	OnReceivedFunc on_received; 
	OnPeerClosedFunc on_peer_closed; 
	OnFailedFunc on_failed;

public:
	auto Send(
			const ByteArray& byte_array, 
			OnSendedFunc on_sended, 
			OnFailedFunc on_failed) -> void {
		this->message_body_and_function_queue.push_back(
			MessageBodyAndFunction(
				MessageBody(byte_array), 
				on_sended, 
				on_failed));
		if(this->message_body_and_function_queue.size() == 1){ //start new
			StartSend();	
		}
	}

private:
	auto StartSend() -> void {
		const auto body = message_body_and_function_queue.front().GetMessageBody();
		const auto header = CreateMessageHeaderFromBody(body);
		const auto message = Message(header, body);
		std::cout << "Sended:";
		OutputByteArray(std::cout, message.ToByteArray()) << std::endl;
		boost::asio::async_write(
			this->socket->GetRawSocketRef(), 
			boost::asio::buffer(message.ToByteArray()),
			boost::bind(&Connection::OnSended, this->shared_from_this(), 
				message_body_and_function_queue.front(),
				boost::asio::placeholders::error
			)
		);
	}

	auto OnSended(
			const MessageBodyAndFunction& message_body_and_function, 
			const boost::system::error_code& error_code) -> void {
		if(!error_code){
			this->message_body_and_function_queue.pop_front();
			message_body_and_function.GetOnSendedFunc()();
			if(!this->message_body_and_function_queue.empty()){
				this->StartSend();		
			}
		}
		else{
			message_body_and_function.GetOnFailedFunc()(ErrorCode(error_code));	
		}
	}

public:
	auto StartReceive() -> void {
		std::cout << "StartReceive" << std::endl;
		ReceiveHeader();	
	}
private:

	auto ReceiveHeader() -> void {
		std::cout << "ReceiveHeader" << std::endl;
		OutputByteArray(std::cout << "\t:", this->received_byte_array) << std::endl;
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
		std::cout << "AppendPartOfHeaderByteArray" << std::endl;
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
				std::cout << "peer socket closed." << std::endl;
			}
		}
	}

	auto CheckIsEnableParseHeader(
			const boost::system::error_code& error_code, 
			std::size_t bytes_transferred) -> void {
		std::cout << "CheckIsEnableParseHeader" << std::endl;
		if(!error_code)	{
			if(MessageHeader::IsEnableParse(this->received_byte_array)){
				std::cout << "\t-> true" << std::endl;
				this->header = MessageHeader::Parse(this->received_byte_array);
				CheckIsReceivedWholeMessageByteArray(error_code, bytes_transferred);
			}
			else{
				std::cout << "\t-> false" << std::endl;
				ReceiveHeader();	
			}
		}
		else {
			this->DoClose();
			on_peer_closed(this->shared_from_this());
			if(this->socket->GetRawSocketRef().is_open()){
				//peer socket closed.
				std::cout << "peer socket closed." << std::endl;
			}
		}
	}

	auto ReceiveBody() -> void {
		std::cout << "ReceiveBody" << std::endl;
		OutputByteArray(std::cout << "\t:", this->received_byte_array) << std::endl;
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
		std::cout << "AppendPartOfBodyByteArray" << std::endl;
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
				std::cout << "peer socket closed." << std::endl;
			}
		}
	}

	auto CheckIsReceivedWholeMessageByteArray(
			const boost::system::error_code& error_code, 
			std::size_t bytes_transferred) -> void {
		std::cout << "CheckIsReceivedWholeMessageByteArray" << std::endl;
		if(!error_code){
			if(IsWholeMessageByteArrayReceived(
					this->header, this->received_byte_array)){
				std::cout << "\t-> true" << std::endl;
				ByteArray message_body;
				std::copy(
					this->received_byte_array.begin() + this->header.GetHeaderSize(),
					this->received_byte_array.begin() + this->header.GetHeaderSize()
						+this->header.GetBodySize(),
					std::back_inserter(message_body)
				);

				//post/dispatchするべきか？
				on_received(message_body);
			
				this->received_byte_array.erase(
					this->received_byte_array.begin(),
					this->received_byte_array.begin() + this->header.GetHeaderSize()	
						+this->header.GetBodySize()	
				);

				CheckIsEnableParseHeader(error_code, bytes_transferred);
			}
			else { //まだ全部受信できてない場合
				std::cout << "\t-> false" << std::endl;
				ReceiveBody();
			}
		}
		else { 
			this->DoClose();		
			on_peer_closed(this->shared_from_this());
			if(this->socket->GetRawSocketRef().is_open()){
				//peer socket closed.
				std::cout << "peer socket closed." << std::endl;
			}
		}
		
	}

public:
	auto Close() -> void {
		this->socket->GetRawSocketRef().get_io_service().post(
			boost::bind(&Connection::DoClose, this->shared_from_this()));	
	}

private:
	auto DoClose() -> void {
		std::cout << "Connection::DoClose called." << std::endl;
		this->socket->GetRawSocketRef().close();	
	}


};
}
}
