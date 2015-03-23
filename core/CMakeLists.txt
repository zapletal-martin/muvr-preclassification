ADD_SUBDIRECTORY(main)
ADD_SUBDIRECTORY(test)
INCLUDE_DIRECTORIES(include)

ADD_TEST(
  NAME preclassification-core-test 
  COMMAND preclassification-core-test
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/test"
)
