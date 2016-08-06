#pragma once


#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace anyrpc
{

template <std::size_t Size>
class stack_data
{
public:
    stack_data() {}
    stack_data(const stack_data&) = delete;

    template <class T>
    T* as()
    {
        static_assert(sizeof(T) <= Size,
                      "Requested type does not fit into struct");
        return reinterpret_cast<T*>(&m_data);
    }

    template <class T>
    const T* as() const
    {
        static_assert(sizeof(T) <= Size,
                      "Requested type does not fit into struct");
        return reinterpret_cast<const T*>(&m_data);
    }

private:
    using data_type = typename std::aligned_storage<Size>::type;
    data_type m_data;
};


namespace v2
{

namespace detail
{
using type_flag_t = enum { Invalid, Bool, Int, Float, String, Array, Map };

template <template <typename> class Functor, class... Args>
void invoke_flagged(const v2::detail::type_flag_t flag, Args&&... args);

} // namespace detail

class value
{
public:
    using bool_type = bool;
    using string_type = std::string;
    using float_type = double;
    using int_type = std::int64_t;
    using map_type = std::unordered_map<std::string, value>;
    using array_type = std::vector<value>;


    value() : m_type(detail::Invalid) {}

    value(const value&);
    value(value&&);

    explicit value(bool_type);
    explicit value(int_type);
    explicit value(float_type);
    explicit value(string_type);
    explicit value(array_type);
    explicit value(map_type);

    ~value();


    value& operator=(const value&);
    value& operator=(value&&);


    value& operator=(bool_type);
    value& operator=(int_type);
    value& operator=(float_type);
    value& operator=(string_type);
    value& operator=(array_type);
    value& operator=(map_type);

    
    bool operator==(const value&) const;
    bool operator!=(const value& rhs) const { return !(*this == rhs); }


    bool is_valid() const { return m_type != detail::Invalid; }


    template <typename T>
    struct visitation;

    template <class Visitor>
    void visit(Visitor& visitor) const
    {
        detail::invoke_flagged<visitation>(m_type, *this, visitor);
    }


    template <class T>
    const T& get() const
    {
        if (m_type == value::type_traits<T>::type_flag)
        {
            return *m_data.as<T>();
        }
        throw std::runtime_error("value::get with wrong type");
    }


private:
    template <detail::type_flag_t Flag>
    struct flagged;

    template <typename Type>
    struct type_traits;

    template <typename T>
    struct copy_construction;

    template <typename T>
    struct move_construction;

    template <typename T>
    struct copy_assign_outer;

    template <typename T>
    struct move_assign_outer;

    template <typename T>
    void assign_inner(T&& x);

    template <typename T>
    struct destruction;

    template <typename T>
    struct equality;

    using data_t = union {
        bool_type m_bool;
        int_type m_integer;
        float_type m_double;
        string_type m_string;
        array_type m_array;
        map_type m_map;
    };
    
    detail::type_flag_t m_type;
    stack_data<sizeof(data_t)> m_data;
};


// clang-format off
//
template <detail::type_flag_t Flag>
struct value::flagged
{
    static constexpr detail::type_flag_t type_flag = Flag;
};

template <> struct value::type_traits<value::bool_type>   : flagged<detail::Bool>   {};
template <> struct value::type_traits<value::int_type>    : flagged<detail::Int>    {};
template <> struct value::type_traits<value::float_type>  : flagged<detail::Float>  {};
template <> struct value::type_traits<value::string_type> : flagged<detail::String> {};
template <> struct value::type_traits<value::array_type>  : flagged<detail::Array>  {};
template <> struct value::type_traits<value::map_type>    : flagged<detail::Map>    {};

// clang-format on


template <typename T>
struct value::visitation
{
    template <class Visitor>
    static void invoke(const value& rSelf, Visitor& visitor)
    {
        visitor(*rSelf.m_data.as<T>());
    }
};

    
//------------------------------------------------------------------------------
// type-dependent run-time dispatch

namespace detail
{
template <template <typename> class Functor, class... Args>
void invoke_flagged(const v2::detail::type_flag_t flag, Args&&... args)
{
    switch (flag)
    {
    case Bool:
        Functor<value::bool_type>::invoke(std::forward<Args>(args)...);
        break;
    case Int:
        Functor<value::int_type>::invoke(std::forward<Args>(args)...);
        break;
    case Float:
        Functor<value::float_type>::invoke(std::forward<Args>(args)...);
        break;
    case String:
        Functor<value::string_type>::invoke(std::forward<Args>(args)...);
        break;
    case Array:
        Functor<value::array_type>::invoke(std::forward<Args>(args)...);
        break;
    case Map:
        Functor<value::map_type>::invoke(std::forward<Args>(args)...);
        break;
    default:
        break;
    }
}
} // namespace detail
} // namespace v2
} // namespace anyrpc
