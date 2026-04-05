#include "scada/qualified_name.h"

#include <gtest/gtest.h>

TEST(QualifiedNameTest, DefaultIsEmpty) {
  scada::QualifiedName qn;
  EXPECT_TRUE(qn.empty());
  EXPECT_EQ("", qn.name());
  EXPECT_EQ(0, qn.namespace_index());
}

TEST(QualifiedNameTest, ConstructWithName) {
  scada::QualifiedName qn("Temperature");
  EXPECT_FALSE(qn.empty());
  EXPECT_EQ("Temperature", qn.name());
  EXPECT_EQ(0, qn.namespace_index());
}

TEST(QualifiedNameTest, ConstructWithNameAndNamespace) {
  scada::QualifiedName qn("Pressure", 3);
  EXPECT_FALSE(qn.empty());
  EXPECT_EQ("Pressure", qn.name());
  EXPECT_EQ(3, qn.namespace_index());
}

TEST(QualifiedNameTest, Equality) {
  scada::QualifiedName a("Temp", 1);
  scada::QualifiedName b("Temp", 1);
  scada::QualifiedName c("Temp", 2);
  scada::QualifiedName d("Other", 1);
  EXPECT_EQ(a, b);
  EXPECT_NE(a, c);
  EXPECT_NE(a, d);
}

TEST(QualifiedNameTest, ToString) {
  scada::QualifiedName qn("hello");
  EXPECT_EQ("hello", ToString(qn));
}

TEST(QualifiedNameTest, ToString16Ascii) {
  scada::QualifiedName qn("hello");
  EXPECT_EQ(u"hello", ToString16(qn));
}

TEST(QualifiedNameTest, ToString16NonAscii) {
  scada::QualifiedName qn("\xd0\x9f\xd1\x80\xd0\xb8\xd0\xb2\xd0\xb5\xd1\x82");  // "Привет" in UTF-8
  EXPECT_EQ(u"\u041F\u0440\u0438\u0432\u0435\u0442", ToString16(qn));
}

TEST(QualifiedNameTest, ToString16Empty) {
  scada::QualifiedName qn;
  EXPECT_TRUE(ToString16(qn).empty());
}
