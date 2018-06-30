add_library(beanbag
        databases.cpp
        path.cpp
    )
target_link_libraries(beanbag fost-jsondb)
target_include_directories(beanbag PUBLIC ../include)
install(TARGETS beanbag LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

add_library(beanbag-smoke STATIC EXCLUDE_FROM_ALL
        databases-tests.cpp
    )
target_link_libraries(beanbag-smoke beanbag fost-urlhandler)
smoke_test(beanbag-smoke)

