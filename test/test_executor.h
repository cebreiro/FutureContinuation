#pragma once
#include "future/executor.h"

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>

class TestExecutor : public cebreiro::Executor
{
public:
	TestExecutor();
	~TestExecutor() override;

	bool IsRunningContext() const override;
	void Post(const std::function<void()>& function, const cebreiro::ExecuteOption& option) override;

	auto GetId() const -> std::thread::id;

private:
	void Run();

private:
	bool _shutdown = false;
	std::thread::id _id;
	std::thread _thread;
	mutable std::mutex _mutex;
	std::condition_variable _condVar;
	std::vector<std::function<void()>> _frontQueue;
	std::vector<std::function<void()>> _backQueue;
};

inline auto CreateExecutor() -> std::shared_ptr<TestExecutor>
{
	return std::make_shared<TestExecutor>();
}