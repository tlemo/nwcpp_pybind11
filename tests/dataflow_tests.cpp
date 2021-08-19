
#include <cpp/dataflow.h>

#include <third_party/gtest/gtest.h>

#include <algorithm>

namespace expr_tests {

TEST(Dataflow, VariableDeclarations) {
  nwcpp::Dataflow dag;

  // empty names are not allowed
  EXPECT_THROW(dag.DeclareVariable(""), std::runtime_error);

  dag.DeclareVariable("foo");

  // variable names must be unique within a dataflow dag
  EXPECT_THROW(dag.DeclareVariable("foo"), std::runtime_error);
}

TEST(Dataflow, VariableValues) {
  nwcpp::Dataflow dag;

  const auto a = dag.DeclareVariable("a");

  // variables don't have an initial value
  EXPECT_FALSE(a->HasValue());
  EXPECT_THROW(a->Eval(), std::runtime_error);

  // assign a value
  a->Assign(0);
  EXPECT_TRUE(a->HasValue());
  EXPECT_EQ(a->Eval(), 0);

  // reset to uninitialized
  a->Reset();
  EXPECT_FALSE(a->HasValue());
  EXPECT_THROW(a->Eval(), std::runtime_error);
}

TEST(Dataflow, VariablesLookup) {
  nwcpp::Dataflow dag;

  // variable a is not declared yet
  EXPECT_EQ(nullptr, dag.LookupVariable("a"));

  // declare and lookup some variables
  const auto a = dag.DeclareVariable("a");
  const auto b = dag.DeclareVariable("foo");
  EXPECT_EQ(a, dag.LookupVariable("a"));
  EXPECT_EQ(b, dag.LookupVariable("foo"));

  // lookups are name sensitive
  EXPECT_EQ(nullptr, dag.LookupVariable("Foo"));

  // ... and whitespace sensitive
  EXPECT_EQ(nullptr, dag.LookupVariable(" foo"));
  EXPECT_EQ(nullptr, dag.LookupVariable("foo "));
}

template <class T>
static bool Equal(const T& a, const T& b) {
  return std::equal(a.begin(), a.end(), b.begin());
}

TEST(Dataflow, VariablesList) {
  nwcpp::Dataflow dag;

  EXPECT_TRUE(Equal(dag.Variables(), {}));

  const auto b = dag.DeclareVariable("b");
  const auto a = dag.DeclareVariable("a");
  const auto c = dag.DeclareVariable("c");

  EXPECT_TRUE(Equal(dag.Variables(), { a, b, c }));
}

TEST(Dataflow, MixingDataflows) {
  nwcpp::Dataflow dag_1;
  nwcpp::Dataflow dag_2;

  const auto a = dag_1.DeclareVariable("a");
  const auto b = dag_1.DeclareVariable("b");
  const auto c = dag_2.DeclareVariable("c");

  // ok
  auto sum_1 = dag_1.CreateBinaryOp('+', a, b);
  auto sum_2 = dag_2.CreateBinaryOp('+', c, c);

  // attempting to use variables from a different dataflow
  EXPECT_THROW(dag_2.CreateBinaryOp('+', a, b), std::runtime_error);

  // attempting to mix nodes from a different workflows
  EXPECT_THROW(dag_2.CreateBinaryOp('+', sum_1, sum_2), std::runtime_error);
  EXPECT_THROW(dag_2.CreateBinaryOp('+', sum_2, sum_1), std::runtime_error);
}

TEST(Dataflow, BinaryOperations) {
  nwcpp::Dataflow dag;

  const auto a = dag.DeclareVariable("a");
  const auto b = dag.DeclareVariable("b");

  const auto sum = dag.CreateBinaryOp('+', a, b);
  const auto product = dag.CreateBinaryOp('*', a, b);
  const auto diff = dag.CreateBinaryOp('-', a, b);
  const auto div = dag.CreateBinaryOp('/', a, b);
  const auto test = dag.CreateBinaryOp('-', sum, diff);

  // variables are not yet initialized
  EXPECT_THROW(sum->Eval(), std::runtime_error);

  a->Assign(1);

  // ... still not initialized
  EXPECT_THROW(sum->Eval(), std::runtime_error);

  b->Assign(5);
  EXPECT_EQ(sum->Eval(), 6);
  EXPECT_EQ(product->Eval(), 5);
  EXPECT_EQ(diff->Eval(), -4);
  EXPECT_EQ(div->Eval(), 0);
  EXPECT_EQ(test->Eval(), 10);

  b->Assign(0);
  EXPECT_EQ(sum->Eval(), 1);
  EXPECT_EQ(product->Eval(), 0);
  EXPECT_EQ(diff->Eval(), 1);
  EXPECT_THROW(div->Eval(), std::runtime_error);  // div by zero
  EXPECT_EQ(test->Eval(), 0);
}

}  // namespace expr_tests
