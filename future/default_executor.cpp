#include "default_executor.h"

#include <future>
#include <ranges>

namespace cebreiro
{
	DefaultExecutor::DefaultExecutor()
	{
		std::vector<std::promise<std::thread::id>> promises;
		std::vector<std::future<std::thread::id>> futures;

		size_t concurrency = std::thread::hardware_concurrency();

		promises.resize(concurrency);
		futures.reserve(concurrency);

		_workerContexts.resize(concurrency);
		_workers.resize(concurrency);


		for (size_t i = 0; i < concurrency; ++i)
		{
			std::promise<std::thread::id>& promise = promises[i];
			futures.push_back(promise.get_future());

			_workerContexts[i] = std::make_unique<WorkerContext>();
			_workers[i] = std::thread([this, i, &promise]()
				{
					promise.set_value(std::this_thread::get_id());
					this->Run(i);
				});
		}

		for (std::future<std::thread::id>& future : futures)
		{
			_workerIds.push_back(future.get());
		}
	}

	DefaultExecutor::~DefaultExecutor()
	{
		_shutdown.store(true);

		for (const std::unique_ptr<WorkerContext>& context : _workerContexts)
		{
			context->condVar.notify_one();
		}

		for (std::thread& worker : _workers)
		{
			worker.join();
		}
	}

	bool DefaultExecutor::IsRunningContext() const
	{
		return std::ranges::any_of(_workerIds, [current = std::this_thread::get_id()](const std::thread::id& workerId)
			{
				return workerId == current;
			});
	}

	void DefaultExecutor::Post(const std::function<void()>& function, const ExecuteOption& option)
	{
		(void)option;

		static std::atomic<size_t> incremental = 0;

		size_t index = incremental.fetch_add(1) % _workers.size();
		WorkerContext& context = *_workerContexts[index];

		{
			std::lock_guard lock(context.mutex);
			context.works.push_back(function);
		}

		context.condVar.notify_one();
	}

	void DefaultExecutor::Run(size_t i)
	{
		WorkerContext& context = *_workerContexts[i];

		std::vector<std::function<void()>> works;

		while (true)
		{
			{
				std::unique_lock lock(context.mutex);
				context.condVar.wait(lock, [this, &context]()
					{
						return this->_shutdown || !context.works.empty();
					});

				if (_shutdown)
				{
					break;
				}

				context.works.swap(works);
			}

			for (const std::function<void()>& work : works)
			{
				work();
			}

			works.clear();
		}
	}
}
