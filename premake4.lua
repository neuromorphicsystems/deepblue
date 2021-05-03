local qt = require 'third_party/chameleon/qt'

newoption {
    trigger = "raspberry-pi",
    description = "Disable GUI apps and enable the external interface"
}

newoption {
    trigger = "command-line",
    description = "Compile only command line tools"
}

solution 'atis'
    configurations {'release', 'debug'}
    location 'build'

    if not _OPTIONS['command-line'] then
        project 'recorder'
            kind 'ConsoleApp'
            language 'C++'
            location 'build'
            files {'source/parse_parameters.hpp', 'source/interface.hpp', 'source/recorder.cpp'}
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
    end


    if not _OPTIONS['raspberry-pi'] then
        project 'raw_to_es'
            kind 'ConsoleApp'
            language 'C++'
            location 'build'
            files {'source/raw_to_es.cpp'}
            defines {'SEPIA_COMPILER_WORKING_DIRECTORY="' .. project().location .. '"'}
            configuration 'release'
                targetdir 'build/release'
                defines {'NDEBUG'}
                flags {'OptimizeSpeed'}
            configuration 'debug'
                targetdir 'build/debug'
                defines {'DEBUG'}
                flags {'Symbols'}
            configuration 'linux'
                links {'pthread'}
                buildoptions {'-std=c++17'}
                linkoptions {'-std=c++17'}
            configuration 'macosx'
                buildoptions {'-std=c++17'}
                linkoptions {'-std=c++17'}
            configuration 'windows'
                files {'.clang-format'}
                buildoptions {'/std:c++17'}
    end

    if not _OPTIONS['raspberry-pi'] and not _OPTIONS['command-line'] then
        project 'viewer'
            kind 'ConsoleApp'
            language 'C++'
            location 'build'
            files {'source/parse_parameters.hpp', 'source/viewer.cpp'}
            defines {'SEPIA_COMPILER_WORKING_DIRECTORY="' .. project().location .. '"'}
            files(qt.moc({
                'third_party/chameleon/source/background_cleaner.hpp',
                'third_party/chameleon/source/dvs_display.hpp',
                'third_party/chameleon/source/delta_t_display.hpp'},
                'build/moc'))
            includedirs(qt.includedirs())
            libdirs(qt.libdirs())
            links(qt.links())
            buildoptions(qt.buildoptions())
            linkoptions(qt.linkoptions())
            configuration 'release'
                targetdir 'build/release'
                defines {'NDEBUG'}
                flags {'OptimizeSpeed'}
            configuration 'debug'
                targetdir 'build/debug'
                defines {'DEBUG'}
                flags {'Symbols'}
            configuration 'linux'
                links {'pthread', 'usb-1.0'}
                buildoptions {'-std=c++17'}
                linkoptions {'-std=c++17'}
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


        project 'player'
            kind 'ConsoleApp'
            language 'C++'
            location 'build'
            files {'source/player.cpp'}
            defines {'SEPIA_COMPILER_WORKING_DIRECTORY="' .. project().location .. '"'}
            files(qt.moc({
                'third_party/chameleon/source/background_cleaner.hpp',
                'third_party/chameleon/source/dvs_display.hpp',
                'third_party/chameleon/source/delta_t_display.hpp'},
                'build/moc'))
            includedirs(qt.includedirs())
            libdirs(qt.libdirs())
            links(qt.links())
            buildoptions(qt.buildoptions())
            linkoptions(qt.linkoptions())
            configuration 'release'
                targetdir 'build/release'
                defines {'NDEBUG'}
                flags {'OptimizeSpeed'}
            configuration 'debug'
                targetdir 'build/debug'
                defines {'DEBUG'}
                flags {'Symbols'}
            configuration 'linux'
                links {'pthread', 'usb-1.0'}
                buildoptions {'-std=c++17'}
                linkoptions {'-std=c++17'}
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
    end
