cmake_minimum_required(VERSION 4.0)
project(tinylang)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(calc
		src/main.cpp
		src/compiler/lexer.cpp
		src/compiler/parser.cpp
		src/compiler/AST.cpp
		src/compiler/sema.cpp
		src/compiler/codegen.cpp)

target_link_libraries(calc LLVM)
