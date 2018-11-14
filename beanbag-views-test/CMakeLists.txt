if(TARGET stress)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/beanbag-raw.tests.html
        ${CMAKE_CURRENT_BINARY_DIR}/beanbag-raw.tests.html COPYONLY)
    add_library(fost-beanbag-views-test STATIC EXCLUDE_FROM_ALL
            dynlib.cpp
            settings.cpp
            thread.cpp
            workerpool.cpp
        )
    target_link_libraries(fost-beanbag-views-test beanbag-views)
    stress_test(fost-beanbag-views-test)
endif()
