#include <anyrpc/variant.h>

namespace anyrpc
{
namespace
{
template <class T>
void destruct(T& x)
{
    (&x)->~T();
}

template <class T>
void copy_construct_at(typename std::decay<T>::type* ptr, const T& x)
{
    new (ptr) typename std::decay<T>::type(x);
}

template <class T>
void move_construct_at(typename std::decay<T>::type* ptr, T&& x)
{
    new (ptr) typename std::decay<T>::type(x);
}
} // anonymous namespace

namespace v2
{

value::value(const value& rhs) : m_type(rhs.m_type)
{
    switch (m_type)
    {
    case Bool:
        copy_construct_at(&m_data.get()->m_bool, rhs.m_data.get()->m_bool);
        break;
    case Int:
        copy_construct_at(&m_data.get()->m_integer, rhs.m_data.get()->m_integer);
        break;
    case Float:
        copy_construct_at(&m_data.get()->m_double, rhs.m_data.get()->m_double);
        break;
    case String:
        copy_construct_at(&m_data.get()->m_string, rhs.m_data.get()->m_string);
        break;
    case Array:
        copy_construct_at(&m_data.get()->m_array, rhs.m_data.get()->m_array);
        break;
    case Map:
        copy_construct_at(&m_data.get()->m_map, rhs.m_data.get()->m_map);
        break;
    default:
        break;
    }
}

value::value(value&& rhs) : m_type(rhs.m_type)
{
    switch (m_type)
    {
    case Bool:
        move_construct_at(&m_data.get()->m_bool, std::move(rhs.m_data.get()->m_bool));
        break;
    case Int:
        move_construct_at(&m_data.get()->m_integer, std::move(rhs.m_data.get()->m_integer));
        break;
    case Float:
        move_construct_at(&m_data.get()->m_double, std::move(rhs.m_data.get()->m_double));
        break;
    case String:
        move_construct_at(&m_data.get()->m_string, std::move(rhs.m_data.get()->m_string));
        break;
    case Array:
        move_construct_at(&m_data.get()->m_array, std::move(rhs.m_data.get()->m_array));
        break;
    case Map:
        move_construct_at(&m_data.get()->m_map, std::move(rhs.m_data.get()->m_map));
        break;
    default:
        break;
    }
    rhs.m_type = Invalid;
}

value& value::operator=(const value& rhs)
{
    this->~value();
    new (this) value(rhs);
    return *this;
}

value& value::operator=(value&& rhs)
{
    if (m_type == rhs.m_type)
    {
        switch (m_type)
        {
        case Bool:
            m_data.get()->m_bool = std::move(rhs.m_data.get()->m_bool);
            break;
        case Int:
            m_data.get()->m_integer = std::move(rhs.m_data.get()->m_integer);
            break;
        case Float:
            m_data.get()->m_double = std::move(rhs.m_data.get()->m_double);
            break;
        case String:
            m_data.get()->m_string = std::move(rhs.m_data.get()->m_string);
            break;
        case Array:
            m_data.get()->m_array = std::move(rhs.m_data.get()->m_array);
            break;
        case Map:
            m_data.get()->m_map = std::move(rhs.m_data.get()->m_map);
            break;
        default:
            break;
        }
    }
    else
    {
        this->~value();
        new (this) value(rhs);
    }
    return *this;
}

value::value(const bool x) : m_type(Bool)
{
    copy_construct_at(&m_data.get()->m_bool, x);
}

value::value(const int_type x) : m_type(Int)
{
    copy_construct_at(&m_data.get()->m_integer, x);
}

value::value(const double x) : m_type(Float)
{
    copy_construct_at(&m_data.get()->m_double, x);
}

value::value(const std::string& s) : m_type(String)
{
    copy_construct_at(&m_data.get()->m_string, s);
}

value::value(const array_type& s) : m_type(Array)
{
    copy_construct_at(&m_data.get()->m_array, s);
}

value::value(const map_type& s) : m_type(Map)
{
    copy_construct_at(&m_data.get()->m_map, s);
}

value::~value()
{
    auto* d = m_data.get();
    switch (m_type)
    {
    case Bool:
        destruct(d->m_bool);
        break;
    case Int:
        destruct(d->m_integer);
        break;
    case Float:
        destruct(d->m_double);
        break;
    case String:
        destruct(d->m_string);
        break;
    case Array:
        destruct(d->m_array);
        break;
    case Map:
        destruct(d->m_map);
        break;
    default:
        break;
    }
    m_type = Invalid;
}

bool value::operator==(const value& rhs) const
{
    if (m_type != rhs.m_type)
    {
        return false;
    }
    switch (m_type)
    {
    case Bool:
        return get<bool>() == rhs.get<bool>();
    case Int:
        return get<int_type>() == rhs.get<int_type>();
    case Float:
        return get<double>() == get<double>();
    case String:
        return get<std::string>() == get<std::string>();
    case Array:
        return get<array_type>() == get<array_type>();
    case Map:
        return get<map_type>() == get<map_type>();
    case Invalid:
        return true;
    default:
        throw std::runtime_error("Undefined type flag");
    }
}
} // namespace v2
} // namespace anyrpc