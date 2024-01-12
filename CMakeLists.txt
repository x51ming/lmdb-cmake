cmake_minimum_required(VERSION 3.5)

project(lmdb
    LANGUAGES C
    VERSION 1.0
)

add_library(lmdb
    lmdb/libraries/liblmdb/midl.c
    lmdb/libraries/liblmdb/midl.h
    # lmdb/libraries/liblmdb/mdb.c
    mdb_env_open.c
    lmdb/libraries/liblmdb/lmdb.h
    LMDBConfig.cmake.in
)

set_target_properties(lmdb
    PROPERTIES PUBLIC_HEADER
        "lmdb/libraries/liblmdb/lmdb.h;lmdb/libraries/liblmdb/midl.h;mdb_custom.h"
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        LMDBConfigVersion.cmake
        VERSION ${PACKAGE_VERSION}
        COMPATIBILITY AnyNewerVersion  # 表示该函数库向下兼容
)

install(TARGETS lmdb
    EXPORT LMDB
)
install(EXPORT LMDB
    FILE LMDBTargets.cmake
    NAMESPACE LMDB::
    DESTINATION lib/cmake
)
configure_file(LMDBConfig.cmake.in
    LMDBConfig.cmake @ONLY)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/LMDBConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/LMDBConfigVersion.cmake
    DESTINATION lib/cmake
)

set(TEST_EXECUTABLES
    mdb_copy
    mdb_dump
    mdb_drop
    mdb_load
    mdb_stat
    mtest
    mplay
    mtest2
    mtest3
    mtest4
    mtest5
    # mtest6
)

foreach(TEST_EXECUTABLE ${TEST_EXECUTABLES})
    add_executable(${TEST_EXECUTABLE}
        lmdb/libraries/liblmdb/${TEST_EXECUTABLE}.c
    )
    target_link_libraries(${TEST_EXECUTABLE}
        lmdb
    )
endforeach()

include_directories(
    lmdb/libraries/liblmdb
)
add_executable(mdb_test_ro
    mdb_test_ro.c
)
target_link_libraries(mdb_test_ro
    lmdb
)