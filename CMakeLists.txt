cmake_minimum_required(VERSION 3.10)
project(colonio-simulations)
find_package(PkgConfig)

set(CMAKE_CXX_STANDARD, 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

include_directories(${LOCAL_ENV_PATH}/include)
link_directories(${LOCAL_ENV_PATH}/lib)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")

# MongoDB(libmongoc)
find_package (libmongoc-1.0 1.16 REQUIRED)

message ("--   mongoc found version \"${MONGOC_VERSION}\"")
message ("--   mongoc include path \"${MONGOC_INCLUDE_DIRS}\"")
message ("--   mongoc libraries \"${MONGOC_LIBRARIES}\"")
include_directories(${MONGOC_INCLUDE_DIRS})

# OpenSSL
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

add_executable(simulations
  src/base.cpp
  src/config.cpp
  src/logger.cpp
  src/main.cpp
  src/plane/plane.cpp
  src/sphere/sphere.cpp
  )

target_link_libraries(simulations
  colonio
  webrtc
  protobuf
  pthread
  ${OPENSSL_LIBRARIES}
  ${MONGOC_LIBRARIES}
  )
