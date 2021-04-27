# Almost all CMake files should start with this
# You should always specify a range with the newest
# and oldest tested versions of CMake. This will ensure
# you pick up the best policies.
cmake_minimum_required(VERSION 3.10...3.20)

# This is your project statement. You should always list languages;
# Listing the version is nice here since it sets lots of useful variables
project(ctcontrastagentexplorer VERSION 0.0.1 LANGUAGES CXX)

# If you set any CMAKE_ variables, that can go here.
# (But usually don't do this, except maybe for C++ standard)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

#Function for adding extra compiler flags if available
include(CheckCXXCompilerFlag)
function(enable_cxx_compiler_flag_if_supported flag)
    string(FIND "${CMAKE_CXX_FLAGS}" "${flag}" flag_already_set)
    if(flag_already_set EQUAL -1)
        check_cxx_compiler_flag("${flag}" flag_supported)
        if(flag_supported)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
        endif()
        unset(flag_supported CACHE)
    endif()
endfunction()
# We want to add extra compiler warnings if supported
enable_cxx_compiler_flag_if_supported("-Wall")
enable_cxx_compiler_flag_if_supported("-Wextra")
enable_cxx_compiler_flag_if_supported("-pedantic")


# Find packages go here.
find_package(Threads REQUIRED) # for threading support

# Finding xraylib library


set(HEADER_LIST 
    "${ctcontrastagentexplorer_SOURCE_DIR}/include/organnode.hpp"    
    )

add_executable(ctcontrastagentexplorer 
    source/caexplorer.cpp    
    ${HEADER_LIST}
	)
target_include_directories(ctcontrastagentexplorer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Make sure you link your targets with this command. It can also link libraries and
# even flags, so linking a target that does not exist will not give a configure-time error.
target_link_libraries(ctcontrastagentexplorer PRIVATE 
    Threads::Threads    
    )

## Install
install(TARGETS ctcontrastagentexplorer 
    EXPORT ctcontrastagentexplorer
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
    )



# Testing
#if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
#    ENABLE_TESTING()
#    include(CTest)
#endif()
#if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
#    add_subdirectory(tests)
#endif()



# Docs by doxygen
#if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_DOC)
#    find_package(Doxygen)
#    find_package(Sphinx)
#    if(Doxygen_FOUND AND Sphinx_FOUND)
#	    add_subdirectory(docs)
#    else()
#	    message(STATUS "Doxygen or Sphinx not found, not building docs")
#    endif()
#endif()
