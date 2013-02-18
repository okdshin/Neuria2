#ifdef ROUTINE_UNIT_TEST
#include "Routine.h"
#include <iostream>

using namespace neuria;
using namespace neuria::shell;

int main(int argc, char* argv[])
{
	GetInput<std::string>("hello", std::cout, std::cin);

    return 0;
}

#endif
