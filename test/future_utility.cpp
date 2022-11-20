#include <gtest/gtest.h>

#include "test_executor.h"
#include "future/future.h"

TEST(FutureUtility, WaitAll)
{
	using namespace cebreiro;

	constexpr size_t expected1 = 123123;
	constexpr size_t expected2 = 456456;
	constexpr size_t expected3 = 789789;

	// arrange
	std::shared_ptr<TestExecutor> e1 = CreateExecutor();
	std::shared_ptr<TestExecutor> e2 = CreateExecutor();
	std::shared_ptr<TestExecutor> e3 = CreateExecutor();

	TestExecutor& executor1 = *e1;
	TestExecutor& executor2 = *e2;
	TestExecutor& executor3 = *e2;
	
	// act
	Future<size_t> f1 = Post([]()
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			return expected1;

		}, executor1);

	Future<size_t> f2 = Post([]()
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
			return expected2;

		}, executor2);

	Future<size_t> f3 = Post([]()
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(40));
			return expected3;

		}, executor3);

	Future<void> waitAll = WaitAll(f1, f2, f3);
	waitAll.Get();

	FutureStatus status1 = f1.Status();
	FutureStatus status2 = f2.Status();
	FutureStatus status3 = f3.Status();

	// assert
	ASSERT_EQ(status1, FutureStatus::Done);
	ASSERT_EQ(status2, FutureStatus::Done);
	ASSERT_EQ(status3, FutureStatus::Done);
	ASSERT_EQ(f1.Get(), expected1);
	ASSERT_EQ(f2.Get(), expected2);
	ASSERT_EQ(f3.Get(), expected3);
}

TEST(FutureUtility, WaitAny)
{
	using namespace cebreiro;

	// arrange
	std::shared_ptr<TestExecutor> e1 = CreateExecutor();
	std::shared_ptr<TestExecutor> e2 = CreateExecutor();
	std::shared_ptr<TestExecutor> e3 = CreateExecutor();

	TestExecutor& executor1 = *e1;
	TestExecutor& executor2 = *e2;
	TestExecutor& executor3 = *e2;

	std::atomic<size_t> counter = 0;
	size_t count1 = 0;
	size_t count2 = 0;
	size_t count3 = 0;

	// act
	Future<void> f1 = Post([&]()
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			count1 = counter.fetch_add(1) + 1;
		}, executor1);

	Future<void> f2 = Post([&]()
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			count2 = counter.fetch_add(1) + 1;
		}, executor2);

	Future<void> f3 = Post([&]()
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			count3 = counter.fetch_add(1) + 1;
		}, executor3);

	Future<void> waitAny = WaitAny(f1, f2, f3);
	waitAny.Get();

	size_t count4 = counter.load();
	FutureStatus status1 = f1.Status();
	FutureStatus status2 = f2.Status();
	FutureStatus status3 = f3.Status();

	f2.Wait();
	f3.Wait();

	// assert
	ASSERT_EQ(status1, FutureStatus::Done);
	ASSERT_EQ(status2, FutureStatus::Pending);
	ASSERT_EQ(status3, FutureStatus::Pending);
	ASSERT_EQ(count1, count4);
	ASSERT_EQ(count3, counter.load());
}

TEST(FutureUtility, Delay)
{
	using namespace cebreiro;

	// arrange
	constexpr size_t delayMilliseconds = 100;
	constexpr size_t threshold = 30;
	using clock = std::chrono::high_resolution_clock;

	// act
	auto start = clock::now();

	Future<void> delay = Delay(delayMilliseconds);
	delay.Get();

	auto end = clock::now();

	uint64_t duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

	// assert
	ASSERT_GE(duration, delayMilliseconds);
	ASSERT_LE(duration, delayMilliseconds + threshold);
}