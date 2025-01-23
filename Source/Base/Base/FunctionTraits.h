#include <tuple>
#include <type_traits>

// Primary template (fallback for unsupported types)
template <typename T>
struct function_traits;

// Specialization for callable objects and lambdas
template <typename T>
struct function_traits : function_traits<decltype(&T::operator())> {};

// Specialization for member function pointers
template <typename Ret, typename Class, typename... Args>
struct function_traits<Ret(Class::*)(Args...) const>
{
    using args_type = std::tuple<Args...>;
    using return_type = Ret;
};

// Specialization for free functions and function pointers
template <typename Ret, typename... Args>
struct function_traits<Ret(*)(Args...)>
{
    using args_type = std::tuple<Args...>;
    using return_type = Ret;
};

template <typename Ret, typename... Args>
struct function_traits<Ret(Args...)>
{
    using args_type = std::tuple<Args...>;
    using return_type = Ret;
};

// Helper aliases for convenience
template <typename T>
using function_ret_t = typename function_traits<T>::return_type;

template <typename T>
using function_args_t = typename function_traits<T>::args_type;