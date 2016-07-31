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

template <template <v2::value::TypeFlag> class Functor, class V, class... Args>
void do_invoke_flagged(V&& self, const v2::value::TypeFlag flag, Args&&... args)
{
    using v = v2::value;
    switch (flag)
    {
    case v::Bool:
        Functor<v::Bool>{self}(std::forward<Args>(args)...);
        break;
    case v::Int:
        Functor<v::Int>{self}(std::forward<Args>(args)...);
        break;
    case v::Float:
        Functor<v::Float>{self}(std::forward<Args>(args)...);
        break;
    case v::String:
        Functor<v::String>{self}(std::forward<Args>(args)...);
        break;
    case v::Array:
        Functor<v::Array>{self}(std::forward<Args>(args)...);
        break;
    case v::Map:
        Functor<v::Map>{self}(std::forward<Args>(args)...);
        break;
    default:
        break;
    }
}

template <template <v2::value::TypeFlag> class Functor, class... Args>
void invoke_flagged(const v2::value& self, const v2::value::TypeFlag flag, Args&&... args)
{
    do_invoke_flagged<Functor, const v2::value&, Args...>(self, flag, std::forward<Args>(args)...);
}
template <template <v2::value::TypeFlag> class Functor, class... Args>
void invoke_flagged(v2::value& self, const v2::value::TypeFlag flag, Args&&... args)
{
    do_invoke_flagged<Functor, v2::value&, Args...>(self, flag, std::forward<Args>(args)...);
}


} // anonymous namespace

namespace v2
{

template <value::TypeFlag Flag>
struct value::copy_construction : public flag_functor
{
    using flag_functor::flag_functor;
    void operator()(const value& rhs) const
    {
        using flag_type = typename flag_traits<Flag>::value_type;
        copy_construct_at(m_this.m_data.as<flag_type>(), rhs.get<flag_type>());
    }
};

template <value::TypeFlag Flag>
struct value::move_construction : public flag_functor
{
    using flag_functor::flag_functor;
    void operator()(value&& rhs) const
    {
        using flag_type = typename flag_traits<Flag>::value_type;
        move_construct_at(m_this.m_data.as<flag_type>(), std::move(*rhs.m_data.as<flag_type>()));
        rhs.m_type = Invalid;
    }
};

template <value::TypeFlag Flag>
struct value::move_assign : public flag_functor
{
    using flag_functor::flag_functor;
    void operator()(value&& rhs) const
    {
        using flag_type = typename flag_traits<Flag>::value_type;
        *m_this.m_data.as<flag_type>() = std::move(*rhs.m_data.as<flag_type>());
    }
};
template <value::TypeFlag Flag>
struct value::destruction : public flag_functor
{
    using flag_functor::flag_functor;
    void operator()() const
    {
        destruct(*m_this.m_data.as<typename flag_traits<Flag>::value_type>());
    }
};

template <value::TypeFlag Flag>
struct value::equality
{
    equality(const value& this_ref) : m_this(this_ref)
    {
    }
    void operator()(const value& rhs, bool& result) const
    {
        using flag_type = typename flag_traits<Flag>::value_type;
        result = (m_this.get<flag_type>() == rhs.get<flag_type>());
    }
    const value& m_this;
};


value::value(const value& rhs) : m_type(rhs.m_type)
{
    if(m_type != Invalid)
    {
        invoke_flagged<copy_construction>(*this, m_type, rhs);
    }
}

value::value(value&& rhs) : m_type(rhs.m_type)
{
    if(m_type != Invalid)
    {
        invoke_flagged<move_construction>(*this, m_type, std::move(rhs));
    }
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
        if(m_type != Invalid)
        {
            invoke_flagged<move_assign>(*this, m_type, std::move(rhs));
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

value::value(const std::string& s) : m_type(String)
{
    copy_construct_at(m_data.as<std::string>(), s);
}

value::value(const array_type& s) : m_type(Array)
{
    copy_construct_at(m_data.as<array_type>(), s);
}

value::value(const map_type& s) : m_type(Map)
{
    copy_construct_at(m_data.as<map_type>(), s);
}

value::~value()
{
    if(m_type != Invalid)
    {
        invoke_flagged<destruction>(*this, m_type);
        m_type = Invalid;
    }
}

bool value::operator==(const value& rhs) const
{
    if (m_type != rhs.m_type)
    {
        return false;
    }
    bool res = false;
    invoke_flagged<equality>(*this, m_type, rhs, res);
    return res;
}
} // namespace v2
} // namespace anyrpc