############################################################################
# Copyright (c) 2011-2014 Saint-Petersburg Academic University
# All Rights Reserved
# See file LICENSE for details.
############################################################################

project(input CXX)

add_library(input STATIC
            path_helper.cpp
            copy_file.cpp)


