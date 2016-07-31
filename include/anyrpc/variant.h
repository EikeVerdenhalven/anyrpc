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
class value
{
public:
    enum TypeFlag
    {
        Invalid,
        Bool,
        Int,
        Float,
        String,
        Array,
        Map
    };


    using int_type = std::int64_t;
    using map_type = std::unordered_map<std::string, value>;
    using array_type = std::vector<value>;

    value() : m_type(Invalid)
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
        return m_type != Invalid;
    }

    template <class T>
    const T& get() const;

private:
    template <TypeFlag Flag>
    struct flagged;

    template <TypeFlag Flag>
    struct flag_traits;

    template <TypeFlag Flag>
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

    template <TypeFlag>
    struct copy_construction;

    template <TypeFlag>
    struct move_construction;

    template <TypeFlag>
    struct copy_assign_outer;

    template <TypeFlag>
    struct move_assign_outer;

    template <typename T>
    void copy_assign_inner(const T& x);

    template <typename T>
    void move_assign_inner(T&& x);

    template <TypeFlag>
    struct destruction;

    template <TypeFlag>
    struct equality;

    TypeFlag m_type;
    stack_data<sizeof(data_t)> m_data;
};

template <>
struct value::flag_traits<value::Bool>
{
    using value_type = bool;
};
template <>
struct value::flag_traits<value::Int>
{
    using value_type = value::int_type;
};
template <>
struct value::flag_traits<value::Float>
{
    using value_type = double;
};
template <>
struct value::flag_traits<value::String>
{
    using value_type = std::string;
};
template <>
struct value::flag_traits<value::Array>
{
    using value_type = value::array_type;
};
template <>
struct value::flag_traits<value::Map>
{
    using value_type = value::map_type;
};


// clang-format off

template <value::TypeFlag Flag> struct value::flagged{ enum { type_flag = Flag }; };

template <> struct value::type_traits<bool> : flagged<value::Bool> {};
template <> struct value::type_traits<value::int_type> : flagged<value::Int> {};
template <> struct value::type_traits<double> : flagged<value::Float> {};
template <> struct value::type_traits<std::string> : flagged<value::String> {};
template <> struct value::type_traits<value::array_type> : flagged<value::Array> {};
template <> struct value::type_traits<value::map_type> : flagged<value::Map> {};

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