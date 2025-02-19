option(SESE_USE_NATIVE_MANAGER "(Deprecated) use the unix-like packages control" OFF)
option(SESE_BUILD_TEST "build unit test targets" OFF)
option(SESE_BUILD_EXAMPLE "build example targets" OFF)
option(SESE_AUTO_VCPKG "auto find vcpkg and include toolchain" OFF)
option(SESE_USE_ASYNC_LOGGER "use async logger" OFF)
option(SESE_USE_ARCHIVE "add archive support" OFF)
option(SESE_REPLACE_EXECINFO "replace the system execinfo implementation" OFF)
option(SESE_DB_USE_SQLITE "add sqlite support" ON)
option(SESE_DB_USE_MARIADB "add mariadb and mysql support" OFF)
option(SESE_DB_USE_POSTGRES "add postgresql support" OFF)

if(SESE_USE_NATIVE_MANAGER)
    message(FATAL_ERROR "The `SESE_USE_NATIVE_MANAGER` option has been deprecated.")
endif()

if(SESE_BUILD_TEST)
    # This sentence must be in the root folder.
    enable_testing()
endif()

include(cmake/MiniSeseTools.cmake)

sese_auto_enable_feature(SESE_BUILD_TEST "tests")
sese_auto_enable_feature(SESE_USE_ARCHIVE "archive")
sese_auto_enable_feature(SESE_REPLACE_EXECINFO "replace-execinfo")
sese_auto_enable_feature(SESE_DB_USE_SQLITE "sqlite3")
sese_auto_enable_feature(SESE_DB_USE_MARIADB "mysql")
sese_auto_enable_feature(SESE_DB_USE_POSTGRES "psql")

option(SESE_ENABLE_ASAN "build with asan" OFF)
option(SESE_ENABLE_COVERAGE "build for coverage test" OFF)
