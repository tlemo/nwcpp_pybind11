
#pragma once

#include <memory>
#include <string>
#include <optional>
#include <map>
#include <vector>

namespace nwcpp {

class Dataflow;

//==============================================================================
//! Simple mixin for suppressing copy & move operations, ex:
//!
//!  class Foo : public NonCopyable {
//!   ...
//!  };
//!
class NonCopyable {
 public:
  NonCopyable() = default;

  // No copy/move semantics
  NonCopyable(const NonCopyable&) = delete;
  NonCopyable& operator=(const NonCopyable&) = delete;
};

//==============================================================================
//! A token used to restrict the access to constructing dataflow nodes
//!
//! It is a "granular friendship" token, used to implement the "passkey" idiom:
//! https://www.spiria.com/en/blog/desktop-software/passkey-idiom-and-better-friendship-c
//! https://arne-mertz.de/2016/10/passkey-idiom
//!
class Passkey {
  friend class Dataflow;

 public:
  Dataflow* const dag = nullptr;

 private:
  explicit Passkey(Dataflow* dag) : dag(dag) {}
};

//==============================================================================
//! Base class for the Dataflow node types
class Node : public NonCopyable {
 public:
  explicit Node(Passkey passkey);
  virtual ~Node() = default;

  //! Returns the node's value
  virtual int Eval() const = 0;

  //! Returns the parent Dataflow instance
  Dataflow* dag() const { return dag_; }

 private:
  Dataflow* dag_ = nullptr;
};

//==============================================================================
//! A named integer variable
//!
//! Initially, variables are unassigned (don't have a value), and they
//! can be explicitly assigned value using the `Assign()` method.
//!
class Variable : public Node {
 public:
  Variable(Passkey passkey, const std::string& name);
  ~Variable();

  const auto& name() const { return name_; }

  //! Returns the node's value
  int Eval() const override;

  //! Returns `true` if a value has been assigned to this variable
  bool HasValue() const { return value_.has_value(); }

  //! Assign a value to this variable
  void Assign(int value) { value_ = value; }

  //! Resets the variable to "unassigned" (no value)
  void Reset() { value_.reset(); }

 private:
  const std::string name_;
  std::optional<int> value_;
};

//==============================================================================
//! A binary operator node
//!
//! Currently, the following operations are supported: '+', '-', '/', '*'
//!
class BinaryOp : public Node {
 public:
  BinaryOp(Passkey passkey, char op, const Node* lhs, const Node* rhs);
  ~BinaryOp();

  //! Returns the node's value
  int Eval() const override;

  auto op() const { return op_; }
  auto lhs() const { return lhs_; }
  auto rhs() const { return rhs_; }

 private:
  const char op_;
  const Node* lhs_ = nullptr;
  const Node* rhs_ = nullptr;
};

//==============================================================================
//! A toy dataflow DAG
//!
//! Nodes are _integer_ variables or operations, edges are data dependencies
//!
class Dataflow : public NonCopyable {
 public:
  Dataflow();
  ~Dataflow();

  //! Returns a new variable node. Name must be unique within the Dataflow.
  Variable* DeclareVariable(const std::string& name);

  //! Returns a new binary operation node
  BinaryOp* CreateBinaryOp(char op, const Node* lhs, const Node* rhs);

  //! Returns the variable with the specified name, or `nullptr`
  Variable* LookupVariable(const std::string& name) const;

  //! Returns the list of variables, sorted by name
  std::vector<Variable*> Variables() const;

  //! Generates a Graphviz representation of the Dataflow graph
  std::string DumpToGraphviz() const;

  //! Removes all the variables & operations from the dataflow
  void clear();

 private:
  std::map<std::string, Variable*> variables_;
  std::vector<std::unique_ptr<Node>> nodes_;
};

}  // namespace nwcpp
