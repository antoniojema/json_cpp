file(GLOB HeaderList
    "include/json/*.h"
)

file(GLOB SRCList
    "*.cpp"
)

SET(CML "CMakeLists.txt")

add_library(
    ${LibName}
    ${HeaderList}
    ${SRCList}
    ${CML}
)

source_group("include/" FILES ${HeaderList})
source_group("src/" FILES ${SRCList})
source_group("CMakeLists/" FILES ${CML})

target_include_directories(
    ${LibName}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/json
)

target_include_directories(
    ${LibName}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
