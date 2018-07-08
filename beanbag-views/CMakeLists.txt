add_library(beanbag-views
        beanbag-raw.cpp
        beanbag-structured.cpp
        beanbag-views.cpp
        patch.cpp
        patch.op-add.cpp
        patch.op-created.cpp
        patch.op-now.cpp
        patch.op-remove.cpp
        patch.op-set.cpp
    )
target_link_libraries(beanbag-views beanbag fost-crypto fost-urlhandler)
set_target_properties(beanbag-views PROPERTIES DEBUG_POSTFIX "-d")
install(TARGETS beanbag-views LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/beanbag-raw.tests.html
    ${CMAKE_CURRENT_BINARY_DIR}/beanbag-raw.tests.html COPYONLY)
add_library(beanbag-views-smoke STATIC EXCLUDE_FROM_ALL
        beanbag-raw.tests.cpp
        beanbag-structured.tests.cpp
        patch.tests.cpp
    )
target_link_libraries(beanbag-views-smoke beanbag-views)
smoke_test(beanbag-views-smoke)

