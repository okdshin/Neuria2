#ifdef CUISHELL_UNIT_TEST
#include "CuiShell.h"
#include <iostream>

using namespace neuria;
using namespace neuria::test;

int main(int argc, char* argv[])
{
	auto shell = CuiShell(std::cout);
	RegisterExitFunc(shell, "bye :)");
	shell.Register("broadcast", "broadcast text to peers.", 
		ShellFunc([](const CuiShell::ArgList& arg_list){
			std::cout << "call broadcast." << std::endl;
			for(const auto& arg : arg_list){
				std::cout << arg << std::endl;	
				std::cout << boost::lexical_cast<int>(arg) << std::endl;	
			}
		}));
	shell.Start();

    return 0;
}

#endif
