
#include "dataflow.h"

#include <assert.h>
#include <stdexcept>
#include <sstream>
#include <iostream>

namespace nwcpp {

static constexpr bool kLifetimeTracing = false;

Node::Node(Passkey passkey) : dag_(passkey.dag) {
  assert(dag_ != nullptr);
}

Variable::Variable(Passkey passkey, const std::string& name)
    : Node(passkey), name_(name) {
  if constexpr (kLifetimeTracing) {
    std::cout << "Variable('" << name << "')\n";
  }
  if (name_.empty()) {
    throw std::runtime_error("Invalid variable name (empty)");
  }
}

Variable::~Variable() {
  if constexpr (kLifetimeTracing) {
    std::cout << "~Variable('" << name_ << "')\n";
  }
}

int Variable::Eval() const {
  if (!value_) {
    std::stringstream msg;
    msg << "Attempting to use unasigned variable '" << name_ << "'";
    throw std::runtime_error(msg.str());
  }
  return *value_;
}

BinaryOp::BinaryOp(Passkey passkey, char op, const Node* lhs, const Node* rhs)
    : Node(passkey), op_(op), lhs_(lhs), rhs_(rhs) {
  assert(lhs_ != nullptr);
  assert(rhs_ != nullptr);
  if (lhs->dag() != dag() || rhs->dag() != dag()) {
    throw std::runtime_error("Can't mix operands from different dataflows");
  }
  if constexpr (kLifetimeTracing) {
    std::cout << "BinaryOp('" << op << "')\n";
  }
}

BinaryOp::~BinaryOp() {
  if constexpr (kLifetimeTracing) {
    std::cout << "~BinaryOp()\n";
  }
}

int BinaryOp::Eval() const {
  const int lhs_value = lhs_->Eval();
  const int rhs_value = rhs_->Eval();
  switch (op_) {
    case '+':
      return lhs_value + rhs_value;
    case '-':
      return lhs_value - rhs_value;
    case '/':
      if (rhs_value == 0) {
        throw std::runtime_error("Division by zero");
      }
      return lhs_value / rhs_value;
    case '*':
      return lhs_value * rhs_value;
    default:
      std::stringstream msg;
      msg << "Unknown op '" << op_ << "'";
      throw std::runtime_error(msg.str());
  }
}

Dataflow::Dataflow() {
  if constexpr (kLifetimeTracing) {
    std::cout << "Dataflow()\n";
  }
}

Dataflow::~Dataflow() {
  if constexpr (kLifetimeTracing) {
    std::cout << "~Dataflow()\n";
  }
}

Variable* Dataflow::DeclareVariable(const std::string& name) {
  Passkey passkey(this);
  auto var = std::make_unique<Variable>(passkey, name);
  const auto ptr = var.get();
  if (!variables_.insert({ var->name(), ptr }).second) {
    std::stringstream msg;
    msg << "Duplicate variable name ('" << var->name() << "')";
    throw std::runtime_error(msg.str());
  }
  nodes_.push_back(std::move(var));
  return ptr;
}

BinaryOp* Dataflow::CreateBinaryOp(char op, const Node* lhs, const Node* rhs) {
  Passkey passkey(this);
  auto operation = std::make_unique<BinaryOp>(passkey, op, lhs, rhs);
  const auto ptr = operation.get();
  nodes_.push_back(std::move(operation));
  return ptr;
}

Variable* Dataflow::LookupVariable(const std::string& name) const {
  const auto it = variables_.find(name);
  return it != variables_.end() ? it->second : nullptr;
}

std::vector<Variable*> Dataflow::Variables() const {
  std::vector<Variable*> vars;
  for (const auto& [name, var] : variables_) {
    vars.push_back(var);
  }
  return vars;
}

static std::string DotNodeId(const Node* node) {
  std::stringstream id;
  id << "Node_" << node;
  return id.str();
}

std::string Dataflow::DumpToGraphviz() const {
  std::stringstream dot;
  dot << "digraph dataflow {\n"
      << "rankdir=TB\n";
  for (const auto& node : nodes_) {
    if (auto var = dynamic_cast<const Variable*>(node.get())) {
      std::stringstream label;
      label << var->name();
      if (var->HasValue()) {
        label << "=" << var->Eval();
      }
      dot << DotNodeId(var) << " [label=\"" << label.str() << "\", "
          << "shape=rect, color=darkgreen, "
          << "style=filled, fillcolor=palegreen]\n";
    } else if (auto binary_op = dynamic_cast<const BinaryOp*>(node.get())) {
      dot << DotNodeId(binary_op) << " [label=\"<lhs>|" << binary_op->op()
          << "|<rhs>\", shape=Mrecord, color=blue, "
          << "style=filled, fillcolor=lightblue]\n";
      dot << DotNodeId(binary_op->lhs()) << "->" << DotNodeId(binary_op)
          << ":lhs\n";
      dot << DotNodeId(binary_op->rhs()) << "->" << DotNodeId(binary_op)
          << ":rhs\n";
    } else {
      throw std::runtime_error("Unexpected node type");
    }
  }
  dot << "}\n";
  return dot.str();
}

void Dataflow::clear() {
  variables_.clear();
  nodes_.clear();
}

}  // namespace nwcpp
