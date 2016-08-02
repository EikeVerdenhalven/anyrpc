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
} // anonymous namespace

namespace v2
{

template <detail::type_flag_t Flag>
struct value::copy_construction
{
    static void invoke(value& self, const value& rhs)
    {
        copy_construct_at(self.m_data.as<flag_t<Flag>>(), rhs.get<flag_t<Flag>>());
    }
};

template <detail::type_flag_t Flag>
struct value::move_construction
{
    static void invoke(value& self, value&& rhs)
    {
        move_construct_at(self.m_data.as<flag_t<Flag>>(), std::move(*rhs.m_data.as<flag_t<Flag>>()));
        detail::invoke_flagged<destruction>(rhs.m_type, rhs);
    }
};

template <detail::type_flag_t Flag>
struct value::copy_assign_outer
{
    static void invoke(value& self, const value& rhs)
    {
        self.copy_assign_inner(rhs.get<flag_t<Flag>>());
    }
};

template <detail::type_flag_t Flag>
struct value::move_assign_outer
{
    static void invoke(value& self, value&& rhs)
    {
        self.move_assign_inner(std::move(*rhs.m_data.as<flag_t<Flag>>()));
        detail::invoke_flagged<destruction>(rhs.m_type, rhs);
    }
};


template <detail::type_flag_t Flag>
struct value::destruction
{
    static void invoke(value& self)
    {
        destruct(*self.m_data.as<flag_t<Flag>>());
        self.m_type = detail::Invalid;
    }
};

template <detail::type_flag_t Flag>
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
    detail::invoke_flagged<copy_construction>(m_type, *this, rhs);
}

value::value(value&& rhs) : m_type(rhs.m_type)
{
    detail::invoke_flagged<move_construction>(m_type, *this, std::move(rhs));
}

value& value::operator=(const value& rhs)
{
    if (this != &rhs)
    {
        detail::invoke_flagged<copy_assign_outer>(rhs.m_type, *this, rhs);
    }
    return *this;
}

value& value::operator=(value&& rhs)
{
    if (this != &rhs)
    {
        detail::invoke_flagged<move_assign_outer>(rhs.m_type, *this, std::move(rhs));
    }
    return *this;
}

value::value(const bool x) : m_type(detail::Bool)
{
    copy_construct_at(m_data.as<bool>(), x);
}

value::value(const int_type x) : m_type(detail::Int)
{
    copy_construct_at(m_data.as<int_type>(), x);
}

value::value(const double x) : m_type(detail::Float)
{
    copy_construct_at(m_data.as<double>(), x);
}

value::value(std::string s) : m_type(detail::String)
{
    move_construct_at(m_data.as<std::string>(), s);
}

value::value(array_type s) : m_type(detail::Array)
{
    move_construct_at(m_data.as<array_type>(), s);
}

value::value(map_type s) : m_type(detail::Map)
{
    move_construct_at(m_data.as<map_type>(), s);
}


value::~value()
{
    detail::invoke_flagged<destruction>(m_type, *this);
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
    else if (m_type != detail::Invalid)
    {
        bool res = false;
        detail::invoke_flagged<equality>(m_type, *this, rhs, res);
        return res;
    }
    else
    {
        return true;
    }
}

} // namespace v2
} // namespace anyrpc