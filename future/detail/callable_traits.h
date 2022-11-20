#pragma once
#include <functional>
#include <type_traits>
#include <tuple>
#include <utility>

namespace cebreiro::detail
{
	template <typename T>
	struct MemberFunctionTraits;

	template <typename Result, typename Class, typename... Args>
	struct MemberFunctionTraits<Result(Class::*)(Args...)>
	{
		using result_type = Result;
		using class_type = Class;

		template <size_t N>
		using arg_type = std::tuple_element_t<N, std::tuple<Args...>>;

		static constexpr size_t arity = sizeof...(Args);
	};

	template <typename Result, typename Class, typename... Args>
	struct MemberFunctionTraits<Result(Class::*)(Args...) const>
	{
		using result_type = Result;
		using class_type = Class;

		template <size_t N>
		using arg_type = std::tuple_element_t<N, std::tuple<Args...>>;

		static constexpr size_t arity = sizeof...(Args);
	};

	template <typename T>
	struct MemberFunctionTraits : public MemberFunctionTraits<decltype(&T::operator())> {};


	template <typename T>
	struct FunctionTraits;

	template <typename Result, typename... Args>
	struct FunctionTraits<Result(*)(Args...)>
	{
		using result_type = Result;

		template <size_t N>
		using arg_type = std::tuple_element_t<N, std::tuple<Args...>>;

		static constexpr size_t arity = sizeof...(Args);
	};


	template <typename T>
	struct UnsafeBindTraits;

	template <typename Rx, typename Fx, typename... Ts>
	struct UnsafeBindTraits<std::_Binder<Rx, Fx, Ts...>> : public MemberFunctionTraits<Fx> {};


	template <typename T, bool = std::is_bind_expression_v<T>, typename = void>
	struct CallableTraits : FunctionTraits<std::decay_t<T>> {};

	template <typename T>
	struct CallableTraits<T, true, void> : UnsafeBindTraits<std::decay_t<T>> {};

	template <typename T>
	struct CallableTraits<T, false, std::void_t<decltype(&std::decay_t<T>::operator())>> : MemberFunctionTraits<std::decay_t<T>> {};
}