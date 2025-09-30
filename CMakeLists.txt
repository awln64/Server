cmake_minimum_required(VERSION 3.8)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("Server")

set(CMAKE_CXX_STANDARD 17)

add_executable("Server" main.cpp "Session.cpp" "Server.cpp")

target_include_directories("Server" PRIVATE ${Boost_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include)