#ifdef COMMANDID_UNIT_TEST
#include "CommandId.h"
#include <iostream>

using namespace neuria::command;

int main(int argc, char* argv[])
{
	CommandId command_id("test");
	std::cout << command_id << std::endl;

    return 0;
}

#endif
