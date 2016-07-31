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

template <class T, class... Args>
void construct_at(T* ptr, Args&&... args)
{
    new (ptr) typename std::decay<T>::type(std::forward<Args>(args)...);
}

template <class T>
void copy_construct_at(typename std::decay<T>::type* ptr, const T& x)
{
    new (ptr) typename std::decay<T>::type(x);
}

template <class T>
void move_construct_at(typename std::decay<T>::type* ptr, T&& x)
{
    new (ptr) typename std::decay<T>::type(std::forward<T>(x));
}

template <template <v2::value::TypeFlag> class Functor, class... Args>
void invoke_flagged(const v2::value::TypeFlag flag, Args&&... args)
{
    using v = v2::value;
    switch (flag)
    {
    case v::Bool:
        Functor<v::Bool>::invoke(std::forward<Args>(args)...);
        break;
    case v::Int:
        Functor<v::Int>::invoke(std::forward<Args>(args)...);
        break;
    case v::Float:
        Functor<v::Float>::invoke(std::forward<Args>(args)...);
        break;
    case v::String:
        Functor<v::String>::invoke(std::forward<Args>(args)...);
        break;
    case v::Array:
        Functor<v::Array>::invoke(std::forward<Args>(args)...);
        break;
    case v::Map:
        Functor<v::Map>::invoke(std::forward<Args>(args)...);
        break;
    default:
        break;
    }
}


} // anonymous namespace

namespace v2
{

template <value::TypeFlag Flag>
struct value::copy_construction
{
    static void invoke(value& self, const value& rhs)
    {
        copy_construct_at(self.m_data.as<flag_t<Flag>>(), rhs.get<flag_t<Flag>>());
    }
};

template <value::TypeFlag Flag>
struct value::move_construction
{
    static void invoke(value& self, value&& rhs)
    {
        move_construct_at(self.m_data.as<flag_t<Flag>>(), std::move(*rhs.m_data.as<flag_t<Flag>>()));
        invoke_flagged<destruction>(rhs.m_type, rhs);
    }
};

template <value::TypeFlag Flag>
struct value::copy_assign_outer
{
    static void invoke(value& self, const value& rhs)
    {
        self.copy_assign_inner(rhs.get<flag_t<Flag>>());
    }
};

template <value::TypeFlag Flag>
struct value::move_assign_outer
{
    static void invoke(value& self, value&& rhs)
    {
        self.move_assign_inner(std::move(*rhs.m_data.as<flag_t<Flag>>()));
        invoke_flagged<destruction>(rhs.m_type, rhs);
    }
};


template <value::TypeFlag Flag>
struct value::destruction
{
    static void invoke(value& self)
    {
        destruct(*self.m_data.as<flag_t<Flag>>());
        self.m_type = Invalid;
    }
};

template <value::TypeFlag Flag>
struct value::equality
{
    static void invoke(const value& self, const value& rhs, bool& res)
    {
        res = self.get<flag_t<Flag>>() == rhs.get<flag_t<Flag>>();
    }
};

template <typename T>
void value::copy_assign_inner(const T& x)
{
    using rhs_type = typename std::decay<T>::type;
    const auto new_flag = type_traits<rhs_type>::type_flag;
    if (m_type == new_flag)
    {
        *m_data.as<rhs_type>() = x;
    }
    else
    {
        destruct(*this);
        construct_at(this, x);
    }
}

template <typename T>
void value::move_assign_inner(T&& x)
{
    using rhs_type = typename std::decay<T>::type;
    const auto new_flag = type_traits<rhs_type>::type_flag;
    if (m_type == new_flag)
    {
        *m_data.as<rhs_type>() = std::move(x);
    }
    else
    {
        destruct(*this);
        construct_at(this, std::move(x));
    }
}


value::value(const value& rhs) : m_type(rhs.m_type)
{
    invoke_flagged<copy_construction>(m_type, *this, rhs);
}

value::value(value&& rhs) : m_type(rhs.m_type)
{
    invoke_flagged<move_construction>(m_type, *this, std::move(rhs));
}

value& value::operator=(const value& rhs)
{
    if (this != &rhs)
    {
        invoke_flagged<copy_assign_outer>(rhs.m_type, *this, rhs);
    }
    return *this;
}

value& value::operator=(value&& rhs)
{
    if (this != &rhs)
    {
        invoke_flagged<move_assign_outer>(rhs.m_type, *this, std::move(rhs));
    }
    return *this;
}

value::value(const bool x) : m_type(Bool)
{
    copy_construct_at(m_data.as<bool>(), x);
}

value::value(const int_type x) : m_type(Int)
{
    copy_construct_at(m_data.as<int_type>(), x);
}

value::value(const double x) : m_type(Float)
{
    copy_construct_at(m_data.as<double>(), x);
}

value::value(std::string s) : m_type(String)
{
    move_construct_at(m_data.as<std::string>(), s);
}

value::value(array_type s) : m_type(Array)
{
    move_construct_at(m_data.as<array_type>(), s);
}

value::value(map_type s) : m_type(Map)
{
    move_construct_at(m_data.as<map_type>(), s);
}


value::~value()
{
    invoke_flagged<destruction>(m_type, *this);
}


value& value::operator=(const bool x)
{
    copy_assign_inner(x);
    return *this;
}

value& value::operator=(const int_type x)
{
    copy_assign_inner(x);
    return *this;
}

value& value::operator=(const double x)
{
    copy_assign_inner(x);
    return *this;
}

value& value::operator=(std::string x)
{
    move_assign_inner(x);
    return *this;
}

value& value::operator=(array_type x)
{
    move_assign_inner(x);
    return *this;
}

value& value::operator=(map_type x)
{
    move_assign_inner(x);
    return *this;
}


bool value::operator==(const value& rhs) const
{
    if (m_type != rhs.m_type)
    {
        return false;
    }
    else if (m_type != Invalid)
    {
        bool res = false;
        invoke_flagged<equality>(m_type, *this, rhs, res);
        return res;
    }
    else
    {
        return true;
    }
}

} // namespace v2
} // namespace anyrpc