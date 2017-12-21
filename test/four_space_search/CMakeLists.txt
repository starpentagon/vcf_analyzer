cmake_minimum_required(VERSION 3.5.1)

# プロジェクト名
set(project_name four_space_search_test)
project(${project_name} CXX)

# Build Type(Release or Debug)
#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)

# ccache
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

# コンパイルオプション
add_definitions("-Wall -std=c++14")
# add_definitions("-Wall -DNDEBUG -std=c++14")  # profile用

# インクルードパス
include_directories(${CMAKE_SOURCE_DIR}/../../src/include)
include_directories(${CMAKE_SOURCE_DIR}/../../realcore/src/include)
include_directories($ENV{GTEST_DIR}/googletest/include)

# ライブラリパス
link_directories($ENV{GTEST_DIR}/googlemock/gtest)

# ソースファイル
add_executable(
    ${project_name}
    ${CMAKE_SOURCE_DIR}/../../realcore/src/Move.cc
    ${CMAKE_SOURCE_DIR}/../../realcore/src/MoveList.cc
    ${CMAKE_SOURCE_DIR}/../../realcore/src/LineNeighborhood.cc
    ${CMAKE_SOURCE_DIR}/../../realcore/src/OpenState.cc
    ${CMAKE_SOURCE_DIR}/../../realcore/src/BoardOpenState.cc
    ${CMAKE_SOURCE_DIR}/../../realcore/src/BitBoard.cc
    ${CMAKE_SOURCE_DIR}/../../realcore/src/BitSearch.cc
    ../../../src/RelaxedFour.cc
    ../../../src/FourSpaceSearch.cc
    ../../../src/FourSpace.cc
    ../../../src/RelaxedFour.cc
    ../../../src/OpponentFourInfo.cc
    ../../../src/OpenRestList.cc
    ../../../src/OpenRestDependency.cc
    ../FourSpaceSearchTest.cc
    ../OpponentFourInfoTest.cc
    ../OpenRestListTest.cc
    ../OpenRestDependencyTest.cc
    ../FourSpaceTest.cc
    ../RelaxedFourTest.cc
)

# ライブラリ
target_link_libraries(${project_name} gtest)
target_link_libraries(${project_name} gtest_main)

# Ubuntuでgoogle testをビルドするためにはpthreadが必要
target_link_libraries(${project_name} pthread)

if(APPLE OR WIN32)
  target_link_libraries(${project_name} boost_system-mt)
  target_link_libraries(${project_name} boost_thread-mt)
else()
  target_link_libraries(${project_name} boost_system)
  target_link_libraries(${project_name} boost_thread)
endif()
