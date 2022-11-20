#pragma once
#include <vector>
#include <thread>
#include <functional>
#include <condition_variable>
#include <mutex>
#include "future/executor.h"

namespace cebreiro
{
	class DefaultExecutor : public Executor
	{
	public:
		DefaultExecutor();
		~DefaultExecutor();

		bool IsRunningContext() const override;
		void Post(const std::function<void()>& function, const ExecuteOption& option) override;

	private:
		void Run(size_t i);

	private:
		std::vector<std::thread> _workers;
		std::vector<std::thread::id> _workerIds;
		std::atomic<bool> _shutdown = false;

		struct WorkerContext
		{
			std::mutex mutex;
			std::condition_variable condVar;
			std::vector<std::function<void()>> works;
		};
		std::vector<std::unique_ptr<WorkerContext>> _workerContexts;
	};
}
