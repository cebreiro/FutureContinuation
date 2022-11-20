#pragma once

namespace cebreiro::detail
{
	template <typename T>
	void ExecutionContextImpl<T>::Set(T&& value)
	{
		std::vector<std::function<void()>> continuations;
		{
			std::lock_guard lock(_mutex);
			_value.emplace(std::move(value));

			if (_status == FutureStatus::Pending)
			{
				_status = FutureStatus::Done;
				_continuations.swap(continuations);
			}
		}

		_condVar.notify_all();

		for (const std::function<void()>& continuation : continuations)
		{
			continuation();
		}
	}

	template <typename T>
	void ExecutionContextImpl<T>::Set(const T& value)
	{
		std::vector<std::function<void()>> continuations;
		{
			std::lock_guard lock(_mutex);
			_value.emplace(value);

			if (_status == FutureStatus::Pending)
			{
				_status = FutureStatus::Done;
				_continuations.swap(continuations);
			}
		}

		_condVar.notify_all();

		for (const std::function<void()>& continuation : continuations)
		{
			continuation();
		}
	}

	template <typename T>
	auto ExecutionContextImpl<T>::Get() -> decltype(auto)
	{
		std::unique_lock lock(_mutex);

		if (_status == FutureStatus::Pending)
		{
			_condVar.wait(lock, [this]()
				{
					return _status != FutureStatus::Pending;
				});
		}

		if (_status == FutureStatus::Canceled)
		{
			throw std::runtime_error("execution is canceled");
		}

		if (!_exceptions.empty())
		{
			std::exception_ptr exception;
			swap(_exceptions[0], exception);

			std::rethrow_exception(exception);
		}

		return std::move(_value.value());
	}
}