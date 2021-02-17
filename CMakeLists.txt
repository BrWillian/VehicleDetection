cmake_minimum_required(VERSION 3.10.0)
project(anpr_project VERSION 1.0.0)

# C++ 2017
set (CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
    set(SYSTEM_64_BITS "64")
else()
	set(SYSTEM_32_BITS "32")
endif()

if(WIN32)
	set(CMAKE_CXX_FLAGS "-MT -W0")
else()
	set(CMAKE_CXX_FLAGS "-Wall -Wextra")
endif()
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")



# Darknet Path
if(NOT DEFINED DARKNET_STATIC_LIB_PATH)
	message(WARNING "DARKNET_STATIC_LIB_PATH not set, using default value.")
	set (DARKNET_STATIC_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../darknet)
endif()
if(NOT DEFINED DARKNET_INCLUDE_PATH)
	message(WARNING "DARKNET_INCLUDE_PATH not set, using default value.")
	set (DARKNET_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../darknet/include)
endif()

if(WIN32)
	set (DARKNET_LIB ${DARKNET_STATIC_LIB_PATH}/darknet.lib)
else()
	set (DARKNET_LIB ${DARKNET_STATIC_LIB_PATH}/libdarknet.a)
endif()

# find OpenMP
if(WIN32)
	set(OpenMP_CXX "${CMAKE_CXX_COMPILER}")
  	set(OpenMP_CXX_FLAGS "-openmp -W0")
  	find_package(OpenMP REQUIRED)
else()
	find_package(OpenMP REQUIRED)
endif()

# find Threads
set(CMAKE_THREAD_PREFER_PTHREAD ON)
find_package(Threads REQUIRED) # if not found: sudo apt-get install libboost-all-dev
if(WIN32)
	# Win32 Pthread
	set(PTHREADS_WINDOWS_LIB "${DARKNET_STATIC_LIB_PATH}/3rdparty/pthreads/lib/pthreadVC_x64.lib")
endif()

# OpenCV Path
if(NOT DEFINED OPENCV_STATIC_PATH)
	message(WARNING "OPENCV_STATIC_PATH not set, using default value.")
	set (OPENCV_STATIC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../opencv/release)
endif()

if (WIN32)
	set (OpenCV_STATIC ON)
endif()

find_package(OpenCV REQUIRED PATHS ${OPENCV_STATIC_PATH} NO_DEFAULT_PATH)

# Output Directory
set(OUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/out)

# Create Output Directory
file(MAKE_DIRECTORY ${OUT_DIRECTORY})

# Executable Output Path
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUT_DIRECTORY})

# Lib Output Path
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUT_DIRECTORY})

# Source Files
set (SRC_FILES src/vehicledetector.cpp)

# Include Directories
set (INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES} include ${OpenCV_INCLUDE_DIRS} ${DARKNET_INCLUDE_PATH})

# Dependencies
set (LIB_DEPENDENCIES ${DARKNET_LIB} ${OpenCV_LIBS} ${PTHREADS_WINDOWS_LIB} Threads::Threads OpenMP::OpenMP_CXX OpenMP::OpenMP_C)

# Generate Library Files
add_library(vh_lib SHARED ${SRC_FILES})
# Position independent code for the library
set_target_properties(vh_lib PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Include Directories in Library generation
include_directories(vh_lib ${INCLUDE_DIRECTORIES})

# Include Directories in Executable generation
include_directories(vh_run ${INCLUDE_DIRECTORIES})

# Linking to dependencies in Library generation
target_link_libraries( vh_lib PRIVATE ${LIB_DEPENDENCIES})

# Lib output file name
set_target_properties(vh_lib PROPERTIES LIBRARY_OUTPUT_NAME vh)
