
# Pybind11 Demo

This is a toy example intended to illustrate how to use [pybind11][1] to create
Python bindings around a C++ library. A goal of this exercise is to create
a Pythonic extension without requiring modifications to the C++ library.

 Directory    | Component                                                        
--------------|-----------------------------------------------------------------
bindings      | Source code for the Python bindings (including a demo notebook)
cpp           | A toy C++ dataflow graph library                                 
docs          | Doxygen documentation definition for the C++ library
tests         | C++ test cases
tests/python  | Python test cases

## Prerequisites

- A C++17 compiler
- Python 3.6+
- CMake 3.17+
- miniconda (optional)
- Doxygen for generating the API documentation (optional)

## Getting the Source Code

This project uses [Git submodules][3] for third party libraries,
so `--recursive` is required when cloning:

```shell
git clone --recursive <repository>
```

Or, the submodules can be initialized after a regular clone:

```shell
git submodule init
git submodule update
```

> Submodules may change. To avoid having to repeatedly run `git submodule update`:
> - Use `git pull --recurse-submodules` when updating the local repository.
> - Or, you can make it automatic: `git config submodule.recurse true`

## Building

> Tip: The use of a virtual Python environment (ex. venv or conda) is
> highly recommended

The easiest way to build & install the Python extension is:

`python setup.py install`

Or, in order to use the CMake project directly (note this will not automatically
install the built Python extension library):

```shell
# from the project's root directory
mkdir build && cd build
cmake .. # defaults to debug build
make -j8 # normally, use the number of cores
```

## Running the Python tests

`python setup.py test`

## Running the C++ tests

```shell
cd build/tests
ctest
```

Or, to directly run a test binary:

```shell
cd build
tests/tests
```

## References

- [Slide deck](https://docs.google.com/presentation/d/11mCjNOOGIuqKdpsb6ITD76moXxchqgdX9eKYSbdo2XI/edit?usp=sharing)
- [pybind11 documentation][1]
- [Python Data (Object) Model](https://docs.python.org/3/reference/datamodel.html)
- [Extending and Embedding the Python Interpreter](https://docs.python.org/3/extending)
- [Packaging binary extensions](https://packaging.python.org/guides/packaging-binary-extensions)
- [Packaging Python Projects](https://packaging.python.org/tutorials/packaging-projects)
- [PyPI · The Python Package Index](https://pypi.org)

[1]: https://pybind11.readthedocs.io/en/stable

