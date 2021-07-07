newoption {
    trigger = "raspberry-pi",
    description = "Disable GUI apps and enable the external interface"
}

solution 'ini300'
    configurations {'release', 'debug'}
    location 'build'
    project 'recorder'
        kind 'ConsoleApp'
        language 'C++'
        location 'build'
        files {'../common/interface.hpp', 'source/*.hpp', 'source/recorder.cpp'}
        defines {'SEPIA_COMPILER_WORKING_DIRECTORY="' .. project().location .. '"'}
        if _OPTIONS['raspberry-pi'] then
            defines {'RASPBERRY_PI'}
        end
        configuration 'release'
            targetdir 'build/release'
            defines {'NDEBUG'}
            flags {'OptimizeSpeed'}
        configuration 'debug'
            targetdir 'build/debug'
            defines {'DEBUG'}
            flags {'Symbols'}
        configuration 'linux'
            links {'pthread', 'usb-1.0', 'stdc++fs'}
            buildoptions {'-std=c++17', '-Wno-psabi'}
            linkoptions {'-std=c++17', '-Wno-psabi'}
        configuration 'macosx'
            includedirs {'/usr/local/include'}
            libdirs {'/usr/local/lib'}
            links {'usb-1.0'}
            buildoptions {'-std=c++17'}
            linkoptions {'-std=c++17'}
        configuration 'windows'
            files {'.clang-format'}
            includedirs {'C:\\Include'}
            links {'C:\\Windows\\SysWOW64\\libusb-1.0'}
            buildoptions {'/std:c++17'}
