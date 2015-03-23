SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/..)

FILE(GLOB MainSources cc/*)
INCLUDE_DIRECTORIES(../include)

ADD_LIBRARY(preclassification-core ${MainSources})
TARGET_LINK_LIBRARIES(preclassification-core)
