#include <gtest/gtest.h>

#include "test_executor.h"
#include "future/future.h"
#include "future/promise.h"


TEST(FutureException, CallableThrows)
{
	using namespace cebreiro;

	// arrange
	std::shared_ptr<TestExecutor> e = CreateExecutor();
	TestExecutor& executor = *e;

	// act
	Future<void> future = StartAsync([]()
		{
			throw std::runtime_error("test");
		}, executor);

	// assert
	ASSERT_ANY_THROW(future.Get());
}

TEST(FutureException, PromiseThrows)
{
	using namespace cebreiro;

	// arrange
	std::shared_ptr<TestExecutor> e = CreateExecutor();
	TestExecutor& executor = *e;

	Promise<void> promise;
	Future<void> future = promise.GetFuture();

	// act
	executor.Post(detail::Wrap([promise = std::move(promise)]() mutable
		{
			try
			{
				throw std::runtime_error("");
			}
			catch (...)
			{
				promise.SetException(std::current_exception());
			}
		}), ExecuteOption{});

	// assert
	ASSERT_ANY_THROW(future.Get());
}

TEST(FutureException, Propagation)
{
	using namespace cebreiro;

	// arrange
	std::shared_ptr<TestExecutor> e = CreateExecutor();
	TestExecutor& executor = *e;

	bool execute1 = false;
	bool execute2 = false;

	// act
	auto fut = StartAsync([]()
		{
			throw std::runtime_error("execption");
		}, executor)
	.Then([&]()
		{
			execute1 = true;

		}, executor)
	.Then([&]()
		{
			execute2 = true;
		}, executor);

	// assert
	ASSERT_ANY_THROW(fut.Get());
	ASSERT_FALSE(execute1);
	ASSERT_FALSE(execute2);
}