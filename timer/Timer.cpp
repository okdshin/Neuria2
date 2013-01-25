#ifdef TIMER_UNIT_TEST
#include "Timer.h"
#include <iostream>
#include "OnTimerErasedFunc.h"

using namespace neuria;
using namespace neuria::timer;

int main(int argc, char* argv[])
{
	boost::asio::io_service service;
	//boost::asio::io_service::work w(service);
	int count = 0;
	auto timer = Timer::Create(service, 1, 
		TimerCallbackFunc([&count]() -> IsContinue {
			std::cout << "hello:" << count << std::endl;
			++count;
			return IsContinue(count > 5 ? false : true);
		}),
		Timer::OnErasedFunc()
	);
	timer->Start();
	service.run();

    return 0;
}

#endif
