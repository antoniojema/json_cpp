
# Take all headers, cpps and CMakeLists
set(HeaderList
    "json_utils.h"
    "json.h"
)

set(SRCList
    "json.cpp"
    "json_parser.cpp"
)

SET(CML "CMakeLists.txt")

add_library(
    ${LibName}
    ${HeaderList}
    ${SRCList}
    ${CML}
)
