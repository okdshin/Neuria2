#pragma once
//ThreadSafeList:20130219
#include <iostream>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>

namespace thread_safe
{
template<class ElementType>
class ThreadSafeList{
public:
	using Ptr = boost::shared_ptr<ThreadSafeList>;

	class Adder {
	private:
		using RawAdderType = boost::function<void (const ElementType&)>;
	public:
		Adder(RawAdderType raw_adder) : raw_adder(raw_adder){}

		auto operator()(const ElementType element)const -> void {
			this->raw_adder(element);	
		}
	private:
		const RawAdderType raw_adder;
	};

	class Remover {
	private:
		using RawRemoverType = 
			boost::function<boost::function<bool (const ElementType&)>>;
	public:
		Remover(RawRemoverType raw_remover) : raw_remover(raw_remover){}

		auto operator()(boost::function<bool (const ElementType&)> decider)const -> void {
			this->raw_remover(decider);
		}
	private:
		const RawRemoverType raw_remover;
	};

	class ListQuoter {
	private:
		using RawListQuoterType = 
			boost::function<boost::function<void (const std::vector<ElementType>&)>>;
	public:
		ListQuoter(RawListQuoterType raw_list_quoter) 
			: raw_list_quoter(raw_list_quoter){}

		auto operator()(boost::function<void (const std::vector<ElementType>&)> receiver)const -> void {
			this->raw_remover(receiver);
		}
	private:
		const RawListQuoterType raw_list_quoter;
	};

	static auto Create(boost::asio::io_service& io_service) -> Ptr {
		return Ptr(new ThreadSafeList(io_service));
	}

	auto Add(const ElementType& element)const -> void {
		this->strand->post([this, element](){
			this->adder(element);
		});
	}

	auto Remove(boost::function<void (const ElementType&)> decider)const -> void {
		this->strand->post([this, decider](){
			this->remover(decider);
		});
	}

	auto QuoteList(boost::function<void (const std::vector<ElementType>&)> receiver)const -> void {
		this->strand->post([this, receiver](){
			this->list_quoter(receiver);	
		});
	}

private:
    ThreadSafeList(//todo
		boost::asio::io_service& io_service) 
		: strand(new boost::asio::io_service::strand(io_service)){}
	
	const boost::scoped_ptr<boost::asio::io_service::strand> strand;
	Adder adder;
	Remover remover;
	ListQuoter list_quoter;
};
}

