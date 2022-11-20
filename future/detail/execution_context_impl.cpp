#include "execution_context_impl.h"

#include <cassert>
#include <iostream>

namespace cebreiro::detail
{
	ExecutionContextImplBase::ExecutionContextImplBase()
	{
	}

	ExecutionContextImplBase::~ExecutionContextImplBase()
	{
		if (_status != FutureStatus::Canceled && !_exceptions.empty() && _exceptions[0])
		{
			try
			{
				std::rethrow_exception(_exceptions[0]);
			}
			catch (const std::exception& e)
			{
				std::cout << e.what() << std::endl;
			}

			// unhandled exception
			// DO NOT fire and forget
			assert(false);
		}
	}

	void ExecutionContextImplBase::Cancel()
	{
		std::vector<std::function<void()>> continuations;
		{
			std::lock_guard lock(_mutex);
			_status = FutureStatus::Canceled;

			_continuations.swap(continuations);
		}

		_condVar.notify_all();

		for (const std::function<void()>& continuation : continuations)
		{
			continuation();
		}
	}

	auto ExecutionContextImplBase::Wait() -> FutureStatus
	{
		std::unique_lock lock(_mutex);
		_condVar.wait(lock, [this]()
			{
				return _status != FutureStatus::Pending;
			});

		return _status;
	}

	auto ExecutionContextImplBase::WaitFor(std::chrono::milliseconds milliseconds) -> FutureStatus
	{
		std::unique_lock lock(_mutex);
		_condVar.wait_for(lock, milliseconds, [this]()
			{
				return _status != FutureStatus::Pending;
			});

		return _status;
	}

	bool ExecutionContextImplBase::IsPending() const
	{
		std::lock_guard lock(_mutex);
		return Unsafe_IsPending();
	}

	bool ExecutionContextImplBase::IsDone() const
	{
		std::lock_guard lock(_mutex);
		return Unsafe_IsDone();
	}

	bool ExecutionContextImplBase::IsCanceled() const
	{
		std::lock_guard lock(_mutex);
		return Unsafe_IsCanceled();
	}

	auto ExecutionContextImplBase::Status() -> FutureStatus
	{
		std::lock_guard lock(_mutex);
		return _status;
	}

	void ExecutionContextImplBase::AddContinuation(const std::function<void()>& continuation)
	{
		{
			std::lock_guard lock(_mutex);

			if (Unsafe_IsPending())
			{
				_continuations.push_back(continuation);
				return;
			}
		}

		continuation();
	}

	void ExecutionContextImplBase::SetException(const std::exception_ptr& exception)
	{
		std::vector<std::function<void()>> continuations;
		{
			std::lock_guard lock(_mutex);

			_exceptions.push_back(exception);

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

	bool ExecutionContextImplBase::Unsafe_IsPending() const
	{
		return _status == FutureStatus::Pending;
	}

	bool ExecutionContextImplBase::Unsafe_IsDone() const
	{
		return _status == FutureStatus::Done;
	}

	bool ExecutionContextImplBase::Unsafe_IsCanceled() const
	{
		return _status == FutureStatus::Canceled;
	}

	void ExecutionContextImpl<void>::Set()
	{
		std::vector<std::function<void()>> continuations;
		{
			std::lock_guard lock(_mutex);

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

	void ExecutionContextImpl<void>::Get()
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
	}
}
