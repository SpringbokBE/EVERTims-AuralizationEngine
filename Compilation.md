# Auralisation Engine

> This project contains pre-built binaries and libraries that should work out of the box. If, however, you want to change any component, you can follow these instructions to be able to recompile them.

## Library dependencies

* The open-source `libmysofa` library for C/C++, which can be found [here][sofa-link]. The library is able to read AES SOFA files that store head-related transfer functions (HRTFs), which are an essential part of the auralisation engine. In this project, we will statically link to this library instead of providing a dll.

* The open-source (de)compression library `zlib`, which can be found [here][zlib-link]. It is used by the `libmysofa` libary. In this project, we will use this as a dynamic library (_zlib1.dll_).

## Build dependencies

* `Microsoft Visual Studio 2019` to build the final executable. Download the latest version [here][msvc-link].

* The (partially) open-source application framework `JUCE`. Download the latest version [here][juce-link]. The auralisation engine is built using this framework. You can use _Projucer.exe_ to open the _AuralisationEngine.jucer_ found this repository. Projucer is able to configure and generate a solution that can be built using Microsoft Visual Studio.

* The open-source build process managing software package `CMake`. Download the latest version [here][cmake-link]. It comes with a handy GUI-tool called _cmake-gui_, which we will use in the build process of `zlib` only.

## `zlib` compilation

### Windows (32-bit)

1. Download the latest GitHub repository from [here][zlib-link] or by using Git (`git clone https://github.com/madler/zlib`).

1. Open CMake (_cmake-gui_) and locate the source code, which can be found in `{root}/zlib`. Build the binaries in a folder of preference, such as `{root}/zlib/build32`.

1. Click on `Configure` in the lower left corner of the CMake tool. Create the build directory if prompted to do so. Choose your latest Visual Studio version as generator and pick `Win32` as platform in the popup window. Click `Finish` to start the configuration. The configuration parameters that appear should be sufficient, we won't install the library but copy the files manually to our project.

1. Click on `Generate` next to the `Configure` button. Next, click on `Open Project` to open the generated solution in Microsoft Visual Studio.

1. Change the configuration to `Release` and the platform to `Win32` from the toolbar. Click on `zlib` in the Solution Explorer to select it. Build `zlib` (only) using the menu or `Ctrl + B`. Make sure the build succeeded.

1. Copy _zlib.dll_ from the `{root}/zlib/build32/Release` folder into the `bin32` folder of this project.

1. Copy _zlib.lib_ from the `{root}/zlib/build32/Release` folder into the `lib32` folder of this project.

### Windows (64-bit)

1. Download the latest GitHub repository from [here][zlib-link] or by using Git (`git clone https://github.com/madler/zlib`).

1. Open CMake (_cmake-gui_) and locate the source code, which can be found in `{root}/zlib`. Build the binaries in a folder of preference, such as `{root}/zlib/build64`.

1. Click on `Configure` in the lower left corner of the CMake tool. Create the build directory if prompted to do so. Choose your latest Visual Studio version as generator and pick `x64` as platform in the popup window. Click `Finish` to start the configuration. The configuration parameters that appear should be sufficient, we won't install the library but copy the files manually to our project.

1. Click on `Generate` next to the `Configure` button. Next, click on `Open Project` to open the generated solution in Microsoft Visual Studio.

1. Change the configuration to `Release` and the platform to `x64` from the toolbar. Click on `zlib` in the Solution Explorer to select it. Build `zlib` (only) using the menu or `Ctrl + B`. Make sure the build succeeded.

1. Copy _zlib.dll_ from the `{root}/zlib/build64/Release` folder into the `bin64` folder of this project.

1. Copy _zlib.lib_ from the `{root}/zlib/build64/Release` folder into the `lib64` folder of this project.

##  `libmysofa` compilation

### Windows (32-bit)

1. Download the latest GitHub repository from [here][sofa-link] or by using Git (`git clone https://github.com/hoene/libmysofa`).

1. Navigate to the `{root}/libmysofa/src` folder. Copy _config.h.in_ to _config.h_ and overwrite its contents with the following.

    ```
    #if !defined _CONFIG_H
    #define _CONFIG_H

    #define CMAKE_INSTALL_PREFIX ""
    #define CPACK_PACKAGE_VERSION_MAJOR 1
    #define CPACK_PACKAGE_VERSION_MINOR 0
    #define CPACK_PACKAGE_VERSION_PATCH 0

    #endif
    ```

1. Navigate to the `{download_folder}/libmysofa/src/hrtf` folder. Copy _mysofa_export.h.in_ to _mysofa_export.h_.

1. Open _libmysofa.sln_ located in `{root}/libmysofa/windows` using Microsoft Visual Studio. Retarget the solution to the latest one if prompted to do so. Change the configuration to `Release` and the platform to `x86` from the toolbar.

1. From the Solution Explorer, right-click on `mysofa` and go to `Properties` at the bottom. From this popup window, change the following, afterwards press `Apply` and `OK` to confirm.

    * Under `C/C++ > General > Additional Include Directories`, add the folder in which _zlib.h_ is to be found (e.g. the root `zlib` folder). Also add the folder in which _zconf.h_ is to be found (e.g. the `zlib/build32` folder).

    * Under `C/C++ > Precompiled Headers > Precompiled Header`, set the option to `Not Using Precompiled Headers`.

    * Under `C/C++ > Advanced > Disable Specific Warnings`, add the number `4996`. This will disable some error messages due to unsafe function calls such as _strcpy_. Contribution to to the repository is desirable.

1. Build the solution using the menu or `Ctrl + Shift + B`. Make sure the build succeeded.

1. Copy _mysofa.lib_ from the `{root}/libmysofa/windows/Release` folder into the `lib32` folder of this project.

### Windows (64-bit)

1. Download the latest GitHub repository from [here][sofa-link] or by using Git (`git clone https://github.com/hoene/libmysofa`).

1. Open _libmysofa.sln_ located in `{root}/libmysofa/windows` using Microsoft Visual Studio. Retarget the solution to the latest one if prompted to do so. Change the configuration to `Release` and the platform to `x64` from the toolbar.

1. Build the solution using the menu or `Ctrl + Shift + B`. Make sure the build succeeded.

1. Copy _mysofa.lib_ from the `{root}/libmysofa/windows/bin/x64/Release` folder into the `lib64` folder of this project.

##  `Auralisation Engine` compilation

### Windows (32-bit)

1. Open the _AuralisationEngine.jucer_ file with Projucer. Under the section `Exporters`, navigate to `Release32`. Click on the MSVC icon on the top of the window to save the project. The solution will automatically open in Microsoft Visual Studio.

1. Change the configuration to `Release32` and the platform to `Win32` from the toolbar. Build the solution using the menu or `Ctrl + Shift + B`. Make sure the build succeeded.

1. The executable _AuralisationEngine32.exe_ can now be run from the `{root}/evertims-auralisation-engine/bin32` folder.

### Windows (64-bit)

1. Open the _AuralisationEngine.jucer_ file with Projucer. Under the section `Exporters`, navigate to `Release64`. Click on the MSVC icon on the top of the window to save the project. The solution will automatically open in Microsoft Visual Studio.

1. Change the configuration to `Release64` and the platform to `x64` from the toolbar. Build the solution using the menu or `Ctrl + Shift + B`. Make sure the build succeeded.

1. The executable _AuralisationEngine64.exe_ can now be run from the `{root}/evertims-auralisation-engine/bin64` folder.

<!-- All weblinks are stored here. -->
[sofa-link]: https://github.com/hoene/libmysofa
[zlib-link]: https://github.com/madler/zlib
[msvc-link]: https://visualstudio.microsoft.com/downloads/
[juce-link]: https://shop.juce.com/get-juce
[cmake-link]: https://cmake.org/download/
