cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)
project(cggi)

find_package(Boost 1.71.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

#add_library(cggi
#    cggi.cpp
#  )
set(cggi_demo_target "cggi-demo")

add_executable(${cggi_demo_target}
    cggi_demo.cpp
  )
#target_link_libraries(${cggi_demo_target}
#    cggi
#  )
install(TARGETS ${cggi_demo_target}
  )
