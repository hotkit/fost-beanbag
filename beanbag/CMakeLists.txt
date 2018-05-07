add_library(fost-beanbag
        databases.cpp
        path.cpp
    )
target_link_libraries(fost-beanbag fost-jsondb)
