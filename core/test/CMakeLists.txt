SET(test_EXECUTABLE preclassification-core-test)

FILE(GLOB TestSources cc/*.cc)
INCLUDE_DIRECTORIES(../include)

FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resources/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

ADD_EXECUTABLE(${test_EXECUTABLE} ${TestSources})
TARGET_LINK_LIBRARIES(${test_EXECUTABLE} gtest gtest_main preclassification-core ${Boost_LIBRARIES} ${OpenCV_LIBRARIES})

