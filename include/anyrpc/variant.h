#pragma once


#include <string>
#include <unordered_map>
#include <vector>


namespace anyrpc
{

template <std::size_t Size>
class stack_data
{
public:
    stack_data()
    {
        std::fill_n(m_data, Size, 0);
    }

    stack_data(const stack_data&) = delete;

    std::size_t size() const
    {
        return Size;
    }

    template <class T>
    T* as()
    {
        return reinterpret_cast<T*>(&m_data);
    }

    template <class T>
    const T* as() const
    {
        return reinterpret_cast<const T*>(&m_data);
    }

private:
    unsigned char m_data[Size];
};


namespace v2
{

namespace detail
{
using type_flag_t = enum { Invalid, Bool, Int, Float, String, Array, Map };

template <template <v2::detail::type_flag_t> class Functor, class... Args>
void invoke_flagged(const v2::detail::type_flag_t flag, Args&&... args)
{
    switch (flag)
    {
    case v2::detail::Bool:
        Functor<v2::detail::Bool>::invoke(std::forward<Args>(args)...);
        break;
    case v2::detail::Int:
        Functor<v2::detail::Int>::invoke(std::forward<Args>(args)...);
        break;
    case v2::detail::Float:
        Functor<v2::detail::Float>::invoke(std::forward<Args>(args)...);
        break;
    case v2::detail::String:
        Functor<v2::detail::String>::invoke(std::forward<Args>(args)...);
        break;
    case v2::detail::Array:
        Functor<v2::detail::Array>::invoke(std::forward<Args>(args)...);
        break;
    case v2::detail::Map:
        Functor<v2::detail::Map>::invoke(std::forward<Args>(args)...);
        break;
    default:
        break;
    }
}
} // namespace detail

class value
{
public:
    using int_type = std::int64_t;
    using map_type = std::unordered_map<std::string, value>;
    using array_type = std::vector<value>;

    value() : m_type(detail::Invalid)
    {
    }
    value(const value&);
    value(value&&);

    value& operator=(const value&);
    value& operator=(value&&);

    ~value();

    bool operator==(const value&) const;
    bool operator!=(const value& rhs) const
    {
        return !(*this == rhs);
    }

    explicit value(bool);
    explicit value(int_type);
    explicit value(double);
    explicit value(std::string);
    explicit value(array_type);
    explicit value(map_type);


    value& operator=(bool);
    value& operator=(int_type);
    value& operator=(double);
    value& operator=(std::string);
    value& operator=(array_type);
    value& operator=(map_type);


    bool is_valid() const
    {
        return m_type != detail::Invalid;
    }

    template <class T>
    const T& get() const;

private:
    template <detail::type_flag_t Flag>
    struct flagged;

    template <detail::type_flag_t Flag>
    struct flag_traits;

    template <detail::type_flag_t Flag>
    using flag_t = typename flag_traits<Flag>::value_type;

    template <typename Type>
    struct type_traits;

    using data_t = union {
        bool m_bool;
        int_type m_integer;
        double m_double;
        std::string m_string;
        array_type m_array;
        map_type m_map;
    };

    template <detail::type_flag_t>
    struct copy_construction;

    template <detail::type_flag_t>
    struct move_construction;

    template <detail::type_flag_t>
    struct copy_assign_outer;

    template <detail::type_flag_t>
    struct move_assign_outer;

    template <typename T>
    void copy_assign_inner(const T& x);

    template <typename T>
    void move_assign_inner(T&& x);

    template <detail::type_flag_t>
    struct destruction;

    template <detail::type_flag_t>
    struct equality;

    detail::type_flag_t m_type;
    stack_data<sizeof(data_t)> m_data;
};

template <>
struct value::flag_traits<detail::Bool>
{
    using value_type = bool;
};
template <>
struct value::flag_traits<detail::Int>
{
    using value_type = value::int_type;
};
template <>
struct value::flag_traits<detail::Float>
{
    using value_type = double;
};
template <>
struct value::flag_traits<detail::String>
{
    using value_type = std::string;
};
template <>
struct value::flag_traits<detail::Array>
{
    using value_type = value::array_type;
};
template <>
struct value::flag_traits<detail::Map>
{
    using value_type = value::map_type;
};


// clang-format off

template <detail::type_flag_t Flag> struct value::flagged{ enum { type_flag = Flag }; };

template <> struct value::type_traits<bool> : flagged<detail::Bool> {};
template <> struct value::type_traits<value::int_type> : flagged<detail::Int> {};
template <> struct value::type_traits<double> : flagged<detail::Float> {};
template <> struct value::type_traits<std::string> : flagged<detail::String> {};
template <> struct value::type_traits<value::array_type> : flagged<detail::Array> {};
template <> struct value::type_traits<value::map_type> : flagged<detail::Map> {};

// clang-format on

template <class T>
inline const T& value::get() const
{
    if (m_type == value::type_traits<T>::type_flag)
    {
        return *m_data.as<T>();
    }
    throw std::runtime_error("value::get with wrong type");
}

} // namespace v2
} // namespace anyrpc