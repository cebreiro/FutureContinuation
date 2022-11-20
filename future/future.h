#pragma once
#include "future/detail/callable_traits.h"
#include "future/detail/execution_context.h"
#include "future/detail/move_only_wrapper.h"
#include "future/execute_options.h"

#ifdef WIN32
#include <Windows.h>
#endif

namespace cebreiro
{
	class Executor;

	template <typename T>
	struct FutureConfigureAwait;

	template <typename T>
	class Future
	{
	public:
		Future() = default;
		Future(const Future&) = delete;
		Future(Future&& rhs) noexcept = default;
		Future& operator=(const Future&) = delete;
		Future& operator=(Future&& rhs) noexcept = default;

		explicit Future(ExecutionContext<T> context);
		~Future();

		void Cancel();
		auto Wait() -> FutureStatus;
		auto WaitFor(std::chrono::milliseconds milliseconds) -> FutureStatus;

		auto Get() -> decltype(auto);

		template <typename Callable>
		auto Then(Callable&& callable, Executor& executor = Executor::Current(), const ExecuteOption& option = ExecuteOption{});

		template <typename Callable>
		auto ContinuationWith(Callable&& callable, Executor& executor = Executor::Current(), const ExecuteOption& option = ExecuteOption{});

		bool IsPending() const;
		bool IsDone() const;
		bool IsCanceled() const;
		auto Status() -> FutureStatus;

		auto ConfigureAwait(Executor& resumeExecutor) ->FutureConfigureAwait<T>;

	private:
		ExecutionContext<T> _context;
	};

	template <typename Callable, typename Result = typename detail::CallableTraits<Callable>::result_type>
	auto Post(Callable&& callable, Executor& executor = Executor::GetDefault(), const ExecuteOption& option = ExecuteOption{})
		-> Future<Result>
	{
		ExecutionContext<Result> context(std::make_shared<detail::ExecutionContextImpl<Result>>());
		executor.Post([callable = detail::Wrap(std::forward<Callable>(callable)), context]() mutable
			{
				try
				{
					if constexpr (std::is_same_v<Result, void>)
					{
						callable();
						context.Set();
					}
					else
					{
						context.Set(callable());
					}
				}
				catch (...)
				{
					context.SetException(std::current_exception());
				}

			}, option);

		return Future(std::move(context));
	}

	template <typename... Futures>
	auto WaitAll(Futures&&... futures) -> Future<void>
	{
		static constexpr size_t count = sizeof...(Futures);

		auto counter = std::make_shared<std::atomic<size_t>>(0);
		ExecutionContext<void> context(std::make_shared<detail::ExecutionContextImpl<void>>());

		auto fn = [counter, context]<typename T>(Future<T>&fut) mutable
		{
			fut.ContinuationWith([counter, context]([[maybe_unused]] Future<T>& self) mutable
				{
					size_t current = counter->fetch_add(1) + 1;
					if (current == count)
					{
						context.Set();
					}
				});
		};

		(fn(futures), ...);

		return Future<void>(std::move(context));
	}

	template <typename... Futures>
	auto WaitAny(Futures&&... futures) -> Future<void>
	{
		auto counter = std::make_shared<std::atomic<size_t>>(0);
		ExecutionContext<void> context(std::make_shared<detail::ExecutionContextImpl<void>>());

		auto fn = [counter, context]<typename T>(Future<T>&fut) mutable
		{
			fut.ContinuationWith([counter, context]([[maybe_unused]] Future<T>& self) mutable
				{
					size_t current = counter->fetch_add(1) + 1;
					if (current == 1)
					{
						context.Set();
					}
				});
		};

		(fn(futures), ...);

		return Future<void>(std::move(context));
	}

	auto Delay(uint64_t milliseconds) -> Future<void>;
}

#include "future/future.ipp"