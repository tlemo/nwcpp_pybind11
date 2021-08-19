
pybind11_add_module(nwcpp
  python_bindings.cpp
)

target_link_libraries(nwcpp PRIVATE dataflow_lib)
