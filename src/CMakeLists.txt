# Copyright 2019 Sviatoslav Dmitriev
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

add_library(Logger LogStream.cpp Logger.cpp)

add_executable(LoggerTest Test.cpp Logger.cpp LogStream.cpp)
add_test(LoggerTest LoggerTest)
