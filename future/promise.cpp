#include "promise.h"

#include "future_await.h"

namespace cebreiro
{
	Promise<void>::Promise()
		: _context(std::make_shared<detail::ExecutionContextImpl<void>>())
	{
	}

	Promise<void>::~Promise()
	{
	}

	void Promise<void>::Set()
	{
		_context.Set();
	}

	void Promise<void>::SetException(const std::exception_ptr& exception)
	{
		_context.SetException(exception);
	}

	auto Promise<void>::GetFuture() -> Future<void>
	{
		return Future(_context);
	}
}
