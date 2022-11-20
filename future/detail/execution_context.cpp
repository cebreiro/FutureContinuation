#include "execution_context.h"

cebreiro::ExecutionContext<void>::ExecutionContext(std::shared_ptr<detail::ExecutionContextImpl<void>> impl)
	: _impl(std::move(impl))
{
}

cebreiro::ExecutionContext<void>::~ExecutionContext()
{
}

void cebreiro::ExecutionContext<void>::Cancel()
{
	_impl->Cancel();
}

void cebreiro::ExecutionContext<void>::Set()
{
	_impl->Set();
}

void cebreiro::ExecutionContext<void>::AddContinuation(const std::function<void()>& continuation)
{
	_impl->AddContinuation(continuation);
}

void cebreiro::ExecutionContext<void>::SetException(const std::exception_ptr& exception)
{
	_impl->SetException(exception);
}

void cebreiro::ExecutionContext<void>::Get()
{
	return _impl->Get();
}

auto cebreiro::ExecutionContext<void>::Wait() -> FutureStatus
{
	return _impl->Wait();
}

auto cebreiro::ExecutionContext<void>::WaitFor(std::chrono::milliseconds milliseconds) -> FutureStatus
{
	return _impl->WaitFor(milliseconds);
}

bool cebreiro::ExecutionContext<void>::IsPending() const
{
	return _impl->IsPending();
}

bool cebreiro::ExecutionContext<void>::IsDone() const
{
	return _impl->IsDone();
}

bool cebreiro::ExecutionContext<void>::IsCanceled() const
{
	return _impl->IsCanceled();
}

auto cebreiro::ExecutionContext<void>::Status() -> FutureStatus
{
	return _impl->Status();
}
