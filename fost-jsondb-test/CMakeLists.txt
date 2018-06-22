add_executable(fost-jsondb-test-backup EXCLUDE_FROM_ALL
        backup.cpp
    )
target_link_libraries(fost-jsondb-test-backup fost-cli fost-jsondb fost-test)
add_custom_command(TARGET fost-jsondb-test-backup
    POST_BUILD COMMAND fost-jsondb-test-backup -b false -v false)
add_dependencies(stress fost-jsondb-test-backup)
add_test(NAME fost-jsondb-test-backup-test
    COMMAND fost-jsondb-test-backup)


add_executable(fost-jsondb-test-file EXCLUDE_FROM_ALL
        filetest.cpp
    )
target_link_libraries(fost-jsondb-test-file fost-cli fost-jsondb fost-test)
add_custom_command(TARGET fost-jsondb-test-file
    POST_BUILD COMMAND fost-jsondb-test-file -b false -v false)
add_dependencies(stress fost-jsondb-test-file)
add_test(NAME fost-jsondb-test-file-test
    COMMAND fost-jsondb-test-file)
