cmake_minimum_required(VERSION 3.1)

project(ParetoCalculator)

enable_testing()

add_subdirectory(ParetoCalculatorLibrary/src)
add_subdirectory(ParetoCalculatorConsole)
add_subdirectory(doc)
add_subdirectory(tests)


