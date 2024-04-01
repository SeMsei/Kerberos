cmake_minimum_required(VERSION 3.23)

project(test_client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
link_directories(SYSTEM ${Boost_LIBRARY_DIRS})

file(GLOB_RECURSE SRC "*.cpp" "*.h" "*.hpp")

add_executable(${PROJECT_NAME} ${SRC})

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
