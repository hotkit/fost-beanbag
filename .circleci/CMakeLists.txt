cmake_minimum_required(VERSION 3.10)
project(fost-beanbag-circleci)

add_custom_target(check)

add_subdirectory(boost)
add_subdirectory(crypto)
add_subdirectory(cord)
add_subdirectory(fost-base)
add_subdirectory(.. fost-beanbag)
add_subdirectory(fost-internet)
add_subdirectory(fost-web)
add_subdirectory(json-schema)
add_subdirectory(threading)
