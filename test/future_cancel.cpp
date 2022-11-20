#include <gtest/gtest.h>

#include "test_executor.h"
#include "future/future.h"

TEST(FutureCancel, Single)
{
	using namespace cebreiro;

	// arrange
	std::shared_ptr<TestExecutor> e = CreateExecutor();
	TestExecutor& executor = *e;

	// act
	Future<void> fut = StartAsync([]()
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}, executor);
	fut.Cancel();

	FutureStatus status = fut.Wait();

	// assert
	ASSERT_ANY_THROW(fut.Get());
	ASSERT_EQ(status, FutureStatus::Canceled);
}

TEST(FutureCancel, Propagation)
{
	using namespace cebreiro;

	// arrange
	std::shared_ptr<TestExecutor> e = CreateExecutor();
	TestExecutor& executor = *e;

	// act
	Future<void> fut1 = StartAsync([]()
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}, executor);

	bool executed1 = false;
	Future<void> fut2 = fut1.Then([&]()
		{
			executed1 = true;
		}, executor);

	bool executed2 = false;
	Future<void> fut3 = fut2.Then([&]()
		{
			executed2 = true;

		}, executor);

	fut1.Cancel();

	// assert
	ASSERT_ANY_THROW(fut1.Get());
	ASSERT_ANY_THROW(fut2.Get());
	ASSERT_ANY_THROW(fut3.Get());

	ASSERT_EQ(fut1.Status(), FutureStatus::Canceled);
	ASSERT_EQ(fut2.Status(), FutureStatus::Done);
	ASSERT_EQ(fut3.Status(), FutureStatus::Done);
}