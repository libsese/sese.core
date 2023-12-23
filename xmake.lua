add_rules("mode.debug", "mode.release")

add_requires("openssl3 3.0.7", "zlib v1.3")
add_requires("gtest v1.14.0", {optional = true})
add_requires("python 3.x", {system = true})

set_languages("c++20")
set_encodings("utf-8")

add_defines("SESE_REPO_HASH=\"xxx\"")
add_defines("SESE_REPO_BRANCH=\"xxx\"")
add_defines("SESE_MAJOR_VERSION=\"xxx\"")
add_defines("SESE_MINOR_VERSION=\"xxx\"")
add_defines("SESE_PATCH_VERSION=\"xxx\"")

add_includedirs("$(projectdir)")

target("sese-core")
    set_kind("shared")
    add_packages("openssl3")
    add_packages("zlib")
    if is_plat("windows") then
        add_defines("WIN32")
        add_packages("advapi32")
        add_packages("crypt32")
        -- add_packages("dbghelp")
        -- add_packages("iphlpapi")
        add_packages("secur32")
        add_packages("ws2_32")
        add_files("sese/native/win/**.cpp")
        add_links("dbghelp", "iphlpapi")
    end
    -- add_files("sese/concurrent/**.cpp")
    add_files("sese/config/**.cpp")
    add_files("sese/convert/**.cpp")
    -- add_files("sese/event/**.cpp")
    add_files("sese/io/**.cpp")
    add_files("sese/net/**.cpp")
    add_files("sese/plugin/**.cpp")
    add_files("sese/record/**.cpp")
    add_files("sese/security/**.cpp")
    add_files("sese/service/**.cpp")
    add_files("sese/system/**.cpp")
    add_files("sese/text/**.cpp")
    add_files("sese/thread/**.cpp")
    add_files("sese/util/**.cpp")
    add_headerfiles("sese/**.h")
    add_rules("utils.symbols.export_all", {export_classes = true})

target("plugin")
    set_kind("static")
    add_files("sese/plugin/**.cpp")
    add_headerfiles("sese/**.h")

option("test")
    set_default(false)
    set_showmenu(true)
    set_description("enable unit test")

target("module")
    set_kind("static")
    add_options("test")
    add_files("gtest/TestPlugin/Module.cpp")
    add_deps("plugin")
    add_links("plugin")

target("mem.d")
    set_kind("binary")
    add_options("test")
    add_files("gtest/TestSharedMemory/Memory.d.cpp")
    add_deps("sese-core")
    add_links("sese-core")

target("test")
    set_kind("binary")
    add_options("test")
    add_defines("SESE_BUILD_TEST")
    add_defines("PROJECT_PATH=\"$(projectdir)\"")
    add_defines("PY_EXECUTABLE=\"python3\"")
    add_defines("PATH_TO_CORE=\"$(sese-core:targetfile())\"")
    add_defines("PATH_TO_MODULE=\"$(module:targetfile())\"")
    add_defines("PATH_TO_MEM_D=\"$(mem.d:targetfile())\"")
    add_packages("gtest")
    add_files("gtest/*.cpp")
    add_files("gtest/TestPlugin/TestPlugin.cpp")
    add_files("gtest/TestSharedMemory/TestSharedMemory.cpp")
    add_deps("module")
    add_deps("mem.d")
    add_deps("sese-core")
    add_links("sese-core")