## MMFreeLyrics 
### An alternative lyric renderer for Hatsune Miku: Project Diva Mega Mix+
----
## Description

A Freetype based lyric renderer for MM+, which uses ImGui for styling
and configuration.

Since ImGui has no built-in text stroking support,the one used here is
a modded version that uses FT_Stroker to implement it.

First time modding a `C++` game...I have to say,It wasn't really pleasant ;(

## Screenshots
![image](https://user-images.githubusercontent.com/31397301/181287341-59d3d235-72b9-4452-8d6e-9ff2daef60d7.png)
![image](https://user-images.githubusercontent.com/31397301/181289185-4ab79121-dbd7-47b8-b4b1-ab079cdab79a.png)
![image](https://user-images.githubusercontent.com/31397301/181289438-2de3cd42-f715-426a-b5df-ab56d76f6266.png)

## Building
- Setup Freetype

    Freetype is managed via `vcpkg`

    You may use vcpkg with `vcpkg install freetype --triplet=x64-windows` + `vcpkg integrate install` to install it

## Known Issues
- Game won't change its internal resolution when resized

## TODO
----
- ~~Automatically build charset on startup~~
- ~~Config file support~~

## References
----
https://github.com/ocornut/imgui

https://github.com/ajkhoury/SigMaker-x64

https://github.com/BroGamer4256/score-mm

https://github.com/ActualMandM/DivaDllMods

https://github.com/blueskythlikesclouds/DivaModLoader