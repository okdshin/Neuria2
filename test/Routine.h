#pragma once
//Routine:20121009
#include <iostream>
#include <string>
#include <boost/lexical_cast.hpp>

namespace neuria{
namespace test{


template<class ValueType>
inline auto GetInput(
	const std::string& prompt, std::ostream& os, std::istream& is) -> ValueType
{
	os << prompt << std::flush;
	std::string input;
	std::getline(is, input);
	try
	{
		return boost::lexical_cast<ValueType>(input);
	}
	catch(boost::bad_lexical_cast& e)
	{
		os << "error. retry." << "\n";
		return GetInput<ValueType>(prompt, os, is);	
	}
}

}
}

