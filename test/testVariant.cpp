#include "anyrpc/variant.h"

#include <gtest/gtest.h>


#define TEST_CHECK_THROW 1

using namespace anyrpc;

namespace v = anyrpc::v2;

namespace testhelper
{
void verify(const v::value& sut, bool& actual)
{
    EXPECT_NO_THROW({ actual = sut.get<bool>(); });

#ifdef TEST_CHECK_THROW
    EXPECT_THROW({ sut.get<v::value::int_type>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<std::string>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<double>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<v::value::map_type>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<v::value::array_type>(); }, std::runtime_error);
#endif
}

void verify(const v::value& sut, v::value::int_type& actual)
{
    EXPECT_NO_THROW({ actual = sut.get<v::value::int_type>(); });
#ifdef TEST_CHECK_THROW
    EXPECT_THROW({ sut.get<bool>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<std::string>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<double>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<v::value::map_type>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<v::value::array_type>(); }, std::runtime_error);
#endif
}

void verify(const v::value& sut, double& actual)
{
    EXPECT_NO_THROW({ actual = sut.get<double>(); });
#ifdef TEST_CHECK_THROW
    EXPECT_THROW({ sut.get<v::value::int_type>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<std::string>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<bool>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<v::value::map_type>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<v::value::array_type>(); }, std::runtime_error);
#endif
}

void verify(const v::value& sut, std::string& actual)
{
    EXPECT_NO_THROW({ actual = sut.get<std::string>(); });
#ifdef TEST_CHECK_THROW
    EXPECT_THROW({ sut.get<v::value::int_type>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<bool>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<double>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<v::value::map_type>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<v::value::array_type>(); }, std::runtime_error);
#endif
}

void verify(const v::value& sut, v::value::array_type& actual)
{
    EXPECT_NO_THROW({ actual = sut.get<v::value::array_type>(); });
#ifdef TEST_CHECK_THROW
    EXPECT_THROW({ sut.get<v::value::int_type>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<std::string>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<double>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<v::value::map_type>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<bool>(); }, std::runtime_error);
#endif
}

void verify(const v::value& sut, v::value::map_type& actual)
{
    EXPECT_NO_THROW({ actual = sut.get<v::value::map_type>(); });
#ifdef TEST_CHECK_THROW
    EXPECT_THROW({ sut.get<v::value::int_type>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<std::string>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<double>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<v::value::array_type>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<bool>(); }, std::runtime_error);
#endif
}

template <class T>
struct values;

template <>
struct values<bool>
{
    static bool v1()
    {
        return true;
    }
    static bool v2()
    {
        return false;
    }
};

template <>
struct values<v::value::int_type>
{
    static v::value::int_type v1()
    {
        return 19239045;
    }
    static v::value::int_type v2()
    {
        return 48035432;
    }
};

template <>
struct values<std::string>
{
    static std::string v1()
    {
        return "test string number 1 sd#*$&*#sjfkasdjfasfdfgEND";
    }
    static std::string v2()
    {
        return "shorter str2";
    }
};

template <>
struct values<double>
{
    static double v1()
    {
        return 3.1414857;
    }
    static double v2()
    {
        return -284235.2454566;
    }
};

template <>
struct values<v::value::map_type>
{
    static v::value::map_type v1()
    {
        return v::value::map_type{{{"val1     END", v::value{1234612ll}},
                                   {"val2         END", v::value{"hallo weltEND"}},
                                   {"third key string stringEND", v::value{3.1456}}}};
    }
    static v::value::map_type v2()
    {
        return v::value::map_type{{{"22val1END", v::value{true}},
                                   {"val2_2END", v::value{v1()}},
                                   {"third value stringEND", v::value{322.1456}}}};
    }
};

template <>
struct values<v::value::array_type>
{
    static v::value::array_type v1()
    {
        return v::value::array_type{
            {v::value{"a string valEND"}, v::value{true}, v::value{false}, v::value{239485ll}, v::value{2.134532}}};
    }
    static v::value::array_type v2()
    {
        return v::value::array_type{
            {v::value{20384ll}, v::value{false}, v::value{"Hello World           END"}, v::value{239485ll}}};
    }
};
}

using variant_types
    = ::testing::Types<bool, v::value::int_type, double, std::string, v::value::array_type, v::value::map_type>;
using variant_types_movable = ::testing::Types<std::string, v::value::array_type, v::value::map_type>;

TEST(variant, DefaultCtorGivesInvalid)
{
    const v::value sut;
    EXPECT_FALSE(sut.is_valid());
}

TEST(variant, MoveMap)
{
    const v::value::map_type expect_map = {{{"entry 1", v::value("string value")}, {"entry 2 ", v::value(1245.663)}}};
    auto testmap = expect_map;

    ASSERT_FALSE(testmap.empty());
    v::value origin{testmap};

    const v::value dest{std::move(origin)};

    EXPECT_FALSE(origin.is_valid());
    EXPECT_TRUE(dest.is_valid());

    v::value::map_type actual;
    testhelper::verify(dest, actual);
    EXPECT_TRUE(actual == expect_map);
}

TEST(variant, MoveArray)
{
    const v::value::array_type expect_array
        = {{v::value("entry 1"), v::value("string value"), v::value("entry 2 "), v::value(1245.663)}};
    auto testarray = expect_array;

    ASSERT_FALSE(testarray.empty());
    v::value origin{testarray};

    const v::value dest{std::move(origin)};

    EXPECT_FALSE(origin.is_valid());
    EXPECT_TRUE(dest.is_valid());

    v::value::array_type actual;
    testhelper::verify(dest, actual);
    EXPECT_TRUE(actual == expect_array);
}


template <typename T>
class variant_monotype : public ::testing::Test
{
public:
    using value_type = T;

    void SetUp() override
    {
        const auto va = testhelper::values<value_type>::v1();
        const auto vb = testhelper::values<value_type>::v2();

        ASSERT_TRUE(va != vb);

        a = v::value{va};
        a_copy = a;
        b = v::value{vb};
    }

    v::value a;
    v::value a_copy;
    v::value b;
};

template <typename T>
class variant_basics : public variant_monotype<T>
{
};

template <typename T>
class variant_move_semantics : public variant_monotype<T>
{
};

TYPED_TEST_CASE(variant_basics, variant_types);
TYPED_TEST_CASE(variant_move_semantics, variant_types_movable);

TYPED_TEST(variant_basics, ConstructFromNonVariant)
{
    using vtype = typename TestFixture::value_type;
    const vtype expected = testhelper::values<vtype>::v1();

    const v::value sut{expected};

    vtype actual;
    testhelper::verify(sut, actual);

    EXPECT_TRUE(actual == expected);
}

TYPED_TEST(variant_basics, AssignFromNonVariant)
{
    using vtype = typename TestFixture::value_type;
    const auto expected = testhelper::values<vtype>::v1();
    v::value sut;

    sut = expected;

    vtype actual;
    testhelper::verify(sut, actual);

    EXPECT_TRUE(actual == expected);
}

TYPED_TEST(variant_basics, equality)
{
    const auto& va = this->a;
    const auto& va_copy = this->a_copy;
    const auto& vb = this->b;

    EXPECT_TRUE(va == va_copy);
    EXPECT_TRUE(va_copy == va);

    EXPECT_FALSE(va == vb);
    EXPECT_FALSE(vb == va);
}

TYPED_TEST(variant_basics, inequality)
{
    const auto& va = this->a;
    const auto& va_copy = this->a_copy;
    const auto& vb = this->b;

    EXPECT_TRUE(va != vb);
    EXPECT_TRUE(vb != va);

    EXPECT_FALSE(va != va_copy);
    EXPECT_FALSE(va_copy != va);
}

TYPED_TEST(variant_basics, MoveCtor)
{
    using vtype = typename TestFixture::value_type;
    const auto origin = testhelper::values<vtype>::v1();

    v::value sut{origin};
    EXPECT_TRUE(sut.is_valid());

    v::value dest{std::move(sut)};
    EXPECT_TRUE(dest.is_valid());
    EXPECT_FALSE(sut.is_valid());

    vtype actual;
    testhelper::verify(dest, actual);
    EXPECT_TRUE(actual == origin);
}

TYPED_TEST(variant_basics, MoveAssign)
{
    using vtype = typename TestFixture::value_type;
    const auto origin = testhelper::values<vtype>::v1();

    v::value sut{origin};

    EXPECT_TRUE(sut.is_valid());

    v::value dest;
    EXPECT_FALSE(dest.is_valid());

    dest = std::move(sut);
    EXPECT_FALSE(sut.is_valid());

    vtype actual;
    testhelper::verify(dest, actual);
    EXPECT_TRUE(actual == origin);
}

TYPED_TEST(variant_move_semantics, ConstructFromRaw)
{
    using vtype = typename TestFixture::value_type;
    const auto origin = testhelper::values<vtype>::v1();
    auto raw_movable = origin;

    v::value sut{std::move(raw_movable)};

    EXPECT_FALSE(origin == raw_movable);
    EXPECT_TRUE(raw_movable.empty());

    vtype actual;
    testhelper::verify(sut, actual);

    EXPECT_TRUE(actual == origin);
}

TYPED_TEST(variant_move_semantics, MoveAssignFromRaw)
{
    using vtype = typename TestFixture::value_type;
    const auto origin = testhelper::values<vtype>::v1();
    auto raw_movable = origin;

    v::value sut;
    EXPECT_FALSE(sut.is_valid());

    sut = std::move(raw_movable);

    EXPECT_FALSE(origin == raw_movable);
    EXPECT_TRUE(raw_movable.empty());

    vtype actual;
    testhelper::verify(sut, actual);

    EXPECT_TRUE(actual == origin);
}


const std::vector<v::value> test_values = {
    {v::value{}, v::value{true}, v::value{false}, v::value{2345ll}, v::value{-245ll}, v::value{2485.1245},
     v::value{"test string in cross-assgnEND"},

     v::value{v::value::array_type{{v::value{"substring test"}, v::value{false}, v::value{234ll}, v::value{9473.134}}}},

     v::value{v::value::map_type{{{"key1______s______END", v::value{true}},
                                  {"key2_____________________________END", v::value{3.141}},
                                  {"k3", v::value{3445ll}}}}}

    }};

class variant_cross_assign : public ::testing::TestWithParam<std::tuple<v::value, v::value>>
{
};


INSTANTIATE_TEST_CASE_P(variant_wrapped_types,
                        variant_cross_assign,
                        ::testing::Combine(::testing::ValuesIn(test_values), ::testing::ValuesIn(test_values)));


TEST_P(variant_cross_assign, CopyAssign)
{
    const auto v1 = std::get<0>(GetParam());
    const auto v2 = std::get<1>(GetParam());

    v::value v1a{v1};
    v::value v2a{v2};

    v1a = v2a;

    EXPECT_TRUE(v1a == v2a);
    EXPECT_TRUE(v1a == v2);
    EXPECT_TRUE(v2a == v2);
}

TEST(variant, TypeCrossAssign)
{
    v::value s1{testhelper::values<v::value::array_type>::v1()};
    v::value s2{testhelper::values<std::string>::v1()};

    s1 = s2;
    EXPECT_TRUE(s1 == s2);
}


TEST(variant, Visitation)
{
    const auto expected_value = testhelper::values<std::string>::v1();
    v::value v1{expected_value};
    
//    v1.visit([&expected_value](const std::string& x){
//        EXPECT_EQ(x, expected_value);
//    });
}

