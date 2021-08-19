
include(CTest)
include(GoogleTest)

add_executable(tests
    dataflow_tests.cpp
    main.cpp)

target_link_libraries(tests PRIVATE dataflow_lib gtest)

gtest_discover_tests(tests)
