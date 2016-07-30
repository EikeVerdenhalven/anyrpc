#pragma once


#include <string>
#include <unordered_map>
#include <vector>

namespace anyrpc
{

template <class Type>
class uninitialized
{
public:
    using value_type = typename std::decay<Type>::type;

    uninitialized()
    {
        std::fill_n(m_data, sizeof(value_type), 0);
    }

    uninitialized(const uninitialized&) = delete;

    value_type* get()
    {
        return reinterpret_cast<value_type*>(&m_data);
    }
    const value_type* get() const
    {
        return reinterpret_cast<const value_type*>(&m_data);
    }

private:
    unsigned char m_data[sizeof(value_type)];
};


namespace v2
{
class value
{
public:
    
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
    
    bool operator== (const value&) const;

    explicit value(const bool rhs);
    explicit value(const int_type x);
    explicit value(const double x);
    explicit value(const std::string& s);
    explicit value(const array_type& s);
    explicit value(const map_type& s);

    bool is_valid() const { return m_type != Invalid; }
    
    template <class T>
    const T& get() const;

private:
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

    using data_t = union {
        bool m_bool;
        int_type m_integer;
        double m_double;
        std::string m_string;
        array_type m_array;
        map_type m_map;
    };

    TypeFlag m_type;
    uninitialized<data_t> m_data;
};

template <>
inline const bool& value::get<bool>() const
{
    if (m_type == Bool)
    {
        return m_data.get()->m_bool;
    }
    throw std::runtime_error("value::get not a bool");
}

template <>
inline const std::int64_t& value::get<value::int_type>() const
{
    if (m_type == Int)
    {
        return m_data.get()->m_integer;
    }
    throw std::runtime_error("value::get not a integer");
}

template <>
inline const double& value::get<double>() const
{
    if (m_type == Float)
    {
        return m_data.get()->m_double;
    }
    throw std::runtime_error("value::get not a float");
}

template <>
inline const std::string& value::get<std::string>() const
{
    if (m_type == String)
    {
        return m_data.get()->m_string;
    }
    throw std::runtime_error("value::get not a string");
}

template <>
inline const value::array_type& value::get<value::array_type>() const
{
    if (m_type == Array)
    {
        return m_data.get()->m_array;
    }
    throw std::runtime_error("value::get not an array");
}

template <>
inline const value::map_type& value::get<value::map_type>() const
{
    if (m_type == Map)
    {
        return m_data.get()->m_map;
    }
    throw std::runtime_error("value::get not a map");
}
} // namespace v2
} // namespace anyrpc