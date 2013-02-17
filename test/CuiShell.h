#pragma once
//CuiShell:20120909
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <cassert>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include "Routine.h"
#include "ShellFunc.h"

namespace neuria{
namespace test{

class CuiShell{
public:
	using ArgList = std::vector<std::string>;

	CuiShell(std::ostream& os=std::cout, std::istream& is=std::cin)
		:	func_dict(), os(os), is(is){
		this->Register("help", "shows this help ;)", 
			ShellFunc(boost::bind(&CuiShell::Help, this, _1)));
	}

	auto Register(const std::string& key, const std::string& explanation, 
			ShellFunc func) -> void {
		this->func_dict[key] = std::make_tuple(explanation, func);
	}

	auto Call(const std::string& command) -> void {
		auto parsed_list = ArgList();
		boost::split(parsed_list, command, boost::is_any_of(" "));
		try{
			if(this->func_dict.find(parsed_list.at(0)) != this->func_dict.end()){
				this->CallFunc(parsed_list);
			}
			else{
				throw NotRegisteredFuncCalledException();
			}
		}
		catch(const NotRegisteredFuncCalledException&){
			this->os << "\"" << command << "\" is invalid command." << std::endl;
		}
		catch(const std::out_of_range&){
			this->os << "too few argument. please check." << std::endl;	
		}
		catch(const boost::bad_lexical_cast& e){
			this->os << boost::format("invalid arguments(%1%)") 
				% e.what() << std::endl;
		}
		
	}

	auto Start() -> void {
		while(true){
			auto command = GetInput<std::string>("command?:", os, is);
			Call(command);
		}
	}

private:
	class NotRegisteredFuncCalledException{};

	auto CallFunc(const ArgList& arg_list) -> void {
		std::get<1>(this->func_dict[arg_list.at(0)])(arg_list);
	}

	auto Help(const ArgList& argment_list) -> void {
		for(auto& func_pair : this->func_dict){
			this->os << boost::format("%1%:%2%") 
				% func_pair.first // key
				% std::get<0>(func_pair.second) // explanation 
				<< std::endl;
		}
	}

	std::map<std::string, std::tuple<std::string, ShellFunc>> func_dict;
	std::ostream& os;
	std::istream& is;
};

inline auto RegisterExitFunc(CuiShell& shell, const std::string& bye_message) -> void {
	shell.Register("exit", "exit this app.", 
		ShellFunc([bye_message](const CuiShell::ArgList&){
			std::cout << bye_message << std::endl; 
			exit(0);
		}));
}

}
}
