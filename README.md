# DFTContextMenuHandler - a windows shell extension

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

This is a shell extension for windows 10 (x64) that adds a file tools submenu to the windows explorer context menu for folders and files in general.

## Build and Installation

The DLL has to be built and installled by the following steps (you probably need administrative access):
- You don't need to copy the file to `C:\Windows\system32` or a similar system path
- Register the DLL (open a command prompt, change to the folder where the DLL (preferably `ReleaseUMinDependency`) was built, and type `regsvr32 DFTContextMenuHandler.dll`)
If you want to use the four additional options (DeleteEmptyFolders, Flatten, Flatten2 and SlideShow), you have to compile the projects in the respective subfolders.

## Authors

- based on a template by [Nick Carruthers](https://www.codeproject.com/script/Membership/View.aspx?mid=161)
- this shell extension: [thomas694](https://github.com/thomas694/DFTContextMenuHandler)

## License <a rel="license" href="https://www.gnu.org/licenses/gpl-3.0"><img alt="GNU GPLv3 license" style="border-width:0" src="https://img.shields.io/badge/License-GPLv3-blue.svg" /></a>

<span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">DFTContextMenuHandler</span> by thomas694 
is licensed under <a rel="license" href="https://www.gnu.org/licenses/gpl-3.0">GNU GPLv3</a>.<br/>
Based on a template from a disappeared source (should have been on CodeProject).
