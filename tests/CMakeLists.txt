

add_executable(solvertest solvertest.cpp)
target_link_libraries(solvertest PRIVATE Solver)
target_include_directories(solvertest PRIVATE ${PROJECT_SOURCE_DIR}/include)
