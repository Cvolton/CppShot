# Changelog
## 0.6.0
* Added an additional white flash to make sure the background is rendered properly
* Added proper DPI scaling support for Windows 8+
* Added x64, arm32 and arm64 versions
* Added support for taking screenshots on secondary monitors
* CppShot crashes no longer keep the taskbar permanently hidden
* Reorganized the internal code structure
* Optimized screenshot saving code
* Other bugfixes & improvements

## 0.5.0
* Enabled Unicode support
  * This means that the supported characters in file names no longer depend on the regional codepage
* Fixed taking screenshots on Windows 9x
* Fixed integer overflow related colorization bugs (mostly affected the Windows 7 Aero border)
* Fixed a bug causing screenshots to be 1px smaller than they were supposed to on each side
* Fixed a bug that caused the file path not to get built properly in some cases
* Fixed the application manifest, so the program can launch on beta versions of Windows Vista again

## 0.4.0
- Fixed a major bug causing parts of screenshots to be transparent
- Added an option to open the current save directory
- Made it possible to change the screenshot saving directory
  - There is currently no settings UI to do this, you have to create this registry key: `HKEY_CURRENT_USER\Software\CppShot` and create a string value called `Path`
- Fixed a bug causing the program to crash if the target program's title includes characters outside of the regional codeset
- Added error messages when hotkey registering fails
- Other UI improvements
  - Added a temporary icon to the EXE file
  - Enabled visual styles

## 0.3.0
- Improved the reliability of taking proper transparent screenshot
  - The program now actually attempts to wait for the backdrop to render properly
- In `CTRL+SHIFT+B` mode you are now guaranteed that the resulting screenshot will have even dimensions
- The save directory now gets automatically created
- Added error messages if the screenshot capture fails
- Internal improvements for code readability that don't really affect the functionality of the software

## 0.2.0
* Fixed out of bounds capture issues (the black rectangle)
* No longer using a 3rd window to steal focus for inactive screenshots
  * This fixes an issue where the window wouldn't be fully invisible on Windows 2000 and as a result it would be visible in the final screenshot
* Backdrop window no longer shows on the taskbar
* Backdrop window should now spawn properly even if your target program is running as admin

## 0.1.0
- Initial release
  - Press CTRL+B to take a standard transparent screenshot of any window.
  - Press CTRL+SHIFT+B to take a transparent active an inactive screenshot of any window (screenshot dimensions are also guaranteed to be divisible by 2 in this mode)