#include "test_executor.h"

TestExecutor::TestExecutor()
{
	std::promise<std::thread::id> p;
	std::future<std::thread::id> fut = p.get_future();
	_thread = std::thread([this, p = std::move(p)]() mutable
	{
		p.set_value(std::this_thread::get_id());
		this->Run();
	});

	_id = fut.get();
}

TestExecutor::~TestExecutor()
{
	{
		std::lock_guard lock(_mutex);
		_shutdown = true;
	}

	_condVar.notify_one();

	_thread.join();
}

bool TestExecutor::IsRunningContext() const
{
	return _id == std::this_thread::get_id();
}

void TestExecutor::Post(const std::function<void()>& function, const cebreiro::ExecuteOption& option)
{
	(void)option;

	{
		std::lock_guard lock(_mutex);
		_frontQueue.push_back(function);
	}

	_condVar.notify_one();
}

auto TestExecutor::GetId() const -> std::thread::id
{
	return _id;
}

void TestExecutor::Run()
{
	Executor::SetCurrent(this);

	while (true)
	{
		std::unique_lock lock(_mutex);
		_condVar.wait(lock, [this]()
		{
			return _shutdown || !_frontQueue.empty();
		});

		if (_shutdown)
		{
			break;
		}

		_backQueue.swap(_frontQueue);
		lock.unlock();

		for (const std::function<void()>& function : _backQueue)
		{
			function();
		}

		_backQueue.clear();
	}

	Executor::RemoveCurrent();
}
