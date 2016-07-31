#include "anyrpc/variant.h"

#include <gtest/gtest.h>

using namespace anyrpc;

namespace v = anyrpc::v2;

namespace testhelper
{
void verify_bool(const v::value& sut, bool& actual)
{
    EXPECT_NO_THROW({ actual = sut.get<bool>(); });

    EXPECT_THROW({ sut.get<v::value::int_type>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<std::string>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<double>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<v::value::map_type>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<v::value::array_type>(); }, std::runtime_error);
}

void verify_integer(const v::value& sut, v::value::int_type& actual)
{
    EXPECT_NO_THROW({ actual = sut.get<v::value::int_type>(); });

    EXPECT_THROW({ sut.get<bool>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<std::string>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<double>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<v::value::map_type>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<v::value::array_type>(); }, std::runtime_error);
}

void verify_float(const v::value& sut, double& actual)
{
    EXPECT_NO_THROW({ actual = sut.get<double>(); });

    EXPECT_THROW({ sut.get<v::value::int_type>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<std::string>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<bool>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<v::value::map_type>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<v::value::array_type>(); }, std::runtime_error);
}

void verify_string(const v::value& sut, std::string& actual)
{
    EXPECT_NO_THROW({ actual = sut.get<std::string>(); });

    EXPECT_THROW({ sut.get<v::value::int_type>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<bool>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<double>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<v::value::map_type>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<v::value::array_type>(); }, std::runtime_error);
}

void verify_array(const v::value& sut, v::value::array_type& actual)
{
    EXPECT_NO_THROW({ actual = sut.get<v::value::array_type>(); });

    EXPECT_THROW({ sut.get<v::value::int_type>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<std::string>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<double>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<v::value::map_type>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<bool>(); }, std::runtime_error);
}

void verify_map(const v::value& sut, v::value::map_type& actual)
{
    EXPECT_NO_THROW({ actual = sut.get<v::value::map_type>(); });

    EXPECT_THROW({ sut.get<v::value::int_type>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<std::string>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<double>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<v::value::array_type>(); }, std::runtime_error);
    EXPECT_THROW({ sut.get<bool>(); }, std::runtime_error);
}
}

TEST(variant, DefaultCtorGivesInvalid)
{
    const v::value sut;
    EXPECT_FALSE(sut.is_valid());
}

TEST(variant, ConstructFromBool)
{
    const bool expected = true;
    const v::value sut{expected};

    bool actual;
    testhelper::verify_bool(sut, actual);

    EXPECT_EQ(actual, expected);
}

TEST(variant, ConstructFromInteger)
{
    using vtype = v::value::int_type;
    const vtype expected = 1235;
    const v::value sut{expected};

    vtype actual;
    testhelper::verify_integer(sut, actual);

    EXPECT_EQ(actual, expected);
}

TEST(variant, ConstructFromDouble)
{
    using vtype = double;
    const vtype expected = 12.119245;
    const v::value sut{expected};

    vtype actual;
    testhelper::verify_float(sut, actual);

    EXPECT_EQ(actual, expected);
}

TEST(variant, ConstructFromString)
{
    using vtype = std::string;
    const vtype expected = "a test string \x00 \xff #$%^&*())(*sdcs";
    const v::value sut{expected};

    vtype actual;
    testhelper::verify_string(sut, actual);

    EXPECT_EQ(actual, expected);
}

TEST(variant, ConstructFromArray)
{
    using vtype = v::value::array_type;
    const vtype expected = {{v::value(true), v::value{1234.24}}};
    const v::value sut{expected};

    vtype actual;
    testhelper::verify_array(sut, actual);

    EXPECT_TRUE(actual == expected);
}

TEST(variant, ConstructFromMap)
{
    using vtype = v::value::map_type;
    const vtype expected = {{{"nbool", v::value(true)}, {"doubleenrty", v::value{1234.24}}}};
    const v::value sut{expected};

    vtype actual;
    testhelper::verify_map(sut, actual);

    EXPECT_TRUE(actual == expected);
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
    testhelper::verify_map(dest, actual);
    EXPECT_TRUE(actual == expect_map);
}

TEST(variant, MoveArray)
{
    const v::value::array_type expect_array = {{v::value("entry 1"), v::value("string value"), v::value("entry 2 "), v::value(1245.663)}};
    auto testarray = expect_array;
    
    ASSERT_FALSE(testarray.empty());
    v::value origin{testarray};
    
    const v::value dest{std::move(origin)};
    
    EXPECT_FALSE(origin.is_valid());
    EXPECT_TRUE(dest.is_valid());
    
    v::value::array_type actual;
    testhelper::verify_array(dest, actual);
    EXPECT_TRUE(actual == expect_array);
}

