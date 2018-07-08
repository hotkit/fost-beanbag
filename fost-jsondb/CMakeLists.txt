add_library(fost-jsondb
        blobdb.cpp
        exception.cpp
    )
target_include_directories(fost-jsondb PUBLIC ../include)
target_link_libraries(fost-jsondb fost-core)
set_target_properties(fost-jsondb PROPERTIES DEBUG_POSTFIX "-d")
install(TARGETS fost-jsondb LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

add_library(fost-jsondb-smoke STATIC EXCLUDE_FROM_ALL
        blobdb-tests.cpp
    )
target_link_libraries(fost-jsondb-smoke fost-jsondb)
smoke_test(fost-jsondb-smoke)
