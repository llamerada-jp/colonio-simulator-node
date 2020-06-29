cmake_minimum_required(VERSION 3.10)
project(colonio-simulations)
find_package(PkgConfig)

set(CMAKE_CXX_STANDARD, 14)

include_directories(${LOCAL_ENV_PATH}/include)
link_directories(${LOCAL_ENV_PATH}/lib)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")

# OpenSSL
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

add_executable(simulations
  src/base.cpp
  src/main.cpp
  src/sphere/sphere.cpp
  )

target_link_libraries(simulations
  colonio
  webrtc
  protobuf
  ${OPENSSL_LIBRARIES}
  pthread
  )
