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
    new (ptr) typename std::decay<T>::type(std::move(x));
}
} // anonymous namespace


//------------------------------------------------------------------------------

namespace v2
{
//------------------------------------------------------------------------------
// value class type-dependent operations
//------------------------------------------------------------------------------
// construction

template <typename T>
struct value::copy_construction
{
    static void invoke(value& self, const value& rhs)
    {
        copy_construct_at(self.m_data.as<T>(), *rhs.m_data.as<T>());
        self.m_type = type_traits<T>::type_flag;
    }
};

template <typename T>
struct value::move_construction
{
    static void invoke(value& self, value&& rhs)
    {
        move_construct_at(self.m_data.as<T>(), std::move(*rhs.m_data.as<T>()));
        self.m_type = type_traits<T>::type_flag;
        detail::invoke_flagged<destruction>(rhs.m_type, rhs);
    }
};

//------------------------------------------------------------------------------
// outer assignment

template <typename T>
struct value::copy_assign_outer
{
    static void invoke(value& self, const value& rhs)
    {
        self.assign_inner(*rhs.m_data.as<T>());
    }
};

template <typename T>
struct value::move_assign_outer
{
    static void invoke(value& self, value&& rhs)
    {
        self.assign_inner(std::move(*rhs.m_data.as<T>()));
        detail::invoke_flagged<destruction>(rhs.m_type, rhs);
    }
};


//------------------------------------------------------------------------------
// inner assignment

template <typename T>
void value::assign_inner(T&& x)
{
    using rhs_type = typename std::decay<T>::type;
    const auto new_flag = type_traits<rhs_type>::type_flag;
    if (m_type == new_flag)
    {
        *m_data.as<rhs_type>() = std::forward<T>(x);
    }
    else
    {
        destruct(*this);
        construct_at(this, std::forward<T>(x));
        m_type = new_flag;
    }
}


//------------------------------------------------------------------------------
// destruction

template <typename T>
struct value::destruction
{
    static void invoke(value& self)
    {
        destruct(*self.m_data.as<T>());
        self.m_type = detail::Invalid;
    }
};


//------------------------------------------------------------------------------
// equality

template <typename T>
struct value::equality
{
    static void invoke(const value& self, const value& rhs, bool& res)
    {
        res = *self.m_data.as<T>() == *rhs.m_data.as<T>();
    }
};


//------------------------------------------------------------------------------
// value class implementation
//------------------------------------------------------------------------------
// outer constructors

value::value(const value& rhs) : m_type(detail::Invalid)
{
    detail::invoke_flagged<copy_construction>(rhs.m_type, *this, rhs);
}

value::value(value&& rhs) : m_type(detail::Invalid)
{
    detail::invoke_flagged<move_construction>(rhs.m_type, *this,
                                              std::move(rhs));
}


//------------------------------------------------------------------------------
// conversion constructors
//
// Note: type flag *must* be set after construction, in case construction throws

value::value(const bool_type x) : value()
{
    copy_construct_at(m_data.as<bool_type>(), x);
    m_type = detail::Bool;
}

value::value(const int_type x) : value()
{
    copy_construct_at(m_data.as<int_type>(), x);
    m_type = detail::Int;
}

value::value(const float_type x) : value()
{
    copy_construct_at(m_data.as<float_type>(), x);
    m_type = detail::Float;
}

value::value(string_type s) : value()
{
    move_construct_at(m_data.as<string_type>(), s);
    m_type = detail::String;
}

value::value(array_type s) : value()
{
    move_construct_at(m_data.as<array_type>(), s);
    m_type = detail::Array;
}

value::value(map_type s) : value()
{
    move_construct_at(m_data.as<map_type>(), s);
    m_type = detail::Map;
}


//------------------------------------------------------------------------------
// outer assignment

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
        detail::invoke_flagged<move_assign_outer>(rhs.m_type, *this,
                                                  std::move(rhs));
    }
    return *this;
}


//------------------------------------------------------------------------------
// conversion assignment

value& value::operator=(const bool_type x)
{
    assign_inner(x);
    return *this;
}

value& value::operator=(const int_type x)
{
    assign_inner(x);
    return *this;
}

value& value::operator=(const float_type x)
{
    assign_inner(x);
    return *this;
}

value& value::operator=(string_type x)
{
    assign_inner(x);
    return *this;
}

value& value::operator=(array_type x)
{
    assign_inner(x);
    return *this;
}

value& value::operator=(map_type x)
{
    assign_inner(x);
    return *this;
}


//------------------------------------------------------------------------------
// destructor

value::~value()
{
    detail::invoke_flagged<destruction>(m_type, *this);
}


//------------------------------------------------------------------------------
// equality

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
