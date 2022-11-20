#pragma once

namespace cebreiro
{
	template <typename T>
	Future<T>::Future(ExecutionContext<T> context)
		: _context(std::move(context))
	{
	}

	template <typename T>
	Future<T>::~Future()
	{
	}

	template <typename T>
	void Future<T>::Cancel()
	{
		_context.Cancel();
	}

	template <typename T>
	auto Future<T>::Wait() -> FutureStatus
	{
		return _context.Wait();
	}

	template <typename T>
	auto Future<T>::WaitFor(std::chrono::milliseconds milliseconds) -> FutureStatus
	{
		return _context.WaitFor(milliseconds);
	}

	template <typename T>
	auto Future<T>::Get() -> decltype(auto)
	{
		return _context.Get();
	}

	template <typename T>
	template <typename Callable>
	auto Future<T>::Then(Callable&& callable, Executor& executor, const ExecuteOption& option)
	{
		using traits = detail::CallableTraits<Callable>;
		using result_type = typename traits::result_type;
		constexpr size_t arity = traits::arity;

		ExecutionContext<result_type> next(std::make_shared<detail::ExecutionContextImpl<result_type>>());

		_context.AddContinuation([prev = _context, next, callable = detail::Wrap(std::forward<Callable>(callable)),
			weak = executor.weak_from_this(), option = option]() mutable
			{
				auto executor = weak.lock();
				if (!executor)
				{
					next.Cancel();
					return;
				}

				executor->Post([prev = std::move(prev), next = std::move(next), callable]() mutable
					{
						try
						{
							if constexpr (arity == 1)
							{
								if constexpr (std::is_same_v<result_type, void>)
								{
									callable(prev.Get());
									next.Set();
								}
								else
								{
									next.Set(callable(prev.Get()));
								}
							}
							else
							{
								(void)prev.Get();

								if constexpr (std::is_same_v<result_type, void>)
								{
									callable();
									next.Set();
								}
								else
								{
									next.Set(callable());
								}
							}
						}
						catch (...)
						{
							next.SetException(std::current_exception());
						}

					}, option);
			});

		return Future<result_type>(std::move(next));
	}

	template <typename T>
	template <typename Callable>
	auto Future<T>::ContinuationWith(Callable&& callable, Executor& executor, const ExecuteOption& option)
	{
		using traits = detail::CallableTraits<Callable>;
		using result_type = typename traits::result_type;
		//constexpr size_t arity = traits::arity;

		ExecutionContext<result_type> next(std::make_shared<detail::ExecutionContextImpl<result_type>>());

		_context.AddContinuation([prev = _context, next, callable = detail::Wrap(std::forward<Callable>(callable)),
			weak = executor.weak_from_this(), option = option]() mutable
			{
				auto executor = weak.lock();
				if (!executor)
				{
					next.Cancel();
					return;
				}

				executor->Post([prev = std::move(prev), next = std::move(next), callable]() mutable
					{
						try
						{
							if constexpr (std::is_invocable_v<Callable, Future<T>&>)
							{
								Future<T> temp(prev);

								if constexpr (std::is_same_v<result_type, void>)
								{
									callable(temp);
									next.Set();
								}
								else
								{
									next.Set(callable(temp));
								}
							}
							else
							{
								static_assert(!sizeof(T), "invalid usage");
							}
						}
						catch (...)
						{
							next.SetException(std::current_exception());
						}

					}, option);
			});

		return Future<result_type>(std::move(next));
	}

	template <typename T>
	bool Future<T>::IsPending() const
	{
		return _context.IsPending();
	}

	template <typename T>
	bool Future<T>::IsDone() const
	{
		return _context.IsDone();
	}

	template <typename T>
	bool Future<T>::IsCanceled() const
	{
		return _context.IsCanceled();
	}

	template <typename T>
	auto Future<T>::Status() -> FutureStatus
	{
		return _context.Status();
	}
}
