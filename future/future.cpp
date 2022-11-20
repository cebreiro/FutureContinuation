#include "future.h"

namespace cebreiro
{
	auto Delay(uint64_t milliseconds) -> Future<void>
	{
#ifdef WIN32
		static HANDLE timerQueue = ::CreateTimerQueue();

		struct Timer
		{
			Timer(uint64_t milliseconds)
				: executionContext(std::make_shared<detail::ExecutionContextImpl<void>>())
			{
				::CreateTimerQueueTimer(
					&timer, 
					timerQueue,
					reinterpret_cast<WAITORTIMERCALLBACK>(OnExpired), 
					this, 
					static_cast<uint32_t>(milliseconds), 
					0, 
					NULL);
			}

			~Timer() {}

			static void CALLBACK OnExpired(void* param, bool timeout)
			{
				(void)timeout;

				Timer* timer = static_cast<Timer*>(param);
				timer->executionContext.Set();

				delete timer;
			}

			HANDLE timer = NULL;
			ExecutionContext<void> executionContext;
		};
		
		auto timer = new Timer(milliseconds);
		return Future<void>(timer->executionContext);
#else
		throw std::runtime_error("not implementation");
#endif
	}
}
