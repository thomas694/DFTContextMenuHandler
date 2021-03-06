# DFTContextMenuHandler - a windows shell extension

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

This is a shell extension for windows 10 (x64) that adds a file tools submenu to the windows explorer context menu for folders and files in general.

## Screenshot

![screenshot of submenu](https://user-images.githubusercontent.com/71355143/97242696-d355b280-17f4-11eb-8c56-cef1cc6b4243.png)

## Functions

The `My File Tools` submenu should be located above the `Send to` item and offers the following functions which are executed on the selected files
- `Convert dots 2 spaces` converts dots in the filenames to spaces
- `Convert spaces 2 dots` converts spaces in the filenames to dots
- `Convert underscores 2 spaces` converts underscores in the filenames to spaces
- `Convert spaces 2 underscores` converts spaces in the filenames to underscores
- `Remove group names (...-name.mp3)` removes `-name` from the filenames
- `Rename extension...` renames the file extensions to the choosen one
- `Append extension (.mp -> .mp3)...` appends the entered text to the extension names
- `Remove n chars from filename...` removes the last N characters from the filenames (use -N to remove the first characters)
- `Set file date/time...` sets the modification timestamp to the entered value
- `Append to filename...` appends the entered text to the filenames
- `Insert before filename...` prepends the entered text to the filenames
- `Empty file(s)...` removes the contents from the files (zero-byte files remain)

The following functions are executed on the selected folder:
- `Flatten folders (del empty ones)...` flattens the subfolder structure by moving all files to the selected folder (automatically adding numbers to filenames to prevent overwrites)
- `Flatten folders2 (+: folder _ file.ext)...` flattens the subfolder structure by moving all files to the selected folder and renaming them to `foldername _ filename.ext` (automatically adding numbers to filenames to prevent overwrites)
- `Delete empty subfolders...` removes empty subfolders after initial confirmation
- `SlideShow...` starts a basic slideshow application for viewing all image files (.bmp/.gif/.jpg/.jpeg/.tif) inside the selected folder and its subfolders
  - the title bar shows the following information: `Slideshow (<Duration>s[ecs], <ImageCnt>/<TotalCurrFolder>, <ImageCntAllFolders>/<TotalAllFolders>) [<FolderCnt>/<TotalFolders>] <Filename>`
  - the following keys can be used:
    - `up / down key` increase / decrease the viewing time (in seconds)
    - `p` pauses / restarts the slideshow
    - `f` skip the rest of the images inside this folder and continue with the next one
    - `ESC` first time stops the slideshow / second time closes the application
    - `Space` toggles between fullscreen and window mode

As per windows standards, `...` behind a menu entry indicates that the function isn't executed immediately, but another dialog opens which still allows it to be cancelled.

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
