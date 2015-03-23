SET(test_EXECUTABLE preclassification-core-test)
# SET_PROPERTY(DIRECTORY . APPEND PROPERTY COMPILE_DEFINITIONS GTEST_USE_OWN_TR1_TUPLE=1)

FILE(GLOB TestSources cc/*)

ADD_EXECUTABLE(${test_EXECUTABLE} ${TestSources})
TARGET_LINK_LIBRARIES(${test_EXECUTABLE} gtest gtest_main preclassification-core ${Boost_LIBRARIES})
