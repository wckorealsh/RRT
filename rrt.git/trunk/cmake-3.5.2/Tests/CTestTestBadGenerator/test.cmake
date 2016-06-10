cmake_minimum_required(VERSION 3.0)

# Settings:
set(CTEST_DASHBOARD_ROOT                "/home/slee198/RRT/rrt.git/trunk/cmake-3.5.2/Tests/CTestTest")
set(CTEST_SITE                          "slee198-dev")
set(CTEST_BUILD_NAME                    "CTestTest-Linux-g++-Depends")

set(CTEST_SOURCE_DIRECTORY              "/home/slee198/RRT/rrt.git/trunk/cmake-3.5.2/Tests/CTestTestBadGenerator")
set(CTEST_BINARY_DIRECTORY              "/home/slee198/RRT/rrt.git/trunk/cmake-3.5.2/Tests/CTestTestBadGenerator")
set(CTEST_CVS_COMMAND                   "CVSCOMMAND-NOTFOUND")
set(CTEST_CMAKE_GENERATOR               "Bad Generator")
set(CTEST_CMAKE_GENERATOR_PLATFORM      "")
set(CTEST_CMAKE_GENERATOR_TOOLSET       "")
set(CTEST_BUILD_CONFIGURATION           "$ENV{CMAKE_CONFIG_TYPE}")
set(CTEST_COVERAGE_COMMAND              "/usr/bin/gcov")
set(CTEST_NOTES_FILES                   "${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}")

CTEST_START(Experimental)
CTEST_CONFIGURE(BUILD "${CTEST_BINARY_DIRECTORY}" RETURN_VALUE res)
CTEST_BUILD(BUILD "${CTEST_BINARY_DIRECTORY}" RETURN_VALUE res)
CTEST_TEST(BUILD "${CTEST_BINARY_DIRECTORY}" RETURN_VALUE res)
