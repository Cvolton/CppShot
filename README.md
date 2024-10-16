# CppShot
A transparent screnshot utility written in C++. Tested for compatibility on Windows 98/2000 onwards.

Download here: https://github.com/Cvolton/CppShot/releases/latest

## Note for Surface RT devices
The arm32 binary of CppShot is not signed, if you wish to use it on newer Windows 8 builds, make sure to sign CppShot.exe using for example [the Jailbreak Sign Tool](https://xdaforums.com/t/windows-rt-8-1-jailbreak-sign-tool.3228929/). Note that if you sign the DLLs, the program won't launch - make sure to only sign the exe.

## Building
Our build environment currently utilizes CMake. These are the currently recommended compiler/platform combinations

### x86/x64 - TDM-GCC 5.1
This is currently the latest compiler I've been able to find that supports Windows 98 and 2000. Note that this compiler doesn't support features beyond C++14. Available for download [here](https://sourceforge.net/projects/tdm-gcc/files/TDM-GCC%20Installer/).

### arm32 - llvm-mingw (UCRT variant)
Unlike MSVC, this compiler supports all arm32 builds down to [Build 7792](https://betawiki.net/wiki/Windows_8_build_7792). MSVCRT variant only supports Windows 8 and up, UCRT supports builds that don't have Windows 8 API features yet. Available [here](https://github.com/mstorsjo/llvm-mingw/).

### arm64 - MSVC
Arm64 is a relatively recent addition to Windows, recent enough that the compiler shouldn't matter at least. llvm-mingw would most likely also work just fine.