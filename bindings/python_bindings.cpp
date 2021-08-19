
#include <cpp/dataflow.h>

#include <third_party/pybind11/pybind11.h>

#include <vector>
#include <memory>
#include <string>
#include <sstream>

namespace nwcpp::python {

static std::string VariableRepr(const Variable& var) {
  std::stringstream repr;
  repr << "Var '" << var.name() << "'";
  if (var.HasValue()) {
    repr << "=" << var.Eval();
  }
  return repr.str();
}

static std::string BinaryOpRepr(const BinaryOp& binary_op) {
  std::stringstream repr;
  repr << "Op '" << binary_op.op() << "'";
  return repr.str();
}

PYBIND11_MODULE(nwcpp, m) {
  m.doc() = "NWCPP Pybind11 Example";

  const auto ref_policy = py::return_value_policy::reference_internal;

  py::class_<Node>(m, "Node")
      .def("eval", &Node::Eval)
      .def("dag", &Node::dag)
      .def("__repr__", [] { return "Node"; })
      .def(
          "__add__",
          [](const Node* lhs, const Node* rhs) {
            return lhs->dag()->CreateBinaryOp('+', lhs, rhs);
          },
          ref_policy,
          py::is_operator())
      .def(
          "__sub__",
          [](const Node* lhs, const Node* rhs) {
            return lhs->dag()->CreateBinaryOp('-', lhs, rhs);
          },
          ref_policy,
          py::is_operator())
      .def(
          "__truediv__",
          [](const Node* lhs, const Node* rhs) {
            return lhs->dag()->CreateBinaryOp('/', lhs, rhs);
          },
          ref_policy,
          py::is_operator())
      .def(
          "__mul__",
          [](const Node* lhs, const Node* rhs) {
            return lhs->dag()->CreateBinaryOp('*', lhs, rhs);
          },
          ref_policy,
          py::is_operator());

  py::class_<Variable, Node>(m, "Variable")
      .def_property_readonly("name", &Variable::name)
      .def_property_readonly("has_value", &Variable::HasValue)
      .def("assign", &Variable::Assign)
      .def("reset", &Variable::Reset)
      .def("__repr__", &VariableRepr);

  py::class_<BinaryOp, Node>(m, "BinaryOp")
      .def_property_readonly("op", &BinaryOp::op)
      .def_property_readonly("lhs", &BinaryOp::lhs)
      .def_property_readonly("rhs", &BinaryOp::rhs)
      .def("__repr__", &BinaryOpRepr);

  py::class_<Dataflow>(m, "Dataflow")
      .def(py::init<>())
      .def("declare_variable",
           &Dataflow::DeclareVariable,
           py::arg("name"),
           ref_policy)
      .def("create_binary_op",
           &Dataflow::CreateBinaryOp,
           py::arg("op"),
           py::arg("lhs"),
           py::arg("rhs"),
           ref_policy)
      .def("lookup_variable",
           &Dataflow::LookupVariable,
           py::arg("name"),
           ref_policy)
      .def_property_readonly("variables", &Dataflow::Variables, ref_policy)
      .def("dump_to_graphviz", &Dataflow::DumpToGraphviz)
      .def("clear", &Dataflow::clear)
      .def(
          "add",
          [](Dataflow& df, const Node* lhs, const Node* rhs) {
            return df.CreateBinaryOp('+', lhs, rhs);
          },
          ref_policy)
      .def(
          "sub",
          [](Dataflow& df, const Node* lhs, const Node* rhs) {
            return df.CreateBinaryOp('-', lhs, rhs);
          },
          ref_policy)
      .def(
          "div",
          [](Dataflow& df, const Node* lhs, const Node* rhs) {
            return df.CreateBinaryOp('/', lhs, rhs);
          },
          ref_policy)
      .def(
          "mul",
          [](Dataflow& df, const Node* lhs, const Node* rhs) {
            return df.CreateBinaryOp('*', lhs, rhs);
          },
          ref_policy);
}

}  // namespace nwcpp::python
